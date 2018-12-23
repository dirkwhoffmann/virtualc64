/*!
 * @file        Isepic.cpp
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

Isepic::Isepic(C64 *c64) : Cartridge(c64)
{
    setDescription("Isepic");
    
    // Allocate 2KB bytes on-board RAM
    setRamCapacity(2048);

    page = 0;
    
    debug("Isepic cartridge created\n");
}

void
Isepic::reset()
{
    Cartridge::reset();
    
    memset(externalRam, 0, ramCapacity);
    page = 0;
}

size_t
Isepic::stateSize()
{
    return Cartridge::stateSize() + sizeof(page);
}

void
Isepic::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::loadFromBuffer(buffer);
    page = read8(buffer);
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
Isepic::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::saveToBuffer(buffer);
    write8(buffer, page);
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

uint8_t
Isepic::peek(uint16_t addr)
{
    if (cartIsHidden()) {
        assert(false);
        return Cartridge::peek(addr);
    }
    
    if (addr == 0xFFFA || addr == 0xFFFB) {
        return externalRam[(page * 256) + (addr & 0xFF)];
    }
    
    if (isROMLaddr(addr)) {
        return externalRam[(page * 256) + (addr & 0xFF)];
    } else if (isROMHaddr(addr)) {
        
        uint8_t exrom = 0x10;
        uint8_t game  = 0x08;
        uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;
        MemoryType source = c64->mem.BankMap[index][5];
        switch (source) {
            case M_KERNAL:
                return c64->mem.rom[addr];
            default:
                return c64->mem.ram[addr];
        }
    } else {
        assert(false);
    }
}

uint8_t
Isepic::peekIO1(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    if (cartIsVisible()) {
        page = ((addr & 0b001) << 2) | (addr & 0b010) | ((addr & 0b100) >> 2);
    }

    return 0;
}

uint8_t
Isepic::peekIO2(uint16_t addr)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);
    
    if (cartIsVisible()) {
        //  debug("Reading %02X from %d:%d\n", externalRam[(page * 256) + (addr & 0xFF)], page, addr & 0xFF);
        return externalRam[(page * 256) + (addr & 0xFF)];
    }
    
    return 0;
}

void
Isepic::poke(uint16_t addr, uint8_t value)
{
    if (cartIsHidden()) {
        // Should never be called on a hidden cartridge
        assert(false);
    }
    
    if (isROMLaddr(addr)) {
        externalRam[(page * 256) + (addr & 0xFF)] = value;
    } else if (isROMHaddr(addr)) {
        c64->mem.ram[addr] = value;
    } else {
        assert(false);
    }
}

void
Isepic::pokeIO1(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    (void)peekIO1(addr);
}

void
Isepic::pokeIO2(uint16_t addr, uint8_t value)
{
    if (cartIsVisible()) {
        // debug("Writing %02X into %d:%d\n", value, page, addr & 0xFF);
        externalRam[(page * 256) + (addr & 0xFF)] = value;
    }
}

void
Isepic::setSwitch(int8_t pos)
{
    debug("Setting switch to %d\n", pos);
    
    bool wasVisible = cartIsVisible();
    Cartridge::setSwitch(pos);
    
    if (wasVisible == cartIsVisible()) {
        return;
    }
    
    c64->suspend();
    
    if (cartIsVisible()) {
        
        debug("Activating Ipsec cartridge\n");
        
        // Enable Ultimax mode
        c64->expansionport.setGameLinePhi2(0);
        c64->expansionport.setExromLinePhi2(1);
        
        // Trigger an NMI
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
    
    } else {
        
        debug("Hiding Ipsec cartridge\n");
        
        c64->expansionport.setGameLinePhi2(1);
        c64->expansionport.setExromLinePhi2(1);
        
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    }
  
    c64->resume();
}

void
Isepic::updatePeekPokeLookupTables()
{
    // Remap all memory locations that are unmapped in Ultimax mode.
    if (cartIsVisible()) {
        
        uint8_t exrom = c64->expansionport.getExromLinePhi2() ? 0x10 : 0x00;
        uint8_t game  = c64->expansionport.getGameLinePhi2() ? 0x08 : 0x00;
        uint8_t index = (c64->processorPort.read() & 0x07) | exrom | game;
        
        debug("Exrom: %d Game: %d index: %X\n", exrom != 0, game != 0, index & 0x07);

        c64->mem.peekSrc[0x1] = M_RAM;
        c64->mem.peekSrc[0x2] = M_RAM;
        c64->mem.peekSrc[0x3] = M_RAM;
        c64->mem.peekSrc[0x4] = M_RAM;
        c64->mem.peekSrc[0x5] = M_RAM;
        c64->mem.peekSrc[0x6] = M_RAM;
        c64->mem.peekSrc[0x7] = M_RAM;
        c64->mem.peekSrc[0x8] = M_CRTLO;
        c64->mem.peekSrc[0x9] = M_CRTLO;
        c64->mem.peekSrc[0xA] = M_RAM;
        c64->mem.peekSrc[0xB] = M_RAM;
        c64->mem.peekSrc[0xC] = M_RAM;
        c64->mem.peekSrc[0xD] = M_IO;
        c64->mem.peekSrc[0xE] = M_CRTHI;
        c64->mem.peekSrc[0xF] = M_CRTHI;
        
        c64->mem.pokeTarget[0x1] = M_RAM;
        c64->mem.pokeTarget[0x2] = M_RAM;
        c64->mem.pokeTarget[0x3] = M_RAM;
        c64->mem.pokeTarget[0x4] = M_RAM;
        c64->mem.pokeTarget[0x5] = M_RAM;
        c64->mem.pokeTarget[0x6] = M_RAM;
        c64->mem.pokeTarget[0x7] = M_RAM;
        c64->mem.pokeTarget[0x8] = M_CRTLO;
        c64->mem.pokeTarget[0x9] = M_CRTLO;
        c64->mem.pokeTarget[0xA] = M_RAM;
        c64->mem.pokeTarget[0xB] = M_RAM;
        c64->mem.pokeTarget[0xC] = M_RAM;
        c64->mem.pokeTarget[0xD] = M_IO;
        c64->mem.pokeTarget[0xE] = M_CRTHI;
        c64->mem.pokeTarget[0xF] = M_CRTHI;
    }
}
