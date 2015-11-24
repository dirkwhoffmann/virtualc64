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

NIBArchive::NIBArchive()
{
	data = NULL;
	dealloc();

    for (unsigned i = 0; i < 85; i++) {
        halftrackToItem[i] = -1;
        startOfHalftrack[i] = 0;
        sizeOfHalftrack[i] = 0;
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
    uint8_t bits[2 * 8 * 0x2000];
    int ones[2 * 8 * 0x2000];

    // Setup halftrackToItem array
    for (i = 0x10, item = 0; i < 0x100; i += 2, item++) {
        if (data[i] >= 2 && data[i] <= 83) {
            halftrackToItem[data[i] + 1] = item; // NIB files start halftrack counting at 0
            fprintf(stderr, "[%d] ", data[i] + 1);
        }
    }
    fprintf(stderr, "\n");

    // Determine size of each stored track item
    for (Halftrack ht = 1; ht < 85; ht++) {
    // for (Halftrack ht = 5; ht < 6; ht++) {

        fprintf(stderr, "Scanning halftrack %d\n", ht);

        // Is halftrack stored in archive?
        int item = halftrackToItem[ht];
        if (item == -1) {
            fprintf(stderr, "Halftrack %d is not stored in NIB file\n", ht);
            continue;
        }
        
        // Store offset to halftrack data
        startOfHalftrack[ht] = item * 0x2000 + 0x100;
        
        // Extract bits of halftrack
        unsigned startOfTrack = 0x100 + item * 0x2000;
        fprintf(stderr, "Halftrack %d is item no %d (offset: %04X)\n", ht, item, startOfTrack);
        for (i = 0; i < 8 * 0x2000; i++) {
            unsigned byte = i / 8;
            unsigned bit = i % 8;
            bits[i] = bits[8 * 0x2000 + i] = (data[startOfTrack + byte] << bit) & 0x80 ? 1 : 0;
        }

        // Scan bits for consecutice '1's
        int cnt = 0;
        for (i = 0; i < 8 * 0x2000; i++) {
            ones[i] = cnt;
            cnt = (bits[i] == 0) ? 0 : cnt + 1;
        }
        for (i = 0; i < 8 * 0x2000 - 1; i++) {
            if (ones[i+1] > ones[i])
                ones[i] = 0;
        }
        ones[i] = 0;
        
        // Search for SYNC marks
        for (i = 0; i < 8 * 0x2000; i++) {
            if (ones[i] > 12) {
                // printf("  SYNC mark at index %d (length: %d)\n", i, ones[i]);
                /*
                for (j = 0; j < 120; j++) {
                    printf("%d", bits[i+j]);
                }
                printf("\n");
                for (j = 0; j < 120; j++) {
                    printf("%d", ones[i+j] < 9 ? ones[i+j] : 9);
                }
                printf("\n");
                 */
            }
        }
        
        // Search longest matching sequence
        int match = 0;
        for (i = 129; i < 8 * 0x2000 - 16; i++) {
            for (match = 0; match < 8 * 0x2000 - i && bits[match+128] == bits[i + match]; match++);
            ones[i] = match;
        }
        // Search for largest match
        int largestMatch = 0, matchIndex = 0;
        for (i = 1; i < 8 * 0x2000 - 16; i++) {
            if (ones[i] > largestMatch) {
                largestMatch = ones[i];
                matchIndex = i;
            }
        }
        printf("Largest match at index %d (%d)\n", matchIndex, largestMatch);
    }
    
    return true;
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

int
NIBArchive::getStartOfItem(int n)
{
    if (n < 0 || n >= 84)
        return -1;

    return startOfHalftrack[n + 1];
}

int
NIBArchive::getSizeOfItem(int n)
{
    if (n < 0 || n >= 84)
        return 0;

    return sizeOfHalftrack[n + 1];
}

const char *
NIBArchive::getNameOfItem(int n)
{
    int size = getSizeOfItem(n);
    
    if (n % 2 == 0) {
        sprintf(name, "Track %d%s", (n / 2) + 1, size == 0 ? " (empty)" : "");
    } else {
        sprintf(name, "Track %d.5%s", (n / 2) + 1, size == 0 ? " (empty)" : "");
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
    fp = getStartOfItem(n);
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
