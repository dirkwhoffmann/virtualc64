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
