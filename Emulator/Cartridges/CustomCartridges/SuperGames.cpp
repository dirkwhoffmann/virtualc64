// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

void
SuperGames::pokeIO2(u16 addr, u8 value)
{
    if (addr == 0xDF00 && protect == false) {

        // Bit 3: Write protection latch
        protect = value & 0b1000;
                
        // Bit 2: Exrom / Game control
        expansionport.setCartridgeMode(value & 0b100 ? CRTMODE_OFF : CRTMODE_16K);
        
        // Bit 0 and 1: Bank select
        bankIn(value & 0b11);
    }
}
