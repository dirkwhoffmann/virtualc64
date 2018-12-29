/*!
 * @file        FinalIII.cpp
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
FinalIII::reset()
{
    CartridgeWithRegister::reset();
    freeezeButtonIsPressed = false;
    qD = true;
    bankIn(0);
}

void
FinalIII::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_16K);
}

uint8_t
FinalIII::peekIO1(uint16_t addr)
{
    // debug("PEEKING FROM IO1\n");

    // I/O space 1 mirrors $1E00 to $1EFF from ROML
    uint16_t offset = addr - 0xDE00;
    assert(0x1E00 + offset == (addr & 0x1FFF));
    return peekRomL(0x1E00 + offset);
}

uint8_t
FinalIII::peekIO2(uint16_t addr)
{
    // I/O space 2 space mirrors $1F00 to $1FFF from ROML
    if (addr == 0xDFFF) {
        debug("PEEKING %04X FROM IO2\n", addr);
    }
    uint16_t offset = addr - 0xDF00;
    assert(0x1F00 + offset == (addr & 0x1FFF));
    return peekRomL(0x1F00 + offset);
}

void
FinalIII::pokeIO2(uint16_t addr, uint8_t value) {
    
    // The control register is mapped to address 0xFF in I/O space 2.
    if (addr == 0xDFFF && writeEnabled()) {
        setControlReg(value);
    } else {
        debug("CONTROL IS HIDDEN\n");
    }
        
#if 0
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
            c64->expansionport.setCartridgeMode(CRT_OFF);
        }
        
        // Bit 6
        nmi ? c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION) :
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
        
        // Bit 5 and 4
        c64->expansionport.setGameAndExrom(game, exrom);
        
        // Bit 1 and 0
        bankIn(bank);
#endif
}

void
FinalIII::nmiDidTrigger()
{
    if (freeezeButtonIsPressed) {
        debug("NMI WHILE FREEZE BUTTON IS PRESSED\n");
        
        // After the NMI has been processed by the CPU, the cartridge's counter
        // has reached a value that overflows qD to 0. This has two side
        // effects. First, the Game line switches to 0. Second, because qD is
        // also connectec to the counter's enable pin, the counter freezes. This
        // keeps qD low until the freeze button is released by the user.
        
        qD = false;
        updateGame();
    }
}

const char *
FinalIII::getButtonTitle(unsigned nr)
{
    return (nr == 1) ? "Freeze" : (nr == 2) ? "Reset" : NULL;
}

void
FinalIII::pressButton(unsigned nr)
{
    assert(nr <= numButtons());
    debug("Pressing %s button.\n", getButtonTitle(nr));
    
    c64->suspend();
    
    switch (nr) {
            
        case 1: // Freeze
            
            freeezeButtonIsPressed = true;
            updateNMI();
            break;
            
        case 2: // Reset
            
            resetWithoutDeletingRam();
            break;
    }
    
    c64->resume();
}

void
FinalIII::releaseButton(unsigned nr)
{
    assert(nr <= numButtons());
    debug("Releasing %s button.\n", getButtonTitle(nr));
    
    c64->suspend();
    
    switch (nr) {
            
        case 1: // Freeze
            
            freeezeButtonIsPressed = false;
            qD = true;
            updateNMI();
            updateGame();
            break;
    }
    
    c64->resume();
}

void
FinalIII::setControlReg(uint8_t value)
{
    control = value;
    
    // Update external lines
    updateNMI();
    updateGame();
    updateExrom();
    
    // Switch memory bank
    bankIn(control & 0x03);
    
}

bool
FinalIII::writeEnabled()
{
    return ((control & 0x80) == 0) || freeezeButtonIsPressed;
}

void
FinalIII::updateNMI()
{
    bool nmi = ((control & 0x40) != 0) && !freeezeButtonIsPressed;
    if (nmi) {
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    } else {
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    }
}

void
FinalIII::updateGame()
{
    bool game = ((control & 0x20) != 0) && qD;
    c64->expansionport.setGameLine(game);
}

void
FinalIII::updateExrom()
{
    bool exrom = ((control & 0x10) != 0);
    c64->expansionport.setExromLine(exrom);
}
