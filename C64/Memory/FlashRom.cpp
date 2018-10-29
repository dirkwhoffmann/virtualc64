/*!
 * @file        FlashRom.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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

#include "FlashRom.h"

FlashRom::FlashRom()
{
    setDescription("FlashRom");
    debug(3, "  Creating FlashRom at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &state,       sizeof(state),          KEEP_ON_RESET },
        { rom,          sizeof(rom),            KEEP_ON_RESET },
        { NULL,         0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
    
    state = FLASH_READ;
    memset(rom, 0xff, sizeof(rom));
}

FlashRom::FlashRom(const uint8_t *buffer) : FlashRom()
{
    assert(buffer != NULL);
    memcpy(rom, buffer, sizeof(rom));
}

FlashRom::~FlashRom()
{
    debug(3, "  Releasing FlashRom...\n");
}

void
FlashRom::reset()
{
    state = FLASH_READ;
}

void
FlashRom::dumpState()
{
    msg("FlashRom:\n");
    msg("---------\n\n");
    msg("Current state: %s\n", getStateAsString());
}

const char *
FlashRom::getStateAsString()
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
        assert(false);
    }
}

uint8_t
FlashRom::peek(uint16_t addr)
{
    uint8_t result;
    
    switch (state) {
        
        case FLASH_AUTOSELECT:
        
        // TODO
        result = 0;
        break;
        
        case FLASH_BYTE_PROGRAM_ERROR:
        
        // TODO
        result = 0;
        break;
        
        case FLASH_SECTOR_ERASE_SUSPEND:
        
        // TODO
        result = 0;
        break;
        
        case FLASH_CHIP_ERASE:
        
        // TODO
        result = 0;
        break;
        
        case FLASH_SECTOR_ERASE:
        
        // TODO
        result = 0;
        break;
        
        case FLASH_SECTOR_ERASE_TIMEOUT:
        
        // TODO
        result = 0;
        break;
        
        default:

        // TODO
        result = 0;
        break;
    }
    
    return result;
}

void
FlashRom::poke(uint16_t addr, uint8_t value)
{
    switch (state) {
        
        case FLASH_READ:
        
        // TODO
        break;
        
        case FLASH_MAGIC_1:
        
        // TODO
        break;
        
        case FLASH_MAGIC_2:
  
        // TODO
        break;
        
        case FLASH_BYTE_PROGRAM:
  
        // TODO
        break;
        
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
      
        // TODO
        break;
        
        case FLASH_CHIP_ERASE:
        default:
        
        // TODO
        break;
    }
}
