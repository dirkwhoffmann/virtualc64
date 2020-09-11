// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

EasyFlash::EasyFlash(C64 &ref) : Cartridge(ref, "EasyFlash")
{
    subComponents = vector <HardwareComponent *> {
        
        &flashRomL,
        &flashRomH
    };
    
    flashRomL.setDescription("FlashRom_L");
    flashRomH.setDescription("FlashRom_H");

    // Allocate 256 bytes on-board RAM
    setRamCapacity(256);
}

void
EasyFlash::resetCartConfig()
{
    expansionport.setCartridgeMode(CRT_ULTIMAX);
}

void
EasyFlash::_reset()
{
    RESET_SNAPSHOT_ITEMS
    Cartridge::_reset();
    
    eraseRAM(0);
    
    // Make sure peekRomL() and peekRomH() conver the whole range
    mappedBytesL = 0x2000;
    mappedBytesH = 0x2000;
}

void
EasyFlash::_dump()
{
    Cartridge::dump();
    
    msg("EasyFlash\n");
    msg("---------\n\n");

    msg("bank = %d\n", bank);
    for (unsigned i = 0; i < 256; i++) {
        msg("%02X ", peekRAM(i));
        if (i % 16 == 15) msg("\n");
    }
    msg("\n");
    
    flashRomL.dump();
    flashRomH.dump();
}

void
EasyFlash::loadChip(unsigned nr, CRTFile *c)
{
    u16 chipSize = c->chipSize(nr);
    u16 chipAddr = c->chipAddr(nr);
    u16 chipBank = c->chipBank(nr);
    u8 *chipData = c->chipData(nr);
    
    if(chipSize != 0x2000) {
        warn("Package %d has chip size %04X. Expected 0x2000.\n", nr, chipSize);
        return;
    }

    if (isROMLaddr(chipAddr)) {
            
        debug(CRT_DEBUG, "Loading Rom bank %dL ...\n", chipBank);
        flashRomL.loadBank(chipBank, chipData);
        bank++;
    
    } else if (isROMHaddr(chipAddr)) {

        debug(CRT_DEBUG, "Loading Rom bank %dH ...\n", bank / 2);
        flashRomH.loadBank(chipBank, chipData);
        bank++;
        
    } else {
        
        warn("Package %d has an invalid load address (%04X).", nr, chipAddr);
        return;
    }
}

u8
EasyFlash::peek(u16 addr)
{
    u8 result;
    
    if (isROMLaddr(addr)) {
        result = flashRomL.peek(bank, addr & 0x1FFF);
        // debug(CRT_DEBUG, "peekL(%x) = %x\n", addr, result);
        return result;
        
    } else if (isROMHaddr(addr)) {
        result = flashRomH.peek(bank, addr & 0x1FFF);
        // debug(CRT_DEBUG, "peekH(%x) = %x\n", addr, result);
        return result;
        
    } else {
        assert(false);
        return 0;
    }
}

void
EasyFlash::poke(u16 addr, u8 value)
{
    Cartridge::poke(addr, value);
}

void
EasyFlash::pokeRomL(u16 addr, u8 value)
{
    plaindebug(CRT_DEBUG, "pokeRomL(%x, %x)\n", addr, value);
    flashRomL.poke(bank, addr & 0x1FFF, value);
}

void
EasyFlash::pokeRomH(u16 addr, u8 value)
{
    plaindebug(CRT_DEBUG, "pokeRomH(%x, %x)\n", addr, value);
    flashRomH.poke(bank, addr & 0x1FFF, value);
}

u8
EasyFlash::peekIO1(u16 addr)
{
    u8 result = (addr & 2) ? (modeReg & 0x87) : bankReg;
    debug(CRT_DEBUG, "peekIO1(%x): %x\n", addr & 0xFF, result);
    return result;
}

u8
EasyFlash::peekIO2(u16 addr)
{
    u8 result = peekRAM(addr & 0xFF);
    debug(CRT_DEBUG, "peekIO2(%x): %x\n", addr & 0xFF, result);
    
    return result;
}

void
EasyFlash::pokeIO1(u16 addr, u8 value)
{
    debug(CRT_DEBUG, "pokeIO1(%x,%x)\n", addr & 0xFF, value);
    
    (addr & 2) ? pokeModeReg(value) : pokeBankReg(value);
}

void
EasyFlash::pokeIO2(u16 addr, u8 value)
{
    debug(CRT_DEBUG, "pokeIO2(%x,%x)\n", addr & 0xFF, value);

    pokeRAM(addr & 0xFF, value);
}

void
EasyFlash::pokeBankReg(u8 value)
{
    bankReg = value;
    bank = value & 0x3F;
    plaindebug(CRT_DEBUG, "Switching to bank %d\n", bank);
}

void
EasyFlash::pokeModeReg(u8 value)
{
    modeReg = value;
    // c64.signalBreakpoint();
    
    setLED((value & 0x80) != 0);
    
    u8 MXG = value & 0x07;
    /* MXG
     * 0 (000) : GAME from jumper, EXROM high (i.e. Ultimax or Off)
     * 1 (001) : Reserved, don’t use this
     * 2 (010) : GAME from jumper, EXROM low (i.e. 16K or 8K)
     * 3 (011) : Reserved, don’t use this
     * 4 (100) : Cartridge ROM off (RAM at $DF00 still available)
     * 5 (101) : Ultimax (Low bank at $8000, high bank at $e000)
     * 6 (110) : 8k Cartridge (Low bank at $8000)
     * 7 (111) : 16k cartridge (Low bank at $8000, high bank at $a000)
     */
    
    bool exrom;
    bool game;
    
    plaindebug(CRT_DEBUG, "MXG = %x\n", MXG);
    switch (MXG) {
            
        case 0b000:
        case 0b001:
            game = jumper;
            exrom = 1;
            break;
            
        case 0b010:
        case 0b011:
            game = jumper;
            exrom = 0;
            break;
            
        case 0b100:
            game = 1;
            exrom = 1;
            break;
            
        case 0b101:
            game = 0;
            exrom = 1;
            break;
            
        case 0b110:
            game = 1;
            exrom = 0;
            break;
            
        case 0b111:
            game = 0;
            exrom = 0;
            break;
            
        default:
            assert(false);
            return;
    }
    
    expansionport.setGameAndExrom(game, exrom);
}
