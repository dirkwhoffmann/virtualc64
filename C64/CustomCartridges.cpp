/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 */
/* This program is free software; you can redistribute it and/or modify
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
// Action Replay
//

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
    setControlReg(0x040);
}

uint8_t
ActionReplay::peekRomL(uint16_t addr)
{
    if (ramIsEnabled()) {
        return externalRam[addr];
    } else {
        return chip[chipL][addr];
    }
}

void
ActionReplay::poke(uint16_t addr, uint8_t value)
{
    if (ramIsEnabled() && addr >= 0x8000 && addr <= 0x9FFF) {
        externalRam[addr - 0x8000] = value;
    }
}

uint8_t
ActionReplay::peekIO1(uint16_t addr)
{
    return regValue;
}

uint8_t
ActionReplay::peekIO2(uint16_t addr)
{
    uint16_t offset = addr - 0xDF00;
    assert(offset <= 0xFF);
    
    // I/O space 2 mirrors $1F00 to $1FFF from the selected bank or RAM.
    if (ramIsEnabled()) {
        return externalRam[0x1F00 + offset];
    } else {
        return chip[chipL][0x1F00 + offset];
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
    if (ramIsEnabled()) {
        externalRam[0x1F00 + (addr & 0xFF)] = value;
    }
}

void
ActionReplay::pressFreezeButton()
{
    // Pressing the freeze bottom pulls down both the NMI and the IRQ line
    c64->suspend();
    setControlReg(0);
    c64->expansionport.setGameLine(0);
    c64->expansionport.setExromLine(1);
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.pullDownIrqLine(CPU::INTSRC_EXPANSION);
    c64->resume();
}

void
ActionReplay::setControlReg(uint8_t value)
{
    regValue = value;
    
    debug(2, "ActionReplay::setControlReg(%02X)\n", value);
    
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
        debug(2, "***** DISABLING AR cart *****\n");
    }
    
    if (resetFreezeMode() || disabled()) {
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
        c64->cpu.releaseIrqLine(CPU::INTSRC_EXPANSION);
    }
}


//
// KCS Power
//

KcsPower::KcsPower(C64 *c64) : Cartridge(c64)
{
    // Allocate 128 bytes on-board RAM
    setRamCapacity(0x80);
}

void
KcsPower::reset()
{
    Cartridge::reset();
    memset(externalRam, 0xFF, 0x80);
}

uint8_t
KcsPower::peekIO1(uint16_t addr)
{
    c64->expansionport.setGameLine(true);
    c64->expansionport.setExromLine(addr & 0x02 ? true : false);
    
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
        return externalRam[addr & 0x7F];
        
    } else {
        
        // Return value from onboard RAM
        return externalRam[addr & 0x7F];
    }
}

void
KcsPower::pokeIO1(uint16_t addr, uint8_t value)
{
    c64->expansionport.setGameLine(false);
    c64->expansionport.setExromLine(addr & 0x02 ? true : false);
}

void
KcsPower::pokeIO2(uint16_t addr, uint8_t value)
{
    if (!(addr & 0x80)) {
        externalRam[addr & 0x7F] = value;
    }
}

void
KcsPower::pressResetButton()
{
    // Pressing the freeze bottom triggers an NMI in ultimax mode
    c64->suspend();
    c64->expansionport.setGameLine(0);
    c64->expansionport.setExromLine(1);
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->resume();
};

void
KcsPower::releaseResetButton()
{
    c64->suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    c64->resume();
};


//
// Simons Basic
//

void
SimonsBasic::reset()
{
    bankIn(0);
    bankIn(1);
}

uint8_t
SimonsBasic::peekIO1(uint16_t addr)
{
    if (addr == 0xDE00) {
        // Switch to 8KB configuration
        c64->expansionport.setGameLine(1);
    }
    return Cartridge::peekIO1(addr);
}

uint8_t
SimonsBasic::readIO1(uint16_t addr)
{
    return Cartridge::peekIO1(addr);
}

void
SimonsBasic::pokeIO1(uint16_t addr, uint8_t value)
{
    if (addr == 0xDE00) {
        // Switch to 16KB configuration
        c64->expansionport.setGameLine(0);
    }
}

//
// Ocean type 1
//

/*
 * For more information: http://codebase64.org/doku.php?id=base:crt_file_format
 *
 * "Memory is divided into 8Kb ($2000) banks. For the lower 128Kb, memory is
 *  banked into $8000-$9FFF and for the upper 128Kb, memory is banked into
 *  $A000-$BFFF. Note that the Terminator 2 cartridge loads all 64 banks at
 *  $8000-$9FFF.
 *
 *  Bank switching is done by writing to $DE00. The lower six bits give the bank
 *  number (ranging from 0-63). Bit 8 in this selection word is always set."
 */

void
Ocean::pokeIO1(uint16_t addr, uint8_t value)
{
    if (addr == 0xDE00)
        bankIn(value & 0x3F);
}


//
// Funplay
//

void
Funplay::pokeIO1(uint16_t addr, uint8_t value)
{
    /*
     * Bank switching is done by writing to $DE00.
     *
     * Bit in DE00 -> 76543210
     *                xx210xx3 <- Bit in selected bank number
     *
     * A value of $86 is written to disable the cartridge.
     */

    if (addr == 0xDE00) {
       
        if (value == 0x86) {
            c64->expansionport.setGameLine(1);
            c64->expansionport.setExromLine(1);
            return;
        }
        
        uint8_t bank = ((value >> 3) & 0x07) | ((value << 3) & 0x08);
        assert(bank < 16);
        bankIn(bank);
    }
}


//
// Supergames
//

void
Supergames::pokeIO2(uint16_t addr, uint8_t value)
{
    /* Bits 0, 1: Bank bits 0 and 1
     * Bits 2:    Exrom / Game control
     * Bits 3:    if 1, further writes to DE00 have no effect (not implemented)
     */
    
    if (addr == 0xDF00) {
        
        uint8_t bank    = value & 0x03;
        uint8_t ctrl    = value & 0x04;
        // uint8_t protect = value & 0x08;
        
        // debug ("value = %02X, bank = %d, ctrl = %d", value, bank, ctrl);
        
        if (ctrl) {
            c64->expansionport.setExromLine(false);
            c64->expansionport.setGameLine(true);
        } else {
            c64->expansionport.setExromLine(0);
            c64->expansionport.setGameLine(0);
        }
        
        bankIn(bank);
    }
}


//
// Epyx Fast Loader
//

void
EpyxFastLoad::reset()
{
    // Make the ROM show up
    initialGameLine = 1;
    initialExromLine = 0;
    Cartridge::reset();
    
    dischargeCapacitor();

}

void
EpyxFastLoad::execute()
{
    checkCapacitor();
}

void
EpyxFastLoad::dischargeCapacitor()
{
    // debug("Discharging capacitor\n");
    
    /* The capacitor will be charged in about 512 cycles (value taken from VICE).
     * We store this value variable 'cycle', so it can be picked up in execute().
     */
    cycle = c64->currentCycle() + 512;
    
    if (c64->expansionport.getGameLine() == 1 && c64->expansionport.getExromLine() == 1) {
    }
    
    c64->expansionport.setExromLine(0);
    c64->expansionport.setGameLine(1);
}

bool
EpyxFastLoad::checkCapacitor()
{
    
    // debug("Capacitor check: Cartridge continues to live for %ld cycles\n", disable_at_cycle - c64->getCycles());
    
    if (c64->currentCycle() > cycle) {
        
        if (c64->expansionport.getGameLine() != 1 || c64->expansionport.getExromLine() != 1) {
        }
            
        // Switch cartridge off
        // Should be really change exrom and game line???
        c64->expansionport.setExromLine(1);
        c64->expansionport.setGameLine(1);
        return false;
    }
    
    return true;
}


uint8_t
EpyxFastLoad::peekRomL(uint16_t addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomL(addr);
}

uint8_t
EpyxFastLoad::peekRomH(uint16_t addr)
{
    dischargeCapacitor();
    return Cartridge::peekRomH(addr);
}

/*
uint8_t
EpyxFastLoad::spypeekRomL(uint16_t addr)
{
    return Cartridge::spypeekRomL(addr);
}

uint8_t
EpyxFastLoad::spypeekRomL(uint16_t addr)
{
    return Cartridge::spypeekRomL(addr);
}
*/

uint8_t
EpyxFastLoad::peekIO1(uint16_t addr)
{
    dischargeCapacitor();
    return 0;
}

uint8_t
EpyxFastLoad::readIO1(uint16_t addr)
{
    return 0;
}

uint8_t
EpyxFastLoad::peekIO2(uint16_t addr)
{
    // I/O 2 mirrors the last 256 ROM bytes
    return chip[0][0x1f00 + (addr & 0xff)];
}


//
// Westermann learning
//

uint8_t
Westermann::peekIO2(uint16_t addr)
{
    // Any read access to I/O space 2 switches to 8KB configuration
    if (addr >= 0xDF00 && addr <= 0xDFFF) {
        c64->expansionport.setGameLine(1);
    }
    return 0;
}

uint8_t
Westermann::spypeekIO2(uint16_t addr)
{
    return 0;
}


//
// Rex
//

uint8_t
Rex::peekIO2(uint16_t addr)
{
    // Any read access to $DF00 - $DFBF disables the ROM
    if (addr >= 0xDF00 && addr <= 0xDFBF) {
        c64->expansionport.setExromLine(1);
        c64->expansionport.setGameLine(1);
    }
    
    // Any read access to $DFC0 - $DFFF switches to 8KB configuration
    if (addr >= 0xDFC0 && addr <= 0xDFFF) {
        c64->expansionport.setExromLine(0);
        c64->expansionport.setGameLine(1);
    }
    
    return 0;
}

uint8_t
Rex::spypeekIO2(uint16_t addr)
{
    return 0;
}


//
// WarpSpeed
//

void
WarpSpeed::reset()
{
    initialGameLine = 0;
    initialExromLine = 0;
    Cartridge::reset();
}

uint8_t
WarpSpeed::peekIO1(uint16_t addr)
{
    return Cartridge::peekRomL(0x1E00 | (addr & 0xFF));
}

uint8_t
WarpSpeed::peekIO2(uint16_t addr)
{
    return Cartridge::peekRomL(0x1F00 | (addr & 0xFF));
}

void
WarpSpeed::pokeIO1(uint16_t addr, uint8_t value)
{
    // debug("Enabling ROM at 0x8000\n");
    c64->expansionport.setGameLine(0);
    c64->expansionport.setExromLine(0);
}

void
WarpSpeed::pokeIO2(uint16_t addr, uint8_t value)
{
    // debug("Disabling ROM at 0x8000\n");
    c64->expansionport.setGameLine(1);
    c64->expansionport.setExromLine(1);
}


//
// Zaxxon
//

void
Zaxxon::reset()
{
    Cartridge::reset();
    
    // Make sure peekRomL() is called for the whole 8KB ROML range.
    assert(chipSize[0] = 0x1000);
    chipSize[0] = 0x2000;
}

uint8_t
Zaxxon::peekRomL(uint16_t addr)
{
    /* "The (Super) Zaxxon carts use a 4Kb ($1000) ROM at $8000-$8FFF (mirrored
     * in $9000-$9FFF) along with two 8Kb ($2000) cartridge banks  located  at
     * $A000-$BFFF. One of the two banks is selected by doing a read access to
     * either the $8000-$8FFF area (bank 0 is selected) or to $9000-$9FFF area
     * (bank 1 is selected)."
     */
    if (addr < 0x1000) {
        bankIn(1);
        return Cartridge::peekRomL(addr);
    } else {
        bankIn(2);
        return Cartridge::peekRomL(addr - 0x1000);
    }
}

uint8_t
Zaxxon::spypeekRomL(uint16_t addr)
{
    if (addr < 0x1000) {
        return Cartridge::peekRomL(addr);
    } else {
        return Cartridge::peekRomL(addr - 0x1000);
    }
}

/*
uint8_t
Zaxxon::peek(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0x8FFF) {
        bankIn(1);
        return Cartridge::peek(addr);
    }
    if (addr >= 0x9000 && addr <= 0x9FFF) {
        bankIn(2);
        return Cartridge::peek(addr - 0x1000);
    }
    
    return Cartridge::peek(addr);
}

uint8_t
Zaxxon::read(uint16_t addr)
{
    return Cartridge::peek(addr);
}
*/


//
// MagicDesk
//

uint8_t
MagicDesk::peekIO1(uint16_t addr)
{
    return regValue;
}

void
MagicDesk::pokeIO1(uint16_t addr, uint8_t value)
{
    regValue = value & 0x8F;
    
    /* This cartridge type is very similar to the OCEAN cart type: ROM  memory
     * is  organized  in  8Kb  ($2000)  banks  located  at  $8000-$9FFF.  Bank
     * switching is done by writing the bank number to $DE00. Deviant from the
     * Ocean type, bit 8 is cleared for selecting one of the ROM banks. If bit
     * 8 is set ($DE00 = $80), the GAME/EXROM lines are disabled,  turning  on
     * RAM at $8000-$9FFF instead of ROM.
     */
    
    if (addr == 0xDE00) {
        c64->expansionport.setExromLine(value & 0x80);
        bankIn(value & 0x0F);
    }
}


//
// COMAL 80
//

void
Comal80::reset()
{
    debug("Comal80::reset\n");
    c64->expansionport.setExromLine(0);
    c64->expansionport.setGameLine(0);
    bankIn(0);
}

uint8_t
Comal80::peekIO1(uint16_t addr)
{
    return regValue; // 'value' contains the latest value passed to pokeIO1()
}

uint8_t
Comal80::peekIO2(uint16_t addr)
{
    return 0;
}

void
Comal80::pokeIO1(uint16_t addr, uint8_t value)
{
    // debug("Comal80::pokeIO(%04X, %02X)\n", addr, value);
    
    if (addr >= 0xDE00 && addr <= 0xDEFF) {

        regValue = value & 0xC7;
        bankIn(value & 0x03);
        
        switch (value & 0xE0) {
                
            case 0xe0: // Disables the cartridge
                c64->expansionport.setExromLine(1);
                c64->expansionport.setGameLine(1);
                break;
                
            case 0x40: // 8 KB configuration
                c64->expansionport.setExromLine(0);
                c64->expansionport.setGameLine(1);
                break;
                
            default:   // 16 KB configuration
                c64->expansionport.setExromLine(0);
                c64->expansionport.setGameLine(0);
                break;
        }
    }
}


//
// Action Replay 3
//

uint8_t
ActionReplay3::peek(uint16_t addr)
{
    if (addr >= 0x8000 && addr <= 0x9FFF) {
        return chip[bank()][addr - 0x8000];
    }
    
    if (addr >= 0xE000 && addr <= 0xFFFF) {
        return chip[bank()][addr - 0xE000];
    }
    
    if (addr >= 0xA000 && addr <= 0xBFFF) {
        return chip[bank()][addr - 0xA000];
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
    return disabled() ? 0 : chip[bank()][0x1F00 + offset];
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
    c64->suspend();
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.pullDownIrqLine(CPU::INTSRC_EXPANSION);
    
    // By setting the control register to 0, exrom/game is set to 1/0
    // which activates ultimax mode. This mode is reset later, in the
    // ActionReplay's interrupt handler.
    setControlReg(0);
    c64->resume();
}

void
ActionReplay3::releaseFreezeButton()
{
    c64->suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    c64->cpu.releaseIrqLine(CPU::INTSRC_EXPANSION);
    c64->resume();
}

/*
 void
 ActionReplay3::pressResetButton()
 {
 // Note: Cartridge requires to keep the RAM
 // TODO: Same as in FinalIII. Add a 'softReset' method to C64 class
 uint8_t ram[0xFFFF];
 
 c64->suspend();
 memcpy(ram, c64->mem.ram, 0xFFFF);
 c64->reset();
 memcpy(c64->mem.ram, ram, 0xFFFF);
 c64->resume();
 }
 */

void
ActionReplay3::setControlReg(uint8_t value)
{
    regValue = value;
    c64->expansionport.setGameLine(game());
    c64->expansionport.setExromLine(exrom());
}


//
// FreezeFrame
//

void
FreezeFrame::reset()
{
    Cartridge::reset();
    
    // In Ultimax mode, the same ROM chip that appears in ROML also appears
    // in ROMH. By default, it get banked in ROML only, so let's bank it in
    // ROMH manually.
    bankInROMH(0, 0x2000, 0);
}

uint8_t
FreezeFrame::peekIO1(uint16_t addr)
{
    // Reading from IO1 switched to 8K game mode
    // if (addr == 0) {
    {
        c64->expansionport.setExromLine(0);
        c64->expansionport.setGameLine(1);
    }
    return 0;
}

uint8_t
FreezeFrame::peekIO2(uint16_t addr)
{
    // Reading from IO2 disables the cartridge
    // if (addr == 0) {
    {
        c64->expansionport.setExromLine(1);
        c64->expansionport.setGameLine(1);
    }
    return 0; 
}

void
FreezeFrame::pressFreezeButton()
{
    // Pressing the freeze button switches to ultimax mode and triggers an NMI
    c64->suspend();
    c64->expansionport.setExromLine(1);
    c64->expansionport.setGameLine(0);
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    c64->resume();
}

void
FreezeFrame::releaseFreezeButton()
{
    c64->suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    c64->resume();
}


//
// GeoRAM
//

GeoRAM::GeoRAM(C64 *c64) : Cartridge(c64)
{
    setDescription("GeoRAM");
}

void
GeoRAM::reset()
{
    if (!persistentRam) {
        debug("Erasing GeoRAM\n");
        memset(externalRam, 0, ramCapacity);
    } else {
        debug("Preserving GeoRAM\n");
    }
}

size_t
GeoRAM::stateSize()
{
    return Cartridge::stateSize() + sizeof(bank) + sizeof(page);
}

void
GeoRAM::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::loadFromBuffer(buffer);
    bank = read8(buffer);
    page = read8(buffer);
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
GeoRAM::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::saveToBuffer(buffer);
    write8(buffer, bank);
    write8(buffer, page);
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

unsigned
GeoRAM::offset(uint8_t addr)
{
    /* From VICE:
     * "The GeoRAM is a banked memory system. It uses the registers at
     *  $dffe and $dfff to determine what part of the GeoRAM memory should
     *  be mapped to $de00-$deff.
     *  The register at $dfff selects which 16k block to map, and $dffe
     *  selects a 256-byte page in that block. Since there are only 64
     *  256-byte pages inside of 16k, the value in $dffe ranges from 0 to 63."
     */
    
    unsigned bankOffset = (bank * 16384) % ramCapacity;
    unsigned pageOffset = (page & 0x3F) * 256;
    return bankOffset + pageOffset + addr;
}

uint8_t
GeoRAM::peekIO1(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    unsigned i = offset(addr - 0xDE00);
    assert(externalRam != NULL);
    assert(i < ramCapacity);
    return externalRam[i];
}

uint8_t
GeoRAM::peekIO2(uint16_t addr)
{
    return 0;
}

void
GeoRAM::pokeIO1(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    unsigned i = offset(addr - 0xDE00);
    assert(externalRam != NULL);
    assert(i < ramCapacity);
    externalRam[i] = value;
}

void
GeoRAM::pokeIO2(uint16_t addr, uint8_t value)
{
    if (addr & 1) {
        bank = value; // Bank select
    } else {
        page = value; // Page select
    }
}
