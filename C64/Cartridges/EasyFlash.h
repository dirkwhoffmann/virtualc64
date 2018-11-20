/*!
 * @header      EasyFlash.h
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

#ifndef _EASYFLASH_INC
#define _EASYFLASH_INC

//! @brief    Type 32 cartridges
class EasyFlash : public Cartridge {
    
    //!@brief    Flash Rom mapping to ROML ($8000 - $9FFF)
    FlashRom flashRomL;

    //!@brief    Flash Rom mapping to ROMH ($A000 - $B000 or $E000 - $FFFF)
    FlashRom flashRomH;
    
    //!@brief    Selected memory bank
    uint8_t bank;

public:
    
    //
    //! @functiongroup Creating and destructing
    //
    
    EasyFlash(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_EASYFLASH; }
    

    //
    //! @functiongroup Methods from VirtualComponent
    //

    void reset();
    void dump();
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void loadChip(unsigned nr, CRTFile *c);
    uint8_t peek(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
    bool hasFreezeButton() { return false; }
    bool hasResetButton() { return false; }
    bool getJumper() { return val[2]; }
    void setJumper(bool value) { val[2] = value; }
    

    
};

#endif
