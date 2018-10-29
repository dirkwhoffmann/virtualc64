/*!
 * @header      FlashRom.h
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

#ifndef _FLASHROM_INC
#define _FLASHROM_INC

#include "C64.h"

/*! @brief    This class implements a Flash Rom module of type Am29F040
 *  @details  Flash Rom modules of this type are used, e.g., by the EasyFlash
 *            cartridge.
 *            The implementation is based on the following ressources:
 *            29F040.pdf:     Data sheet published by AMD
 *            flash040core.c: Part of the VICE emulator
 */
class FlashRom : VirtualComponent {

    //! @brief    Flash Rom states (taken from VICE)
    typedef enum {
        FLASH_READ,
        FLASH_MAGIC_1,
        FLASH_MAGIC_2,
        FLASH_AUTOSELECT,
        FLASH_BYTE_PROGRAM,
        FLASH_BYTE_PROGRAM_ERROR,
        FLASH_ERASE_MAGIC_1,
        FLASH_ERASE_MAGIC_2,
        FLASH_ERASE_SELECT,
        FLASH_CHIP_ERASE,
        FLASH_SECTOR_ERASE,
        FLASH_SECTOR_ERASE_TIMEOUT,
        FLASH_SECTOR_ERASE_SUSPEND
    } FlashRomState;

    //! @brief    Current Flash Rom state
    FlashRomState state;
    
    //! @brief    Rom memory cells
    uint8_t rom[0x2000];
    
public:
    
    //! @brief    Constructor
    FlashRom();
    
    //! @brief    Destructor
    ~FlashRom();
    
    //! @brief    Method from VirtualComponent
    void reset();
    void dumpState();
    
    //! @brief    Returns the current Flash Rom state as a string
    const char *getStateAsString(); 
    
    //! @brief    Performs a read access at the specified memory address
    uint8_t peek(uint16_t addr);
    
    //! @brief    Returns the Rom contents at the specified memory address
    uint8_t spypeek(uint16_t addr) {assert(addr < 0x2000); return rom[addr]; }

    //! @brief    Performs a write access at the specified memory address
    void poke(uint16_t addr, uint8_t value);

};

#endif 
