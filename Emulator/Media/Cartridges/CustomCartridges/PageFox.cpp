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

void
PageFox::_didReset(bool hard)
{
    eraseRAM(0);
}

void
PageFox::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);

    if (category == Category::State) {

        os << tab("Control Register");
        os << hex(ctrlReg);
        os << tab("bankSelect()");
        os << hex(bankSelect()) << std::endl;
        os << tab("chipSelect()");
        os << hex(chipSelect()) << std::endl;
        os << tab("bank()");
        os << hex(bank()) << std::endl;
        os << tab("disable()");
        os << hex(disabled()) << std::endl;
    }
}

void
PageFox::resetCartConfig()
{
    pokeIO1(0xDE80, 0);
}

u8
PageFox::peekRomL(u16 addr)
{
    u8 result;

    if (ramIsVisible()) {
        result = peekRAM(ramAddrL(addr));
        debug(CRT_DEBUG, "peekRomL(%x) [RAM] = %x\n", addr, result);
    } else {
        result = Cartridge::peekRomL(addr);
        debug(CRT_DEBUG, "peekRomL(%x) = %x\n", addr, result);
    }

    return result;
}

u8
PageFox::spypeekRomL(u16 addr) const
{
    u8 result;

    if (ramIsVisible()) {
        result = peekRAM(ramAddrL(addr));
    } else {
        result = Cartridge::spypeekRomL(addr);
    }

    return result;
}

u8
PageFox::peekRomH(u16 addr)
{
    u8 result;

    if (ramIsVisible()) {
        result = peekRAM(ramAddrH(addr));
        debug(CRT_DEBUG, "peekRomH(%x) [RAM] = %x\n", addr, result);
    } else {
        result = Cartridge::peekRomH(addr);
        debug(CRT_DEBUG, "peekRomH(%x) = %x\n", addr, result);
    }

    return result;
}

u8
PageFox::spypeekRomH(u16 addr) const
{
    u8 result;

    if (ramIsVisible()) {
        result = peekRAM(ramAddrH(addr));
    } else {
        result = Cartridge::spypeekRomH(addr);
    }

    return result;
}

void
PageFox::pokeRomL(u16 addr, u8 value)
{

    if (ramIsVisible()) {
        debug(CRT_DEBUG, "pokeRomL [RAM] (%x, %x)\n", addr, value);
        pokeRAM(ramAddrL(addr), value);
    } else {
        debug(CRT_DEBUG, "pokeRomL(%x, %x)\n", addr, value);
    }
    mem.ram[0x8000 + addr] = value;
}

void
PageFox::pokeRomH(u16 addr, u8 value)
{
    if (ramIsVisible()) {
        debug(CRT_DEBUG, "pokeRomH [RAM] (%x, %x)\n", addr, value);
        pokeRAM(ramAddrH(addr), value);
    } else {
        debug(CRT_DEBUG, "pokeRomH(%x, %x)\n", addr, value);
    }
    mem.ram[0xA000 + addr] = value;
}

u8
PageFox::peekIO1(u16 addr)
{
    return ctrlReg & 0b11111;
}

u8
PageFox::spypeekIO1(u16 addr) const
{
    return ctrlReg & 0b11111;
}

void
PageFox::pokeIO1(u16 addr, u8 value)
{
    debug(CRT_DEBUG, "pokeIO1(%x, %x)\n", addr, value);

    ctrlReg = value;

    if (disabled()) {
        expansionPort.setCartridgeMode(CRTMODE_OFF);
        debug(CRT_DEBUG, "CRT disabled\n");
    } else {
        expansionPort.setCartridgeMode(CRTMODE_16K);
        debug(CRT_DEBUG, "16K mode\n");
    }

    bankIn(bank());
    mem.updatePeekPokeLookupTables();

    // c64.signalStop();
}

void
PageFox::updatePeekPokeLookupTables()
{
    if (ramIsVisible()) {
        mem.pokeTarget[0x8] = mem.pokeTarget[0x9] = M_CRTLO;
        mem.pokeTarget[0xA] = mem.pokeTarget[0xB] = M_CRTHI;
    }
}

}
