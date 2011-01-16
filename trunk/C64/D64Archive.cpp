/*
 * (C) 2007 Dirk W. Hoffmann. All rights reserved.
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

typedef struct D64TrackInfo {
	int numberOfSectors;
	int sectorsIn;
	int offset;
} D64TrackInfo;

static const D64TrackInfo D64Map[] =
{
	{ 0,  0,   0 }, // Ignore - track starts at 1, sector starts at 0
	{ 21, 0,   0x00000 },
	{ 21, 21,  0x01500 },
	{ 21, 42,  0x02A00 },
 	{ 21, 63,  0x03F00 },
	{ 21, 84,  0x05400 },
	{ 21, 105, 0x06900 },
	{ 21, 126, 0x07E00 },
	{ 21, 147, 0x09300 },
	{ 21, 168, 0x0A800 },
	{ 21, 189, 0x0BD00 }, 
	{ 21, 210, 0x0D200 },
	{ 21, 231, 0x0E700 },
	{ 21, 252, 0x0FC00 },
	{ 21, 273, 0x11100 },
	{ 21, 294, 0x12600 },
	{ 21, 315, 0x13B00 },
	{ 21, 336, 0x15000 },
	{ 19, 357, 0x16500 }, // Track 18, Directory
	{ 19, 376, 0x17800 },
	{ 19, 395, 0x18B00 },
	{ 19, 414, 0x19E00 },
	{ 19, 433, 0x1B100 },
	{ 19, 452, 0x1C400 },
	{ 19, 471, 0x1D700 },
	{ 18, 490, 0x1EA00 },
	{ 18, 508, 0x1FC00 },
	{ 18, 526, 0x20E00 },
	{ 18, 544, 0x22000 },
	{ 18, 562, 0x23200 },
	{ 18, 580, 0x24400 },
	{ 17, 598, 0x25600 },
	{ 17, 615, 0x26700 },
	{ 17, 632, 0x27800 },
	{ 17, 649, 0x28900 },
	{ 17, 666, 0x29A00 },
	{ 17, 683, 0x2AB00 },
	{ 17, 700, 0x2BC00 },
	{ 17, 717, 0x2CD00 },
	{ 17, 734, 0x2DE00 },
	{ 17, 751, 0x2EF00 },
	// Unusual, tracks 41 & 42
	{ 17, 768, 0x30000 },
	{ 17, 785, 0x31100 }
};

D64Archive::D64Archive()
{
	cleanup();
}

D64Archive::~D64Archive()
{
	cleanup();
}

bool 
D64Archive::isD64File(const char *filename)
{
	bool fileOK = false;
	
	assert (filename != NULL);
	
	if (!checkFileSuffix(filename, ".D64") && !checkFileSuffix(filename, ".d64"))
		return false;
	
	fileOK = checkFileSize(filename, 174848, 174848)
	|| checkFileSize(filename, 175531, 175531)
	|| checkFileSize(filename, 196608, 196608)
	|| checkFileSize(filename, 197376, 197376)
	|| checkFileSize(filename, 205312, 205312)
	|| checkFileSize(filename, 206114, 206114);
	
	// Unfortunaltely, D64 containers do not contain magic bytes,
	// so we can't check anything further here
	
	return fileOK;
}

D64Archive *
D64Archive::archiveFromFile(const char *filename)
{
	D64Archive *archive;
		
	fprintf(stderr, "Loading D64 archive from D64 file...\n");
	archive = new D64Archive();	
	if (!archive->readFromFile(filename)) {
		delete archive;
		archive = NULL;
	}
	fprintf(stderr, "D64 archive loaded successfully.\n");
	
	return archive;
}

D64Archive *
D64Archive::archiveFromArbitraryFile(const char *filename)
{
	if (D64Archive::isD64File(filename)) {
		return D64Archive::archiveFromFile(filename);
	}

	// FOR DEBUGGING (CONVERT D64 TO D64)
	if (D64Archive::isD64File(filename)) {
		fprintf(stderr, "Creating D64 archive from D64 file...\n");
		return D64Archive::archiveFromOtherArchive(D64Archive::archiveFromFile(filename));
	}
	
	if (T64Archive::isT64File(filename)) {
		fprintf(stderr, "Creating D64 archive from T64 file...\n");
		return D64Archive::archiveFromOtherArchive(T64Archive::archiveFromFile(filename));
	}

	if (PRGArchive::isPRGFile(filename)) {
		fprintf(stderr, "Creating D64 archive from PRG file...\n");
		return D64Archive::archiveFromOtherArchive(PRGArchive::archiveFromFile(filename));
	}

	if (P00Archive::isP00File(filename)) {
		fprintf(stderr, "Creating D64 archive from P00 file...\n");
		return D64Archive::archiveFromOtherArchive(P00Archive::archiveFromFile(filename));
	}
	
	return NULL;
}

D64Archive *
D64Archive::archiveFromOtherArchive(Archive *otherArchive)
{
	if (otherArchive == NULL)
		return NULL;
	
	fprintf(stderr, "Creating D64 archive from other archive...\n");
	D64Archive *archive = new D64Archive();
	if (!archive->writeArchive(otherArchive)) {
		delete archive;
		archive = NULL;
	}
	
	return archive;
}


const char *
D64Archive::getTypeOfContainer() 
{
	return "D64";
}

void 
D64Archive::cleanup()
{
}

bool 
D64Archive::fileIsValid(const char *filename)
{
	return D64Archive::isD64File(filename);
}

bool 
D64Archive::readFromBuffer(const void *buffer, unsigned length)
{
	unsigned track = 0;
	int numberOfErrors = 0;
	
	switch (length)
	{
		case 174848:
			// 35 tracks, no errors
			numTracks = 35;
			break;
		case 175531:
			// 35 tracks, 683 error bytes
			numTracks = 35;
			numberOfErrors = 683;
			break;
		case 196608:
			// 40 tracks, no errors
			numTracks = 40;
			break;
		case 197376:
			// 40 tracks, 768 error bytes
			numTracks = 40;
			numberOfErrors = 768;
			break;
		case 205312:
			// numTracks = 42; (???)
			// break;
		case 206114:
			// numTracks = 42; (???)
			// numberOfErrors = (???)
			// break;
		default:
			return false;
	}
	
	// Read tracks
	uint8_t *source = (uint8_t *)buffer;
	for(track = 1; track <= numTracks; track++) {
		
		uint8_t *destination = &data[D64Map[track].offset];
		int sectors = D64Map[track].numberOfSectors;
		memcpy(destination, source, 256 * sectors);
		source += 256 * sectors;
	}
	
	// Read errors
	if (numberOfErrors > 0) {
		memcpy(errors, source, numberOfErrors);
	}

	return true;	
}

int 
D64Archive::jumpToNextSector(int pos)
{ 
	int nTrack, nSector, nOffset;
	
	nTrack = nextTrack(pos);
	nSector = nextSector(pos);
	nOffset = offset(nTrack, nSector);

	// Warn, if we got an invalid track/sector combination
	if (nTrack > (int)numTracks || nSector > D64Map[nTrack].numberOfSectors) {
		fprintf(stderr, "WARNING: Sector links to %d/%d which is an invalid track/sector combination\n", nTrack, nSector);
		return pos;
	}

	// fprintf(stderr, "Jumping to %d/%d \n", nTrack, nSector);
	return nOffset;
}

void 
D64Archive::dumpDir()
{	
}

int 
D64Archive::findDirectoryEntry(int itemNr)
{
	// The Block Allocation Map (BAM) is stored on track 18 - sector 0; 
	// the directory starts at track 18 - sector 1.
	int i = 0;
	int track = 18;
	int sector = 1;
	int pos = offset(track, sector);
	bool last_sector = (data[pos] == 0x00);
	const char emptyEntry[] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
	
	while (1) {
		// Are we currently looking at a valid directory entry?
		if (memcmp(&data[pos], emptyEntry, 32)==0) {
			return -1;
		}
	
		// Check, if we already reached the entry we're looking for
		if (i == itemNr) {
			return pos;
		}
		
		// Jump to the next item
		i++;
		if (i % 8 == 0) {
			// Jump to the next sector
			if (last_sector) {
				return -1;
			}	
			// debug("Jump to next sector...\n");
			pos = jumpToNextSector(pos);
			last_sector = (data[pos] == 0x00);
		} else {
			// Jump inside sector
			pos += 0x20;
		}
	}
}
	
bool 
D64Archive::isEndOfFile(int offset)
{
	int track = nextTrack(offset);
	int sector = nextSector(offset);
	
	if (track == 0x00) {
		if (offset % 256 == sector)
			return true;
	}
	return false;
}

unsigned 
D64Archive::numberOfSectors(unsigned halftrack)
{
	assert(halftrack >= 1 && halftrack <= 84);
	
	// convert halftrack number to track number 
	unsigned track = (halftrack + 1) / 2;
	
	return D64Map[track].numberOfSectors;
}

unsigned 
D64Archive::numberOfTracks()
{ 
	assert(numTracks == 35 || numTracks == 40 || numTracks == 42); 
	return numTracks; 
}

int 
D64Archive::offset(int track, int sector)
{	
	return D64Map[track].offset + (sector * 256);
}
	
const char *
D64Archive::getName()
{
	int i, pos = offset(18, 0) + 0x90;
	
	for (i = 0; i < 255; i++) {
		if (data[pos+i] == 0xA0) 
			break;
		name[i] = data[pos+i];
	}
	name[i] = 0x00;
	return name;
}

int 
D64Archive::getNumberOfItems()
{
	int i = 0;
	
	while (findDirectoryEntry(i) != -1) {
		i++;
	}
	return i;
}

const char *
D64Archive::getNameOfItem(int n)
{
	int i, pos = findDirectoryEntry(n);

	if (pos < 0) return NULL;
	pos += 0x05; // filename begins here
	for (i = 0; i < 16; i++) {
		if (data[pos+i] == 0xA0)
			break;
		name[i] = data[pos+i];
	}
	name[i] = 0x00;
	return name;
}

const char *
D64Archive::getTypeOfItem(int n)
{
	int pos = findDirectoryEntry(n) + 0x02;
	int type = data[pos] & 0x07;
	
	switch (type) {
		case 0x00: 
			return "DEL";
		case 0x01:
			return "SEQ";
		case 0x02:
			return "PRG";
		case 0x03:
			return "USR";
		case 0x04:
			return "REL";
		case 0x05:
			return "101(?)";
		case 0x06:
			return "110(?)";
		case 0x07:
			return "111(?)";
		default:
			return "(?)";
	}
}

	
int 
D64Archive::getSizeOfItem(int n)
{
	int size = 0;
	int pos;

	// fprintf(stderr, "getSizeOfItem:%d\n", n);

	// jump to beginning of the n-th directory entry
	pos = findDirectoryEntry(n);
	if (pos < 0) return 0;

	// file size is at 1E,1F ($1E+$1F*256)
	size = data[pos+30] + data[pos+31] * 256;

	return size;
}

uint16_t 
D64Archive::getDestinationAddrOfItem(int n)
{
	int pos;
	int track;
	int sector;
	uint16_t result;
	
	// Search for beginning of file data
	pos = findDirectoryEntry(n);
	track = data[pos+0x03];
	sector = data[pos+0x04];
	if ((pos = offset(track, sector)) < 0)
		return 0;

	result = data[pos+2] + (data[pos+3] << 8); 
	
	fprintf(stderr, "Loading item %d to address %04X\n", n, result);
	return result;
}

void 
D64Archive::selectItem(int item)
{
	fp = -1;
	
	fprintf(stderr, "selectItem:%d\n", item);

	// check, if item exists
	if (item >= getNumberOfItems())
		return;
		
	// find directory entry
	if ((fp = findDirectoryEntry(item)) < 0)
		return;

	fprintf(stderr, "First data sector: %02X, %02X", data[fp+0x03], data[fp+0x04]);

	// find first data sector
	if ((fp = offset(data[fp+0x03], data[fp+0x04])) < 0)
		return;
	
	
	// Skip t/s sequence
	fp += 2;
	
	// Skip destination address
	fp += 2;

	// We finally reached the first real data byte :-)
	fprintf(stderr, "Item selected (%d,%d)\n", data[fp+0x03], data[fp+0x04]);
}

int 
D64Archive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp];
	
	// check for end of file
	if (isEndOfFile(fp)) {
		fp = -1;	
	} else {
		// advance file pointer
		if (isLastByteOfSector(fp)) {
			fp = jumpToNextSector(fp)+2;
		} else {
			fp++;
		}
	}
	
	// fprintf(stderr, "%02X ", result);
	return result;
}

uint8_t *
D64Archive::findSector(unsigned halftrack, unsigned sector) 
{ 
	assert(1 <= halftrack && halftrack <= 84);
	assert(sector < numberOfSectors(halftrack));
	
	// fprintf(stderr,"findSector:%d %d\n", halftrack, sector);

	// Halftrack mapping: 1 -> 1.0, 2 -> 1.5, 3 -> 2.0, 4 -> 2.5, etc.
	if (halftrack % 2 == 0) {
		fprintf(stderr, "WARNING: Trying to access half track in D64 image. Ignoring request.\n");
		return NULL;
	}
	
	int pos;
	if ((pos = offset((halftrack+1) / 2, sector)) < 0)
		return NULL;
	
	return &data[pos];
}

void 
D64Archive::dumpSector(int track, int sector)
{
	int pos = offset(track, sector);
	
	printf("Sector %d/%d\n", track, sector);
	for (int i = 0; i < 256; i++) {
		printf("%02X ", data[pos++]);
	}
}

// 
// Convert arbitrary archives to D64 format
// 

void 
D64Archive::clear() 
{
	fprintf(stderr, "Clearing tracks and sectors\n");
	
	numTracks = 35;
	memset(data, 0x00, sizeof(data));
	memset(errors, 0x00, sizeof(errors));

#if 0
	// Initialize sector headers
	uint8_t track = 1, sector = 0;
	do {
		//fprintf(stderr, "Writing header for %d/%d\n", track, sector);
		// data[offset(track, sector) + 1] = 0xFF;
	} while (nextTrackAndSector(track, sector, &track, &sector));			 
#endif
}

void 
D64Archive::markSectorAsUsed(uint8_t track, uint8_t sector)
{
	// For each track and sector, there exists a single bit in the BAM. 1 = used, 0 = unused
	
	// fprintf(stderr,"Marking track %d and sector %d as used\n", track, sector);
	
	// First byte of BAM
	int bam = offset(18,0);
	
	// Select byte group correspondig to track
	bam += (4 * track); 

	// Select byte carrying the information for sector
	int offset = 1 + (sector >> 3);
	assert(offset >= 1 && offset <= 3);
	
	// Select bit for this sector
	uint8_t bitmask = 0x01 << (sector & 0x07);

	if (data[bam+offset] & bitmask) {
		// Clear bit
		data[bam + offset] &= ~bitmask; 

		// Descrease number of free sectors
		assert(data[bam] > 0);
		data[bam]--;
	}
}

//! Return the next physical track and sector
bool 
D64Archive::nextTrackAndSector(uint8_t track, uint8_t sector, uint8_t *nextTrack, uint8_t *nextSector, bool skipDirectoryTrack) 
{
	unsigned highestSectorNumberInThisTrack = D64Map[track].numberOfSectors - 1;
		
	// PROBLEM?: A REAL VC1541 DISK USUALLY SHOWS AN INTERLEAVE OF 10
	
	if (sector < highestSectorNumberInThisTrack) {
		sector++;
	} else if (track < numTracks) {
		track++;
		sector = 0;
	} else {
		return false; // there is no next sector
	}

	if (track == 18 && skipDirectoryTrack) {
		track = 19;
		sector = 0;
	}

	*nextTrack = track;
	*nextSector = sector;
	
	return true;
}

bool 
D64Archive::writeToFile(const char *filename)
{
	FILE *file;
	
	if (!(file = fopen(filename, "w")))
		return false;
	
	// write 683 sectors
	for (int i = 0; i < 683; i++) {
		for (int j = 0; j < 256; j++){
			fputc(data[i*256+j], file);
		}		
	}
	
	fclose(file);
	return true;
}

void
D64Archive::writeBAM(const char *name)
{
	int pos;

	fprintf(stderr, "Writing BAM\n");
	
	// 00/01: Track/Sector location of the first directory sector (should be 18/1)
	markSectorAsUsed(18, 0);
	pos = offset(18,0);
	data[pos++] = 18; 
	data[pos++] = 1;
	
	// 02: Disk DOS version type (see note below)
	data[pos++] = 0x41; // "A"
	
	// 03: Unused
	pos++;
	
	// 04-8F: BAM entries for each track, in groups  of  four  bytes
	for (unsigned k = 1; k <= 35; k++) {
		if (k == 18) {
			data[pos++] = 0; // no free sectors on directory track
			data[pos++] = 0x00;
			data[pos++] = 0x00;
			data[pos++] = 0x00;
		} else {
			int sectors = D64Map[k].numberOfSectors;
			data[pos++] = sectors; // Number of free sectors on this track
			data[pos++] = 0xFF;    // Occupation bitmap: 1 = sector is free
			data[pos++] = 0xFF;	   
			if (sectors == 21) data[pos++] = 0x1F;
			else if (sectors == 19) data[pos++] = 0x07;
			else if (sectors == 18) data[pos++] = 0x03;
			else if (sectors == 17) data[pos++] = 0x01;
			else assert(0);		
		}
	}
	assert(pos == offset(18,0) + 0x90);

	// 90-9F: Disk Name (padded with $A0)
	size_t len = strlen(name);
	for (unsigned k = 0; k < 16; k++)
		data[pos++] = (len > k) ? name[k] : 0xA0;
	
	assert(pos == offset(18,0) + 0xA0);
	
	// A0-A1: Filled with $A0
	data[pos++] = 0xA0;
	data[pos++] = 0xA0;
	
	// A2-A3: Disk ID
	data[pos++] = 0x56;
	data[pos++] = 0x54;
	
    // A4: Usually $A0
	data[pos++] = 0xA0;
	
	// A5-A6: DOS type
	data[pos++] = 0x32; // "2"
	data[pos++] = 0x41; // "A"
	
	// A7-AA: Filled with $A0
	data[pos++] = 0xA0;
	data[pos++] = 0xA0;
	data[pos++] = 0xA0;
	data[pos++] = 0xA0;
	
	assert(pos == offset(18,0) + 0xAB);	
}

bool
D64Archive::writeDirectoryEntry(unsigned nr, const char *name, uint8_t startTrack, uint8_t startSector, unsigned filesize)
{
	int pos;
	
	if (nr >= 144) {
		fprintf(stderr, "Directories with more than 144 entries are not supported\n");
		return false;
	}

	fprintf(stderr, "Writing directory entry number %d\n", nr);

	// determine sector and relative sector position
	uint8_t sector = 1 + (nr / 8);
	uint8_t rel = (nr % 8) * 0x20;
	
	markSectorAsUsed(18, sector);

	// link to this sector if it is not the first
	if (sector >= 2) {
		pos = offset(18, sector - 1);
		data[pos++] = 18;
		data[pos] = sector;
	}

	pos = offset(18, sector) + rel;
	
	// 00-01: Next directory sector (item 0) or 00 00 (other items)
	if (nr == 0) {
		pos++; // don't modify
		pos++; // don't modify
	} else {
		data[pos++] = 0x00;
		data[pos++] = 0x00;
	}
		
	// 02: File type (0x82 = PRG)
	data[pos++] = 0x82;
	
	// 03-04: Track/sector location of first sector of file
	data[pos++] = startTrack;
	data[pos++] = startSector;
	
	// 05-14: 16 character filename (in PETASCII, padded with $A0)
	size_t len = strlen(name);
	for (unsigned k = 0; k < 16; k++)
		data[pos++] = (len > k) ? name[k] : 0xA0;
	
	assert(pos == offset(18, sector) + ((int)nr * 0x20) + 0x15);
	
	// 1E-1F: File size in sectors, low/high byte order
	pos = offset(18, sector) + rel + 0x1E;
	filesize += 2; // Each file stores 2 additional bytes containing the load address
	uint16_t fileSizeInSectors = (filesize % 254 == 0) ? filesize / 254 : filesize / 254 + 1; 
	data[pos++] = LO_BYTE(fileSizeInSectors);
	data[pos++] = HI_BYTE(fileSizeInSectors);

	return true;
}

bool 
D64Archive::writeByteToSector(uint8_t byte, uint8_t *t, uint8_t *s)
{
	uint8_t track = *t;
	uint8_t sector = *s;
	
	int pos = offset(track, sector);
	uint8_t positionOfLastDataByte = data[pos + 1];
	
	if (positionOfLastDataByte == 0xFF) {
		printf("%d/%d is full. ", track, sector);
		// No rool in this sector, proceed to next one
		if (!nextTrackAndSector(track, sector, &track, &sector, true /* skip directory track */)) {
			// Sorry, disk is full
			return false;
		}
		printf("Switching to %d/%d\n", track, sector);
		// link previous sector with the new one 
		data[pos++] = track;
		data[pos] = sector;
		pos = offset(track, sector);
		positionOfLastDataByte = 0;
	}
	
	// Write byte
	if (positionOfLastDataByte == 0) {
		markSectorAsUsed(track, sector);
		data[pos + 2] = byte;
		data[pos + 1] = 0x02;
	} else {
		positionOfLastDataByte++;
		data[pos + positionOfLastDataByte] = byte;
		data[pos + 1] = positionOfLastDataByte;		
	}
	
	*t = track;
	*s = sector;
	
	return true;
}

bool 
D64Archive::writeArchive(Archive *archive)
{	
	// Current position of data write ptr
	uint8_t track = 1, sector = 0;
		
	// Clear all tracks and sectors
	clear();
	
	// Write BAM
	writeBAM(archive->getName());
			
	// Loop over all entries in archive	
	for (int i = 0; i < archive->getNumberOfItems(); i++) {
		
		fprintf(stderr, "Writing directory entry %d out of %d (%s)\n", i, archive->getNumberOfItems(), archive->getName());		
		writeDirectoryEntry(i, archive->getNameOfItem(i), track, sector, archive->getSizeOfItem(i));
				
		// Every file is preceded with two bytes containing its load address
		writeByteToSector(LO_BYTE(archive->getDestinationAddrOfItem(i)), &track, &sector);
		writeByteToSector(HI_BYTE(archive->getDestinationAddrOfItem(i)), &track, &sector);
						  
		// Write raw data to disk
		int byte;
		archive->selectItem(i);	
		while ((byte = archive->getByte()) != EOF) {						
			writeByteToSector(byte, &track, &sector);
		}
		
		// Item i has been written. Goto next free sector and proceed with the next item
		(void)nextTrackAndSector(track, sector, &track, &sector, true /* skip directory track */);
	}

	// All items have been written to disk
	writeToFile("/Users/hoff/tmp/test.d64");
	
	return true;
}
