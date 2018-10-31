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

FlashRom::FlashRom(uint8_t **buffer) : CartridgeRom(buffer)
{
    state = (FlashRomState)read8(buffer);
    baseState = (FlashRomState)read8(buffer);
}

FlashRom::FlashRom(uint16_t _size, uint16_t _loadAddress, const uint8_t *buffer) :
CartridgeRom(_size, _loadAddress, buffer)
{
    state = FLASH_READ;
    baseState = FLASH_READ;
}

void
FlashRom::reset()
{
    state = FLASH_READ;
    baseState = FLASH_READ;
}

size_t
FlashRom::stateSize()
{
    size_t result = CartridgeRom::stateSize();
    
    result += 1; // state
    result += 1; // baseState

    return result;
}

void
FlashRom::loadFromBuffer(uint8_t **buffer)
{
    CartridgeRom::loadFromBuffer(buffer);
    state = (FlashRomState)read8(buffer);
    baseState = (FlashRomState)read8(buffer);
}

void
FlashRom::saveToBuffer(uint8_t **buffer)
{
    CartridgeRom::saveToBuffer(buffer);
    write8(buffer, (uint8_t)state);
    write8(buffer, (uint8_t)baseState);
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
    assert(addr < size);
    
    uint8_t result;
    
    switch (state) {
        
        case FLASH_AUTOSELECT:
        
        // TODO
        result = rom[addr];
        break;
        
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

void
FlashRom::poke(uint16_t addr, uint8_t value)
{
    switch (state) {
        
        case FLASH_READ:
        
        if (addr == 0x5555 && value == 0xAA) {
            
            state = FLASH_MAGIC_1;
            return;
        }
        
        return;
        
        case FLASH_MAGIC_1:
        
        if (addr == 0x2AAA && value == 0x55) {
            
            state = FLASH_MAGIC_2;
            return;
        }
        
        state = baseState;
        return;
        
        case FLASH_MAGIC_2:
        
        if (addr == 0x5555) {
            
            switch(value) {
                
                case 0xF0:
                
                state = FLASH_READ;
                baseState = FLASH_READ;
                return;
                
                case 0x90:
                
                state = FLASH_AUTOSELECT;
                baseState = FLASH_AUTOSELECT;
                return;
                
                case 0xA0:
                state = FLASH_BYTE_PROGRAM;
                return;
                
                case 0x80:
                state = FLASH_ERASE_MAGIC_1;
                return;
            }
        }
        
        state = baseState;
        break;
        
        case FLASH_BYTE_PROGRAM:
  
        if (!byteProgram(addr, value)) {
            
            state = FLASH_BYTE_PROGRAM_ERROR;
            return;
        }
        
        state = baseState;
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
            return;
        }
        if (value == 0xF0) {
            
            state = FLASH_READ;
            baseState = FLASH_READ;
            return;
        }
        return;
        
        case FLASH_CHIP_ERASE:
        default:
        
        // TODO
        break;
    }
}

bool
FlashRom::byteProgram(uint16_t addr, uint8_t value)
{
    assert(addr < size);
    
    uint8_t oldValue = rom[addr];
    uint8_t newValue = rom[addr] & value;
    
    rom[addr] = newValue;
    return oldValue == newValue;
}

