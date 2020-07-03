// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
    return HardwareComponent::stateSize() + size;
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
