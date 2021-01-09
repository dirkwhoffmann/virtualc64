// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

KcsPower::KcsPower(C64 &ref) : Cartridge(ref)
{
    // Allocate 128 bytes on-board RAM
    setRamCapacity(0x80);
}

void
KcsPower::_reset()
{
    Cartridge::_reset();
    eraseRAM(0xFF);
}

u8
KcsPower::peekIO1(u16 addr)
{
    expansionport.setGameAndExrom(1, addr & 0x02 ? 1 : 0);
    return peekRomL(0x1E00 | (addr & 0xFF));
}

u8
KcsPower::spypeekIO1(u16 addr) const
{
    return spypeekRomL(0x1E00 | (addr & 0xFF));
}

u8
KcsPower::peekIO2(u16 addr)
{
    if (addr & 0x80) {
        
        /*
         u8 exrom = expansionport.getExromLine() ? 0x80 : 0x00;
         u8 game = expansionport.getGameLine() ? 0x40 : 0x00;
         return exrom | game | (vic.getDataBusPhi1() & 0x3F);
         */
        return peekRAM(addr & 0x7F);
        
    } else {
        
        // Return value from onboard RAM
        return peekRAM(addr & 0x7F);
    }
}

u8
KcsPower::spypeekIO2(u16 addr) const
{
    return peekRAM(addr & 0x7F);
}

void
KcsPower::pokeIO1(u16 addr, u8 value)
{
    expansionport.setGameAndExrom(0, (addr & 0b10) ? 1 : 0);
}

void
KcsPower::pokeIO2(u16 addr, u8 value)
{
    if (!(addr & 0x80)) {
        pokeRAM(addr & 0x7F, value);
    }
}

const char *
KcsPower::getButtonTitle(unsigned nr)
{
    return (nr == 1) ? "Freeze" : nullptr;
}

void
KcsPower::pressButton(unsigned nr)
{
    if (nr == 1) {
 
        // Pressing the button triggers an NMI in Ultimax mode
        suspend();
        expansionport.setCartridgeMode(CRTMODE_ULTIMAX);
        cpu.pullDownNmiLine(INTSRC_EXP);
        resume();
    }
};

void
KcsPower::releaseButton(unsigned nr)
{
    if (nr == 1) {
    
        suspend();
        cpu.releaseNmiLine(INTSRC_EXP);
        resume();
    }
};
