/*!
 * @file        EasyFlash.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
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

EasyFlash::EasyFlash(C64 *c64) : Cartridge(c64)
{
    bank = 0;
    
    // Allocate 256 bytes on-board RAM
    setRamCapacity(256);
}

void
EasyFlash::reset()
{
    Cartridge::reset();
    
    bank = 0;
    memset(externalRam, 0xFF, ramCapacity);
}

void
EasyFlash::loadChip(unsigned nr, CRTFile *c)
{
    static int bankL;
    static int bankH;
    
    uint16_t chipSize = c->chipSize(nr);
    uint16_t chipAddr = c->chipAddr(nr);
    uint8_t *chipData = c->chipData(nr);

    if (nr == 0) {
        bankL = bankH = 0;
    }
    
    if(chipSize != 0x2000) {
        warn("Package %d has chip size %04X. Expected 0x2000.\n", nr, chipSize);
        return;
    }

    if (isROMHaddr(chipAddr)) {
        
        debug("Loading Rom bank %dH ...\n", bankH);
        flashRomH.loadBank(bankH++, chipData);
        
    } else if (isROMLaddr(chipAddr)) {

        debug("Loading Rom bank %dL ...\n", bankL);
        flashRomL.loadBank(bankL++, chipData);
        
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

uint8_t
EasyFlash::peekIO1(uint16_t addr)
{
    // debug("WARNING: peekIO1\n");
    return 0;
}

uint8_t
EasyFlash::peekIO2(uint16_t addr)
{
    return externalRam[addr & 0xFF];
}

void
EasyFlash::pokeIO1(uint16_t addr, uint8_t value)
{
    if (addr == 0xDE00) { // Bank register
        
        bank = value & 0x3F;
        
        bankIn(2 * bank); // ROML
        bankIn(2 * bank + 1); // ROMH
    }
    
    else if (addr == 0xDE02) { // Mode register
        
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
            game = getJumper();
            exrom = 1;
            break;
            
            case 0b010:
            game = getJumper();
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
            warn("Ignoring invalid MXG combination %X.", MXG);
            return;
        }
        
        c64->expansionport.setGameLine(game);
        c64->expansionport.setExromLine(exrom);
    }
}

void
EasyFlash::pokeIO2(uint16_t addr, uint8_t value)
{
    externalRam[addr & 0xFF] = value;
}



