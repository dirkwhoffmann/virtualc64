// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"

namespace vc64 {

Isepic::Isepic(C64 &ref) : Cartridge(ref)
{
    // Reset the page selector flipflops
    page = 0;

    // We start with an enabled cartridge (without triggering an NMI)
    Cartridge::setSwitch(1);
}

void
Isepic::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        os << std::endl;

        os << tab("Selected Memory Page");
        os << bol(page) << std::endl;
        os << tab("Latched Peek Source");
        os << MemoryTypeEnum::key(oldPeekSource) << std::endl;
        os << tab("Latched Poke Target");
        os << MemoryTypeEnum::key(oldPokeTarget) << std::endl;
    }
}

void
Isepic::_didReset(bool hard)
{
    // eraseRAM(0);
    page = 0;
}

u8
Isepic::peek(u16 addr)
{
    assert((addr & 0xF000) == 0xF000);

    // Intercept if the NMI vector is accessed
    if (cartIsVisible() && (addr == 0xFFFA || addr == 0xFFFB)) {
        return peekRAM((u16)(page << 8 | (addr & 0xFF)));
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
Isepic::spypeekIO1(u16 addr) const
{
    return 0;
}

u8
Isepic::peekIO2(u16 addr)
{
    assert(addr >= 0xDF00 && addr <= 0xDFFF);

    if (cartIsVisible()) {
        return peekRAM((u16)(page << 8 | (addr & 0xFF)));
    } else {
        return Cartridge::peekIO2(addr);
    }
}

u8
Isepic::spypeekIO2(u16 addr) const
{
    if (cartIsVisible()) {
        return peekRAM((u16)(page << 8 | (addr & 0xFF)));
    } else {
        return Cartridge::spypeekIO2(addr);
    }
}

void
Isepic::poke(u16 addr, u8 value)
{
    assert((addr & 0xF000) == 0xF000);

    // Intercept if the NMI vector is accessed
    if (cartIsVisible() && (addr == 0xFFFA || addr == 0xFFFB)) {
        pokeRAM((u16)(page << 8 | (addr & 0xFF)), value);
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
        pokeRAM((u16)(page << 8 | (addr & 0xFF)), value);
    } else {
        Cartridge::pokeIO2(addr, value);
    }
}

const char *
Isepic::getSwitchDescription(isize pos) const
{
    return (pos == -1) ? "Off" : (pos == 1) ? "On" : "";
}

void
Isepic::setSwitch(isize pos)
{
    bool oldVisible = cartIsVisible();
    Cartridge::setSwitch(pos);
    bool newVisible = cartIsVisible();

    if (oldVisible != newVisible) {

        // Enforce a call to updatePeekPokeLookupTables()
        expansionPort.setCartridgeMode(CRTMODE_OFF);

        if (newVisible) {

            trace(CRT_DEBUG, "Activating Ipsec cartridge\n");

            // Trigger NMI
            cpu.pullDownNmiLine(INTSRC_EXP);
            cpu.releaseNmiLine(INTSRC_EXP);

        } else {

            trace(CRT_DEBUG, "Hiding Ipsec cartridge\n");
        }
    }
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

}
