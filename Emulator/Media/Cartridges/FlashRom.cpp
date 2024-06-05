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
#include "FlashRom.h"
#include "IOUtils.h"

namespace vc64 {

const char *
FlashRom::getStateAsString(FlashState state)
{
    switch(state) {

        case FLASH_READ:return "FLASH_READ";
        case FLASH_MAGIC_1: return "FLASH_MAGIC_1";
        case FLASH_MAGIC_2: return "FLASH_MAGIC_2";
        case FLASH_AUTOSELECT: return "FLASH_AUTOSELECT";
        case FLASH_BYTE_PROGRAM: return "FLASH_BYTE_PROGRAM";
        case FLASH_BYTE_PROGRAM_ERROR: return "FLASH_BYTE_PROGRAM_ERROR";
        case FLASH_ERASE_MAGIC_1: return "FLASH_ERASE_MAGIC_1";
        case FLASH_ERASE_MAGIC_2: return "FLASH_ERASE_MAGIC_2";
        case FLASH_ERASE_SELECT: return "FLASH_ERASE_SELECT";
        case FLASH_CHIP_ERASE: return "FLASH_CHIP_ERASE";
        case FLASH_SECTOR_ERASE: return "FLASH_SECTOR_ERASE";
        case FLASH_SECTOR_ERASE_TIMEOUT: return "FLASH_SECTOR_ERASE_TIMEOUT";
        case FLASH_SECTOR_ERASE_SUSPEND: return "FLASH_SECTOR_ERASE_SUSPEND";

        default:
            fatalError;
    }
}

FlashRom::FlashRom(C64 &ref) : SubComponent(ref)
{
    state = FLASH_READ;
    baseState = FLASH_READ;

    rom = new u8[romSize];
    memset(rom, 0xFF, romSize);
}

FlashRom::~FlashRom()
{
    delete [] rom;
}

void
FlashRom::loadBank(isize bank, u8 *data)
{
    assert(data);
    memcpy(rom + (u32)bank * 0x2000, data, 0x2000);
}

void
FlashRom::_reset(bool hard)
{
    state = FLASH_READ;
    baseState = FLASH_READ;
}

void
FlashRom::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << std::endl;

        os << tab("Flash ROM State");
        os << FlashStateEnum::key(state) << std::endl;
        os << tab("Base State");
        os << FlashStateEnum::key(baseState) << std::endl;
    }
}

void
FlashRom::operator << (SerCounter &worker)
{
    serialize(worker);
    worker.count += romSize;
}

void
FlashRom::operator << (SerReader &worker)
{
    serialize(worker);
    worker.copy(rom, romSize);
}

void
FlashRom::operator << (SerWriter &worker)
{
    serialize(worker);
    worker.copy(rom, romSize);
}

u8
FlashRom::peek(u32 addr)
{
    return const_cast<const FlashRom*>(this)->spypeek(addr);
}

u8
FlashRom::peek(isize bank, u16 addr)
{
    assert(isBankNumber(bank));
    return peek((u32)bank * 0x2000 + addr);
}

u8
FlashRom::spypeek(u32 addr) const
{
    assert(addr < romSize);

    u8 result;

    switch (state) {

        case FLASH_AUTOSELECT:

            switch(addr & 0xFF) {

                case 0:
                    return 0x01; // Manufacturer ID

                case 1:
                    return 0xA4; // Device ID

                case 2:
                    return 0;
            }
            return rom[addr];

        case FLASH_BYTE_PROGRAM_ERROR:

            // TODO
            result = rom[addr];
            break;

        case FLASH_SECTOR_ERASE_SUSPEND:

            // TODO
            result = rom[addr];
            break;

        case FLASH_CHIP_ERASE:

            // TODO
            result = rom[addr];
            break;

        case FLASH_SECTOR_ERASE:

            // TODO
            result = rom[addr];
            break;

        case FLASH_SECTOR_ERASE_TIMEOUT:

            // TODO
            result = rom[addr];
            break;

        default:

            // TODO
            result = rom[addr];
            break;
    }

    return result;
}

u8
FlashRom::spypeek(isize bank, u16 addr) const
{
    assert(isBankNumber(bank));
    return spypeek((u32)bank * 0x2000 + addr);
}

void
FlashRom::poke(u32 addr, u8 value)
{
    assert(addr < romSize);

    switch (state) {

        case FLASH_READ:

            if (firstCommandAddr(addr) && value == 0xAA) {

                state = FLASH_MAGIC_1;
                trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                return;
            }
            return;

        case FLASH_MAGIC_1:

            if (secondCommandAddr(addr) && value == 0x55) {

                state = FLASH_MAGIC_2;
                trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                return;
            }

            state = baseState;
            trace(CRT_DEBUG, "Back to %s\n", getStateAsString(state));
            return;

        case FLASH_MAGIC_2:

            if (firstCommandAddr(addr)) {

                switch(value) {

                    case 0xF0:

                        state = FLASH_READ;
                        baseState = FLASH_READ;
                        trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                        return;

                    case 0x90:

                        state = FLASH_AUTOSELECT;
                        baseState = FLASH_AUTOSELECT;
                        trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                        return;

                    case 0xA0:
                        state = FLASH_BYTE_PROGRAM;
                        trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                        return;

                    case 0x80:
                        state = FLASH_ERASE_MAGIC_1;
                        trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                        return;
                }
            }

            state = baseState;
            trace(CRT_DEBUG, "Back to %s\n", getStateAsString(state));
            break;

        case FLASH_BYTE_PROGRAM:

            if (!doByteProgram(addr, value)) {

                state = FLASH_BYTE_PROGRAM_ERROR;
                trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                return;
            }

            state = baseState;
            trace(CRT_DEBUG, "Back to %s\n", getStateAsString(state));
            return;

        case FLASH_ERASE_MAGIC_1:

            // TODO
            break;

        case FLASH_ERASE_MAGIC_2:

            // TODO
            break;

        case FLASH_ERASE_SELECT:

            // TODO
            break;

        case FLASH_SECTOR_ERASE_TIMEOUT:

            // TODO
            break;

        case FLASH_SECTOR_ERASE:

            // TODO
            break;

        case FLASH_SECTOR_ERASE_SUSPEND:

            // TODO
            break;

        case FLASH_BYTE_PROGRAM_ERROR:
        case FLASH_AUTOSELECT:

            if (addr == 0x5555 && value == 0xAA) {

                state = FLASH_MAGIC_1;
                trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                return;
            }
            if (value == 0xF0) {

                state = FLASH_READ;
                baseState = FLASH_READ;
                trace(CRT_DEBUG, "%s\n", getStateAsString(state));
                return;
            }
            return;

        case FLASH_CHIP_ERASE:
        default:

            // TODO
            break;
    }
}

void 
FlashRom::poke(isize bank, u16 addr, u8 value)
{
    assert(isBankNumber(bank)); 
    poke((u32)bank * 0x2000 + addr, value);
}

bool
FlashRom::doByteProgram(u32 addr, u8 value)
{
    assert(addr < romSize);

    rom[addr] &= value;
    return rom[addr] == value;
}

bool 
FlashRom::doByteProgram(isize bank, u16 addr, u8 value)
{
    assert(isBankNumber(bank));
    return doByteProgram((u32)bank * 0x2000 + addr, value);
}

void
FlashRom::doChipErase() {

    trace(CRT_DEBUG, "Erasing chip ...\n");
    memset(rom, 0xFF, romSize);
}

void
FlashRom::doSectorErase(u32 addr)
{
    assert(addr < romSize);

    trace(CRT_DEBUG, "Erasing sector %d\n", addr >> 4);
    memset(rom + (addr & 0x0000), 0xFF, sectorSize);
}

void 
FlashRom::doSectorErase(isize bank, u16 addr) 
{
    assert(isBankNumber(bank)); 
    doSectorErase((u32)bank * 0x2000 + addr);
}

}
