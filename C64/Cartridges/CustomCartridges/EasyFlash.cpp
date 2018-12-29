/*!
 * @file        EasyFlash.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

EasyFlash::EasyFlash(C64 *c64) : Cartridge(c64, "EasyFlash")
{
    flashRomL.setDescription("FlashRom_L");
    flashRomH.setDescription("FlashRom_H");

    bank = 0;

    // Allocate 256 bytes on-board RAM
    setRamCapacity(256);

    // Start in Ultimax mode
    jumper = false;
}

void
EasyFlash::reset()
{
    Cartridge::reset();
    
    bank = 0;
    eraseRAM(0xFF);
    jumper = false;
}

void
EasyFlash::dump()
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
EasyFlash::stateSize()
{
    return Cartridge::stateSize()
    + 1
    + 1
    + flashRomL.stateSize()
    + flashRomH.stateSize();
}

void
EasyFlash::didLoadFromBuffer(uint8_t **buffer)
{
    Cartridge::didLoadFromBuffer(buffer);
    bank = read8(buffer);
    jumper = (bool)read8(buffer);
    flashRomL.loadFromBuffer(buffer);
    flashRomH.loadFromBuffer(buffer);
}

void
EasyFlash::didSaveToBuffer(uint8_t **buffer)
{
    Cartridge::didSaveToBuffer(buffer);
    write8(buffer, bank);
    write8(buffer, (uint8_t)jumper);
    flashRomL.saveToBuffer(buffer);
    flashRomH.saveToBuffer(buffer);
}

void
EasyFlash::resetCartConfig()
{
    c64->expansionport.setCartridgeMode(CRT_ULTIMAX);
}

void
EasyFlash::loadChip(unsigned nr, CRTFile *c)
{
    static int bank;
    
    uint16_t chipSize = c->chipSize(nr);
    uint16_t chipAddr = c->chipAddr(nr);
    uint8_t *chipData = c->chipData(nr);

    if (nr == 0) {
        bank = 0;
    }
    
    if(chipSize != 0x2000) {
        warn("Package %d has chip size %04X. Expected 0x2000.\n", nr, chipSize);
        return;
    }

    // Check for missing banks
    if (bank % 2 == 0 && isROMHaddr(chipAddr)) {
        debug(1, "Skipping Rom bank %dL ...\n", bank / 2);
        bank++;
    }
    if (bank % 2 == 1 && isROMLaddr(chipAddr)) {
        debug(1, "Skipping Rom bank %dH ...\n", bank / 2);
        bank++;
    }

    if (isROMLaddr(chipAddr)) {
            
        debug(1, "Loading Rom bank %dL ...\n", bank / 2);
        flashRomL.loadBank(bank / 2, chipData);
        bank++;
    
    } else if (isROMHaddr(chipAddr)) {

        debug(1, "Loading Rom bank %dH ...\n", bank / 2);
        flashRomH.loadBank(bank / 2, chipData);
        bank++;
        
    } else {
        
        warn("Package %d has an invalid load address (%04X).", nr, chipAddr);
        return;
    }
}

uint8_t
EasyFlash::peek(uint16_t addr)
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

/*
uint8_t
EasyFlash::spypeek(uint16_t addr)
{
    if (isROMLaddr(addr)) {
        return flashRomL.spypeek(bank, addr & 0x1FFF);
        
    } else if (isROMHaddr(addr)) {
        return flashRomH.spypeek(bank, addr & 0x1FFF);
        
    } else {
        assert(false);
        return 0;
    }
}
*/

void
EasyFlash::poke(uint16_t addr, uint8_t value)
{
    if (isROMLaddr(addr)) {
        flashRomL.poke(bank, addr & 0x1FFF, value);
        
    } else if (isROMHaddr(addr)) {
        flashRomH.poke(bank, addr & 0x1FFF, value);
        
    } else {
        assert(false);
    }
}

uint8_t
EasyFlash::peekIO1(uint16_t addr)
{
    // debug("WARNING: peekIO1\n");
    return 0;
}

uint8_t
EasyFlash::peekIO2(uint16_t addr)
{
    return peekRAM(addr & 0xFF);
}

void
EasyFlash::pokeIO1(uint16_t addr, uint8_t value)
{
    if (addr == 0xDE00) { // Bank register
        
        bank = value & 0x3F;
        return;
    }
    
    if (addr == 0xDE02) { // Mode register
        
        setLED((value & 0x80) != 0);
        
        uint8_t MXG = value & 0x07;
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
        
        c64->expansionport.setGameAndExrom(game, exrom);
    }
}

void
EasyFlash::pokeIO2(uint16_t addr, uint8_t value)
{
    pokeRAM(addr & 0xFF, value);
}



