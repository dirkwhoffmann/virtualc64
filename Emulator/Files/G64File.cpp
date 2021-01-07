// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "G64File.h"

bool
G64File::isCompatibleName(const std::string &name)
{
    auto s = suffix(name);
    return s == "g64" || s == "G64";
}

bool
G64File::isCompatibleStream(std::istream &stream)
{
    // const u8 magicBytes[] = { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31 };
    const u8 magicBytes[] = { 'G', 'C', 'R', '-', '1', '5', '4', '1' };

    if (streamLength(stream) < 0x2AC) return false;
    return matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

G64File::G64File(usize capacity)
{
    assert(capacity > 0);
    
    data = new u8[capacity];
    size = capacity;
}

G64File *
G64File::makeWithDisk(Disk *disk)
{
    assert(disk);
    
    // Determine empty halftracks
    bool empty[85];
    for (Halftrack ht = 1; ht <= 84; ht++) {
        empty[ht] = disk->halftrackIsEmpty(ht);
    }
    
    // Determine file offsets for all halftracks
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
    usize length = pos + 84 * 4; // Speed zones entries
    u8 *buffer = new u8[length];
    
    // Write header, number of tracks, and track length
    pos = 0;
    strcpy((char *)buffer, "GCR-1541");
    buffer[9]  = 84;                       // 0x54 (Number of tracks)
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
    
    std::stringstream stream;
    stream.write((char *)buffer, length);
    stream.seekg(0, std::ios::beg);
    
    return make <G64File> (stream);
}

G64File *
G64File::makeWithDisk(class Disk *disk, ErrorCode *err)
{
    *err = ERROR_OK;
    
    try { return makeWithDisk(disk); }
    catch (VC64Error &exception) { *err = exception.errorCode; }
    return nullptr;
}

usize
G64File::getSizeOfHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    usize offset = getStartOfHalftrack(ht);
    return offset ? LO_HI(data[offset], data[offset+1]) : 0;
}

void
G64File::copyHalftrack(Halftrack ht, u8 *buf)
{
    assert(buf);

    usize start = getStartOfHalftrack(ht) + 2;
    usize len = getSizeOfHalftrack(ht);
    
    memcpy(buf, data + start, len);
}

usize
G64File::getStartOfHalftrack(Halftrack ht)
{
    assert(isHalftrackNumber(ht));
    
    usize offset = 0x008 + (4 * ht);
    return LO_LO_HI_HI(data[offset], data[offset+1], data[offset+2], data[offset+3]);
}

