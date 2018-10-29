/*!
 * @file        CartridgeRom.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
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

#include "CartridgeRom.h"

/*
CartridgeRom::CartridgeRom(uint16_t sizeInBytes)
{
    size = sizeInBytes;
    rom = new uint8_t[sizeInBytes];
}
*/

CartridgeRom::CartridgeRom(uint8_t **buffer)
{
    assert(buffer != NULL);
    size = read16(buffer);
    rom = new uint8_t[size];
    readBlock(buffer, rom, size);
}

CartridgeRom::CartridgeRom(uint16_t _size, uint16_t _loadAddress, const uint8_t *buffer)
{
    size = _size;
    loadAddress = _loadAddress;
    rom = new uint8_t[size];
    if (buffer) {
        memcpy(rom, buffer, size);
    }
}

CartridgeRom::~CartridgeRom()
{
    assert(rom != NULL);
    delete[] rom;
}

size_t
CartridgeRom::stateSize()
{
    return 2 + 2 + size;
}

void
CartridgeRom::loadFromBuffer(uint8_t **buffer)
{
    if (rom != NULL) {
        assert(size > 0);
        delete[] rom;
    }
    
    size = read16(buffer);
    loadAddress = read16(buffer);
    rom = new uint8_t[size];
    readBlock(buffer, rom, size);
}

void
CartridgeRom::saveToBuffer(uint8_t **buffer)
{
    write16(buffer, size);
    write16(buffer, loadAddress);
    writeBlock(buffer, rom, size);
}

bool
CartridgeRom::mapsToL() {
    assert(rom != NULL);
    return loadAddress == 0x8000 && size <= 0x2000;
}

bool
CartridgeRom::mapsToLH() {
    assert(rom != NULL);
    return loadAddress == 0x8000 && size > 0x2000;
}

bool
CartridgeRom::mapsToH() {
    assert(rom != NULL);
    return loadAddress == 0xA000 || loadAddress == 0xE000;
}

uint8_t
CartridgeRom::peek(uint16_t addr)
{
    assert(addr < size);
    return rom[addr];
}
