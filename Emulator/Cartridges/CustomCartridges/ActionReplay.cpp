// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

//
// Action Replay (hardware revision 3)
//

u8
ActionReplay3::peek(u16 addr)
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

u8
ActionReplay3::peekIO1(u16 addr)
{
    return 0;
}

u8
ActionReplay3::peekIO2(u16 addr)
{
    u16 offset = addr - 0xDF00;
    return disabled() ? 0 : packet[bank()]->peek(0x1F00 + offset);
}

void
ActionReplay3::pokeIO1(u16 addr, u8 value)
{
    if (!disabled())
        setControlReg(value);
}

const char *
ActionReplay3::getButtonTitle(unsigned nr)
{
    return (nr == 1) ? "Freeze" : (nr == 2) ? "Reset" : NULL;
}

void
ActionReplay3::pressButton(unsigned nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Pressing %s button.\n", getButtonTitle(nr));
    
    suspend();
    
    switch (nr) {
            
        case 1: // Freeze
            
            cpu.pullDownNmiLine(INTSRC_EXP);
            cpu.pullDownIrqLine(INTSRC_EXP);
            
            // By setting the control register to 0, exrom/game is set to 1/0
            // which activates ultimax mode. This mode is reset later, in the
            // ActionReplay's interrupt handler.
            setControlReg(0);
            break;
            
        case 2: // Reset
            
            resetWithoutDeletingRam();
            break;
    }
    
    resume();
}

void
ActionReplay3::releaseButton(unsigned nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Releasing %s button.\n", getButtonTitle(nr));
    
    suspend();
    
    switch (nr) {
            
        case 1: // Freeze
            
            cpu.releaseNmiLine(INTSRC_EXP);
            cpu.releaseIrqLine(INTSRC_EXP);
            break;
    }
    
    resume();
}

void
ActionReplay3::setControlReg(u8 value)
{
    control = value;
    expansionport.setGameAndExrom(game(), exrom());
}


//
// Action Replay (hardware revision 4 and above)
//

ActionReplay::ActionReplay(C64 &ref) : Cartridge(ref)
{
    // Allocate 8KB on-board memory
    setRamCapacity(0x2000);
}

void
ActionReplay::_reset()
{
    Cartridge::_reset();
    setControlReg(0);
}

void
ActionReplay::resetCartConfig()
{
    trace(CRT_DEBUG, "Starting ActionReplay cartridge in 8K game mode.\n");
    expansionport.setCartridgeMode(CRTMode_8K);
}

u8
ActionReplay::peek(u16 addr)
{
    if (ramIsEnabled(addr)) {
        return peekRAM(addr & 0x1FFF);
    }
    return Cartridge::peek(addr);
}
 
void
ActionReplay::poke(u16 addr, u8 value)
{
    if (ramIsEnabled(addr)) {
        pokeRAM(addr & 0x1FFF, value);
    }
}

u8
ActionReplay::peekIO1(u16 addr)
{
    return control;
}

u8
ActionReplay::peekIO2(u16 addr)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    u16 offset = addr & 0xFF;
    
    // I/O space 2 mirrors $1F00 to $1FFF from the selected ROM bank or RAM.
    if (ramIsEnabled(addr)) {
        return peekRAM(0x1F00 + offset);
    } else {
        return packet[chipL]->peek(0x1F00 + offset);
    }
}

void
ActionReplay::pokeIO1(u16 addr, u8 value)
{
    if (!disabled())
    setControlReg(value);
}

void
ActionReplay::pokeIO2(u16 addr, u8 value)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    u16 offset = addr & 0xFF;
    
    if (ramIsEnabled(addr)) {
        pokeRAM(0x1F00 + offset, value);
    }
}

const char *
ActionReplay::getButtonTitle(unsigned nr)
{
    return (nr == 1) ? "Freeze" : (nr == 2) ? "Reset" : NULL;
}

void
ActionReplay::pressButton(unsigned nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Pressing %s button.\n", getButtonTitle(nr));
    
    suspend();
    
    switch (nr) {
            
        case 1: // Freeze
            
            // Turn Ultimax mode on
            setControlReg(0x23);
            
            // Pressing the freeze bottom pulls down both the NMI and the IRQ line
            cpu.pullDownNmiLine(INTSRC_EXP);
            cpu.pullDownIrqLine(INTSRC_EXP);
            break;
            
        case 2: // Reset
            
            resetWithoutDeletingRam();
            break;
    }
    
    resume();
}

void
ActionReplay::releaseButton(unsigned nr)
{
    assert(nr <= numButtons());
    trace(CRT_DEBUG, "Releasing %s button.\n", getButtonTitle(nr));
    
    suspend();
    
    switch (nr) {
            
        case 1: // Freeze
            
            cpu.releaseNmiLine(INTSRC_EXP);
            cpu.releaseIrqLine(INTSRC_EXP);
            break;
    }
    
    resume();
}

void
ActionReplay::setControlReg(u8 value)
{
    control = value;
    
    trace(CRT_DEBUG, "PC: %04X setControlReg(%02X)\n", cpu.getPC0(), value);
    
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
    
    expansionport.setGameAndExrom(game(), exrom());
    
    bankInROML(bank(), 0x2000, 0);
    bankInROMH(bank(), 0x2000, 0);
    
    if (disabled()) {
        trace(CRT_DEBUG, "Action Replay cartridge disabled.\n");
    }
    
    if (resetFreezeMode() || disabled()) {
        cpu.releaseNmiLine(INTSRC_EXP);
        cpu.releaseIrqLine(INTSRC_EXP);
    }
}

bool
ActionReplay::ramIsEnabled(u16 addr)
{
    if (control & 0x20) {
        
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
AtomicPower::ramIsEnabled(u16 addr)
{
    if (control & 0x20) {
        
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

