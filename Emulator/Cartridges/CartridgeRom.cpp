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
CartridgeRom::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
}

void 
CartridgeRom::newserialize(util::SerCounter &worker)
{
    serialize(worker);
    worker.count += size;
}

void 
CartridgeRom::newserialize(util::SerResetter &worker)
{
    assert(false); // TODO
}

void 
CartridgeRom::newserialize(util::SerReader &worker)
{
    serialize(worker);

    // Delete the old packet and create a new one with the proper size
    if (rom) delete[] rom;
    rom = new u8[size];

    // Read packet data
    for (int i = 0; i < size; i++) rom[i] = util::read8(worker.ptr);
}

void 
CartridgeRom::newserialize(util::SerWriter &worker)
{
    serialize(worker);

    // Write packet data
    for (int i = 0; i < size; i++) util::write8(worker.ptr, rom[i]);
}

isize
CartridgeRom::_size()
{
    util::SerCounter counter;
    serialize(counter);
    
    return size + counter.count;
}

isize
CartridgeRom::_load(const u8 *buffer)
{
    util::SerReader reader(buffer);
    serialize(reader);
    
    // Delete the old packet and create a new one with the proper size
    if (rom) delete[] rom;
    rom = new u8[size];
    
    // Read packet data
    for (int i = 0; i < size; i++) rom[i] = util::read8(reader.ptr);

    trace(SNP_DEBUG, "Recreated from %ld bytes\n", isize(reader.ptr - buffer));
    return isize(reader.ptr - buffer);
}

isize
CartridgeRom::_save(u8 *buffer)
{
    util::SerWriter writer(buffer);
    serialize(writer);

    // Write packet data
    for (int i = 0; i < size; i++) util::write8(writer.ptr, rom[i]);

    return isize(writer.ptr - buffer);
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
