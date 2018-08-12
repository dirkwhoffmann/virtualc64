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

#include "NIBArchive.h"

const uint8_t
NIBArchive::magicBytes[] = { /* "MNIB-1541-RAW" */
    0x4d, 0x4e, 0x49, 0x42, 0x2d, 0x31, 0x35, 0x34, 0x31, 0x2d, 0x52, 0x41, 0x57, 0x00 };

NIBArchive::NIBArchive()
{
    setDescription("NIBArchive");
	data = NULL;
	dealloc();

    for (unsigned i = 0; i < 85; i++) {
        length[i] = 0;
        memset(halftrack[i], 0, sizeof(halftrack[i]));
    }
    selectedtrack = 0;
    fp = -1;
}

NIBArchive *
NIBArchive::makeNIBArchiveWithBuffer(const uint8_t *buffer, size_t length)
{
    NIBArchive *archive = new NIBArchive();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

NIBArchive *
NIBArchive::makeNIBArchiveWithFile(const char *filename)
{
    NIBArchive *archive = new NIBArchive();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

NIBArchive::~NIBArchive()
{
	dealloc();
}

bool
NIBArchive::isNIB(const uint8_t *buffer, size_t length)
{
    // File size = 0x100 (header) + no_of_tracks * 0x2000
    if (length % 0x2000 != 0x100)
        return false;
    
    return checkBufferHeader(buffer, length, magicBytes);
}

bool 
NIBArchive::isNIBFile(const char *filename)
{
 	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".NIB") && !checkFileSuffix(filename, ".nib"))
		return false;
	
    // File size = 0x100 (header) + no_of_tracks * 0x2000
    if (getSizeOfFile(filename) % 0x2000 != 0x100)
		return false;
	
	if (!checkFileHeader(filename, magicBytes))
		return false;
	
	return true;
}

/*
NIBArchive *
NIBArchive::archiveFromNIBFile(const char *filename)
{
	NIBArchive *archive = new NIBArchive();
    
	if (!archive->readFromFile(filename)) {
		delete archive;
        return NULL;
	}

    if (!archive->scan()) {
        delete archive;
        return NULL;
    }

    archive->debug(1, "NIB archive created from file %s.\n", filename);
    return archive;
}
*/

bool
NIBArchive::scan()
{
    uint8_t bits[8 * 0x2000];
    int start, end, gap;
    
    // Iterate through all header entries
    unsigned i, item;
    for (i = 0x10, item = 0; i < 0x100; i += 2, item++) {
        
        // Does item no 'item' exist in NIB file? 
        if (data[i] < 2 || data[i] > 83)
            continue;
        unsigned ht = data[i] + 1;
        
        // Convert byte stream into a bit stream
        unsigned j, startOfTrack = 0x100 + item * 0x2000;
        for (j = 0; j < sizeof(bits); j++) {
            bits[j] = (data[startOfTrack + j/8] << (j%8)) & 0x80 ? 1 : 0;
        }
        
        // Determine track bounds and alignment offset
        if (!scanTrack(ht, bits, &start, &end, &gap))
            continue;
        
        // Copy track data into destination buffer
        printf("Halftrack: %d Start: %d End: %d Length: %x Gap: %x\n",
                   ht, start, end, (end - start) / 8, gap / 8);
        length[ht] = end - start;
        size_t len1 = length[ht] - gap;
        size_t len2 = gap;
        memcpy(halftrack[ht], bits + start + gap, len1);
        memcpy(halftrack[ht] + len1, bits + start, len2);
    }
    
    for (unsigned ht = 1; ht <= 84; ht++) {
        printf("Halftrack %02d: ", ht);
        for (unsigned b = 0; b < 64; b++) printf("%d", halftrack[ht][b]);
        printf(" Length: %d\n", length[ht]);
    }
    
    return true;
}

bool
NIBArchive::scanTrack(unsigned ht, uint8_t *bits, int *start, int *end, int *gap)
{
    // Find loop
    if (!scanForLoop(bits, sizeof(bits), start, end)) {
        printf("Halftrack: %d LOOP DETECTION FAILED.\n", ht);
        return false;
    }
    
    // Find gap (for track alignment)
    return scanForGap(bits + *start, *end - *start, gap);
}

bool
NIBArchive::scanForLoop(uint8_t *bits, int length, int *start, int *end)
{
    uint8_t stripped[8 * 0x2000];  // Bit stream with shortened SYNC sequences
    int index[8 * 0x2000];         // Index mapping from stripped bits to unstripped bits
    int length_stripped;           // Number of bits in shortened sequence

    // Beware that the length of the SYNC sequences may differ in the repeated bit sequence.
    // Therefore, we perform the matching operation with a copy of the original bit stream.
    // The copy is a stripped version where all SYNC sequences are of the same size.
    //
    // The code below creates the following data structures:
    //
    // i:           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
    // bits[i]:     0  0  1  0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  0  1  0  1  1
    // stripped[i]: 0  0  1  0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  0  1  0  1  1  ?  ?
    // index:       0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 20 21 22 23 24 25 ?? ??
    // length:     24

    int idx, onecnt; uint8_t bit;
    for (length_stripped = idx = onecnt = 0; idx < 8 * 0x2000; idx++) {
        
        // Read bit from raw data stream and count consecutive '1's
        bit = bits[idx];
        onecnt = bit ? onecnt + 1 : 0;
        if (onecnt <= 10) {
            stripped[length_stripped] = bit;
            index[length_stripped] = idx;
            length_stripped++;
        }
    }

    // Now we are ready to search for the loop
    
    int pos1, pos2, tracklength;
    for (pos1 = 0, pos2 = 1; pos2 < length_stripped - 1024 /* minimum matching size */; pos2++) {
        if (memcmp(stripped+pos1, stripped+pos2, length_stripped-pos2) == 0) {
            
            *start = index[pos1];
            *end = index[pos2];
            tracklength = *end - *start;
            
            /*
            printf("Match found at (%d,%d)\n", pos1, pos2);
            for (unsigned k = 0; k < 30; k++)
                printf("%d", bits[pos1+k]);
            printf("\n");
            for (unsigned k = 0; k < 30; k++)
                printf("%d", bits[pos2+k]);
            printf("\n");
            */
            
            // Check loop bounds
            if (tracklength < 8 * MIN_TRACK_LENGTH) {
                printf("Warning: Track is too short (%d bits). Discarding.\n", tracklength);
                return false;
            }
            if (tracklength > 8 * MAX_TRACK_LENGTH) {
                printf("Halftrack: Track is too long (%d bits). Discarding.\n", tracklength);
                return false;
            }

            return true;
        }
    }
    
    return false;
}

bool
NIBArchive::scanForGap(uint8_t *bits, int length, int *gap)
{
    uint8_t tmpbuf[2 * 8 * 0x2000];
    int nonsync[2 * 8 * 0x2000];
    int i, onecnt, gapsize;
    
    // Setup double buffer
    assert(2 * length <= sizeof(tmpbuf));
    memcpy(tmpbuf, bits, length);
    memcpy(tmpbuf + length, bits, length);
    
    // Count number of bits after SYNC sequences
    // i:           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
    // tmpbuf[i]:   0  0  1  0  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  0  0  1  0  1  1
    // nonsync[i]: 77 78 79 80  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  2  3  4  5  6
    for (nonsync[0] = onecnt = 0, i = 1; i < 2 * length; i++) {

        onecnt = tmpbuf[i] ? onecnt + 1 : 0;
        if (onecnt >= 10) {
            for (unsigned j = 0; j < 10; j++) nonsync[i - j] = 0;
        } else {
            nonsync[i] = nonsync[i - 1] + 1;
        }
    }
    /*
    for (unsigned k = 135; k < 190; k++) {
        printf("%2d ", bits[k]);
    }
    printf("\n");
    for (unsigned k = 135; k < 190; k++) {
        printf("%2d ", nonsync[k] % 99);
    }
    printf("\n");
    */
    
    // Search for biggest gap
    for (i = gapsize = 0; i < 2 * length; i++) {
        if (gapsize < nonsync[i]) {
            *gap = (i % length) + 1;
            gapsize = nonsync[i];
        }
    }

    return true;
}

void NIBArchive::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
}

bool 
NIBArchive::hasSameType(const char *filename)
{
	return NIBArchive::isNIBFile(filename);
}

bool 
NIBArchive::readFromBuffer(const uint8_t *buffer, size_t length)
{	
	if ((data = (uint8_t *)malloc(length)) == NULL)
		return false;

	memcpy(data, buffer, length);
	size = length;

    // Scan raw data for tracks
    scan();
    
	return true;
}

size_t
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
NIBArchive::getStartOfItem(unsigned n)
{
    return (n < 84) ? halftrack[n + 1] : -1;
}
#endif

size_t
NIBArchive::getSizeOfItem(unsigned n)
{
    return n < 84 ? length[n + 1] : 0;
}

const char *
NIBArchive::getNameOfItem(unsigned n)
{
    assert(n < getNumberOfItems());
    
    if (n < 84) {
        if (n % 2 == 0) {
            sprintf(name, "Track %d", (n / 2) + 1);
        } else {
            sprintf(name, "Track %d.5", (n / 2) + 1);
        }
        return name;
    }
    return "";
}

const char *
NIBArchive::getTypeOfItem(unsigned n)
{
    return ""; // (n % 2 == 0) ? "Full" : "Half";
}

void 
NIBArchive::selectItem(unsigned n)
{
    if (n < 84) {
        selectedtrack = n + 1;
        fp = 0;
    }
}

int
NIBArchive::getByte()
{
	if (fp < 0) return -1;
		
	int result = (halftrack[selectedtrack][fp++] << 7);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 6);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 5);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 4);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 3);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 2);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 1);
    if (fp < length[selectedtrack]) result |= (halftrack[selectedtrack][fp++] << 0);
    else fp = -1;
    
	return result;
}
