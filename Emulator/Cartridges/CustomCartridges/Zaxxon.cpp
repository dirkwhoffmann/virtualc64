// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void
Zaxxon::oldReset()
{
    Cartridge::oldReset();
    
    // Make sure peekRomL() is called for the whole 8KB ROML range.
    mappedBytesL = 0x2000;
}

u8
Zaxxon::peekRomL(u16 addr)
{
    /* "The (Super) Zaxxon carts use a 4Kb ($1000) ROM at $8000-$8FFF (mirrored
     * in $9000-$9FFF) along with two 8Kb ($2000) cartridge banks  located  at
     * $A000-$BFFF. One of the two banks is selected by doing a read access to
     * either the $8000-$8FFF area (bank 0 is selected) or to $9000-$9FFF area
     * (bank 1 is selected)."
     */
    if (addr < 0x1000) {
        bankIn(1);
        return Cartridge::peekRomL(addr);
    } else {
        bankIn(2);
        return Cartridge::peekRomL(addr - 0x1000);
    }
}

u8
Zaxxon::spypeekRomL(u16 addr)
{
    if (addr < 0x1000) {
        return Cartridge::peekRomL(addr);
    } else {
        return Cartridge::peekRomL(addr - 0x1000);
    }
}
