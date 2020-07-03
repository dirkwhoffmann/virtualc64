// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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

u8
FreezeFrame::peekIO1(u16 addr)
{
    // Reading from IO1 switched to 8K game mode
    c64->expansionport.setCartridgeMode(CRT_8K);
    return 0;
}

u8
FreezeFrame::peekIO2(u16 addr)
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
