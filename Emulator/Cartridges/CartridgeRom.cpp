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
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // Internal state
        { &size,        sizeof(size),        KEEP_ON_RESET },
        { &loadAddress, sizeof(loadAddress), KEEP_ON_RESET },
        { NULL,         0,                   0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

CartridgeRom::CartridgeRom(u16 size, u16 loadAddress, const u8 *buffer) : CartridgeRom()
{
    this->size = size;
    this->loadAddress = loadAddress;
    rom = new u8[size];
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
CartridgeRom::didLoadFromBuffer(u8 **buffer)
{
    if (rom) delete[] rom;
    rom = new u8[size];
    
    readBlock(buffer, rom, size);
}

void
CartridgeRom::didSaveToBuffer(u8 **buffer)
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

u8
CartridgeRom::peek(u16 addr)
{
    assert(addr < size);
    return rom[addr];
}
