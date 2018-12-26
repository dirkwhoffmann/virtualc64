/*!
 * @file        Westermann.cpp
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

uint8_t
Westermann::peekIO2(uint16_t addr)
{
    // Any read access to I/O space 2 switches to 8KB configuration
    if (addr >= 0xDF00 && addr <= 0xDFFF) {
        c64->expansionport.setCartridgeMode(CRT_8K);
    }
    return 0;
}

uint8_t
Westermann::spypeekIO2(uint16_t addr)
{
    return 0;
}
