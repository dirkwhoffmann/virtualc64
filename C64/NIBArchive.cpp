/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, org
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

#include "Disk525.h"
#include "NIBArchive.h"
#include "stdio.h"

NIBArchive::NIBArchive()
{
	data = NULL;
	dealloc();

    for (unsigned i = 0; i < 85; i++) {
        halftrackToItem[i] = -1;
        length[i] = 0;
        memset(halftrack[i], 0, sizeof(halftrack[i]));
    }
}

NIBArchive::~NIBArchive()
{
	dealloc();
}

bool 
NIBArchive::isNIBFile(const char *filename)
{
    /* "MNIB-1541-RAW" */
	int magic_bytes[] = { 0x4d, 0x4e, 0x49, 0x42, 0x2d, 0x31, 0x35, 0x34, 0x31, 0x2d, 0x52, 0x41, 0x57, EOF };
	
	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".NIB") && !checkFileSuffix(filename, ".nib"))
		return false;
	
    // File size = 0x100 (header) + no_of_tracks * 0x2000
    if (getSizeOfFile(filename) % 0x2000 != 0x100)
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
	return true;
}

NIBArchive *
NIBArchive::archiveFromNIBFile(const char *filename)
{
	NIBArchive *archive;
	
	fprintf(stderr, "Loading NIB archive from NIB file...\n");
	archive = new NIBArchive();
	if (!archive->readFromFile(filename) || !archive->scan()) {
        fprintf(stderr, "Failed to load archive\n");
		delete archive;
		archive = NULL;
	}
	
    return archive;
}

bool
NIBArchive::scan()
{
    unsigned i, item;
    uint8_t bits_raw[8 * 0x2000];       // Bit stream as stores in the NIB file
    uint8_t bits_aligned[8 * 0x2000];   // Bit stream with shortened SYNC sequences
    int index[8 * 0x2000];              // Index mapping from bits_align to bits_raw
    int length;                         // Number of bits in shortened sequence
    
    // Setup halftrackToItem array
    for (i = 0x10, item = 0; i < 0x100; i += 2, item++) {
        if (data[i] >= 2 && data[i] <= 83) {
            halftrackToItem[data[i] + 1] = item; // NIB files start halftrack counting at 0
            fprintf(stderr, "[%d] ", data[i] + 1);
        }
    }
    fprintf(stderr, "\n");

    for (Halftrack ht = 1; ht <= 84; ht++) {
        
        // Is halftrack stored in archive?
        int item = halftrackToItem[ht];
        if (item == -1)
            continue;

        // fprintf(stderr, "Scanning halftrack %d (item %d)\n", ht, item);

        // Extract bits of halftrack and shorten SYNCs
        // i:          0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
        // raw[i]:     0  0  1  0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  0  1  0  1  1
        // aligned[i]: 0  0  1  0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  0  1  0  1  1  ?  ?
        // index:      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 20 21 22 23 24 25 ?? ??
        // length:     24
        unsigned startOfTrack = 0x100 + item * 0x2000;
        int idx, onecnt; uint8_t bit;
        for (length = idx = onecnt = 0; idx < 8 * 0x2000; idx++) {
            bit = (data[startOfTrack + (idx / 8)] << (idx % 8)) & 0x80 ? 1 : 0;
            onecnt = bit ? onecnt + 1 : 0;
            bits_raw[idx] = bit;
            if (onecnt <= 10) {
                bits_aligned[length] = bit;
                index[length] = idx;
                length++;
            }
        }
        // fprintf(stderr, "Stripped %d SYNC bits\n", 8 * 0x2000 - length);
        
        /*
        for (unsigned k = 135; k < 190; k++) {
             printf("%2d ", bits_raw[k]);
        }
        printf("\n");
        for (unsigned k = 135; k < 190; k++) {
            printf("%2d ", k - 135);
        }
        printf("\n");
        for (unsigned k = 135; k < 190; k++) {
            printf("%2d ", bits_aligned[k]);
        }
        printf("\n");
        for (unsigned k = 135; k < 190; k++) {
            printf("%2d ", index[k] - 135);
        }
        printf("\n");
        */
        
        // Find loop
        int startBit = 0, stopBit = 0;
        if (!scanTrack(bits_aligned, length, &startBit, &stopBit)) {
            printf("Halftrack: %d (item %d) LOOP DETECTION FAILED.\n", ht, item);
            continue;
        }
        
        // Check loop length
        unsigned tracklength = index[stopBit] - index[startBit];
        if (tracklength < 8 * MIN_TRACK_LENGTH) {
            printf("Halftrack: %d (item %d) TRACK TOO SHORT (%x)\n", ht, item, tracklength);
            continue;
        }
        if (tracklength > 8 * MAX_TRACK_LENGTH) {
            printf("Halftrack: %d (item %d) TRACK TOO LARGE (%x)\n", ht, item, tracklength);
            continue;
        }
            
        // Proper track length found
        startBit = index[startBit];
        stopBit = index[stopBit];
        
        printf("Halftrack: %d (item %d) Start: %d Stop: %d Length: %d (%x bytes) Stripped: %d\n",
                ht, item, startBit, stopBit, tracklength, tracklength / 8, 8 * 0x2000 - length);
    }
    return true;
}

bool
NIBArchive::scanTrack(uint8_t *bits, int length, int *startBit, int *stopBit)
{
    // Search for loop
    int pos1, pos2;
    pos1 = 0;
    for (pos2 = pos1 + 1; pos2 < length - 1024 /* minimum match size */; pos2++) {
        if (memcmp(bits+pos1, bits+pos2, length-pos2) == 0) {
            
            *startBit = pos1;
            *stopBit = pos2;
            /*
            printf("Match found at (%d,%d)\n", pos1, pos2);
            for (unsigned k = 0; k < 30; k++)
                printf("%d", bits[pos1+k]);
            printf("\n");
            for (unsigned k = 0; k < 30; k++)
                printf("%d", bits[pos2+k]);
            printf("\n");
             */
            return true;
        }
    }
    
    return false;
}

void NIBArchive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
    // fp_eof = -1;
}

bool 
NIBArchive::fileIsValid(const char *filename)
{
	return NIBArchive::isNIBFile(filename);
}

bool 
NIBArchive::readFromBuffer(const uint8_t *buffer, unsigned length)
{	
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;

	memcpy(data, buffer, length);
	size = length;

	return true;
}

unsigned
NIBArchive::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

const char *
NIBArchive::getName()
{
    return "NIB archive";
}

int 
NIBArchive::getNumberOfItems()
{
    return 84;
}

#if 0
int
NIBArchive::getStartOfItem(int n)
{
    if (n < 0 || n >= 84)
        return -1;

    return halftrack[n + 1];
}
#endif

int
NIBArchive::getSizeOfItem(int n)
{
    if (n < 0 || n >= 84)
        return 0;

    return length[n + 1];
}

const char *
NIBArchive::getNameOfItem(int n)
{
    if (n < 0 || n >= 84)
        return "";
    
    if (n % 2 == 0) {
        sprintf(name, "Track %d", (n / 2) + 1);
    } else {
        sprintf(name, "Track %d.5", (n / 2) + 1);
    }
    
	return name;
}

const char *
NIBArchive::getTypeOfItem(int n)
{
    return ""; // (n % 2 == 0) ? "Full" : "Half";
}

void 
NIBArchive::selectItem(int n)
{
    if (n < 0 || n >= 84)
        return;
    
    fp = (halftrack[n] - halftrack[0]);
}

int
NIBArchive::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp++];
	
	// check for end of file
    if (fp % 0x2000 == 0x100)
		fp = -1;

	return result;
}
