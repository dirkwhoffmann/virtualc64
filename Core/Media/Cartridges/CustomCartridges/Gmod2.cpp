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

u8
Gmod2::peekIO1(u16 addr)
{
    return 0;
}

void
Gmod2::pokeIO1(u16 addr, u8 value)
{
    trace(CRT_DEBUG, "pokeIO1(%x, %x)\n", addr, value);

    control = value;

    // Update external lines
    expansionPort.setExromLine(exrom());

    // Switch memory bank
    bankIn(value & 0x3F);
}

}
