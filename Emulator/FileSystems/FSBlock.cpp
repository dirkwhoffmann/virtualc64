// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "FSDevice.h"

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
FSBlock::dump()
{
    
}

void
FSBlock::importBlock(const u8 *src)
{
    assert(src != nullptr);
    memcpy(data, src, 256);
}

void
FSBlock::exportBlock(u8 *dst)
{
    assert(dst != nullptr);
    memcpy(dst, data, 256);
}
