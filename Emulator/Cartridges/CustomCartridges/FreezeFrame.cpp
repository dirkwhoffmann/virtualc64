// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

void
FreezeFrame::_reset()
{
    Cartridge::_reset();
    
    // In Ultimax mode, the same ROM chip that appears in ROML also appears
    // in ROMH. By default, it it appears in ROML only, so let's bank it in
    // ROMH manually.
    bankInROMH(0, 0x2000, 0);
}

u8
FreezeFrame::peekIO1(u16 addr)
{
    // Reading from IO1 switched to 8K game mode
    expansionport.setCartridgeMode(CRTMODE_8K);
    return 0;
}

u8
FreezeFrame::spypeekIO1(u16 addr) const
{
    return 0;
}

u8
FreezeFrame::peekIO2(u16 addr)
{
    // Reading from IO2 disables the cartridge
    expansionport.setCartridgeMode(CRTMODE_OFF);
    return 0;
}

u8
FreezeFrame::spypeekIO2(u16 addr) const
{
    return 0;
}

const char *
FreezeFrame::getButtonTitle(unsigned nr) const
{
    return nr == 1 ? "Freeze" : nullptr;
}

void
FreezeFrame::pressButton(unsigned nr)
{
    if (nr == 1) {
        
        // Pressing the freeze button triggers an NMI in Ultimax mode
        suspend();
        expansionport.setCartridgeMode(CRTMODE_ULTIMAX);
        cpu.pullDownNmiLine(INTSRC_EXP);
        resume();
    }
}

void
FreezeFrame::releaseButton(unsigned nr)
{
    if (nr == 1) {
        
        suspend();
        cpu.releaseNmiLine(INTSRC_EXP);
        resume();
    }
}
