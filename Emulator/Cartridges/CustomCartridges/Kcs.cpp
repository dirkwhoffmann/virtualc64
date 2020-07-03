// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
KcsPower::peekIO1(u16 addr)
{
    c64->expansionport.setGameAndExrom(1, addr & 0x02 ? 1 : 0);
    return peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
KcsPower::spypeekIO1(u16 addr)
{
    return peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
KcsPower::peekIO2(u16 addr)
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
KcsPower::pokeIO1(u16 addr, uint8_t value)
{
    c64->expansionport.setGameAndExrom(0, (addr & 0b10) ? 1 : 0);
}

void
KcsPower::pokeIO2(u16 addr, uint8_t value)
{
    if (!(addr & 0x80)) {
        pokeRAM(addr & 0x7F, value);
    }
}

void
KcsPower::pressButton(unsigned nr)
{
    if (nr == 1) {
 
        // Pressing the button triggers an NMI in Ultimax mode
        suspend();
        c64->expansionport.setCartridgeMode(CRT_ULTIMAX);
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
        resume();
    }
};

void
KcsPower::releaseButton(unsigned nr)
{
    if (nr == 1) {
    
        suspend();
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
        resume();
    }
};
