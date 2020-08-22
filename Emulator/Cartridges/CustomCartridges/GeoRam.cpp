// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

void
GeoRAM::_reset()
{
    if (!getPersistentRam()) {
        debug(CRT_DEBUG, "Erasing GeoRAM\n");
        eraseRAM(0);
    } else {
        debug(CRT_DEBUG, "Preserving GeoRAM\n");
    }
}

size_t
GeoRAM::oldStateSize()
{
    return Cartridge::oldStateSize() + 2;
}

void
GeoRAM::oldDidLoadFromBuffer(u8 **buffer)
{
    Cartridge::oldDidLoadFromBuffer(buffer);
    bank = read8(buffer);
    page = read8(buffer);
}

void
GeoRAM::oldDidSaveToBuffer(u8 **buffer)
{
    Cartridge::oldDidSaveToBuffer(buffer);
    write8(buffer, bank);
    write8(buffer, page);
}

u8
GeoRAM::peekIO1(u16 addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    return peekRAM(offset(addr - 0xDE00));
}

u8
GeoRAM::peekIO2(u16 addr)
{
    return 0;
}

void
GeoRAM::pokeIO1(u16 addr, u8 value)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    pokeRAM(offset(addr - 0xDE00), value);
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

unsigned
GeoRAM::offset(u8 addr)
{
    /* From VICE:
     * "The GeoRAM is a banked memory system. It uses the registers at
     *  $dffe and $dfff to determine what part of the GeoRAM memory should
     *  be mapped to $de00-$deff.
     *  The register at $dfff selects which 16k block to map, and $dffe
     *  selects a 256-byte page in that block. Since there are only 64
     *  256-byte pages inside of 16k, the value in $dffe ranges from 0 to 63."
     */
    
    unsigned bankOffset = (bank * 16384) % getRamCapacity();
    unsigned pageOffset = (page & 0x3F) * 256;
    return bankOffset + pageOffset + addr;
}
