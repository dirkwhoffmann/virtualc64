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
Comal80::oldReset()
{
    CartridgeWithRegister::oldReset();
    expansionport.setCartridgeMode(CRT_16K);
    bankIn(0);
}

void
Comal80::pokeIO1(u16 addr, u8 value)
{
    if (addr >= 0xDE00 && addr <= 0xDEFF) {
        
        control = value & 0xC7;
        bankIn(value & 0x03);
        
        switch (value & 0xE0) {
                
            case 0xe0:
                expansionport.setCartridgeMode(CRT_OFF);
                break;
                
            case 0x40:
                expansionport.setCartridgeMode(CRT_8K);
                break;
                
            default:
                expansionport.setCartridgeMode(CRT_16K);
                break;
        }
    }
}
