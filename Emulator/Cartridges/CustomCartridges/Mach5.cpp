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
Mach5::reset()
{
    Cartridge::reset();
    // c64->expansionport.setCartridgeMode(CRT_8K);
    // bankIn(0);
}

uint8_t
Mach5::peekIO1(u16 addr)
{
    // debug("Mach5::peekIO1(%x)", addr);
    return peekRomL(0x1E00 | LO_BYTE(addr));
}

uint8_t
Mach5::peekIO2(u16 addr)
{
    // debug("Mach5::peekIO2(%x)", addr);
    return peekRomL(0x1F00 | LO_BYTE(addr));
}

void
Mach5::pokeIO1(u16 addr, uint8_t value)
{
    debug("Enabling Mach5 in 8K game mode\n");
    c64->expansionport.setCartridgeMode(CRT_8K);
}

void
Mach5::pokeIO2(u16 addr, uint8_t value)
{
    debug("Switching Mach5 off\n");
    c64->expansionport.setCartridgeMode(CRT_OFF);
}
