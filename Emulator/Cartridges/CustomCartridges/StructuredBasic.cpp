// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void
StructuredBasic::resetCartConfig()
{
    expansionport.setCartridgeMode(CRTMODE_8K);
}

u8
StructuredBasic::peekIO1(u16 addr)
{
    trace(CRT_DEBUG, "peekIO1(%x)\n", addr);

    switch (addr & 0b11) {
            
        case 0:
        case 1:
            expansionport.setCartridgeMode(CRTMODE_8K);
            bankIn(0);
            break;
            
        case 2:
            expansionport.setCartridgeMode(CRTMODE_8K);
            bankIn(1);
            break;
            
        case 3:
            expansionport.setCartridgeMode(CRTMODE_OFF);
            break;
    }
    return 0;
}

u8
StructuredBasic::spypeekIO1(u16 addr) const
{
    return 0;
}

void
StructuredBasic::pokeIO1(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeIO1(%x,%x)\n", addr, value);

    peekIO1(addr);
}
