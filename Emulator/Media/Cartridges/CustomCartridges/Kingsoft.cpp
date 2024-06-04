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
Kingsoft::resetCartConfig()
{
    // Start in 16KB game mode by reading from I/O space 1
    (void)peekIO1(0);
}

void
Kingsoft::updatePeekPokeLookupTables()
{
    // Tweak lookup tables if we run in Ultimax mode
    if (c64.getUltimax()) {

        // $0000 - $7FFF and $C000 - $DFFF are usable the normal way
        u8 exrom = 0x10;
        u8 game  = 0x08;
        u8 index = (cpu.readPort() & 0x07) | exrom | game;

        for (isize bank = 0x1; bank <= 0x7; bank++) {

            MemoryType type = mem.bankMap[index][bank];
            mem.peekSrc[bank] = mem.pokeTarget[bank] = type;
        }

        for (isize bank = 0xC; bank <= 0xD; bank++) {

            MemoryType type = mem.bankMap[index][bank];
            mem.peekSrc[bank] = mem.pokeTarget[bank] = type;
        }
    }
}

u8
Kingsoft::peekIO1(u16 addr)
{
    // Switch to 16KB game mode
    expansionPort.setCartridgeMode(CRTMODE_16K);

    // Bank in second packet to ROMH
    bankInROMH(1, 0x2000, 0);

    return 0;
}

u8
Kingsoft::spypeekIO1(u16 addr) const
{
    return 0;
}

void
Kingsoft::pokeIO1(u16 addr, u8 value)
{
    // Switch to (faked) Ultimax mode
    expansionPort.setCartridgeMode(CRTMODE_ULTIMAX);

    // Bank in third packet to ROMH
    bankInROMH(2, 0x2000, 0);
}

}
