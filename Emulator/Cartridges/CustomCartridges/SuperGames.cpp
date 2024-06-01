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
SuperGames::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        os << std::endl;

        os << tab("Write Protection Latch");
        os << bol(protect);
    }
}

void
SuperGames::pokeIO2(u16 addr, u8 value)
{
    if (addr == 0xDF00 && protect == false) {

        // Bit 3: Write protection latch
        protect = value & 0b1000;

        // Bit 2: Exrom / Game control
        expansionPort.setCartridgeMode(value & 0b100 ? CRTMODE_OFF : CRTMODE_16K);

        // Bit 0 and 1: Bank select
        bankIn(value & 0b11);
    }
}

}
