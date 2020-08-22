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
SimonsBasic::_reset()
{
    bankIn(0);
    bankIn(1);
}

u8
SimonsBasic::peekIO1(u16 addr)
{
    if (addr == 0xDE00) {
        expansionport.setCartridgeMode(CRT_8K);
    }
    return Cartridge::peekIO1(addr);
}

void
SimonsBasic::pokeIO1(u16 addr, u8 value)
{
    if (addr == 0xDE00) {
        expansionport.setCartridgeMode(CRT_16K);
    }
}
