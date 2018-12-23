/*!
 * @file        Kingsoft.h
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
Kingsoft::resetCartConfig()
{
    // Start in 16KB game mode by reading from I/O space 1
    (void)peekIO1(0);
}

void
Kingsoft::updatePeekPokeLookupTables()
{
    // Tweak lookup tables if we run in Ultimax mode
    if (c64->getUltimax()) {
        
        // $0000 - $7FFF and $C000 - $DFFF are usable the normal way
        uint8_t exrom = 0x10;
        uint8_t game  = 0x08;
        uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;

        for (unsigned bank = 0x1; bank <= 0x7; bank++) {
            MemoryType type = c64->mem.bankMap[index][bank];
            c64->mem.peekSrc[bank] = c64->mem.pokeTarget[bank] = type;
        }
        for (unsigned bank = 0xC; bank <= 0xD; bank++) {
            MemoryType type = c64->mem.bankMap[index][bank];
            c64->mem.peekSrc[bank] = c64->mem.pokeTarget[bank] = type;
        }
    }
}

uint8_t
Kingsoft::peekIO1(uint16_t addr)
{
    debug("Switching to 16KB game mode\n");
    
    // Switch to 16KB game mode
    c64->expansionport.setGameLine(0);
    c64->expansionport.setExromLine(0);
 
    // Bank in second packet to ROMH
    bankInROMH(1, 0x2000, 0);
    
    return 0;
}

void
Kingsoft::pokeIO1(uint16_t addr, uint8_t value)
{
    debug("Switching to (modified) Ultimax mode\n");
    
    // Switch to (modified) Ultimax mode
    c64->expansionport.setGameLine(0);
    c64->expansionport.setExromLine(1);
    
    // Bank in third packet to ROMH
    bankInROMH(2, 0x2000, 0);
}

