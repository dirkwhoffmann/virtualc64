// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

void
FinalIII::resetCartConfig()
{
    expansionport.setCartridgeMode(CRTMODE_16K);
}

void
FinalIII::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
    Cartridge::_reset(hard);
    
    qD = true;
    bankIn(0);
}

u8
FinalIII::peekIO1(u16 addr)
{
    // I/O space 1 mirrors $1E00 to $1EFF from ROML
    return peekRomL(addr & 0x1FFF);
}

u8
FinalIII::spypeekIO1(u16 addr) const
{
    return spypeekRomL(addr & 0x1FFF);
}

u8
FinalIII::peekIO2(u16 addr)
{
    // I/O space 2 space mirrors $1F00 to $1FFF from ROML
    return peekRomL(addr & 0x1FFF);
}

u8
FinalIII::spypeekIO2(u16 addr) const
{
    return spypeekRomL(addr & 0x1FFF);
}

void
FinalIII::pokeIO2(u16 addr, u8 value) {
    
    // The control register is mapped to address 0xFF in I/O space 2.
    if (addr == 0xDFFF && writeEnabled()) {
        setControlReg(value);
    }
}

void
FinalIII::nmiDidTrigger()
{
    if (freeezeButtonIsPressed) {
        trace(CRT_DEBUG, "NMI while freeze button is pressed.\n");
        
        /* After the NMI has been processed by the CPU, the cartridge's counter
         * has reached a value that overflows qD to 0. This has two side
         * effects. First, the Game line switches to 0. Second, because qD is
         * also connectec to the counter's enable pin, the counter freezes. This
         * keeps qD low until the freeze button is released by the user.
         */
        qD = false;
        updateGame();
    }
}

void
FinalIII::setControlReg(u8 value)
{
    control = value;
    
    // Update external lines
    updateNMI();
    updateGame();
    expansionport.setExromLine(exrom());
    
    // Switch memory bank
    bankIn(control & 0x03);
    
}

bool
FinalIII::writeEnabled() const
{
    return !hidden() || freeezeButtonIsPressed;
}

void
FinalIII::updateNMI()
{
    if (nmi() && !freeezeButtonIsPressed) {
        cpu.releaseNmiLine(INTSRC_EXP);
    } else {
        cpu.pullDownNmiLine(INTSRC_EXP);
    }
}

void
FinalIII::updateGame()
{
    expansionport.setGameLine(game() && qD);
}

const string
FinalIII::getButtonTitle(isize nr) const
{
    return nr == 1 ? "Freeze" : nr == 2 ? "Reset" : "";
}

void
FinalIII::pressButton(isize nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Pressing %s button.\n", getButtonTitle(nr).c_str());
    
    suspended {
        
        switch (nr) {
                
            case 1: // Freeze
                
                freeezeButtonIsPressed = true;
                updateNMI();
                break;
                
            case 2: // Reset
                
                c64.softReset();
                break;
        }
    }
}

void
FinalIII::releaseButton(isize nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Releasing %s button.\n", getButtonTitle(nr).c_str());
    
    suspended {
        
        switch (nr) {
                
            case 1: // Freeze
                
                freeezeButtonIsPressed = false;
                qD = true;
                updateNMI();
                updateGame();
                break;
        }
    }
}
