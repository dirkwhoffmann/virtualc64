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
	// Initialize references
	iec = NULL;
	c64 = NULL;
	
	// Create sub components
	mem = new VC1541Memory();
	cpu = new CPU();		
	via1 = new VIA1();
	via2 = new VIA2();
		
	// Connect components
	cpu->setMemory(mem);
	mem->setCPU(cpu);
	mem->setDrive(this);
	via1->setDrive(this);
	via2->setDrive(this);
	
	printf("Created virtual 1541 drive at address %p\n", this);
}

VC1541::~VC1541()
{
	// Release sub components
	delete cpu;	
	delete mem;

	printf("Released virtual 1541 drive\n");
}

void 
VC1541::reset()
{
	ejectDisc();
	cpu->reset();
	// cpu->setHardBreakpoint(0xEAA1);
	mem->reset();
	via1->reset();
	via2->reset();
	
	byteReadyTimer = 0;
	track = 40;
	offset = 0;
	noOfFFBytes = 0;
	readFlag = writeFlag = syncFlag = false;
	clearDisk();
}

#if 0
void 
VC1541::executeOneCycle()
{
	via1->execute(1);
	via2->execute(1);
	cpu->executeOneCycle();

	if (byteReadyTimer == 0)
		return;

	if (byteReadyTimer > 1) {
		byteReadyTimer--;
		return;
	}

	// Reset timer
	byteReadyTimer = VC1541_CYCLES_PER_BYTE;
		
	if (readFlag == false && writeFlag == false && syncFlag == false) {
	}
	readFlag = writeFlag = syncFlag = false;

	// Rotate disk
	rotateDisk();
	if (readHead() != 0xFF) signalByteReady();
	
	// Read or write data
	if (via2->isReadMode()) {
		via2->ora = readHead();
	} else {
		writeOraToDisk();
	}	
}
#endif

void 
VC1541::executeOneCycle()
{
	via1->execute(1);
	via2->execute(1);
	cpu->executeOneCycle(0);

	if (tracingEnabled()) {
		dumpState();
	}
	
	if (byteReadyTimer == 0)
		return;

	if (byteReadyTimer > 1) {
		byteReadyTimer--;
		return;
	}

	// Reset timer
	byteReadyTimer = VC1541_CYCLES_PER_BYTE;
				
	if (readFlag == false && writeFlag == false && syncFlag == false) {
		// don't rotate
		// return;
		// debug("[%04X]", cpu->getPC());
		// debug("*");
	}
	
	// printf("r : %d w : %d s : %d\n", readFlag, writeFlag, syncFlag);
	readFlag = writeFlag = syncFlag = false;

	// Rotate disk
	rotateDisk();
	if (readHead() == 0xFF)
		noOfFFBytes++;
	else
		noOfFFBytes = 0;
	//if (readHead() != 0xFF) signalByteReady();
	if (noOfFFBytes <= 1) signalByteReady();
		
	// Read or write data
	if (via2->isReadMode()) {
		via2->ora = readHead();
	} else {
		writeOraToDisk();
	}	
}

void 
VC1541::simulateAtnInterrupt()
{
	if (via1->atnInterruptsEnabled()) {
		via1->indicateAtnInterrupt();
		cpu->setIRQLineATN();
		debug("CPU is interrupted by ATN line.\n");
	} else {
		debug("Sorry, want to interrupt, but CPU does not accept ATN line interrupts\n");
	}
}

bool
VC1541::load(FILE *file)
{
	return true;
}

bool 
VC1541::save(FILE *file)
{
	return true;
}

void 
VC1541::startRotating() 
{ 
	debug("Starting drive engine (%2X)\n", cpu->getPC());
	getListener()->driveMotorAction(true);
	if (c64->getWarpLoad())
		c64->cpu->setWarpMode(true);
		
	byteReadyTimer = 100; // ??? which value is appropriate?
}

void 
VC1541::stopRotating() 
{ 
	debug("Stopping drive engine (%2X)\n", cpu->getPC()); 
	getListener()->driveMotorAction(false);
	if (c64->getWarpLoad())
		c64->cpu->setWarpMode(false);
}

void 
VC1541::rotateDisk()
{ 
	offset++; 
	if (offset >= length[track]) offset = 0; 
}

void 
VC1541::moveHead(int distance)
{
	track += distance;
	if (track < 0) track = 0;
	if (track > 83) track = 83;
//	offset = 0;
	offset = offset % length[track];

	if (distance == 1)
		debug("Head up (to %2.1f) at %4X\n", (track + 2) / 2.0, cpu->getPC());
	else if (distance == -1)
		debug("Head down (to %2.1f) at %4X\n", (track + 2) / 2.0, cpu->getPC());
	else 
		debug("Head ???\n");
}

void
VC1541::clearHalftrack(int nr)
{
	assert(nr >= 1 && nr <= 84);
	
	length[nr-1] = sizeof(data[nr-1]);
	memset(data[nr-1], 0, length[nr-1]);
}

void
VC1541::clearDisk()
{
	int i;
	for (i = 1; i <= 84; i++)
		clearHalftrack(i);
}

//#if 0
void gcr_conv4(uint8_t *from, uint8_t *to)
{
const uint16_t gcr_table[16] = {
	0x0a, 0x0b, 0x12, 0x13, 0x0e, 0x0f, 0x16, 0x17,
	0x09, 0x19, 0x1a, 0x1b, 0x0d, 0x1d, 0x1e, 0x15
};

	uint16_t g;

	g = (gcr_table[*from >> 4] << 5) | gcr_table[*from & 15];
	*to++ = g >> 2;
	*to = (g << 6) & 0xc0;
	from++;

	g = (gcr_table[*from >> 4] << 5) | gcr_table[*from & 15];
	*to++ |= (g >> 4) & 0x3f;
	*to = (g << 4) & 0xf0;
	from++;

	g = (gcr_table[*from >> 4] << 5) | gcr_table[*from & 15];
	*to++ |= (g >> 6) & 0x0f;
	*to = (g << 2) & 0xfc;
	from++;

	g = (gcr_table[*from >> 4] << 5) | gcr_table[*from & 15];
	*to++ |= (g >> 8) & 0x03;
	*to = g;
}
//#endif

void
VC1541::encodeGcr(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4, uint8_t *dest)
{
	const uint16_t gcr[16] = { 
		0x0a, 0x0b, 0x12, 0x13, 0x0e, 0x0f, 0x16, 0x17, 0x09, 0x19, 0x1a, 0x1b, 0x0d, 0x1d, 0x1e, 0x15 
	};
	uint16_t shift_reg;

	// shift in first data byte
	shift_reg = 0;	
	shift_reg |= gcr[b1 >> 4];
	shift_reg <<= 5;
	shift_reg |= gcr[b1 & 0x0F];
	dest[0] = (shift_reg >> 2) & 0xFF;
	
	// shift in second data byte
	shift_reg <<= 5;	
	shift_reg |= gcr[b2 >> 4];
	shift_reg <<= 5;
	shift_reg |= gcr[b2 & 0x0F];
	dest[1] = (shift_reg >> 4) & 0xFF;

	// shift in third data byte
	shift_reg <<= 5;	
	shift_reg |= gcr[b3 >> 4];
	shift_reg <<= 5;
	shift_reg |= gcr[b3 & 0x0F];
	dest[2] = (shift_reg >> 6) & 0xFF;

	// shift in fourth data byte
	shift_reg <<= 5;	
	shift_reg |= gcr[b4 >> 4];
	shift_reg <<= 5;
	shift_reg |= gcr[b4 & 0x0F];
	dest[3] = (shift_reg >> 8) & 0xFF;
	dest[4] = shift_reg & 0xFF;

//#if 0
	// compare to FRODO (for debugging)
	{
		uint8_t source[4];
		source[0] = b1; source[1] = b2; source[2] = b3; source[3] = b4;
		uint8_t frodo[5];
		gcr_conv4(source, frodo);
		if (dest[0] != frodo[0] || dest[1] != frodo[1] || dest[2] != frodo[2] || dest[3] != frodo[3] || dest[4] != frodo[4]) {
			debug("%2X %2X %2X %2X: %2X %2X mismatch\n", b1,b2,b3,b4, dest[0], frodo[0]);
			exit(1);
		}
	}
	return;
//#endif
}

int 
VC1541::encodeSector(D64Archive *a, uint8_t halftrack, uint8_t sector, uint8_t *dest)
{
	int i;
	uint8_t *source, *ptr = dest;
	uint8_t id_lo, id_hi, checksum;
	uint8_t track = (halftrack + 1) / 2;
	
	assert(a != NULL);
	assert(ptr != NULL);
	assert(1 <= track && track <= 42);
	
	if ((source = a->findSector(halftrack, sector)) == 0) {
		warn("Can't encode halftrack. Not supported by the D64 format.\n");
		return 0;
	}
	debug("Encoding track %d, sector %d\n", track, sector);

	// Get disk id and compute checksum
	id_lo = a->diskIdLow();
	id_hi = a->diskIdHi();
	checksum = id_lo ^ id_hi ^ track ^ sector;
	
	// printf("Disk ID: %2X%2X\n", id_hi, id_lo);

	// Write SYNC mark
	for (i = 0; i < 5; i++, ptr++)
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
	for (i = 0; i < 5; i++, ptr++)
		*ptr = 0xFF;

	// Compute data checksum 
	checksum = source[0];
	for (i = 1; i < 256; i++)
		checksum ^= source[i];
		
	// Write magic byte (data mark), first three data bytes
	encodeGcr(0x07, source[0], source[1], source[2], ptr);
	ptr += 5;
	
	// Write chunks of data
	for (i = 3; i < 255; i += 4) {
		encodeGcr(source[i], source[i+1], source[i+2], source[i+3], ptr);
		ptr += 5;
	}
	assert(i == 255);
	
	// Write last byte, checksum, 0x00, 0x00
	encodeGcr(source[255], checksum, 0, 0, ptr);
	ptr += 5;

	// Write gap (8 x 0x55)
	for (i = 0; i < 8; i++, ptr++)
		*ptr = 0x55;
	
	// returns number of encoded bytes
	return ptr - dest;
}

void 
VC1541::insertDisc(Archive *a)
{
	warn("Can only mount D64 images.\n");
}

void 
VC1541::insertDisc(D64Archive *a)
{
	int i,j;
	uint8_t *dest;
	
	assert(a != NULL);

	clearDisk();
	writeProtection = false;
	
	// For each full track...
	for (i = 1; i <= 2*a->numberOfTracks(); i += 2) {
		// For each sector...
		for (j = 0, dest = data[i-1]; j < a->numberOfSectors(i); j++) {
 			dest += encodeSector(a, i, j, dest);
		}

		length[i-1] = dest - data[i-1];
		debug("Length of track %d: %d bytes\n", i, length[i-1]); 
	}

	for (i = 1; i <= 84; i++) {
		assert(length[i-1] <= 7928);
	}

	getListener()->driveDiscAction(true);
}

void 
VC1541::ejectDisc()
{
	// release write protection
	writeProtection = false;
	
	// zero out data
	clearDisk();
	
	getListener()->driveDiscAction(false);
}

#if 0
void 
VC1541::dumpState()
{
	char buf[128];

	sprintf(buf, "VC 1541 drive: VIA 1 at %p VIA 2 at %p\n", via1, via2);
	getListener()->logAction(strdup(buf));
	via1->dumpState();
	via2->dumpState();
	sprintf(buf, "Engine: %s red LED: %s\n",
		via2->engineRunning() ? "on" : "off",
		via2->redLEDshining() ? "on" : "off");
	getListener()->logAction(strdup(buf));

}
#endif
void 
VC1541::dumpState()
{
	debug("head timer: %d %d %d %d V-enable: %d track: %d, offset: %d (%d -> %d) ff_bytes: %d\n", 
	byteReadyTimer, readFlag, writeFlag, syncFlag, via2->overflowEnabled(),
	track, offset, readHead(), readHeadLookAhead(), noOfFFBytes);
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
	debug("G64 image imported successfully (%d bytes total, size = %d)\n", fileProperties.st_size, size);

	// Analyze data
	debug("    Version: %2X\n", (int)filedata[0x08]);
	debug("    Number of tracks: %d\n", (int)filedata[0x09]);
	debug("    Size of track: %d\n", (int)((filedata[0x0B] << 8) | filedata[0x0A]));
	for (track = 0; track < 84; track++) {
		uint32_t offset;
		uint16_t track_length, i;
		
		offset  = (filedata[4*track+0x0F] << 24) | (filedata[4*track+0x0E] << 16) | (filedata[4*track+0x0D] << 8) | filedata[4*track + 0x0C];
		track_length = (filedata[offset + 1] << 8) | filedata[offset];

		if (offset != 0) {
			debug("    Track %2.1f: Offset: %8X Length: %04X\n", (track + 2) / 2.0, offset, track_length);
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


