/*!
 * @file        CartridgeRom.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
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

CartridgeRom::CartridgeRom()
{
    setDescription("CartridgeRom");
    debug(3, "  Creating cartridge Rom at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Internal state
        { &size,        sizeof(size),        KEEP_ON_RESET },
        { &loadAddress, sizeof(loadAddress), KEEP_ON_RESET },
        { NULL,         0,                   0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

/*
CartridgeRom::CartridgeRom(uint8_t **buffer) : CartridgeRom()
{
    assert(buffer != NULL);
    loadFromBuffer(buffer);
}
*/

CartridgeRom::CartridgeRom(uint16_t size, uint16_t loadAddress, const uint8_t *buffer) : CartridgeRom()
{
    this->size = size;
    this->loadAddress = loadAddress;
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
    return VirtualComponent::stateSize() + size;
}

void
CartridgeRom::didLoadFromBuffer(uint8_t **buffer)
{
    if (rom) delete[] rom;
    rom = new uint8_t[size];
    
    readBlock(buffer, rom, size);
}

void
CartridgeRom::didSaveToBuffer(uint8_t **buffer)
{
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
