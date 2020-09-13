// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

PageFox::PageFox(C64 &ref) : Cartridge(ref, "PageFox")
{
    // Allocate 32KB RAM
    setRamCapacity(0x8000);
}

void
PageFox::_reset()
{
    /*
    RESET_SNAPSHOT_ITEMS
    Cartridge::_reset();
    
    eraseRAM(0);
    */
}

void
PageFox::_dump()
{
    Cartridge::_dump();
    
    msg("PageFox\n");
    msg("-------\n\n");
    
    msg("  bankSelect() : %x\n", bankSelect());
    msg("  chipSelect() : %x\n", chipSelect());
    msg("        bank() : %x\n", bank());
    msg("     disable() : %x\n", disabled());    
}

void
PageFox::resetCartConfig()
{
    pokeIO1(0xDE80, 0);
}

u8
PageFox::peekRomL(u16 addr)
{
    u8 result;
    
    if (ramIsVisible()) {
        result = peekRAM(ramAddrL(addr));
        plaindebug(CRT_DEBUG, "peekRomL(%x) [RAM] = %x\n", addr, result);
    } else {
        result = Cartridge::peekRomL(addr);
        plaindebug(CRT_DEBUG, "peekRomL(%x) = %x\n", addr, result);
    }
    
    return result;
}

u8
PageFox::peekRomH(u16 addr)
{
    u8 result;
    
    if (ramIsVisible()) {
        result = peekRAM(ramAddrH(addr));
        plaindebug(CRT_DEBUG, "peekRomH(%x) [RAM] = %x\n", addr, result);
    } else {
        result = Cartridge::peekRomH(addr);
        plaindebug(CRT_DEBUG, "peekRomH(%x) = %x\n", addr, result);
    }

    return result;
}

void
PageFox::pokeRomL(u16 addr, u8 value)
{

    if (chipSelect() == 0b10) {
        plaindebug(CRT_DEBUG, "pokeRomL [RAM] (%x, %x)\n", addr, value);
        pokeRAM(ramAddrL(addr), value);
    } else {
        plaindebug(CRT_DEBUG, "pokeRomL(%x, %x)\n", addr, value);
    }
    mem.ram[0x8000 + addr] = value;
}

void
PageFox::pokeRomH(u16 addr, u8 value)
{
    if (chipSelect() == 0b10) {
        plaindebug(CRT_DEBUG, "pokeRomH [RAM] (%x, %x)\n", addr, value);
        pokeRAM(ramAddrH(addr), value);
    } else {
        plaindebug(CRT_DEBUG, "pokeRomH(%x, %x)\n", addr, value);
    }
    mem.ram[0xA000 + addr] = value;
}

u8
PageFox::peekIO1(u16 addr)
{
    u8 result = ctrlReg & 0b11111;
    plaindebug(CRT_DEBUG, "peekIO1(%x) = %x\n", addr, result);
    return result;
}

void
PageFox::pokeIO1(u16 addr, u8 value)
{
    plaindebug(CRT_DEBUG, "pokeIO1(%x, %x)\n", addr, value);

    ctrlReg = value;
    
    if (disabled()) {
        expansionport.setCartridgeMode(CRT_OFF);
        plaindebug(CRT_DEBUG, "CRT disabled\n");
    } else {
        expansionport.setCartridgeMode(CRT_16K);
        plaindebug(CRT_DEBUG, "16K mode\n");
    }
    
    bankIn(chipSelect());
    mem.updatePeekPokeLookupTables();
    
    // c64.signalStop();
}

void
PageFox::updatePeekPokeLookupTables()
{
    if (ramIsVisible()) {
        mem.pokeTarget[0x8] = mem.pokeTarget[0x9] = M_CRTLO;
        mem.pokeTarget[0xA] = mem.pokeTarget[0xB] = M_CRTHI;
    }
}
