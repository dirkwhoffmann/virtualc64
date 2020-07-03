/*!
 * @file        Comal80.cpp
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
Comal80::reset()
{
    CartridgeWithRegister::reset();
    c64->expansionport.setCartridgeMode(CRT_16K);
    bankIn(0);
}

void
Comal80::pokeIO1(uint16_t addr, uint8_t value)
{
    if (addr >= 0xDE00 && addr <= 0xDEFF) {
        
        control = value & 0xC7;
        bankIn(value & 0x03);
        
        switch (value & 0xE0) {
                
            case 0xe0:
                c64->expansionport.setCartridgeMode(CRT_OFF);
                break;
                
            case 0x40:
                c64->expansionport.setCartridgeMode(CRT_8K);
                break;
                
            default:
                c64->expansionport.setCartridgeMode(CRT_16K);
                break;
        }
    }
}
