/*!
 * @file        SuperGames.cpp
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
Supergames::pokeIO2(uint16_t addr, uint8_t value)
{
    /* Bits 0, 1: Bank bits 0 and 1
     * Bits 2:    Exrom / Game control
     * Bits 3:    if 1, further writes to DE00 have no effect (not implemented)
     */
    
    if (addr == 0xDF00) {
        
        if (value & 0x04) {
            c64->expansionport.setCartridgeMode(CRT_8K);
        } else {
            c64->expansionport.setCartridgeMode(CRT_16K);
        }
        
        bankIn(value & 0x03);
    }
}
