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

namespace vc64 {

void
Ocean::bankIn(isize nr)
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

}
