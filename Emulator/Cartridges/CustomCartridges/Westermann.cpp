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
Westermann::peekIO2(u16 addr)
{
    // Reading from I/O 2 switched the cartridge on
    expansionPort.setCartridgeMode(CRTMODE_8K);
    return 0;
}

u8
Westermann::spypeekIO2(u16 addr) const
{
    return 0;
}

}
