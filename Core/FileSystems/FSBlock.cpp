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
#include "FSBlock.h"
#include "FileSystem.h"

namespace vc64 {

FSBlock::FSBlock(FileSystem& _device, u32 _nr) : device(_device), nr(_nr)
{
    memset(data, 0, sizeof(data));
}

FSBlockType
FSBlock::type() const
{
    TSLink ts = device.layout.tsLink(nr);

    if (ts.t == 18) {
        return ts.s == 0 ? FSBlockType::BAM : FSBlockType::DIR;
    } else {
        return FSBlockType::DATA;
    }
}

void
FSBlock::writeBAM(const string &name)
{
    auto petName = PETName<16>(name);
    writeBAM(petName);
}

void
FSBlock::writeBAM(PETName<16> &name)
{
    // Don't call this methods on blocks other than the BAM block
    assert(this == device.bamPtr());

    // Location of the first directory sector
    data[0x00] = 18;
    data[0x01] = 1;
    
    // Disk DOS version type ('A')
    data[0x02] = 0x41;
    
    // Unused
    data[0x03] = 0x00;
    
    // BAM entries for each track (in groups of four bytes)
    for (Track k = 1; k <= 35; k++) {

        u8 *p = data + 4 * k;

        if (k == 18) {

            p[0] = 17;      // 17 out of 19 blocks are free
            p[1] = 0xFC;    // Mark first two blocks as allocated
            p[2] = 0xFF;
            p[3] = 0x07;

        } else {
            
            p[0] = (u8)device.layout.numSectors(k);
            p[1] = 0xFF;
            p[2] = 0xFF;
            p[3] = p[0] == 21 ? 0x1F : p[0] == 19 ? 0x07 : p[0] == 18 ? 0x03 : 0x01;
        }
    }
    
    // Disk Name (padded with $A0)
    name.write(data + 0x90);
    
    // Filled with $A0
    data[0xA0] = 0xA0;
    data[0xA1] = 0xA0;
    
    // Disk ID
    data[0xA2] = 0x56;
    data[0xA3] = 0x54;
    
    // Usually $A0
    data[0xA4] = 0xA0;
    
    // DOS type
    data[0xA5] = 0x32;  // "2"
    data[0xA6] = 0x41;  // "A"
    
    // Filled with $A0
    data[0xA7] = 0xA0;
    data[0xA8] = 0xA0;
    data[0xA9] = 0xA0;
    data[0xAA] = 0xA0;
}

void
FSBlock::dump() const
{
    
}

FSUsage
FSBlock::itemType(u32 byte) const
{
    switch (type()) {
            
        case FSBlockType::BAM:
            
            switch (byte) {
                    
                case 0x00: return FSUsage::FIRST_DIR_TRACK;
                case 0x01: return FSUsage::FIRST_DIR_SECTOR;
                case 0x02: return FSUsage::DOS_VERSION;
                case 0xA2: return FSUsage::DISK_ID;
                case 0xA3: return FSUsage::DISK_ID;
                case 0xA5: return FSUsage::DOS_TYPE;
                case 0xA6: return FSUsage::DOS_TYPE;
            }
            if (byte >= 0x04 && byte <= 0x8F) return FSUsage::ALLOCATION_BITS;
            if (byte >= 0x90 && byte <= 0x9F) return FSUsage::DISK_NAME;

            return FSUsage::UNUSED;

        case FSBlockType::DIR:
            
            if (byte == 0) return FSUsage::TRACK_LINK;
            if (byte == 1) return FSUsage::SECTOR_LINK;

            byte &= 0x1F;
            
            switch (byte) {
                    
                case 0x02: return FSUsage::FILE_TYPE;
                case 0x03: return FSUsage::FIRST_FILE_TRACK;
                case 0x04: return FSUsage::FIRST_FILE_SECTOR;
                case 0x15: return FSUsage::FIRST_REL_TRACK;
                case 0x16: return FSUsage::FIRST_REL_SECTOR;
                case 0x17: return FSUsage::REL_RECORD_LENGTH;
                case 0x1E: return FSUsage::FILE_LENGTH_LO;
                case 0x1F: return FSUsage::FILE_LENGTH_HI;
            }
            
            if (byte >= 0x05 && byte <= 0x14) return FSUsage::FILE_NAME;
            if (byte >= 0x18 && byte <= 0x1D) return FSUsage::GEOS;

            return FSUsage::UNUSED;
            
        case FSBlockType::DATA:
            
            if (byte == 0) return FSUsage::TRACK_LINK;
            if (byte == 1) return FSUsage::SECTOR_LINK;
            
            return FSUsage::DATA;
            
        default:
            fatalError;
    }
}

Fault
FSBlock::check(u32 byte, u8 *expected, bool strict) const
{
    assert(byte < 256);
    u8 value = data[byte];

    switch (type()) {
            
        case FSBlockType::BAM:
            
            switch (byte) {
                    
                case 0x00: EXPECT_BYTE(18);               break;
                case 0x01: EXPECT_BYTE(1);                break;
                case 0x02: EXPECT_BYTE(0x41);             break;
                case 0xA0:
                case 0xA1:
                case 0xA4: if (strict) EXPECT_BYTE(0xA0); break;
                case 0xA5: EXPECT_BYTE('2');              break;
                case 0xA6: EXPECT_BYTE('A');              break;
                case 0xA7:
                case 0xA8:
                case 0xA9:
                case 0xAA: if (strict) EXPECT_BYTE(0xA0); break;
            }
            
            if (strict && byte >= 0xAB && byte <= 0xFF) EXPECT_BYTE(0x00);

            return Fault::OK;
            
        case FSBlockType::DIR:
            
            if (byte == 0) EXPECT_TRACK_REF (data[byte + 1]);
            if (byte == 1) EXPECT_SECTOR_REF(data[byte - 1]);

            if (!utl::isZero(data + byte, 0x20)) {

                switch (byte & 0x1F) {
                        
                    case 0x03: EXPECT_TRACK_REF (data[byte + 1]); break;
                    case 0x04: EXPECT_SECTOR_REF(data[byte - 1]); break;
                    case 0x15: EXPECT_TRACK_REF (data[byte + 1]); break;
                    case 0x16: EXPECT_SECTOR_REF(data[byte - 1]); break;
                    case 0x17: EXPECT_MAX(254);                   break;
                }
            }
            
            return Fault::OK;
            
        case FSBlockType::DATA:
            
            if (byte == 0 && strict) EXPECT_TRACK_REF (data[byte + 1]);
            if (byte == 1 && strict) EXPECT_SECTOR_REF(data[byte - 1]);

            return Fault::OK;
            
        default:
            fatalError;
    }
}

isize
FSBlock::check(bool strict) const
{
    isize count = 0;
    u8 expected;
    
    for (u32 i = 0; i < 256; i++) {
        
        Fault err = check(i, &expected, strict);
        if (err != Fault::OK) {
            count++;
            loginfo(FS_DEBUG, "Block %ld [%d.%d]: %s\n",
                  nr, i / 4, i % 4, FaultEnum::key(err));
        }
    }
    
    return count;
}

void
FSBlock::importBlock(const u8 *src)
{
    assert(src);
    memcpy(data, src, 256);
}

void
FSBlock::exportBlock(u8 *dst)
{
    assert(dst);
    memcpy(dst, data, 256);
}

}
