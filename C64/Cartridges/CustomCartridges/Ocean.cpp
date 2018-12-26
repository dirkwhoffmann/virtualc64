/*!
 * @file        Ocean.cpp
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

/*
 * For more information: http://codebase64.org/doku.php?id=base:crt_file_format
 *
 * "Memory is divided into 8Kb ($2000) banks. For the lower 128Kb, memory is
 *  banked into $8000-$9FFF and for the upper 128Kb, memory is banked into
 *  $A000-$BFFF. Note that the Terminator 2 cartridge loads all 64 banks at
 *  $8000-$9FFF.
 *
 *  Bank switching is done by writing to $DE00. The lower six bits give the bank
 *  number (ranging from 0-63). Bit 8 in this selection word is always set."
 */

void
Ocean::pokeIO1(uint16_t addr, uint8_t value)
{
    if (addr == 0xDE00) {
        bankIn(value & 0x3F);
    }
}
