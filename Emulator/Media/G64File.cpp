// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "G64File.h"
#include "Disk.h"
#include "IOUtils.h"

namespace vc64 {

bool
G64File::isCompatible(const fs::path &path)
{
    auto s = util::uppercased(path.extension().string());
    return s == ".G64";
}

bool
G64File::isCompatible(std::istream &stream)
{
    // const u8 magicBytes[] = { 0x47, 0x43, 0x52, 0x2D, 0x31, 0x35, 0x34, 0x31 };
    const u8 magicBytes[] = { 'G', 'C', 'R', '-', '1', '5', '4', '1' };

    if (util::streamLength(stream) < 0x2AC) return false;
    return util::matchingStreamHeader(stream, magicBytes, sizeof(magicBytes));
}

G64File::G64File(isize capacity)
{
    assert(capacity > 0);
    
    data = new u8[capacity];
    size = capacity;
}

void
G64File::init(Disk &disk)
{
    // Determine empty halftracks
    bool empty[85];
    for (Halftrack ht = 1; ht <= 84; ht++) {
        empty[ht] = disk.halftrackIsEmpty(ht);
    }
    
    // Determine file offsets for all halftracks
    u32 offset[85];
    u32 pos = 0x015C;
    for (Halftrack ht = 1; ht <= 84; ht++) {
        if (empty[ht]) {
            offset[ht] = 0;
        } else {
            offset[ht] = pos;
            pos += 2 /* Length */ + maxBytesOnTrack /* Data */;
        }
    }
    
    // Allocate memory
    isize length = pos + 84 * 4; // Speed zones entries
    u8 *buffer = new u8[length];
    
    // Write header, number of tracks, and track length
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

            auto numDataBytes = disk.lengthOfHalftrack(ht) / 8;
            auto numFillBytes = maxBytesOnTrack - numDataBytes;

            if (disk.lengthOfHalftrack(ht) % 8 != 0) {
                warn("Size of halftrack %ld is not a multiple of 8\n", ht);
            }
            assert(pos == offset[ht]);
            buffer[pos++] = LO_BYTE(numDataBytes);
            buffer[pos++] = HI_BYTE(numDataBytes);
            
            for (isize i = 0; i < numDataBytes; i++) {
                buffer[pos++] = disk.data.halftrack[ht][i];
            }
            for (isize i = 0; i < numFillBytes; i++) {
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
    
    init(stream);
}

isize
G64File::getSizeOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    
    isize offset = getStartOfHalftrack(ht);
    return offset ? LO_HI(data[offset], data[offset+1]) : 0;
}

void
G64File::copyHalftrack(Halftrack ht, u8 *buf) const
{
    assert(buf);

    isize start = getStartOfHalftrack(ht) + 2;
    isize len = getSizeOfHalftrack(ht);
    
    memcpy(buf, data + start, len);
}

isize
G64File::getStartOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    
    isize offset = 0x008 + (4 * ht);
    return LO_LO_HI_HI(data[offset], data[offset+1], data[offset+2], data[offset+3]);
}

}
