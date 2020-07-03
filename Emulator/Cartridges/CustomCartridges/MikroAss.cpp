// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

uint8_t
MikroAss::peekIO1(uint16_t addr)
{
    // debug("MikroAss::peekIO1(%x)", addr);
    return peekRomL(0x1E00 | LO_BYTE(addr));
}

uint8_t
MikroAss::peekIO2(uint16_t addr)
{
    // debug("MikroAss::peekIO2(%x)", addr);
    return peekRomL(0x1F00 | LO_BYTE(addr));
}
