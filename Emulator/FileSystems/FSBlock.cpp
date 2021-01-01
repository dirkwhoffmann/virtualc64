// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "FSDevice.h"

FSBlock::FSBlock(FSDevice& _device, u32 _nr) : device(_device), nr(_nr)
{
    memset(data, 0, sizeof(data));
}

FSBlockType
FSBlock::type()
{
    TSLink ts = device.layout.tsLink(nr);
  
    if (ts.t == 18) {
        return ts.s == 0 ? FSBlockType_BAM : FSBlockType_DIR;
    } else {
        return FSBlockType_DATA;
    }
}

void
FSBlock::writeBAM(const char *name)
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
    for (unsigned k = 1; k <= 35; k++) {

        u8 *p = data + 4 * k;

        if (k == 18) {
            
            p[0] = 0;    // No free blocks on the directory track
            p[1] = 0x00;
            p[2] = 0x00;
            p[3] = 0x00;
            
        } else {
            
            p[0] = device.layout.numSectors(k);
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
    
    // AUsually $A0
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
FSBlock::dump()
{
    
}

FSUsage
FSBlock::itemType(u32 byte)
{
    switch (type()) {
            
        case FSBlockType_BAM:
            
            switch (byte) {
                    
                case 0x00: return FSUsage_FIRST_DIR_TRACK;
                case 0x01: return FSUsage_FIRST_DIR_SECTOR;
                case 0x02: return FSUsage_DOS_VERSION;
                case 0xA2: return FSUsage_DISK_ID;
                case 0xA3: return FSUsage_DISK_ID;
                case 0xA5: return FSUsage_DOS_TYPE;
                case 0xA6: return FSUsage_DOS_TYPE;
            }
            if (byte >= 0x04 && byte <= 0x8F) return FSUsage_ALLOCATION_BITS;
            if (byte >= 0x90 && byte <= 0x9F) return FSUsage_DISK_NAME;

            return FSUsage_UNUSED;

        case FSBlockType_DIR:
            
            if (byte == 0) return FSUsage_TRACK_LINK;
            if (byte == 1) return FSUsage_SECTOR_LINK;

            byte &= 0x1F;
            
            switch (byte) {
                    
                case 0x02: return FSUsage_FILE_TYPE;
                case 0x03: return FSUsage_FIRST_FILE_TRACK;
                case 0x04: return FSUsage_FIRST_FILE_SECTOR;
                case 0x15: return FSUsage_FIRST_REL_TRACK;
                case 0x16: return FSUsage_FIRST_REL_SECTOR;
                case 0x17: return FSUsage_REL_RECORD_LENGTH;
                case 0x1E: return FSUsage_FILE_LENGTH_LO;
                case 0x1F: return FSUsage_FILE_LENGTH_HI;
            }
            
            if (byte >= 0x05 && byte <= 0x14) return FSUsage_FILE_NAME;
            if (byte >= 0x18 && byte <= 0x1D) return FSUsage_GEOS;

            return FSUsage_UNUSED;
            
        case FSBlockType_DATA:
            
            if (byte == 0) return FSUsage_TRACK_LINK;
            if (byte == 1) return FSUsage_SECTOR_LINK;
            
            return FSUsage_DATA;
            
        default:
            assert(false);
    }
    
    return FSUsage_UNKNOWN;
}

FSError
FSBlock::check(u32 byte, u8 *expected, bool strict)
{
    assert(byte < 256);
    u8 value = data[byte];

    switch (type()) {
            
        case FSBlockType_BAM:
            
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

            return FSError_OK;
            
        case FSBlockType_DIR:
            
            if (byte == 0) EXPECT_TRACK_REF (data[byte + 1]);
            if (byte == 1) EXPECT_SECTOR_REF(data[byte - 1]);
                        
            if (!isZero(data + byte, 0x20)) {
            
                switch (byte & 0x1F) {
                        
                    case 0x03: EXPECT_TRACK_REF (data[byte + 1]); break;
                    case 0x04: EXPECT_SECTOR_REF(data[byte - 1]); break;
                    case 0x15: EXPECT_TRACK_REF (data[byte + 1]); break;
                    case 0x16: EXPECT_SECTOR_REF(data[byte - 1]); break;
                    case 0x17: EXPECT_MAX(254);                   break;
                }
            }
            
            return FSError_OK;
            
        case FSBlockType_DATA:
            
            if (byte == 0 && strict) EXPECT_TRACK_REF (data[byte + 1]);
            if (byte == 1 && strict) EXPECT_SECTOR_REF(data[byte - 1]);

            return FSError_OK;
            
        default:
            assert(false);
    }
    
    return FSError_OK;
}

unsigned
FSBlock::check(bool strict)
{
    FSError error;
    unsigned count = 0;
    u8 expected;
    
    for (u32 i = 0; i < 256; i++) {
        
        if ((error = check(i, &expected, strict)) != FSError_OK) {
            count++;
            debug(FS_DEBUG, "Block %d [%d.%d]: %s\n", nr, i / 4, i % 4, FSErrorName(error));
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
