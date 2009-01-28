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

D64Archive::D64Archive()
{
	path = NULL;
	numTracks = 0;
	fp = -1;
}

D64Archive::~D64Archive()
{
	if (path) free(path);
	// if (data) free(data);
}

void D64Archive::eject()
{
	if (path) free(path);
	//if (data) free(data);
	path = NULL;
	//data = NULL;
	//size = 0;
}

bool D64Archive::fileIsValid(const char *filename)
{
	assert (filename != NULL);

	if (!checkFileSuffix(filename, ".D64") && !checkFileSuffix(filename, ".d64"))
		return false;

	if (!checkFileSize(filename, 174848, 174848))
		return false;

	// Unfortunaltely, D64 containers do not contain magic bytes,
	// so we can't check anything further here

	return true;
}

bool D64Archive::loadFile(const char *filename)
{
	struct stat fileProperties;
	FILE *file;
	int i;
	
	assert (filename != NULL);

	// Free old data
	eject();
	
	// Check file type
	if (!fileIsValid(filename)) 
		return false;
	
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
		
	// Read data (only 35 track images are supported yet)
	numTracks = 35;
	for (i = 0; i < 174848; i++) {
		data[i] = (uint8_t)fgetc(file);
	}
	fclose(file);

	path = strdup(filename);
			
	printf("D64 container imported successfully (%d items)\n", getNumberOfItems());
	
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
	int i = 0;
	int track = 18;
	int sector = 1;
	int pos = offset(track, sector);
	bool last_sector = (data[pos] == 0x00);

	while (1) {
		// Are we currently looking at a valid directory entry?
		if (data[pos+3] == 0x00) {
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
	
	// Tracks 1..17 contain 21 sectors
	if (track <= 17) return 21;
	// Tracks 18..24 contain 19 sectors
	if (track <= 24) return 19;
	// Tracks 25..30 contain 18 sectors
	if (track <= 30) return 18;
	// Tracks 31..35 contain 17 sectors
	if (track <= 35) return 17;
	// Tracks 36..42 contain 17 sectors (not VC1541 standard format)
	if (track <= 42) return 17;
	return 0;
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
	int trackOffset[] = {
		0x00000, 0x01500, 0x02A00, 0x03F00, 0x05400, 0x06900, 0x07E00, 0x09300, 0x0A800, 0x0BD00, 
		0x0D200, 0x0E700, 0x0FC00, 0x11100, 0x12600, 0x13B00, 0x15000, 0x16500, 0x17800, 0x18B00,
		0x19E00, 0x1B100, 0x1C400, 0x1D700, 0x1EA00, 0x1FC00, 0x20E00, 0x22000, 0x23200, 0x24400, 
		0x25600, 0x26700, 0x27800, 0x28900, 0x29A00, 0x2AB00, 0x2BC00, 0x2CD00, 0x2DE00, 0x2EF00
	};

	if (track < 1 || track > 35) {
		fprintf(stderr, "D64Container: Requested track %d does not exists\n", track);
		return -1;
	}
	if (sector < 0 || sector > 20) {
		fprintf(stderr, "D64Container: Requested sector %d does not exists\n", track);
		return -1;
	}
		
	return trackOffset[track-1] + (256 * sector); 
}
	
char 
*D64Archive::getPath() 
{
	return path;
}

char *D64Archive::getName()
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

	while (findDirectoryEntry(i) != -1) i++;
	return i;
}

char *D64Archive::getNameOfItem(int n)
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

char *D64Archive::getTypeOfItem(int n)
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

	fprintf(stderr, "getSizeOfItem:%d\n", n);

	// jump to beginning of the n-th directory entry
	pos = findDirectoryEntry(n);
	if (pos < 0) return 0;
	
	// jump to the first data sector
	if ((pos = offset(data[pos+0x03], data[pos+0x04])) < 0)
		return -1;
	
	while (data[pos] != 0x00) {
		size += 254;
		if ((pos = jumpToNextSector(pos)) < 0)
			return -1;
	}
	
	size += data[pos+1]-1;
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
	printf("Destination address of item %d is %X\n", n, result);
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
	printf("Item selected (%d,%d)\n", data[fp+0x03], data[fp+0x04]);
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
	
	fprintf(stderr,"findSector:%d %d\n", halftrack, sector);

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
