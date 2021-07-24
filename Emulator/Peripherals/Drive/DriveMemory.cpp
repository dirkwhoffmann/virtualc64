// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DriveMemory.h"
#include "C64.h"
#include "Checksum.h"
#include "IO.h"

DriveMemory::DriveMemory(C64 &ref, Drive &dref) : C64Component(ref), drive(dref)
{
    for (isize bank = 0; bank < 32; bank += 8) {

        usage[bank + 0] = DRVMEM_RAM;
        usage[bank + 1] = DRVMEM_RAM;
        usage[bank + 6] = DRVMEM_VIA1;
        usage[bank + 7] = DRVMEM_VIA2;
    }
}

void 
DriveMemory::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)

    // Initialize RAM with powerup pattern (pattern from Hoxs64)
    for (unsigned i = 0; i < sizeof(ram); i++) {
        mem[i] = (i & 64) ? 0xFF : 0x00;
    }

    for (unsigned i = 0; i < 0x0800; i++) {
        mem[i] = (i & 64) ? 0xFF : 0x00;
    }
}

void
DriveMemory::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::BankMap) {
        
        DrvMemType oldsrc = usage[0];
        isize oldi = 0;

        for (isize i = 0; i <= 64; i++) {
            DrvMemType newsrc = i < 64 ? usage[i] : (DrvMemType)-1;
            if (oldsrc != newsrc) {
                os << "        ";
                os << util::hex((u16)(oldi << 10)) << " - ";
                os << util::hex((u16)((i << 10) - 1)) << " : ";
                os << DrvMemTypeEnum::key(oldsrc) << std::endl;
                oldsrc = newsrc; oldi = i;
            }
        }
    }
    
    if (category & dump::State) {
        
        os << tab("Drive ROM");
        os << bol(c64.hasRom(ROM_TYPE_VC1541)) << std::endl;
    }
}

u16
DriveMemory::romAddr() const
{
    for (isize i = 0; i < 64; i++) {
        if (usage[i] == DRVMEM_ROM) return (u16)(i * 1024);
    }
    return 0;
}

u16
DriveMemory::romSize() const
{
    u16 result = 0;
    
    for (isize i = 0; i < 64; i++) {
        if (usage[i] == DRVMEM_ROM) result += 1024;
    }
    return result;
}

u32
DriveMemory::romCRC32() const
{
    return hasRom() ? util::crc32(mem + romAddr(), romSize()) : 0;
}

u64
DriveMemory::romFNV64() const
{
    return hasRom() ? util::fnv_1a_64(mem + romAddr(), romSize()) : 0;
}

void
DriveMemory::deleteRom()
{
    for (isize i = 0; i < 64; i++) {
        if (usage[i] == DRVMEM_ROM || usage[i] == DRVMEM_ROM_C000) {
            usage[i] = DRVMEM_NONE;
        }
    }
}

void
DriveMemory::loadRom(const RomFile *file)
{
    assert(file != nullptr);
    
    loadRom(file->data, file->size);
}

void
DriveMemory::loadRom(const u8 *buf, isize size, u16 addr)
{
    assert(buf != nullptr);
    assert(addr + size <= 0x10000);

    // Delete old ROM (if any)
    deleteRom();

    // Install new ROM
    for (isize i = 0; i < size; i++) {
        mem[addr + i] = buf[i];
    }
    
    // Update bank map
    for (isize i = 0; i < size; i += 1024) {
        usage[(addr + i) >> 10] = DRVMEM_ROM;
    }
}

void
DriveMemory::loadRom(const u8 *buf, isize size)
{
    // 16KB Roms are mapped to 0xC000 with a mirror at 0x8000
    if (size == 0x4000) {
        loadRom(buf, size, 0xC000);
        for (isize i = 32; i < 48; i++) usage[i] = DRVMEM_ROM_C000;
    }
    
    // 24KB Roms are mapped to 0xA000
    if (size == 0x6000) {
        loadRom(buf, size, 0xA000);
    }

    // 32KB Roms are mapped to 0x8000
    if (size == 0x8000) {
        loadRom(buf, size, 0x8000);
    }
}

void
DriveMemory::saveRom(const string &path)
{
    u16 addr = romAddr();
    u16 size = romSize();
            
    debug(true, "Saving Rom at %x (%x bytes)\n", addr, size);
    
    RomFile *file = RomFile::make <RomFile> (mem + addr, size);
    file->writeToFile(path);
    delete file;
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
            result = mem[addr];
            break;

        case DRVMEM_ROM_C000:
            result = mem[addr | 0xC000];
            break;

        case DRVMEM_VIA1:
            result = drive.via1.peek(addr & 0xF);
            break;
            
        case DRVMEM_VIA2:
            result = drive.via2.peek(addr & 0xF);
            break;
            
        default:
            assert(false);
            return 0;
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
            result = mem[addr];
            break;
            
        case DRVMEM_ROM_C000:
            result = mem[addr | 0xC000];
            break;

        case DRVMEM_VIA1:
            result = drive.via1.spypeek(addr & 0xF);
            break;
            
        case DRVMEM_VIA2:
            result = drive.via2.spypeek(addr & 0xF);
            break;
            
        default:
            assert(false);
            return 0;
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
            
        default:
            break;
    }
}
