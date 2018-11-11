/*!
 * @file        ActionReplay.cpp
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

//
// Action Replay (hardware version 3)
//

//! @brief    An older generation Action Replay cartridge
uint8_t
ActionReplay3::peek(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        return packet[bank()]->peek(addr - 0x8000);
    }
    
    if (addr >= 0xE000 && addr <= 0xFFFF) {
        return packet[bank()]->peek(addr - 0xE000);
    }
    
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        return packet[bank()]->peek(addr - 0xA000);
    }
    
    assert(false);
    return 0;
}

uint8_t
ActionReplay3::peekIO1(uint16_t addr)
{
    return 0;
}

uint8_t
ActionReplay3::peekIO2(uint16_t addr)
{
    uint16_t offset = addr - 0xDF00;
    return disabled() ? 0 : packet[bank()]->peek(0x1F00 + offset);
}

void
ActionReplay3::pokeIO1(uint16_t addr, uint8_t value)
{
    if (!disabled())
    setControlReg(value);
}

void
ActionReplay3::pressFreezeButton()
{
    suspend();
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.pullDownIrqLine(CPU::INTSRC_EXPANSION);
    
    // By setting the control register to 0, exrom/game is set to 1/0
    // which activates ultimax mode. This mode is reset later, in the
    // ActionReplay's interrupt handler.
    setControlReg(0);
    resume();
}

void
ActionReplay3::releaseFreezeButton()
{
    suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.releaseIrqLine(CPU::INTSRC_EXPANSION);
    resume();
}

void
ActionReplay3::setControlReg(uint8_t value)
{
    regValue = value;
    c64->expansionport.setGameLine(game());
    c64->expansionport.setExromLine(exrom());
}


//
// Action Replay (hardware version 4 and above)
//

//! @brief    A newer generation Action Replay cartridge
ActionReplay::ActionReplay(C64 *c64) : Cartridge(c64)
{
    debug("ActionReplay constructor\n");
    
    // Allocate 8KB on-board memory
    setRamCapacity(0x2000);
}

void
ActionReplay::reset()
{
    Cartridge::reset();
    setControlReg(0);
}

void
ActionReplay::resetCartConfig()
{
    debug("Starting ActionReplay cartridge in 8K game mode.\n");
    
    // Start in 8K game mode
    c64->expansionport.setGameLine(1);
    c64->expansionport.setExromLine(0);
}

uint8_t
ActionReplay::peek(uint16_t addr)
{
    if (ramIsEnabled(addr)) {
        return externalRam[addr & 0x1FFF];
    }
    return Cartridge::peek(addr);
}
 
void
ActionReplay::poke(uint16_t addr, uint8_t value)
{
    if (ramIsEnabled(addr)) {
        externalRam[addr & 0x1FFF] = value;
    } else {
        debug("poke(%04X, %02X)\n", addr, value);
        // Cartridge::poke(addr, value);
    }
    
}

uint8_t
ActionReplay::peekIO1(uint16_t addr)
{
    debug("peekIO1(uint16_t %04X)\n", addr);
    return regValue;
}

uint8_t
ActionReplay::peekIO2(uint16_t addr)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    uint16_t offset = addr & 0xFF;
    
    // I/O space 2 mirrors $1F00 to $1FFF from the selected ROM bank or RAM.
    if (ramIsEnabled(addr)) {
        return externalRam[0x1F00 + offset];
    } else {
        return packet[chipL]->peek(0x1F00 + offset);
    }
}

void
ActionReplay::pokeIO1(uint16_t addr, uint8_t value)
{
    if (!disabled())
    setControlReg(value);
}

void
ActionReplay::pokeIO2(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    uint16_t offset = addr & 0xFF;
    
    if (ramIsEnabled(addr)) {
        externalRam[0x1F00 + offset] = value;
    }
}

void
ActionReplay::pressFreezeButton()
{
    // Pressing the freeze bottom pulls down both the NMI and the IRQ line
    suspend();
    setControlReg(0);
    c64->expansionport.setGameLine(0);
    c64->expansionport.setExromLine(1);
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.pullDownIrqLine(CPU::INTSRC_EXPANSION);
    resume();
}

void
ActionReplay::releaseFreezeButton()
{
    debug("releaseFreezeButton\n");
    suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.releaseIrqLine(CPU::INTSRC_EXPANSION);
    resume();
}

void
ActionReplay::setControlReg(uint8_t value)
{
    regValue = value;
    
    debug(1, "PC: %04X setControlReg(%02X)\n", c64->cpu.getPC(), value);
    
    assert((value & 0x80) == 0);
    /*  "7    extra ROM bank selector (A15) (unused)
     *   6    1 = resets FREEZE-mode (turns back to normal mode)
     *   5    1 = enable RAM at ROML ($8000-$9FFF) &
     *                          I/O2 ($DF00-$DFFF = $9F00-$9FFF)
     *   4    ROM bank selector high (A14)
     *   3    ROM bank selector low  (A13)
     *   2    1 = disable cartridge (turn off $DE00)
     *   1    1 = /EXROM high
     *   0    1 = /GAME low" [VICE]
     */
    
    c64->expansionport.setGameLine(game());
    c64->expansionport.setExromLine(exrom());
    
    bankInROML(bank(), 0x2000, 0);
    bankInROMH(bank(), 0x2000, 0);
    
    if (disabled()) {
        debug(2, "Action Replay cartridge disabled.\n");
    }
    
    if (resetFreezeMode() || disabled()) {
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
        c64->cpu.releaseIrqLine(CPU::INTSRC_EXPANSION);
    }
}

bool
ActionReplay::ramIsEnabled(uint16_t addr)
{
    if (regValue & 0x20) {
        
        if (addr >= 0xDF00 && addr <= 0xDFFF) { // RAM mirrored in IO2
            return true;
        }
        
        return addr >= 0x8000 && addr <= 0x9FFF; // RAM mapped to ROML
    }
    
    return false;
}


//
// Atomic Power 
//

bool
AtomicPower::game()
{
    return specialMapping() ? 0 : ActionReplay::game();
}

bool
AtomicPower::exrom()
{
    return specialMapping() ? 0 : ActionReplay::exrom();
}

bool
AtomicPower::ramIsEnabled(uint16_t addr)
{
    if (regValue & 0x20) {
        
        if (addr >= 0xDF00 && addr <= 0xDFFF) { // RAM mirrored in IO2
            return true;
        }
        if (specialMapping()) {
            return addr >= 0xA000 && addr <= 0xBFFF; // RAM mapped to ROMH
        } else {
            return addr >= 0x8000 && addr <= 0x9FFF; // RAM mapped to ROML
        }
    }
 
    return false;
}

