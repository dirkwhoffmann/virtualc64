/*!
 * @file        Kcs.cpp
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

KcsPower::KcsPower(C64 *c64) : Cartridge(c64)
{
    // Allocate 128 bytes on-board RAM
    setRamCapacity(0x80);
}

void
KcsPower::reset()
{
    Cartridge::reset();
    eraseRAM(0xFF);
}

uint8_t
KcsPower::peekIO1(uint16_t addr)
{
    c64->expansionport.setGameAndExrom(1,  addr & 0x02 ? 1 : 0);
    return peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
KcsPower::spypeekIO1(uint16_t addr)
{
    return peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
KcsPower::peekIO2(uint16_t addr)
{
    if (addr & 0x80) {
        
        /*
         uint8_t exrom = c64->expansionport.getExromLine() ? 0x80 : 0x00;
         uint8_t game = c64->expansionport.getGameLine() ? 0x40 : 0x00;
         return exrom | game | (c64->vic.getDataBusPhi1() & 0x3F);
         */
        return peekRAM(addr & 0x7F);
        
    } else {
        
        // Return value from onboard RAM
        return peekRAM(addr & 0x7F);
    }
}

void
KcsPower::pokeIO1(uint16_t addr, uint8_t value)
{
    c64->expansionport.setGameAndExrom(0, addr & 0x02 ? 1 : 0);
}

void
KcsPower::pokeIO2(uint16_t addr, uint8_t value)
{
    if (!(addr & 0x80)) {
        pokeRAM(addr & 0x7F, value);
    }
}

void
KcsPower::pressResetButton()
{
    // Pressing the freeze bottom triggers an NMI in ultimax mode
    suspend();
    c64->expansionport.setCartridgeMode(CRT_ULTIMAX);
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    resume();
};

void
KcsPower::releaseResetButton()
{
    suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    resume();
};
