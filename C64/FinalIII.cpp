/*!
 * @file        FinalIII.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
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
FinalIII::reset()
{
    bankIn(0);
    initialGameLine = 0;
    initialExromLine = 0;
}

uint8_t
FinalIII::peekIO1(uint16_t addr)
{
    // I/O space 1 mirrors $1E00 to $1EFF from ROML
    uint16_t offset = addr - 0xDE00;
    return peekRomL(0x1E00 + offset);
}

uint8_t
FinalIII::peekIO2(uint16_t addr)
{
    // I/O space 2 space mirrors $1F00 to $1FFF from ROML
    uint16_t offset = addr - 0xDF00;
    return peekRomL(0x1F00 + offset);
}

void
FinalIII::pokeIO2(uint16_t addr, uint8_t value) {
    
    // 0xDFFF is Final Cartridge's internal control register
    if (addr == 0xDFFF) {
        
        /*  "7      Hide this register (1 = hidden)
         *   6      NMI line   (0 = low = active) *1)
         *   5      GAME line  (0 = low = active) *2)
         *   4      EXROM line (0 = low = active)
         *   2-3    unassigned (usually set to 0)
         *   0-1    number of bank to show at $8000
         *
         *   1) if either the freezer button is pressed,
         *      or bit 6 is 0, then an NMI is generated
         *
         *   2) if the freezer button is pressed, GAME
         *      is also forced low" [VICE]
         */
        
        uint8_t hide  = value & 0x80;
        uint8_t nmi   = value & 0x40;
        uint8_t game  = value & 0x20;
        uint8_t exrom = value & 0x10;
        uint8_t bank  = value & 0x03;
                
        // Bit 7
        if (hide) {
            c64->expansionport.setGameLine(1);
            c64->expansionport.setExromLine(1);
        }
        
        // Bit 6
        nmi ? c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION) :
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
        
        // Bit 5 and 4
        c64->expansionport.setGameLine(game);
        c64->expansionport.setExromLine(exrom);
        
        // Bit 1 and 0
        bankIn(bank);
        // bankIn(bank+4);
    }
}

void
FinalIII::pressFreezeButton() {
    
    // The freezer is enabled by selecting bank 0 in ultimax mode and
    // triggering an NMI
    suspend();
    pokeIO2(0xDFFF, 0x10);
    resume();
}
