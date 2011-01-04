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

D64Archive *D64Archive::archiveFromFile(const char *filename)
{
	D64Archive *archive;
	
	archive = new D64Archive();	
	if (!archive->readFromFile(filename)) {
		delete archive;
		archive = NULL;
	}
	return archive;
}

const char *D64Archive::getTypeOfContainer() 
{
	return "D64";
}

void D64Archive::cleanup()
{
}

bool D64Archive::fileIsValid(const char *filename)
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

bool D64Archive::readDataFromFile(FILE *file, struct stat fileProperties)
{
	unsigned track = 0;
	int numberOfErrors = 0;
		
	switch (fileProperties.st_size)
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
//			numTracks = 42; ???
			break;
		case 206114:
//			numTracks = 42; ???
//			numberOfErrors = ???
			break;
	}
	
	// Read each track
	for(track = 1; track <= numTracks; track++) {
		
		void *ptr = &data[D64Map[track].offset];
		int sectors = D64Map[track].numberOfSectors;
		int n = 0;
		n = fread(ptr, 256, sectors, file);
		assert (n == sectors);
	}

	// Read errors
	if (numberOfErrors > 0) {
		int n = 0;
		n = fread(errors, 1, numberOfErrors, file);
		assert(n == numberOfErrors);
	}
		
	fprintf(stderr, "D64 Container imported successfully (%d bytes total, tracks = %d)\n", (int)fileProperties.st_size, numTracks);
	return true;
}

int 
D64Archive::jumpToNextSector(int pos)
{ 
	int nTrack, nSector, nOffset;
	
	nTrack = nextTrack(pos);
	nSector = nextSector(pos);
	nOffset = offset(nTrack, nSector);
		
	return nOffset;
	
}

void D64Archive::dumpDir()
{	
}

int D64Archive::findDirectoryEntry(int itemNr)
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
	
const char *D64Archive::getName()
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

int D64Archive::getNumberOfItems()
{
	int i = 0;
	
	while (findDirectoryEntry(i) != -1) {
		i++;
	}
	return i;
}

const char *D64Archive::getNameOfItem(int n)
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

const char *D64Archive::getTypeOfItem(int n)
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

	
int D64Archive::getSizeOfItem(int n)
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

uint16_t D64Archive::getDestinationAddrOfItem(int n)
{
	int pos;
	int track;
	int sector;
	uint16_t result;
	
	fprintf(stderr, "getDestinationAddrOfItem:%d\n", n);

	// Search for beginning of file data
	pos = findDirectoryEntry(n);
	track = data[pos+0x03];
	sector = data[pos+0x04];
	if ((pos = offset(track, sector)) < 0)
		return 0;

	result = data[pos+2] + (data[pos+3] << 8); 
	fprintf(stderr, "Destination address of item %d is %X\n", n, result);
	return result;
}

void D64Archive::selectItem(int item)
{
	fp = -1;
	
	fprintf(stderr, "selectItem:%d\n", item);

	// check, if item exists
	if (item >= getNumberOfItems())
		return;
		
	// find directory entry
	if ((fp = findDirectoryEntry(item)) < 0)
		return;

	// find first data sector
	if ((fp = offset(data[fp+0x03], data[fp+0x04])) < 0)
		return;
		
	// skip t/s sequence
	fp += 2;
	
	// skip destination address
	fp += 2;

	// We now reached the first real data byte :)
	fprintf(stderr, "Item selected (%d,%d)\n", data[fp+0x03], data[fp+0x04]);
}

int D64Archive::getByte()
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
	return result;
}

uint8_t *D64Archive::findSector(unsigned halftrack, unsigned sector) 
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
