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
MikroAss::peekIO1(u16 addr)
{
    return peekRomL(0x1E00 | LO_BYTE(addr));
}

u8
MikroAss::spypeekIO1(u16 addr) const
{
    return spypeekRomL(0x1E00 | LO_BYTE(addr));
}

u8
MikroAss::peekIO2(u16 addr)
{
    return peekRomL(0x1F00 | LO_BYTE(addr));
}

u8
MikroAss::spypeekIO2(u16 addr) const
{
    return spypeekRomL(0x1F00 | LO_BYTE(addr));
}

}
