/*!
 * @file        Expert.cpp
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

// This implementation is based mainly by the following documents:
// Schematics and explanation by Martin Sikström:
// https://people.kth.se/~e93_msi/c64/expert.html

#include "C64.h"

Expert::Expert(C64 *c64) : Cartridge(c64)
{
    setDescription("Expert");
    
    active = false;

    // Allocate 8KB bytes persistant RAM
    setRamCapacity(0x2000);
    persistentRam = true;
    
    debug("Expert cartridge created\n");
}

void
Expert::reset()
{
    // Cartridge::reset();
    debug("Expert::reset()\n");
}

void
Expert::dump()
{
    Cartridge::dump();
    
    msg("               active: %d\n", active);
    msg("             switch: %d ", switchPos);
    if (switchInPrgPosition()) msg("(PRG)\n");
    if (switchInOffPosition()) msg("(OFF)\n");
    if (switchInOnPosition()) msg("(ON)\n");
    msg("         NMI vector: %04X\n", LO_HI(externalRam[0x1FFA], externalRam[0x1FFB]));
    msg("         IRQ vector: %04X\n", LO_HI(externalRam[0x1FFE], externalRam[0x1FFF]));
    msg("       Reset vector: %04X\n", LO_HI(externalRam[0x1FFC], externalRam[0x1FFD]));
}

size_t
Expert::stateSize()
{
    return Cartridge::stateSize() + 1;
}

void
Expert::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::loadFromBuffer(buffer);
    active = read8(buffer);
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
Expert::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::saveToBuffer(buffer);
    write8(buffer, (uint8_t)active);
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
Expert::loadChip(unsigned nr, CRTFile *c)
{
    uint16_t chipSize = c->chipSize(nr);
    uint16_t chipAddr = c->chipAddr(nr);
    uint8_t *chipData = c->chipData(nr);
    
    if (nr != 0 || chipSize != 0x2000 || chipAddr != 0x8000) {
        warn("Corrupted CRT file. Aborting.");
        return;
    }

    // Initialize RAM with data from CRT file
    debug("Copying file contents into Expert RAM\n");
    assert(externalRam != NULL);
    assert(ramCapacity == chipSize);
    memcpy(externalRam, chipData, chipSize);
    
    dump();
}

void
Expert::pressFreezeButton() {
    
    debug("Expert::pressFreezeButton\n");
    
    c64->suspend();

    // Switch on cartridge
    active = true;
    
    // Trigger NMI
    c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    
    c64->resume();
}

void
Expert::releaseFreezeButton()
{
    debug("Expert::releaseFreezeButton\n");

    c64->suspend();
    c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    c64->resume();
}

void
Expert::pressResetButton()
{
    debug("Expert::pressResetButton\n");

    c64->suspend();

    dump();
    
    if (switchInOnPosition()) {
        debug("Switching on cartridge\n");
        active = true;
    }
    Cartridge::pressResetButton();
    dump();
    
    c64->resume();
}

void
Expert::updatePeekPokeLookupTables()
{
    // debug("Setting up faked Ultimax mode...\n");
    
    for (unsigned i = 0; i < 16; i++) {
        c64->mem.peekSrc[i] = c64->mem.pokeTarget[i] = M_CRTLO;
    }
    /*
    c64->mem.peekSrc[0x8] = c64->mem.pokeTarget[0x8] = M_CRTLO;
    c64->mem.peekSrc[0x9] = c64->mem.pokeTarget[0x9] = M_CRTLO;

    c64->mem.peekSrc[0xE] = c64->mem.pokeTarget[0xE] = M_CRTLO;
    c64->mem.peekSrc[0xF] = c64->mem.pokeTarget[0xF] = M_CRTLO;
     */
}

uint8_t
Expert::peek(uint16_t addr)
{
    /*
    if (addr >= 0xFF00) {
        debug("Expert::peek(%04X)\n", addr);
    }
    */
    
    if (cartridgeRamIsVisible(addr)) {
        
        // Get value from cartridge RAM
        return externalRam[addr & 0x1FFF];
        
    } else {
        
        // Get value as if no cartridge was attached
        return c64->mem.peek(addr, 1, 1);
    }
}

uint8_t
Expert::peekIO1(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    debug("Expert::peekIO1\n");
    
    // Any IO1 access disabled the cartridge
    // if (!switchInOnPosition())
    active = false;
    
    return 0;
}

void
Expert::poke(uint16_t addr, uint8_t value)
{
    if (cartridgeRamIsVisible(addr)) {
        
        // Write value into cartridge RAM if it is write enabled
        if (cartridgeRamIsWritable(addr)) {
            debug("Writing RAM cell %04X with %02X\n", addr & 0x1FFF, value);
            externalRam[addr & 0x1FFF] = value;
        }
    
    } else {
    
        // Write value as if no cartridge was attached
        c64->mem.poke(addr, value, 1, 1);
    }
}

void
Expert::pokeIO1(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    debug("Expert::pokeIO1\n");
    
    // Any IO1 access disabled the cartridge
    active = false;
}

void
Expert::setSwitch(int8_t pos)
{
    debug("Setting switch to %d\n", pos);

    Cartridge::setSwitch(pos);
}

bool
Expert::cartridgeRamIsVisible(uint16_t addr)
{
    if (addr < 0x8000) {
        return false;
    }
    if (addr < 0xA000) {
        return switchInPrgPosition() || (switchInOnPosition() && active);
    }
    if (addr < 0xE000) {
        return false;
    }
    return switchInOnPosition() && active;
}

bool
Expert::cartridgeRamIsWritable(uint16_t addr)
{
    return isROMLaddr(addr);
}
