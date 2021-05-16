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
Mach5::_reset(bool hard)
{
    Cartridge::_reset(hard);
}

u8
Mach5::peekIO1(u16 addr)
{
    return peekRomL(0x1E00 | LO_BYTE(addr));
}

u8
Mach5::spypeekIO1(u16 addr) const
{
    return spypeekRomL(0x1E00 | LO_BYTE(addr));
}

u8
Mach5::peekIO2(u16 addr)
{
    return peekRomL(0x1F00 | LO_BYTE(addr));
}

u8
Mach5::spypeekIO2(u16 addr) const
{
    return spypeekRomL(0x1F00 | LO_BYTE(addr));
}

void
Mach5::pokeIO1(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "Enabling Mach5 in 8K game mode\n");
    expansionport.setCartridgeMode(CRTMODE_8K);
}

void
Mach5::pokeIO2(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "Switching Mach5 off\n");
    expansionport.setCartridgeMode(CRTMODE_OFF);
}
