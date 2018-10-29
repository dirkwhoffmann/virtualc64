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
    romCount = 0;
    memset(flashRom, 0, sizeof(flashRom));
    
    // Allocate 256 bytes on-board RAM
    setRamCapacity(256);
}

void
EasyFlash::dealloc()
{
    for (unsigned i = 0; i < 128; i++) {
        if (flashRom[i]) {
            delete flashRom[i];
            flashRom[i] = NULL;
            romCount--;
        }
    }
    assert(romCount == 0);
}

void
EasyFlash::reset()
{
    Cartridge::reset();
    
    // Create the Flash Roms when the cartridge is reset the first time
    /*
    if (romCount == 0) {
    for (unsigned i = 0; i < MAX_PACKETS; i++) {
        
        if (chip[i] == NULL) break;

        if (chipSize[i] != 0x2000) {
            panic("Invalid chip size in EasyFlash cartrige spotted: %04X\n");
            assert(false);
            return;
        }
        
        flashRom[i] = new FlashRom(chip[i]);
    }
    */
 
    for (unsigned i = 0; i < 128; i++) {
        if (flashRom[i])
        flashRom[i]->reset();
    }
}

size_t
EasyFlash::stateSize()
{
    size_t result = Cartridge::stateSize();
    
    result += 1; // romCount
    
    for (unsigned i = 0; i < 128; i++) {
        if (flashRom[i] != NULL) {
            debug("%d SIZE\n", flashRom[i]->stateSize());
            result += flashRom[i]->stateSize();
        }
    }
    
    return result;
}

void
EasyFlash::loadFromBuffer(uint8_t **buffer)
{
    debug("EasyFlash::loadFromBuffer\n");
    
    uint8_t *old = *buffer;
    Cartridge::loadFromBuffer(buffer);
    romCount = read8(buffer);
    
    dealloc();
    for (unsigned i = 0; i < romCount; i++) {
        flashRom[i] = new FlashRom();
        flashRom[i]->loadFromBuffer(buffer);
    }
    
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
EasyFlash::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    Cartridge::saveToBuffer(buffer);
    write8(buffer, romCount);
    for (unsigned i = 0; i < romCount; i++) {
        if (flashRom[i]) {
            flashRom[i]->saveToBuffer(buffer);
        }
    }
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

/*
void
EasyFlash::loadChip(unsigned nr, CRTFile *c)
{
    assert(nr < MAX_PACKETS);
    assert(c != NULL);
    
    uint16_t start = c->chipAddr(nr);
    uint16_t size  = c->chipSize(nr);
    uint8_t  *data = c->chipData(nr);
    
    if (size != 0x2000) {
        warn("Ignoring chip %d: Got size %04X, expected %04X.\n", size, 0x2000);
        return;
    }
    
    if (flashRom[nr]) {
        delete flashRom[nr];
    }
    flashRom[nr] = new FlashRom(data);
    chipStartAddress[nr] = start;
    chipSize[nr] = size;
}
*/

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
    if (addr == 0xDE00) {
        
        // Bank register
        int bank = value & 0x3F;
        
        bankIn(2 * bank); // ROML
        bankIn(2 * bank + 1); // ROMH
    }
    
    else if (addr == 0xDE02) {
        
        // Mode register
        
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



