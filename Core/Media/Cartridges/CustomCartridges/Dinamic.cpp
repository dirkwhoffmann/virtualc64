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
Dinamic::peekIO1(u16 addr)
{
    if (addr <= 0xDE0F) bankIn(addr & 0xF);
    return 0;
}

u8
Dinamic::spypeekIO1(u16 addr) const
{
    return 0;
}

}
