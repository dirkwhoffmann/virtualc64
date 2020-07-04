// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

Isepic::Isepic(C64 *c64, C64 &ref) : Cartridge(c64, ref)
{
    setDescription("Isepic");
    
    // Allocate 2KB bytes on-board RAM
    setRamCapacity(2048);

    // Reset the page selector flipflops
    page = 0;

    // We start with an enabled cartridge (without triggering an NMI)
    Cartridge::setSwitch(1);
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
Isepic::didLoadFromBuffer(u8 **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    page = read8(buffer);
    oldPeekSource = (MemoryType)read32(buffer);
    oldPokeTarget = (MemoryType)read32(buffer);
}

void
Isepic::didSaveToBuffer(u8 **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write8(buffer, page);
    write32(buffer, (MemoryType)oldPeekSource);
    write32(buffer, (MemoryType)oldPokeTarget);
}

u8
Isepic::peek(u16 addr)
{
    assert((addr & 0xF000) == 0xF000);

    // Intercept if the NMI vector is accessed
    if (cartIsVisible() && (addr == 0xFFFA || addr == 0xFFFB)) {
        return peekRAM((page * 256) + (addr & 0xFF));
    } else {
        return mem.peek(addr, oldPeekSource);
    }
}

u8
Isepic::peekIO1(u16 addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    
    if (cartIsVisible()) {
        page = ((addr & 0b001) << 2) | (addr & 0b010) | ((addr & 0b100) >> 2);
    }

    return 0;
}

u8
Isepic::peekIO2(u16 addr)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);

    if (cartIsVisible()) {
        return peekRAM((page * 256) + (addr & 0xFF));
    } else {
        return Cartridge::peekIO2(addr);
    }
}

void
Isepic::poke(u16 addr, u8 value)
{
    assert((addr & 0xF000) == 0xF000);

    // Intercept if the NMI vector is accessed
    if (cartIsVisible() && (addr == 0xFFFA || addr == 0xFFFB)) {
        pokeRAM((page * 256) + (addr & 0xFF), value);
    } else {
        mem.poke(addr, value, oldPokeTarget);
    }
}

void
Isepic::pokeIO1(u16 addr, u8 value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);

    (void)peekIO1(addr);
}

void
Isepic::pokeIO2(u16 addr, u8 value)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);

    if (cartIsVisible()) {
        pokeRAM((page * 256) + (addr & 0xFF), value);
    } else {
        Cartridge::pokeIO2(addr, value);
    }
}

const char *
Isepic::getSwitchDescription(i8 pos)
{
    return (pos == -1) ? "Off" : (pos == 1) ? "On" : NULL;
}

void
Isepic::setSwitch(i8 pos)
{
    vc64.suspend();

    bool oldVisible = cartIsVisible();
    Cartridge::setSwitch(pos);
    bool newVisible = cartIsVisible();

    if (oldVisible != newVisible) {

        // Enforce a call to updatePeekPokeLookupTables()
        expansionport.setCartridgeMode(CRT_OFF);

        if (newVisible) {

            debug(CRT_DEBUG, "Activating Ipsec cartridge\n");

            // Trigger NMI
            cpu.pullDownNmiLine(CPU::INTSRC_EXPANSION);
            cpu.releaseNmiLine(CPU::INTSRC_EXPANSION);

        } else {

            debug(CRT_DEBUG, "Hiding Ipsec cartridge\n");
        }
    }

    vc64.resume();
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

    oldPeekSource = mem.peekSrc[0xF];
    oldPokeTarget = mem.pokeTarget[0xF];

    mem.peekSrc[0xF] = M_CRTHI;
    mem.pokeTarget[0xF] = M_CRTHI;
}
