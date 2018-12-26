/*!
 * @file        Zaxxon.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

void
Zaxxon::reset()
{
    Cartridge::reset();
    
    // Make sure peekRomL() is called for the whole 8KB ROML range.
    mappedBytesL = 0x2000;
}

uint8_t
Zaxxon::peekRomL(uint16_t addr)
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

uint8_t
Zaxxon::spypeekRomL(uint16_t addr)
{
    if (addr < 0x1000) {
        return Cartridge::peekRomL(addr);
    } else {
        return Cartridge::peekRomL(addr - 0x1000);
    }
}
