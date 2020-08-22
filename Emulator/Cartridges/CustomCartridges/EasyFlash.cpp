// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

EasyFlash::EasyFlash(C64 *c64, C64 &ref) : Cartridge(c64, ref, "EasyFlash")
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
    Cartridge::_reset();
    
    eraseRAM(0xFF);
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

size_t
EasyFlash::oldStateSize()
{
    return Cartridge::oldStateSize()
    + 1
    + 1
    + flashRomL.oldStateSize()
    + flashRomH.oldStateSize();
}

void
EasyFlash::oldDidLoadFromBuffer(u8 **buffer)
{
    Cartridge::oldDidLoadFromBuffer(buffer);
    bank = read8(buffer);
    jumper = (bool)read8(buffer);
    flashRomL.oldLoadFromBuffer(buffer);
    flashRomH.oldLoadFromBuffer(buffer);
}

void
EasyFlash::oldDidSaveToBuffer(u8 **buffer)
{
    Cartridge::oldDidSaveToBuffer(buffer);
    write8(buffer, bank);
    write8(buffer, (u8)jumper);
    flashRomL.oldSaveToBuffer(buffer);
    flashRomH.oldSaveToBuffer(buffer);
}

void
EasyFlash::loadChip(unsigned nr, CRTFile *c)
{
    static int bank;
    
    u16 chipSize = c->chipSize(nr);
    u16 chipAddr = c->chipAddr(nr);
    u8 *chipData = c->chipData(nr);

    if (nr == 0) {
        bank = 0;
    }
    
    if(chipSize != 0x2000) {
        warn("Package %d has chip size %04X. Expected 0x2000.\n", nr, chipSize);
        return;
    }

    // Check for missing banks
    if (bank % 2 == 0 && isROMHaddr(chipAddr)) {
        debug(CRT_DEBUG, "Skipping Rom bank %dL ...\n", bank / 2);
        bank++;
    }
    if (bank % 2 == 1 && isROMLaddr(chipAddr)) {
        debug(CRT_DEBUG, "Skipping Rom bank %dH ...\n", bank / 2);
        bank++;
    }

    if (isROMLaddr(chipAddr)) {
            
        debug(CRT_DEBUG, "Loading Rom bank %dL ...\n", bank / 2);
        flashRomL.loadBank(bank / 2, chipData);
        bank++;
    
    } else if (isROMHaddr(chipAddr)) {

        debug(CRT_DEBUG, "Loading Rom bank %dH ...\n", bank / 2);
        flashRomH.loadBank(bank / 2, chipData);
        bank++;
        
    } else {
        
        warn("Package %d has an invalid load address (%04X).", nr, chipAddr);
        return;
    }
}

u8
EasyFlash::peek(u16 addr)
{
    if (isROMLaddr(addr)) {
        return flashRomL.peek(bank, addr & 0x1FFF);
        
    } else if (isROMHaddr(addr)) {
        return flashRomH.peek(bank, addr & 0x1FFF);
        
    } else {
        assert(false);
        return 0;
    }
}

void
EasyFlash::poke(u16 addr, u8 value)
{
    if (isROMLaddr(addr)) {
        flashRomL.poke(bank, addr & 0x1FFF, value);
        
    } else if (isROMHaddr(addr)) {
        flashRomH.poke(bank, addr & 0x1FFF, value);
        
    } else {
        assert(false);
    }
}

u8
EasyFlash::peekIO1(u16 addr)
{
    return 0;
}

u8
EasyFlash::peekIO2(u16 addr)
{
    return peekRAM(addr & 0xFF);
}

void
EasyFlash::pokeIO1(u16 addr, u8 value)
{
    if (addr == 0xDE00) { // Bank register
        
        bank = value & 0x3F;
        return;
    }
    
    if (addr == 0xDE02) { // Mode register
        
        setLED((value & 0x80) != 0);
        
        u8 MXG = value & 0x07;
        /* MXG
         * 000 : GAME from jumper, EXROM high (i.e. Ultimax or Off)
         * 001 : Reserved, don’t use this
         * 010 : GAME from jumper, EXROM low (i.e. 16K or 8K)
         * 011 : Reserved, don’t use this
         * 100 : Cartridge ROM off (RAM at $DF00 still available)
         * 101 : Ultimax (Low bank at $8000, high bank at $e000)
         * 110 : 8k Cartridge (Low bank at $8000)
         * 111 : 16k cartridge (Low bank at $8000, high bank at $a000)
         */
        
        bool exrom;
        bool game;
        
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
}

void
EasyFlash::pokeIO2(u16 addr, u8 value)
{
    pokeRAM(addr & 0xFF, value);
}



