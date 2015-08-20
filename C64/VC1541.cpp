/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

VC1541::VC1541()
{
	name = "1541";
    debug(2, "Creating virtual VC1541 at address %p\n", this);
	
	// Create sub components
	mem = new VC1541Memory();
	cpu = new CPU();
	cpu->setName("1541CPU");
    
    resetDisk();
}

VC1541::~VC1541()
{
	debug(2, "Releasing VC1541...\n");
	
	delete cpu;	
	delete mem;
}

void
VC1541::resetDrive(C64 *c64)
{
    debug (2, "Resetting VC1541...\n");
    
    // Establish bindings
    this->c64 = c64;
    iec = c64->iec;
    
    // Reset subcomponents
    mem->reset(c64);
    cpu->reset(c64, mem);
    cpu->setPC(0xEAA0);
    via1.reset(c64);
    via2.reset(c64);
    
    // VC1541 properties
    rotating = false;
    redLED = false;
    
    syncMark = false;
    byteReadyTimer = 0;
    track = 40;
    offset = 0;
    noOfFFBytes = 0;
    latched_readmode = true;
    latched_ora = 0;
}

void
VC1541::resetDisk()
{
    debug (2, "Resetting disk in VC1541...\n");

    // Disk properties
    clearDisk();
    diskInserted = false;
    writeProtected = false;
}

void
VC1541::ping()
{
    debug(2, "Pinging VC1541...\n");
    c64->putMessage(MSG_VC1541_LED, redLED ? 1 : 0);
    c64->putMessage(MSG_VC1541_MOTOR, rotating ? 1 : 0);
    c64->putMessage(MSG_VC1541_DISK, diskInserted ? 1 : 0);

    cpu->ping();
    mem->ping();
    via1.ping();
    via2.ping();

}

uint32_t
VC1541::stateSize()
{
    uint32_t result = 16;

    for (unsigned i = 0; i < 84; i++)
        result += sizeof(data[i]);
    
    result += 2*84;
    
    result += cpu->stateSize();
    result += via1.stateSize();
    result += via2.stateSize();
    result += mem->stateSize();
    
    return result;
}

void
VC1541::loadFromBuffer(uint8_t **buffer)
{	
    uint8_t *old = *buffer;
    
    // Disk data storage
	for (unsigned i = 0; i < 84; i++)
		for (unsigned j = 0; j < sizeof(data[i]); j++)
			data[i][j] = read8(buffer);
    numTracks = read8(buffer);
	for (unsigned i = 0; i < 84; i++)
		length[i] = read16(buffer);
    
    // Drive properties
	rotating = (bool)read8(buffer);
    redLED = (bool)read8(buffer);
    diskInserted = (bool)read8(buffer);
    writeProtected = (bool)read8(buffer);

    // Read/Write logic
    track = (int)read16(buffer);
    offset = (int)read16(buffer);
    syncMark = (bool)read8(buffer);
    byteReadyTimer = (int)read16(buffer);
	noOfFFBytes = (int)read16(buffer);
    latched_readmode = (bool)read8(buffer);
    latched_ora = (uint8_t)read8(buffer);
    
    // Subcomponents
	cpu->loadFromBuffer(buffer);
    via1.loadFromBuffer(buffer);
    via2.loadFromBuffer(buffer);
    mem->loadFromBuffer(buffer);
    
    debug(2, "  VC1541 state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
VC1541::saveToBuffer(uint8_t **buffer)
{	
    uint8_t *old = *buffer;
    
    // Disk data storage
	for (unsigned i = 0; i < 84; i++)
		for (unsigned j = 0; j < sizeof(data[i]); j++)
			write8(buffer, data[i][j]);
    write8(buffer, numTracks);
	for (unsigned i = 0; i < 84; i++)
		write16(buffer, length[i]);
    
    // Drive properties
	write8(buffer, (uint8_t)rotating);
    write8(buffer, (uint8_t)redLED);
    write8(buffer, (uint8_t)diskInserted);
    write8(buffer, (uint8_t)writeProtected);

    // Read/Write logic
    write16(buffer, (uint16_t)track);
    write16(buffer, (uint16_t)offset);
    write8(buffer, (uint8_t)syncMark);
    write16(buffer, (uint16_t)byteReadyTimer);
    write16(buffer, (uint16_t)noOfFFBytes);
    write8(buffer, (uint8_t)latched_readmode);
    write8(buffer, latched_ora);

    // Subcomponents
    cpu->saveToBuffer(buffer);
    via1.saveToBuffer(buffer);
    via2.saveToBuffer(buffer);
	mem->saveToBuffer(buffer);
    
    debug(4, "  VC1541 state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
VC1541::dumpState()
{
	msg("VC1541\n");
	msg("------\n\n");
	msg("         Head timer : %d\n", byteReadyTimer);
	msg("              Track : %d\n", track);
	msg("       Track offset : %d\n", offset);
	msg("Sync bytes in a row : %d\n", noOfFFBytes);
	msg("  Symbol under head : %02X\n", readHead());
	msg("        Next symbol : %02X\n", readHeadLookAhead());
	msg("\n");
    msg("Directory track\n");
    dumpFullTrack(18);
}




bool
VC1541::executeOneCycle()
{
    bool result;
    
    via1.execute();
    via2.execute();
    result = cpu->executeOneCycle();
    
    // Decrement byte ready counter to 1, if active
    if (byteReadyTimer == 0)
        return result;
    
    if (byteReadyTimer > 1) {
        byteReadyTimer--;
        return result;
    }
    
    byteReadyTimer = VC1541_CYCLES_PER_BYTE;
    
    // Byte is complete.
    
    if (!latched_readmode) {
        
        // Write to disk
        noOfFFBytes = 0;
        setSyncMark(0);
        writeHead(latched_ora);
        signalByteReady();

    } else {
        
        // "Eine auftretende SYNC-Markierung löst beim Diskcontroller das hardwaremäßig festgelegte
        //  SYNC-Signal aus. Während dieses Signal auftritt, wird der Schreib-/Leseport des Diskcontrollers
        //  gesperrt, das heißt, nicht mehr mit Daten versorgt - und auch kein BYTE READY mehr ausgelöst
        //  -, bis die SYNC-Zone auf der Diskette vorbei ist. Aus diesem Grund enthält der VIA 2 in seinem
        //  Port zum Schreib-/Lesekopf nach dem Auftreten des SYNC-Signals einen undefinierten Wert.
        //  Dieser entspricht der Bitfolge, die bis zur Feststellung des SYNC-Bereichs durch den Controller
        //  eingelesen wurde." [Die Floppy 1570/1571, Markt und Technik]
        
        // Read from disk
        if (readHead() == 0xFF)
            noOfFFBytes++;
        else
            noOfFFBytes = 0;
    
        if (noOfFFBytes <= 1) { // no sync, yet
            via2.ora = readHead();
            signalByteReady();
            setSyncMark(0);
        } else {
            setSyncMark(1);
        }
    }
    
    // Prepare for next byte
    rotateDisk();
    latched_readmode = via2.isReadMode();
    latched_ora = via2.ora;
    
    return result;
}

#if 0
bool
VC1541::executeOneCycle()
{
    bool result;
    
    via1->execute(1);
    via2->execute(1);
    result = cpu->executeOneCycle();
    
    if (byteReadyTimer == 0)
        return result;
    
    if (byteReadyTimer > 1) {
        byteReadyTimer--;
        return result;
    }
    
    // Reset timer
    byteReadyTimer = VC1541_CYCLES_PER_BYTE;
    
    // Rotate disk
    rotateDisk();
    
    // Old sync mark logic (brittle)
    setSyncMark(readHead() == 0xFF);

    if (readHead() == 0xFF) {
        // Sync found
        noOfFFBytes++;
    } else {
        noOfFFBytes = 0;
    }
    
    if (noOfFFBytes <= 1)
        signalByteReady();
    
    // Read or write data
    // if (via2->isReadMode()) {
    if (readmode) {
        via2->ora = readHead();
    } else {
        writeOraToDisk();
        signalByteReady();
    }	
    // latch...
    readmode = via2->isReadMode();
    
    return result;
}
#endif

void 
VC1541::simulateAtnInterrupt()
{
	if (via1.atnInterruptsEnabled()) {
		via1.indicateAtnInterrupt();
		cpu->setIRQLineATN();
		// debug("CPU is interrupted by ATN line.\n");
	} else {
		// debug("Sorry, want to interrupt, but CPU does not accept ATN line interrupts\n");
	}
}

void
VC1541::activateRedLED()
{
    redLED = true; c64->putMessage(MSG_VC1541_LED, 1);
}

void
VC1541::deactivateRedLED()
{
    redLED = false; c64->putMessage(MSG_VC1541_LED, 0);
}

void
VC1541::startRotating()
{ 
	debug(2, "Starting drive engine (%2X)\n", cpu->getPC());
	rotating = true;
	byteReadyTimer = VC1541_CYCLES_PER_BYTE;
	c64->putMessage(MSG_VC1541_MOTOR, 1);
}

void 
VC1541::stopRotating() 
{ 
	debug(2, "Stopping drive engine (%2X)\n", cpu->getPC()); 
	rotating = false;

	c64->putMessage(MSG_VC1541_MOTOR, 0);
}

void 
VC1541::rotateDisk()
{ 
	offset++; 
	if (offset >= length[track]) offset = 0; 
}

void 
VC1541::moveHeadUp()
{
    if (track < 83) track++;
	offset = offset % length[track];

    debug(3, "Moving head up to %2.1f\n", (track + 2) / 2.0);
}

void
VC1541::moveHeadDown()
{
    if (track > 0) track--;
    offset = offset % length[track];
    
    debug(3, "Moving head down to %2.1f\n", (track + 2) / 2.0);
}

#if 0
void
VC1541::writeOraToDisk()
{
    writeByteToDisk(via2->ora);
}
#endif

void
VC1541::clearHalftrack(int nr)
{
	assert(nr >= 1 && nr <= 84);
	
	length[nr-1] = sizeof(data[nr-1]);
	// memset(data[nr-1], 0, length[nr-1]);
	memset(data[nr-1], 0x55, length[nr-1]);
}

void
VC1541::clearDisk()
{
	int i;
	for (i = 1; i <= 84; i++)
		clearHalftrack(i);
}

void
VC1541::encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest)
{
    uint64_t shift_reg = 0;
    
    // shift in
    shift_reg = gcr[b1 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b1 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b2 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b2 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b3 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b3 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b4 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b4 & 0x0F];

    // shift out
    dest[4] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[3] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[2] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[1] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[0] = shift_reg & 0xFF;
}

void
VC1541::decodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t b5, uint8_t *dest)
{
    uint64_t shift_reg;
   
    // Create inverse lookup table
    uint8_t lookup[32];
    memset(lookup, 0, sizeof(lookup));
    for (unsigned i = 0; i < 16; i++)
        lookup[gcr[i]] = i;

    // Shift in
    shift_reg = b1;
    shift_reg = (shift_reg << 8) | b2;
    shift_reg = (shift_reg << 8) | b3;
    shift_reg = (shift_reg << 8) | b4;
    shift_reg = (shift_reg << 8) | b5;

    // Shift out
    dest[3] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[3] |= (lookup[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[2] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[2] |= (lookup[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[1] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[1] |= (lookup[shift_reg & 0x1F] << 4); shift_reg >>= 5;
    dest[0] = lookup[shift_reg & 0x1F]; shift_reg >>= 5;
    dest[0] |= (lookup[shift_reg & 0x1F] << 4);
}

int
VC1541::encodeSector(D64Archive *a, uint8_t halftrack, uint8_t sector, uint8_t *dest, int gap)
{
	int i;
	uint8_t *source, *ptr = dest;
	uint8_t id_lo, id_hi, checksum;
	uint8_t track = (halftrack + 1) / 2;
		
	assert(a != NULL);
	assert(ptr != NULL);
	assert(1 <= track && track <= 42);
	
	if ((source = a->findSector(halftrack, sector)) == 0) {
		warn("Can't find halftrack data in archive\n");
		return 0;
	}
	// debug(3, "Encoding track %d, sector %d\n", track, sector);

	// Get disk id and compute checksum
	id_lo = a->diskIdLow();
	id_hi = a->diskIdHi();
	checksum = id_lo ^ id_hi ^ track ^ sector;
	
	// Write SYNC mark
	for (i = 0; i < 6; i++, ptr++)
		*ptr = 0xFF;
		
	// Write magic byte (header mark), checksum, sector and track
	encodeGcr(0x08, checksum, sector, track, ptr);
	ptr += 5;
	
	// Write id lo, id hi, 0x0F, 0x0F
	encodeGcr(id_lo, id_hi, 0x0F, 0x0F, ptr);
	ptr += 5;
	
	// Write gap (9 x 0x55)
	for (i = 0; i < 9; i++, ptr++)
		*ptr = 0x55;

	// Write SYNC mark
	for (i = 0; i < 6; i++, ptr++)
		*ptr = 0xFF;

	// Compute data checksum 
	checksum = source[0];
	for (i = 1; i < 256; i++)
		checksum ^= source[i];
		
	// Encode magic byte (data mark), first three data bytes
	encodeGcr(0x07, source[0], source[1], source[2], ptr);
	ptr += 5;
	
	// Encode chunks of data
	for (i = 3; i < 255; i += 4) {
		encodeGcr(source[i], source[i+1], source[i+2], source[i+3], ptr);
		ptr += 5;
	}
	assert(i == 255);
	
	// Encode last byte, checksum, 0x00, 0x00
	encodeGcr(source[255], checksum, 0, 0, ptr);
	ptr += 5;
	
	// Write gap
	for (i = 0; i < gap; i++, ptr++)
		*ptr = 0x55;
	
	// returns number of encoded bytes
	return ptr - dest;
}

void
VC1541::decodeSector(uint8_t *source, uint8_t *dest)
{
    assert(dest != NULL);

    int i;
    uint8_t databytes[4];
    uint8_t *ptr = dest;
    
    debug(2, "Decoding sector\n");
    
    // Skip SYNC mark
    source += 6;
    
    // Skip magic byte (header mark), checksum, sector and track
    source += 5;
    
    // Skip id lo, id hi, 0x0F, 0x0F
    source += 5;
    
    // Skip gap (9 x 0x55)
    source += 9;
    
    // Skip SYNC mark
    source += 6;
    
    // Decode magic byte (data mark), first three data bytes
    decodeGcr(source[0], source[1], source[2], source[3], source[4], databytes);

    if (databytes[0] != 0x07)
        warn("Expected magic byte 07. Found %02X\n", databytes[0]);

    *(ptr++) = databytes[1];
    *(ptr++) = databytes[2];
    *(ptr++) = databytes[3];
    source += 5;
    
    // Decode chunks of data
    for (i = 3; i < 255; i += 4) {
        decodeGcr(source[0], source[1], source[2], source[3], source[4], ptr);
        source += 5;
        ptr += 4;
    }
    assert(i == 255);
    
    // Decode last byte, checksum, 0x00, 0x00
    decodeGcr(source[0], source[1], source[2], source[3], source[4], databytes);
    *(ptr++) = databytes[0];
    
    // Hopefully, 256 bytes have been decoded...
    assert(ptr-dest == 256);
}

void 
VC1541::insertDisk(Archive *a)
{
	warn("Can only mount D64 images.\n");
}

void
VC1541::encodeDisk(D64Archive *a)
{
    unsigned i,j;
    uint8_t *dest;
    
    assert(a != NULL);
    
    numTracks = a->numberOfTracks();
    
    // For each full track...
    for (i = 1; i <= 2*a->numberOfTracks(); i += 2) {

        // For each sector...
        int gap =  (7928 - (a->numberOfSectors(i) * 356)) / a->numberOfSectors(i);
        for (j = 0, dest = data[i-1]; j < a->numberOfSectors(i); j++) {
            dest += encodeSector(a, i, j, dest, gap);
        }
        
        length[i-1] = dest - data[i-1];
        debug(3, "Length of track %d: %d bytes\n", i, dest - data[i-1]);
    }
    
    for (i = 1; i <= 84; i++) {
        assert(length[i-1] <= 7928);
    }
}

unsigned
VC1541::decodeDisk(uint8_t *dest)
{
    unsigned track, halftrack, count;
    
    // For each full track...
    for (track = 1, halftrack = count = 0; track <= numTracks; track++, halftrack += 2) {
        
        debug(4, "Decoding track %d %s\n", track, dest == NULL ? "(test run)" : "");
        
        for (unsigned j = 0; j < length[halftrack]; j++) {
            
            // debug(4, "    Decoding ?? %d %s\n", j, dest == NULL ? "(test run)" : "");

            uint8_t *sector_start = &data[halftrack][j];
            if (sector_start[0] != 0xFF) continue;
            if (sector_start[1] != 0xFF) continue;
            if (sector_start[2] != 0xFF) continue;
            if (sector_start[3] != 0xFF) continue;
            if (sector_start[4] != 0xFF) continue;
            if (sector_start[5] != 0xFF) continue;
            if (sector_start[6] != 0x52) continue;
            
            // Sync mark found (reached start of sector)
            count++;
            if (dest != NULL) {
                decodeSector(sector_start, dest);
                dest += 256;
            }
        }
    }
    return count * 256; 
}

void 
VC1541::insertDisk(D64Archive *a)
{
	assert(a != NULL);

    // debug(2, "VC1541::insertDisk(D64Archive *)");

    ejectDisk();
    encodeDisk(a);

    diskInserted = true;
    setWriteProtection(false);
	c64->putMessage(MSG_VC1541_DISK, 1);
}

void 
VC1541::ejectDisk()
{
	// Open lid (write protection light barrier will be blocked)
	setWriteProtection(true);

	// Drive will notice the change in its interrupt routine...
	sleepMicrosec((uint64_t)200000);
	
	// Remove disk (write protection light barrier is no longer blocked)
	setWriteProtection(false);
		
    resetDisk();
	c64->putMessage(MSG_VC1541_DISK, 0);
}
			
void 
VC1541::dumpTrack(int t)
{	
	if (t < 0) t = track;

	int min = offset - 40, max = offset + 20;
	if (min < 0) min = 0;
	if (max > length[t]) max = length[t];
	
	msg("Dumping track %d (length = %d)\n", t, length[t]);
	for (int i = min; i < offset; i++)
		msg("%02X ", data[t][i]);
	msg("(%02X) ", data[t][offset]);
	for (int i = offset+1; i < max; i++)
		msg("%02X ", data[t][i]);
	msg("\n");
}

void 
VC1541::dumpFullTrack(int t)
{		
	if (t < 0) t = track;
	
	msg("Dumping track %d (length = %d)\n", t, length[t]);
	for (int i = 0; i < offset; i++)
		msg("%02X ", data[t][i]);
	msg("(%02X) ", data[t][offset]);
	for (int i = offset+1; i < length[t]; i++)
		msg("%02X ", data[t][i]);
	msg("\n");
}

bool 
VC1541::isG64Image(const char *filename)
{
	int magic_bytes[] = { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31, EOF };

	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".G64") && !checkFileSuffix(filename, ".g64"))
		return false;
		
	if (!checkFileHeader(filename, magic_bytes))
		return false;
		
	return true;
}

bool 
VC1541::readG64Image(const char *filename)
{
	struct stat fileProperties;
	FILE *file;
	uint8_t *filedata;
	int size, c, track;
	
	assert (filename != NULL);
		
	// Get file properties
    if (stat(filename, &fileProperties) != 0) {
		// Could not open file...
		return false;
	}
	
	// Open file
	if (!(file = fopen(filename, "r"))) {
		// Can't open for read (Huh?)
		return false;
	}

	// Allocate memory
	if ((filedata = (uint8_t *)malloc(fileProperties.st_size)) == NULL) {
		// Didn't get enough memory
		return false;
	}
		
	// Read data
	size = 0;
	c = fgetc(file);
	while(c != EOF) {
		filedata[size++] = (uint8_t)c;
		c = fgetc(file);
	}
	fclose(file);			
	debug(1, "G64 image imported successfully (%d bytes total, size = %d)\n", fileProperties.st_size, size);

	// Analyze data
	debug(1, "    Version: %2X\n", (int)filedata[0x08]);
	debug(1, "    Number of tracks: %d\n", (int)filedata[0x09]);
	debug(1, "    Size of track: %d\n", (int)((filedata[0x0B] << 8) | filedata[0x0A]));
	for (track = 0; track < 84; track++) {
		uint32_t offset;
		uint16_t track_length, i;
		
		offset  = (filedata[4*track+0x0F] << 24) | (filedata[4*track+0x0E] << 16) | (filedata[4*track+0x0D] << 8) | filedata[4*track + 0x0C];
		track_length = (filedata[offset + 1] << 8) | filedata[offset];

		if (offset != 0) {
			debug(1, "    Track %2.1f: Offset: %8X Length: %04X\n", (track + 2) / 2.0, offset, track_length);
		}
		
		// copy data
		if (offset) {
			for (i = 0; i < track_length; i++)
				data[track][i] = filedata[offset + 2 + i];
			length[track] = track_length;
		} else {
			for (i = 0; i < 7928; i++)
				data[track][i] = 0;
			length[track] = 7928;
		}
	}
	
	free(filedata);
	return true;
}

bool
VC1541::exportToD64(const char *filename)
{
    D64Archive *archive;
    
    assert(filename != NULL);
    
    // Create archive
    if ((archive = D64Archive::archiveFromDrive(this)) == NULL)
        return false;
    
    // Write archive to disk
    archive->writeToFile(filename);
    
    delete archive;
    return true;
}


