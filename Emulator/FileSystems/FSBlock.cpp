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
    Track t; Sector s;
    device.layout.translateBlockNr(nr, &t, &s);
    
    if (t == 18) {
        return s == 0 ? FS_BAM_BLOCK : FS_DIR_BLOCK;
    } else {
        return FS_DATA_BLOCK;
    }
}

FSItemType
FSBlock::itemType(u32 byte)
{
    return FSI_UNKNOWN;
}

void
FSBlock::writeBAM(const char *name)
{
    FSName fsName = FSName(name);
    writeBAM(fsName); 
}

void
FSBlock::writeBAM(FSName &name)
{
    // Don't call this methods on blocks other than the BAM block
    assert(this == device.blockPtr(18, 0));
        
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

unsigned
FSBlock::check(bool strict)
{
    FSError error;
    unsigned count = 0;
    u8 expected;
    
    for (u32 i = 0; i < 256; i++) {
        
        if ((error = check(i, &expected, strict)) != FS_OK) {
            count++;
            trace(FS_DEBUG, "Block %d [%d.%d]: %s\n", nr, i / 4, i % 4, sFSError(error));
        }
    }
    
    return count;
}

FSError
FSBlock::check(u32 pos, u8 *expected, bool strict)
{
    return FS_OK;
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
