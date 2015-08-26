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
    
    sendSoundMessages = true; 
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
    byteReadyTimer = 0;
    track = 40;
    offset = 0;
    zone = 0;
    read_shiftreg = 0;
    read_shiftreg_pipe = 0;
    write_shiftreg = 0;
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
    uint32_t result = 15;

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
    readBlock(buffer, data[0], sizeof(data));
    numTracks = read8(buffer);
	for (unsigned i = 0; i < 84; i++)
		length[i] = read16(buffer);
    
    // Drive properties
    byteReadyTimer = read16(buffer);
	rotating = (bool)read8(buffer);
    redLED = (bool)read8(buffer);
    diskInserted = (bool)read8(buffer);
    writeProtected = (bool)read8(buffer);
    sendSoundMessages = (bool)read8(buffer);
    
    // Read/Write logic
    track = read8(buffer);
    offset = read16(buffer);
    zone = read8(buffer);
    read_shiftreg = read8(buffer);
    read_shiftreg_pipe = read8(buffer);
    write_shiftreg = read8(buffer);
    
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
    writeBlock(buffer, data[0], sizeof(data));
    write8(buffer, numTracks);
    for (unsigned i = 0; i < 84; i++)
        write16(buffer, length[i]);
    
    // Drive properties
    write16(buffer, byteReadyTimer);
	write8(buffer, (uint8_t)rotating);
    write8(buffer, (uint8_t)redLED);
    write8(buffer, (uint8_t)diskInserted);
    write8(buffer, (uint8_t)writeProtected);
    write8(buffer, (uint8_t)sendSoundMessages);
    
    // Read/Write logic
    write8(buffer, track);
    write16(buffer, offset);
    write8(buffer, zone);
    write8(buffer, read_shiftreg);
    write8(buffer, read_shiftreg_pipe);
    write8(buffer, write_shiftreg);

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
	msg("               SYNC : %d\n", SYNC());
	msg("  Symbol under head : %02X\n", readHead());
	msg("\n");
    msg("Directory track\n");
    dumpFullTrack(18);
}

void
VC1541::executeByteReady()
{
    read_shiftreg_pipe = read_shiftreg;
    read_shiftreg = readHead();

    if (readMode() && !SYNC()) {
        byteReady(read_shiftreg);
    }

    if (writeMode()) {
        writeHead(write_shiftreg);
        write_shiftreg = via2.ora;
        byteReady();
    }
    
    rotateDisk();
}

inline void
VC1541::byteReady(uint8_t byte)
{
    // On the VC1541 logic board, the byte ready signal is computed by a NAND gate with three inputs.
    // Two of them are clock lines ensuring that a signal is generated every eigths bit.
    // The third signal is hard-wired to pin CA2 of VIA2. By pulling CA2 low, the CPU can silence the
    // the byte ready line. E.g., this is done when moving the drive head to a different track
    if (via2.CA2()) {
        via2.ira = byte;
        byteReady();
    }
}

inline void
VC1541::byteReady()
{
    if (via2.overflowEnabled()) cpu->setV(1);
}


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
VC1541::setZone(uint8_t z)
{
    assert (z <= 3);
    
    if (z != zone) {
        debug(2, "Switching from disk zone %d to disk zone %d\n", zone, z);
        zone = z;
    }
}

void
VC1541::setRedLED(bool b)
{
    if (!redLED && b) {
        redLED = true;
        c64->putMessage(MSG_VC1541_LED, 1);
    } else if (redLED && !b) {
        redLED = false;
        c64->putMessage(MSG_VC1541_LED, 0);
    }
}

void
VC1541::setRotating(bool b)
{
    if (!rotating && b) {
        rotating = true;
        c64->putMessage(MSG_VC1541_MOTOR, 1);
    } else if (rotating && !b) {
        rotating = false;
        c64->putMessage(MSG_VC1541_MOTOR, 0);
    }
    
}

void
VC1541::moveHeadUp()
{
    debug(2, "track = %d, Moving head up to %2.1f\n", track, (track + 2) / 2.0);

    if (track < 83) {
        float position = (float)offset / (float)length[track];
        track++;
        offset = position * length[track];
    }

    debug(2, "offset: %d length: %d\n", offset, length[track]);
    
    assert(offset < length[track]);
    
    c64->putMessage(MSG_VC1541_HEAD, 1);
    if (track % 2 == 0 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_SOUND, 1); // play sound for full tracks, only
}

void
VC1541::moveHeadDown()
{
    debug(3, "track = %d, Moving head down to %2.1f\n", track, (track + 2) / 2.0);

    if (track > 0) {
        float position = (float)offset / (float)length[track];
        track--;
        offset = position * length[track];
    }
    
    assert(offset < length[track]);
    
    c64->putMessage(MSG_VC1541_HEAD, 0);
    if (track % 2 == 0 && sendSoundMessages)
        c64->putMessage(MSG_VC1541_HEAD_SOUND, 0); // play sound for full tracks, only
}


void
VC1541::clearHalftrack(unsigned halftrack)
{
	assert(halftrack >= 1 && halftrack <= 84);
	
    debug(2, "Clearing halftrack %d\n");

    memset(startOfHalftrack(halftrack), 0x55, 7928);
}

void
VC1541::clearDisk()
{
    for (unsigned i = 1; i <= 84; i++) {
		clearHalftrack(i);
        setLengthOfHalftrack(i, 7928);
    }
}


// ---------------------------------------------------------------------------------------------
//                               Data encoding and decoding
// ---------------------------------------------------------------------------------------------


void
VC1541::encodeDisk(D64Archive *a)
{
    // Interleave patterns (no interleave for now)
    int zone1[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, -1 };
    int zone2[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, -1 };
    int zone3[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, -1 };
    int zone4[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, -1 };

    unsigned track, encodedBytes;

    assert(a != NULL);
    
    clearDisk();
    numTracks = a->numberOfTracks();
    
    debug(2, "Encoding D64 archive with %d tracks", numTracks);

    // Zone 1: Tracks 1 - 17 (21 sectors, tailgap 9/9
    for (track = 1; track <= 17; track++) {
        assert(21 == a->numberOfSectors(trackToHalftrack(track)));
        encodedBytes = encodeTrack(a, track, zone1, 9, 9);
    }

    // Zone 2: Tracks 18 - 24 (19 sectors, tailgap 9/19 (even/odd sectors))
    for (track = 18; track <= 24; track++) {
        encodedBytes = encodeTrack(a, track, zone2, 9, 19);
    }

    // Zone 3: Tracks 25 - 30 (18 sectors, tailgap 9/13 (even/odd sectors))
    for (track = 25; track <= 30; track++) {
        encodedBytes = encodeTrack(a, track, zone3, 9, 13);
    }
    
    // Zone 4: Tracks 31 - 35..42 (17 sectors, tailgap 9/10 (even/odd sectors))
    for (track = 31; track <= a->numberOfTracks(); track++) {
        encodedBytes = encodeTrack(a, track, zone4, 9, 10);
    }
    
    // Clear remaining tracks (if any)
    for (track = numTracks + 1; track <= 42; track++) {
        unsigned halftrack = trackToHalftrack(track);
        setLengthOfHalftrack(halftrack, encodedBytes);
        setLengthOfHalftrack(halftrack + 1, encodedBytes);
    }

    for (unsigned i = 1; i <= 84; i++) {
        assert(length[i-1] <= 7928);
    }
}

unsigned
VC1541::encodeTrack(D64Archive *a, uint8_t track, int *sector, uint8_t tailGapEven, uint8_t tailGapOdd)
{
    unsigned encodedBytes, totalEncodedBytes = 0;

    assert(1 <= track && track <= 42);
    assert(a != NULL);
    
    debug(4, "Encoding track %d\n", track);

    uint8_t *dest = startOfTrack(track);
    
    // Scan the interleave pattern and encode each sector
    for (unsigned i = 0; sector[i] != -1; i++) {
        
        encodedBytes = encodeSector(a, track, sector[i], dest, (i % 2) ? tailGapOdd : tailGapEven);
        dest += encodedBytes;
        totalEncodedBytes += encodedBytes;
    }

    setLengthOfTrack(track, totalEncodedBytes);
    setLengthOfHalftrack(trackToHalftrack(track)+1, totalEncodedBytes);
    return totalEncodedBytes;
}

unsigned
VC1541::encodeSector(D64Archive *a, uint8_t track, uint8_t sector, uint8_t *dest, int gap)
{
    uint8_t *source, *ptr = dest;
    uint8_t halftrack = trackToHalftrack(track);
    
    assert(1 <= track && track <= 42);
    assert(a != NULL);
    assert(ptr != NULL);
    
    // Get source address from archive
    if ((source = a->findSector(halftrack, sector)) == 0) {
        warn("Can't find halftrack data in archive\n");
        return 0;
    }
    
    debug(4, "  Encoding sector %d\n", track, sector);
    
    // Get disk id and compute checksum
    uint8_t id_lo = a->diskIdLow();
    uint8_t id_hi = a->diskIdHi();
    uint8_t checksum = id_lo ^ id_hi ^ track ^ sector; // Header checksum byte
    
    encodeSync(ptr); // 0xFF 0xFF 0xFF 0xFF 0xFF
    ptr += 5;
    encodeGcr(0x08, checksum, sector, track, ptr); // header block ID, checksum, sector and track
    ptr += 5;
    encodeGcr(id_lo, id_hi, 0x0F, 0x0F, ptr); // Sector ID (LO/HI), 0x0F, 0x0F
    ptr += 5;
    encodeGap(ptr, 9); // 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55
    ptr += 9;
    encodeSync(ptr); // 0xFF 0xFF 0xFF 0xFF 0xFF
    ptr += 5;
    
    checksum = source[0];
    for (unsigned i = 1; i < 256; i++) // Data checksum byte
        checksum ^= source[i];
    
    encodeGcr(0x07, source[0], source[1], source[2], ptr); // data block ID, first three data bytes
    ptr += 5;
    for (unsigned i = 3; i < 255; i += 4, ptr += 5)
        encodeGcr(source[i], source[i+1], source[i+2], source[i+3], ptr); // Data chunks
    encodeGcr(source[255], checksum, 0, 0, ptr); // Last byte, checksum, 0x00, 0x00
    ptr += 5;
    
    assert(ptr - dest == 354);

    encodeGap(ptr, gap); // 0x55 0x55 ... 0x55 (tail gap)
    ptr += gap;
    
    // Return number of encoded bytes
    return ptr - dest;
}

void
VC1541::encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest)
{
    uint64_t shift_reg = 0;
    
    // Shift in
    shift_reg = gcr[b1 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b1 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b2 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b2 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b3 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b3 & 0x0F];
    shift_reg = (shift_reg << 5) | gcr[b4 >> 4];
    shift_reg = (shift_reg << 5) | gcr[b4 & 0x0F];

    // Shift out
    dest[4] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[3] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[2] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[1] = shift_reg & 0xFF; shift_reg >>= 8;
    dest[0] = shift_reg & 0xFF;
}


void
VC1541::insertDisk(D64Archive *a)
{
    assert(a != NULL);
    
    ejectDisk();
    encodeDisk(a);
    
    diskInserted = true;
    setWriteProtection(false);
    c64->putMessage(MSG_VC1541_DISK, 1);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 1);
}

void
VC1541::insertDisk(Archive *a)
{
    warn("Can only mount D64 images.\n");
}


unsigned
VC1541::decodeDisk(uint8_t *dest, int *error)
{
    unsigned track, halftrack, numBytes = 0;
    uint8_t tmpbuf[2 * 7928];
    
    if (error) *error = 0; // We assume the best
    
    // For each full track ...
    for (track = 1, halftrack = 0; track <= numTracks; track++, halftrack += 2) {
        
        debug(3, "Decoding track %d %s\n", track, dest == NULL ? "(test run)" : "");
        
        // Copy the track into temporary buffer
        // Buffer is double sized, so we can read safely beyond the array bounds

        assert(length[halftrack] < 7928);
        memcpy(tmpbuf, data[halftrack], length[halftrack]);
        memcpy(tmpbuf + length[halftrack], data[halftrack], length[halftrack]);

        if (dest)
            numBytes += decodeTrack(tmpbuf, dest + numBytes, error);
        else
            numBytes += decodeTrack(tmpbuf, NULL, error);

    }
    return numBytes;
}

unsigned
VC1541::decodeTrack(uint8_t *source, uint8_t *dest, int *error)
{
    
    unsigned numBytes = 0;
    int sectorID = -1, sectorStart[21]; // A track can contain up to 21 sectors

    // Initialize offset table
    for (unsigned i = 0; i < 21; sectorStart[i++] = -1);
    
    // Collect start addresses of all sectors in this track
    for (unsigned i = 2; i < 7928 + 512; i++) {
        
        if (source[i-2] != 0xFF || source[i-1] != 0xFF || source[i] == 0xFF)
            continue;
    
        uint8_t data[4];
        decodeGcr(source[i], source[i+1], source[i+2], source[i+3], source[i+4], data);

        // We found: |  0xFF  |  0xFF  | !0xFF   |
        //                             | data[0] | data[1] | data[2] | data[3] |
        //                                $08 => Header block
        //                                $07 => Data block


        if (data[0] == 0x08) { // Header block
            
            // databyte[1] = header block checksum
            // databyte[2] = sector number
            // databyte[3] = track number
            
            sectorID = data[2];
            debug(2, "Found header block (%d/%d)\n", data[3], data[2], data[1]);

        } else if (data[0] == 0x07) { // Data block

            if (sectorID == -1) {
                // we need to see the header block, first
                continue;
            }

            if (sectorID < 0 || sectorID > 20) {
                warn("Skipping sector %d. Sector number of range (0 - 20).\n", sectorID);
                if (error) *error = 1;
                continue;
            }

            debug("Found data block for sector %d at offset %d\n", sectorID, i);
            sectorStart[sectorID] = i;

        } else {
            warn("Skipping sector %d. Unknown header ID (expected $07 or $08, found $%02X).\n", sectorID, sectorID);
            if (error) *error = 1;
        }
    }
    
    // Decode all sectors that have been found
    for (unsigned i = 0; i < 21 && sectorStart[i] != -1; i++, numBytes += 256) {
        debug(2, "   Decoding sector %d\n", i);
        if (dest) {
            decodeSector(source + sectorStart[i], dest);
            dest += 256;
        }
    }
    
    return numBytes;
}

void
VC1541::decodeSector(uint8_t *source, uint8_t *dest)
{
    uint8_t databytes[4];
    uint8_t *ptr = dest;
    
    if (dest == NULL) return;
    
    // Decode the first three data bytes
    decodeGcr(source[0], source[1], source[2], source[3], source[4], databytes);
    assert(databytes[0] == 0x07);
    
    *(ptr++) = databytes[1];
    *(ptr++) = databytes[2];
    *(ptr++) = databytes[3];
    source += 5;
    
    // Keep on going
    for (unsigned i = 3; i < 255; i += 4, source += 5, ptr += 4) {
        decodeGcr(source[0], source[1], source[2], source[3], source[4], ptr);
    }
    
    // Decode the last byte
    decodeGcr(source[0], source[1], source[2], source[3], source[4], databytes);
    *(ptr++) = databytes[0];
    
    debug(2, "%d bytes written.\n", ptr-dest);
    assert(ptr - dest == 256);
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

void 
VC1541::ejectDisk()
{
    if (!hasDisk())
        return;
    
	// Open lid (write protection light barrier will be blocked)
	setWriteProtection(true);

	// Drive will notice the change in its interrupt routine...
	sleepMicrosec((uint64_t)200000);
	
	// Remove disk (write protection light barrier is no longer blocked)
	setWriteProtection(false);
		
    resetDisk();
	c64->putMessage(MSG_VC1541_DISK, 0);
    if (sendSoundMessages)
        c64->putMessage(MSG_VC1541_DISK_SOUND, 0);
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
    for (int i = 0; i < offset; i++) {
        if (i % 32 == 0) msg("\n%04X: ", i);
		msg("%02X ", data[t][i]);
    }
	msg("(%02X) ", data[t][offset]);
    for (int i = offset+1; i < length[t]; i++) {
        if (i % 32 == 0) msg("\n%04X: ", i);
		msg("%02X ", data[t][i]);
    }
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


