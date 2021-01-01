// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

u8
Rex::peekIO2(u16 addr)
{
    // Any read access to $DF00 - $DFBF disables the ROM
    if (addr >= 0xDF00 && addr <= 0xDFBF) {
        expansionport.setCartridgeMode(CRTMode_OFF);
    }
    
    // Any read access to $DFC0 - $DFFF switches to 8KB configuration
    if (addr >= 0xDFC0 && addr <= 0xDFFF) {
        expansionport.setCartridgeMode(CRTMode_8K);
    }
    
    return 0;
}
