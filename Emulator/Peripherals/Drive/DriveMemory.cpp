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
#include "DriveMemory.h"
#include "Emulator.h"
#include "Checksum.h"
#include "IOUtils.h"

namespace vc64 {

DriveMemory::DriveMemory(C64 &ref, Drive &dref) : SubComponent(ref), drive(dref)
{
    updateBankMap();
}

u16
DriveMemory::romSize() const
{
    // Check for a 16KB Rom (by checking for a mirrored byte)
    if (rom[0x0000] != 0 && rom[0x0000] == rom[0x4000]) return 0x4000;

    // Check for a 24KB Rom
    if (rom[0x0000] == 0 && rom[0x2000] != 0) return 0x6000;

    // Check for a 32KB Rom
    if (rom[0x0000] != 0 && rom[0x0000] != rom[0x4000]) return 0x8000;

    // No Rom installed
    return 0;
}

u16
DriveMemory::romAddr() const
{
    // Check for a 16KB Rom (by checking for a mirrored byte)
    if (rom[0x0000] != 0 && rom[0x0000] == rom[0x4000]) return 0xC000;

    // Check for a 24KB Rom
    if (rom[0x0000] == 0 && rom[0x2000] != 0) return 0xA000;

    // Check for a 32KB Rom
    if (rom[0x0000] != 0 && rom[0x0000] != rom[0x4000]) return 0x8000;

    // No Rom installed
    return 0;
}

u32
DriveMemory::romCRC32() const
{
    isize size = romSize();
    isize offset = romAddr() & 0x7FFF;
    
    return size ? util::crc32(rom + offset, size) : 0;
}

u64
DriveMemory::romFNV64() const
{
    isize size = romSize();
    isize offset = romAddr() & 0x7FFF;
    
    return size ? util::fnv64(rom + offset, size) : 0;
}

void
DriveMemory::deleteRom()
{
    memset(rom, 0, sizeof(rom));
    updateBankMap();
}

void
DriveMemory::loadRom(const RomFile &file)
{
    loadRom(file.data.ptr, file.data.size);
}

void
DriveMemory::loadRom(const u8 *buf, isize size)
{
    deleteRom();

    switch (size) {
            
        case 0x4000: // 16KB Roms are mapped to 0xC000 with a mirror at 0x8000
            
            for (isize i = 0; i < size; i++) rom[0x4000 + i] = buf[i];
            for (isize i = 0; i < size; i++) rom[0x0000 + i] = buf[i];
            break;

        case 0x6000: // 24KB Roms are mapped to 0xA000

            for (isize i = 0; i < size; i++) rom[0x2000 + i] = buf[i];
            break;

        case 0x8000: // 32KB Roms are mapped to 0x8000

            for (isize i = 0; i < size; i++) rom[0x0000 + i] = buf[i];
            break;
    }
    
    // Update the current configuration in auto-update mode
    if (drive.config.autoConfig) drive.autoConfigure();
    
    updateBankMap();
}

void
DriveMemory::saveRom(const fs::path &path)
{
    u16 addr = romAddr();
    u16 size = romSize();

    debug(true, "Saving Rom at %x (%x bytes)\n", addr, size);
    
    RomFile file = RomFile(rom + (addr & 0x7FFF), size);
    file.writeToFile(path);
}

u8
DriveMemory::peek(u16 addr)
{
    u8 result;
    
    switch (usage[addr >> 10]) {
            
        case DRVMEM_NONE:
            
            result = addr >> 8 & 0x1F;
            break;
            
        case DRVMEM_RAM:
            
            result = ram[addr & 0x07FF];
            break;
            
        case DRVMEM_EXP:
            
            result = ram[addr];
            break;
            
        case DRVMEM_ROM:
            
            result = rom[addr & 0x7FFF];
            break;

        case DRVMEM_VIA1:
            
            result = drive.via1.peek(addr & 0xF);
            break;
            
        case DRVMEM_VIA2:
            
            result = drive.via2.peek(addr & 0xF);
            break;

        case DRVMEM_PIA:
            
            result = drive.pia.peek(addr);
            break;
            
        default:
            fatalError;
    }
    
    return result;
}

u8
DriveMemory::spypeek(u16 addr) const
{
    u8 result;
    
    switch (usage[addr >> 10]) {
            
        case DRVMEM_NONE:
            
            result = addr >> 8 & 0x1F;
            break;
            
        case DRVMEM_RAM:
            
            result = ram[addr & 0x07FF];
            break;
            
        case DRVMEM_EXP:
            
            result = ram[addr];
            break;
            
        case DRVMEM_ROM:
            
            result = rom[addr & 0x7FFF];
            break;

        case DRVMEM_VIA1:
            
            result = drive.via1.spypeek(addr & 0xF);
            break;
            
        case DRVMEM_VIA2:
            
            result = drive.via2.spypeek(addr & 0xF);
            break;
            
        case DRVMEM_PIA:
            
            result = drive.pia.spypeek(addr);
            break;

        default:
            fatalError;
    }

    return result;
}

void 
DriveMemory::poke(u16 addr, u8 value)
{
    switch (usage[addr >> 10]) {

        case DRVMEM_RAM:
            
            ram[addr & 0x07FF] = value;
            break;
            
        case DRVMEM_EXP:
            
            ram[addr] = value;
            break;
            
        case DRVMEM_VIA1:
            
            drive.via1.poke(addr & 0xF, value);
            break;
            
        case DRVMEM_VIA2:
            
            drive.via2.poke(addr & 0xF, value);
            break;
            
        case DRVMEM_PIA:
            
            drive.pia.poke(addr, value);
            break;

        default:
            break;
    }
}

void
DriveMemory::updateBankMap()
{
    auto config = drive.getConfig();
    
    // Start from scratch
    for (isize i = 0; i < 64; i++) usage[i] = DRVMEM_NONE;
    
    // Add built-in RAM and IO chips
    for (isize bank = 0; bank < 32; bank += 8) {
        
        usage[bank + 0] = DRVMEM_RAM;
        usage[bank + 1] = DRVMEM_RAM;
        usage[bank + 6] = DRVMEM_VIA1;
        usage[bank + 7] = DRVMEM_VIA2;
    }
    
    // Add ROMs
    switch (romSize()) {
            
        case 0x4000: // 16KB (ROM is mirrored)
            for (isize i = 32; i < 64; i++) usage[i] = DRVMEM_ROM;
            break;
            
        case 0x6000: // 24KB
            for (isize i = 40; i < 64; i++) usage[i] = DRVMEM_ROM;
            break;
            
        case 0x8000: // 32KB
            for (isize i = 32; i < 64; i++) usage[i] = DRVMEM_ROM;
            break;
    }
    
    // Add expansion RAM
    switch (config.ram) {
            
        case DRVRAM_8000_9FFF:
            for (isize i = 32; i < 40; i++) usage[i] = DRVMEM_EXP;
            break;

        case DRVRAM_6000_7FFF:
            for (isize i = 24; i < 32; i++) usage[i] = DRVMEM_EXP;
            break;

        default:
            break;
    }
    
    // Map the PIA (Dolphin DOS 3) $5000 - $5FFF
    if (config.parCable == PAR_CABLE_DOLPHIN3) {
        
        for (isize i = 20; i < 24; i++) usage[i] = DRVMEM_PIA;
    }
}

}
