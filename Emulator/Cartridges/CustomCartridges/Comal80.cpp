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
#include "C64.h"

void
Comal80::_reset(bool hard)
{
    Cartridge::_reset(hard);
    expansionport.setCartridgeMode(CRTMODE_16K);
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
                expansionport.setCartridgeMode(CRTMODE_OFF);
                break;
                
            case 0x40:
                expansionport.setCartridgeMode(CRTMODE_8K);
                break;
                
            default:
                expansionport.setCartridgeMode(CRTMODE_16K);
                break;
        }
    }
}
