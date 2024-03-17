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
#include "CartridgeRom.h"

namespace vc64 {

CartridgeRom::CartridgeRom(C64 &ref) : SubComponent(ref)
{
}

CartridgeRom::CartridgeRom(C64 &ref, u16 size, u16 loadAddress, const u8 *buffer) : CartridgeRom(ref)
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
    assert(rom);
    delete[] rom;
}

void
CartridgeRom::operator << (SerCounter &worker)
{
    serialize(worker);
    worker.count += size;
}

void
CartridgeRom::operator << (SerReader &worker)
{
    serialize(worker);
    
    // Delete the old packet and create a new one with the proper size
    if (rom) delete[] rom;
    rom = new u8[size];
    
    // Read packet data
    for (int i = 0; i < size; i++) rom[i] = read8(worker.ptr);
}

void
CartridgeRom::operator << (SerWriter &worker)
{
    serialize(worker);
    
    // Write packet data
    for (int i = 0; i < size; i++) write8(worker.ptr, rom[i]);
}

bool
CartridgeRom::mapsToL() const {
    assert(rom);
    return loadAddress == 0x8000 && size <= 0x2000;
}

bool
CartridgeRom::mapsToLH() const {
    assert(rom);
    return loadAddress == 0x8000 && size > 0x2000;
}

bool
CartridgeRom::mapsToH() const {
    assert(rom);
    return loadAddress == 0xA000 || loadAddress == 0xE000;
}

u8
CartridgeRom::peek(u16 addr)
{
    assert(addr < size);
    return rom[addr];
}

u8
CartridgeRom::spypeek(u16 addr) const
{
    assert(addr < size);
    return rom[addr];
}

}
