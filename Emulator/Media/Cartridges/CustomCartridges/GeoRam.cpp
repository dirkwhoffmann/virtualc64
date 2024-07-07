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

GeoRAM::GeoRAM(C64 &ref, isize kb) : Cartridge(ref), kb(kb)
{
    // The RAM capacity must be a power of two between 64 and 4096
    if ((kb & (kb - 1)) || kb < 64 || kb > 4096) {
        throw Error(ERROR_OPT_INV_ARG, "64, 128, 256, ..., 4096");
    }

    setRamCapacity(KB(kb));
}

void
GeoRAM::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    Cartridge::_dump(category, os);
    os << std::endl;

    if (category == Category::State) {

        os << tab("Bank");
        os << dec(bank) << std::endl;
        os << tab("Page");
        os << dec(page) << std::endl;
    }
}

u8
GeoRAM::peekIO1(u16 addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    return peekRAM((u16)offset(addr & 0xFF));
}

u8
GeoRAM::spypeekIO1(u16 addr) const
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    return peekRAM((u16)offset(addr & 0xFF));
}

u8
GeoRAM::peekIO2(u16 addr)
{
    return 0;
}

u8
GeoRAM::spypeekIO2(u16 addr) const
{
    return 0;
}

void
GeoRAM::pokeIO1(u16 addr, u8 value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    pokeRAM((u16)offset(addr & 0xFF), value);
}

void
GeoRAM::pokeIO2(u16 addr, u8 value)
{
    if (addr & 1) {
        bank = value; // Bank select
    } else {
        page = value; // Page select
    }
}

isize
GeoRAM::offset(u8 addr) const
{
    /* From VICE:
     * "The GeoRAM is a banked memory system. It uses the registers at
     *  $dffe and $dfff to determine what part of the GeoRAM memory should
     *  be mapped to $de00-$deff.
     *  The register at $dfff selects which 16k block to map, and $dffe
     *  selects a 256-byte page in that block. Since there are only 64
     *  256-byte pages inside of 16k, the value in $dffe ranges from 0 to 63."
     */

    isize bankOffset = (bank * 16384) % getRamCapacity();
    isize pageOffset = (page & 0x3F) * 256;
    return bankOffset + pageOffset + addr;
}

}
