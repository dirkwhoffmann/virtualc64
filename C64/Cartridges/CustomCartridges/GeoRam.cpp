/*!
 * @file        GeoRam.cpp
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

GeoRAM::GeoRAM(C64 *c64) : Cartridge(c64)
{
    setDescription("GeoRAM");
}

void
GeoRAM::reset()
{
    if (!getPersistentRam()) {
        debug("Erasing GeoRAM\n");
        eraseRAM(0);
    } else {
        debug("Preserving GeoRAM\n");
    }
}

size_t
GeoRAM::stateSize()
{
    return Cartridge::stateSize() + 2;
}

void
GeoRAM::didLoadFromBuffer(uint8_t **buffer)
{
    bank = read8(buffer);
    page = read8(buffer);
}

void
GeoRAM::didSaveToBuffer(uint8_t **buffer)
{
    write8(buffer, bank);
    write8(buffer, page);
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
    
    unsigned bankOffset = (bank * 16384) % getRamCapacity();
    unsigned pageOffset = (page & 0x3F) * 256;
    return bankOffset + pageOffset + addr;
}

uint8_t
GeoRAM::peekIO1(uint16_t addr)
{
    assert(addr >= 0xDE00 && addr <= 0xDEFF);
    return peekRAM(offset(addr - 0xDE00));
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
    pokeRAM(offset(addr - 0xDE00), value);
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
