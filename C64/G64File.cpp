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

#include "G64File.h"
#include "Disk.h"

const uint8_t /* "GCR-1541" */
G64File::magicBytes[] = { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31, 0x00 };

G64File::G64File()
{
    setDescription("G64Archive");
}

G64File::G64File(size_t capacity)
{
    assert(capacity > 0);
    assert(data == NULL);
    
    size = capacity; 
    data = new uint8_t[capacity];
}

G64File *
G64File::makeG64ArchiveWithBuffer(const uint8_t *buffer, size_t length)
{
    G64File *archive = new G64File();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

G64File *
G64File::makeG64ArchiveWithFile(const char *filename)
{
    G64File *archive = new G64File();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

G64File *
G64File::makeG64ArchiveWithDisk(Disk *disk)
{
    assert(disk != NULL);
    
    // Determine empty (half)tracks
    bool empty[85];
    for (Halftrack ht = 1; ht <= 84; ht++) {
        empty[ht] = disk->halftrackIsEmpty(ht);
    }
    
    // Determine file offsets for all (half)tracks
    uint32_t offset[85];
    unsigned pos = 0x015C;
    for (Halftrack ht = 1; ht <= 84; ht++) {
        if (empty[ht]) {
            offset[ht] = 0;
        } else {
            offset[ht] = pos;
            pos += 2 /* Length */ + maxBytesOnTrack /* Data */;
        }
    }
    
    // Allocate memory
    size_t length = pos + 84 * 4; /* speed zones entries */
    uint8_t *buffer = new uint8_t[length];
    
    // Write header, number of tracks, and track length
    pos = 0;
    memcpy(buffer, G64File::magicBytes, 9);
    buffer[9]  = 84; // 0x54 (Number of tracks)
    buffer[10] = LO_BYTE(maxBytesOnTrack); // 0xF8
    buffer[11] = HI_BYTE(maxBytesOnTrack); // 0x1E

    // Write track offsets
    pos = 12;
    for (Halftrack ht = 1; ht <= 84; ht++) {
        buffer[pos++] = offset[ht] & 0xFF;
        buffer[pos++] = (offset[ht] >> 8) & 0xFF;
        buffer[pos++] = (offset[ht] >> 16) & 0xFF;
        buffer[pos++] = (offset[ht] >> 24) & 0xFF;
    }
    
    // Dump track data
    for (Halftrack ht = 1; ht <= 84; ht++) {
        
        if (!empty[ht]) {

            uint16_t numDataBytes = disk->lengthOfHalftrack(ht) / 8;
            uint16_t numFillBytes = maxBytesOnTrack - numDataBytes;

            if (disk->lengthOfHalftrack(ht) % 8 != 0) {
                printf("WARNING: Size of halftrack %d is not a multiple of 8\n", ht);
            }
            assert(pos == offset[ht]);
            buffer[pos++] = LO_BYTE(numDataBytes);
            buffer[pos++] = HI_BYTE(numDataBytes);
            
            for (unsigned i = 0; i < numDataBytes; i++) {
                buffer[pos++] = disk->data.halftrack[ht][i];
            }
            for (unsigned i = 0; i < numFillBytes; i++) {
                buffer[pos++] = 0xFF;
            }
        }
    }
    
    // Write speed zone area (32 bit, little endian)
    for (Halftrack ht = 1; ht <= 84; ht++) {
        buffer[pos++] = Disk::trackDefaults[(ht + 1) / 2].speedZone;
        buffer[pos++] = 0;
        buffer[pos++] = 0;
        buffer[pos++] = 0;
    }
    assert(pos == length);
    
    return G64File::makeG64ArchiveWithBuffer(buffer, length);
}

bool
G64File::isG64(const uint8_t *buffer, size_t length)
{
    if (length < 0x02AC) return false;
    return checkBufferHeader(buffer, length, magicBytes);
}

bool 
G64File::isG64File(const char *filename)
{
	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".G64") && !checkFileSuffix(filename, ".g64"))
		return false;
	
	if (!checkFileSize(filename, 0x02AC, -1))
		return false;
	
	if (!checkFileHeader(filename, magicBytes))
		return false;
	
	return true;
}

void G64File::dealloc()
{
	if (data) free(data);
	data = NULL;
	size = 0;
	fp = -1;
    fp_eof = -1;
}

bool 
G64File::hasSameType(const char *filename)
{
	return G64File::isG64File(filename);
}

size_t
G64File::writeToBuffer(uint8_t *buffer)
{
    assert(data != NULL);
    
    if (buffer) {
        memcpy(buffer, data, size);
    }
    return size;
}

const char *
G64File::getName()
{
    return "G64 archive";
}

int 
G64File::getNumberOfItems()
{
    return 84;
}

uint32_t
G64File::getStartOfItem(unsigned n)
{
    if (n >= 84) return -1;
    
    int offset = 0x00C + (4 * n);
    return LO_LO_HI_HI(data[offset], data[offset+1], data[offset+2], data[offset+3]);
}

size_t
G64File::getSizeOfItem(unsigned n)
{
    uint32_t offset = getStartOfItem(n);
    return offset ? (LO_HI(data[offset], data[offset+1])) : 0;
}

const char *
G64File::getNameOfItem(unsigned n)
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
G64File::getTypeOfItem(unsigned n)
{
    return ""; // (n % 2 == 0) ? "Full" : "Half";
}

void 
G64File::selectItem(unsigned n)
{
    fp = getStartOfItem(n);
    fp += 2; // skip length information
    fp_eof = fp + getSizeOfItem(n);
}

int
G64File::getByte()
{
	int result;
	
	if (fp < 0)
		return -1;
		
	// get byte
	result = data[fp++];
	
	// check for end of file
	if (fp == fp_eof)
		fp = -1;

	return result;
}

