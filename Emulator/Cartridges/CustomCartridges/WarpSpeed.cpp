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
WarpSpeed::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_16K);
}

uint8_t
WarpSpeed::peekIO1(u16 addr)
{
    return Cartridge::peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
WarpSpeed::peekIO2(u16 addr)
{
    return Cartridge::peekRomL(0x1F00 | (addr & 0xFF));
}

void
WarpSpeed::pokeIO1(u16 addr, uint8_t value)
{
    c64->expansionport.setCartridgeMode(CRT_16K);
}

void
WarpSpeed::pokeIO2(u16 addr, uint8_t value)
{
    c64->expansionport.setCartridgeMode(CRT_OFF);
}

void
WarpSpeed::pressButton(unsigned nr)
{
    assert(nr <= numButtons());

    c64->suspend();
    resetWithoutDeletingRam();
    c64->resume();
}
