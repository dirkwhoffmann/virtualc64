// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "G64File.h"

const uint8_t /* "GCR-1541" */
G64File::magicBytes[] = { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31 };

bool
G64File::isG64Buffer(const uint8_t *buffer, size_t length)
{
    if (length < 0x02AC) return false;
    return matchingBufferHeader(buffer, magicBytes, sizeof(magicBytes));
}

bool
G64File::isG64File(const char *filename)
{
    assert(filename != NULL);
    
    if (!checkFileSuffix(filename, ".G64") && !checkFileSuffix(filename, ".g64"))
        return false;
    
    if (!checkFileSize(filename, 0x02AC, -1))
        return false;
    
    if (!matchingFileHeader(filename, magicBytes, sizeof(magicBytes)))
        return false;
    
    return true;
}

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
G64File::makeWithBuffer(const uint8_t *buffer, size_t length)
{
    G64File *archive = new G64File();
    
    if (!archive->readFromBuffer(buffer, length)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

G64File *
G64File::makeWithFile(const char *filename)
{
    G64File *archive = new G64File();
    
    if (!archive->readFromFile(filename)) {
        delete archive;
        return NULL;
    }
    
    return archive;
}

G64File *
G64File::makeWithDisk(Disk *disk)
{
    assert(disk != NULL);
    
    // Determine empty (half)tracks
    bool empty[85];
    for (Halftrack ht = 1; ht <= 84; ht++) {
        empty[ht] = disk->halftrackIsEmpty(ht);
    }
    
    // Determine file offsets for all (half)tracks
    u32 offset[85];
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

            u16 numDataBytes = disk->lengthOfHalftrack(ht) / 8;
            u16 numFillBytes = maxBytesOnTrack - numDataBytes;

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
    
    return G64File::makeWithBuffer(buffer, length);
}

void
G64File::selectHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    selectedHalftrack = ht;
    tFp = getStartOfHalftrack(ht) + 2 /* length info */;
    tEof = tFp + getSizeOfHalftrack();
}

void
G64File::seekHalftrack(long offset)
{
    assert(isHalftrackNumber(selectedHalftrack));
    
    tFp = getStartOfHalftrack(selectedHalftrack) + 2 + offset;
    
    if (tFp >= size)
        tFp = -1;
}

size_t
G64File::getSizeOfHalftrack()
{
    assert(isHalftrackNumber(selectedHalftrack));
    
    long offset = getStartOfHalftrack(selectedHalftrack);
    return offset ? LO_HI(data[offset], data[offset+1]) : 0;
}

long
G64File::getStartOfHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    int offset = 0x008 + (4 * ht);
    return LO_LO_HI_HI(data[offset], data[offset+1], data[offset+2], data[offset+3]);
}

