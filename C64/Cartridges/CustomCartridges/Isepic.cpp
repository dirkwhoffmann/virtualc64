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

    // Reset the page selector flipflops
    page = 0;

    // We start with an enabled cartridge (without triggering an NMI)
    Cartridge::setSwitch(1);

    debug("Isepic cartridge created\n");
}

void
Isepic::reset()
{
    Cartridge::reset();
    eraseRAM(0);
    page = 0;
}

size_t
Isepic::stateSize()
{
    return Cartridge::stateSize() + 9;
}

void
Isepic::didLoadFromBuffer(uint8_t **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    page = read8(buffer);
    oldPeekSource = (MemoryType)read32(buffer);
    oldPokeTarget = (MemoryType)read32(buffer);
}

void
Isepic::didSaveToBuffer(uint8_t **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write8(buffer, page);
    write32(buffer, (MemoryType)oldPeekSource);
    write32(buffer, (MemoryType)oldPokeTarget);
}

uint8_t
Isepic::peek(uint16_t addr)
{
    switch (addr & 0xF000) {

        case 0xD000:

        assert(false);
        // Intercept if IO2 is accessed
        /*
        if (cartIsVisible() && oldPeekSourceD == M_IO && (addr & 0xDF00) == 0xDF00) {
            return peekRAM((page * 256) + (addr & 0xFF));
        }
        return c64->mem.peek(addr, oldPeekSourceD);
        */
        return 0;

        case 0xF000:

        // Intercept if NMI vector is accessed
        if (cartIsVisible() && (addr == 0xFFFA || addr == 0xFFFB)) {
            return peekRAM((page * 256) + (addr & 0xFF));
        }
        return c64->mem.peek(addr, oldPeekSource);

        default:

        assert(false);
        // return Cartridge::peek(addr);
        return 0;
    }
}

uint8_t
Isepic::peekIO1(uint16_t addr)
{
    // debug("peekIO1(%X)\n", addr);
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
        return peekRAM((page * 256) + (addr & 0xFF));
    } else {
        return Cartridge::peekIO2(addr);
    }
}

void
Isepic::poke(uint16_t addr, uint8_t value)
{
    switch (addr & 0xF000) {

        case 0xD000:

        assert(false);

        // Intercept if IO2 is accessed
        /*
        if (cartIsVisible() && oldPokeTargetD == M_IO && (addr & 0xDF00) == 0xDF00) {
            pokeRAM((page * 256) + (addr & 0xFF), value);
            return;
        }
        c64->mem.poke(addr, value, oldPokeTargetD);
         */
        break;

        case 0xF000:

        // Intercept if NMI vector is accessed
        if (cartIsVisible() && (addr == 0xFFFA || addr == 0xFFFB)) {
            pokeRAM((page * 256) + (addr & 0xFF), value);
            return;
        }
        c64->mem.poke(addr, value, oldPokeTarget);
        break;

        default:

        assert(false);
    }
}

void
Isepic::pokeIO1(uint16_t addr, uint8_t value)
{
    debug("pokeIO1(%X)\n", addr);
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    (void)peekIO1(addr);
}

void
Isepic::pokeIO2(uint16_t addr, uint8_t value)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);

    if (cartIsVisible()) {
        pokeRAM((page * 256) + (addr & 0xFF), value);
    } else {
        Cartridge::pokeIO2(addr, value);
    }
}

const char *
Isepic::getSwitchDescription(int8_t pos)
{
    return (pos == -1) ? "Off" : (pos == 1) ? "On" : NULL;
}

void
Isepic::setSwitch(int8_t pos)
{
    c64->suspend();

    debug("Setting switch from %d to %d\n", getSwitch(), pos);

    bool oldVisible = cartIsVisible();
    Cartridge::setSwitch(pos);
    bool newVisible = cartIsVisible();

    if (!oldVisible && newVisible) {
        
        debug("Activating Ipsec cartridge\n");

        // Force updatePeekPokeLookupTables()to be called
        c64->expansionport.setCartridgeMode(CRT_OFF);
        
        // Trigger NMI
        c64->cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
        c64->cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);
    }

    if (oldVisible && !newVisible) {

        debug("Hiding Ipsec cartridge\n");
        
        // Force updatePeekPokeLookupTables()to be called
        c64->expansionport.setCartridgeMode(CRT_OFF);
    }

    c64->resume();
}

void
Isepic::updatePeekPokeLookupTables()
{
    /* If the ISEPIC cartridge is active, it intercepts memory accesses to the
     * NMI vector at $FFFA / $FFFB. This is done by an inverter and two 8-bit
     * NANDs of type SN5430 that compare the address lines with the bit pattern
     * 1111.1111.1111.101x. If the pattern matches, it enables Ultimax mode by
     * pulling down the GAME line.
     *
     * To emulate this custom behaviour, we redirect the peekSource and
     * pokeTarget for the uppermost memory page to the cartridge.
     */

    oldPeekSource = c64->mem.peekSrc[0xF];
    oldPokeTarget = c64->mem.pokeTarget[0xF];

    c64->mem.peekSrc[0xF] = M_CRTHI;
    c64->mem.pokeTarget[0xF] = M_CRTHI;
}
