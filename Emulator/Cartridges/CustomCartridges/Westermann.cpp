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
Westermann::peekIO2(uint16_t addr)
{
    // Reading from I/O 2 switched the cartridge on
    c64->expansionport.setCartridgeMode(CRT_8K);
    return 0;
}
