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

Ocean::Ocean(C64 &ref) : Cartridge(ref)
{
};

void
Ocean::bankIn(unsigned nr)
{
    // The same 8KB page is banked into both ROML and ROMH
    bankInROML(nr, 0x2000, 0);
    bankInROMH(nr, 0x2000, 0);
}

u8
Ocean::peekIO1(u16 addr)
{
    // When read, we get the same values as if no cartridge is attached
    return vic.getDataBusPhi1();
}

u8
Ocean::peekIO2(u16 addr)
{
    // When read, we get the same values as if no cartridge is attached
    return vic.getDataBusPhi1();
}

void
Ocean::pokeIO1(u16 addr, u8 value)
{
    // Cartridges use $DE00 to switch banks
    bankIn(value & 0x3F);
}
