/*!
 * @file        FreezeFrame.cpp
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
FreezeFrame::reset()
{
    Cartridge::reset();
    
    // In Ultimax mode, the same ROM chip that appears in ROML also appears
    // in ROMH. By default, it it appears in ROML only, so let's bank it in
    // ROMH manually.
    bankInROMH(0, 0x2000, 0);
}

uint8_t
FreezeFrame::peekIO1(uint16_t addr)
{
    // Reading from IO1 switched to 8K game mode
    c64->expansionport.setCartridgeMode(CRT_8K);
    return 0;
}

uint8_t
FreezeFrame::peekIO2(uint16_t addr)
{
    // Reading from IO2 disables the cartridge
    c64->expansionport.setCartridgeMode(CRT_OFF);
    return 0;
}

void
FreezeFrame::pressButton(unsigned nr)
{
    if (nr == 1) {
        
        // Pressing the freeze button triggers an NMI in Ultimax mode
        suspend();
        c64->expansionport.setCartridgeMode(CRT_ULTIMAX);
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
        resume();
    }
}

void
FreezeFrame::releaseButton(unsigned nr)
{
    if (nr == 1) {
        
        suspend();
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
        resume();
    }
}
