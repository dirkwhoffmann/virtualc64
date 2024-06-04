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
StructuredBasic::resetCartConfig()
{
    expansionPort.setCartridgeMode(CRTMODE_8K);
}

u8
StructuredBasic::peekIO1(u16 addr)
{
    trace(CRT_DEBUG, "peekIO1(%x)\n", addr);

    switch (addr & 0b11) {

        case 0:
        case 1:
            expansionPort.setCartridgeMode(CRTMODE_8K);
            bankIn(0);
            break;

        case 2:
            expansionPort.setCartridgeMode(CRTMODE_8K);
            bankIn(1);
            break;

        case 3:
            expansionPort.setCartridgeMode(CRTMODE_OFF);
            break;
    }
    return 0;
}

u8
StructuredBasic::spypeekIO1(u16 addr) const
{
    return 0;
}

void
StructuredBasic::pokeIO1(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeIO1(%x,%x)\n", addr, value);

    peekIO1(addr);
}

}
