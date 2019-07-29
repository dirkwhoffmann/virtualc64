/*!
 * @header      Isepic.h
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

#ifndef _ISEPIC_INC
#define _ISEPIC_INC

#include "Cartridge.h"

class Isepic : public Cartridge {
    
    //! @brief   Selected page inside the selected RAM bank.
    uint8_t page;

    //! @brief   Original page mappings to 0xD
    MemoryType oldPeekSourceD;
    MemoryType oldPeekSourceF;
    MemoryType oldPokeTargetD;
    MemoryType oldPokeTargetF;

public:
    Isepic(C64 *c64);
    CartridgeType getCartridgeType() override { return CRT_ISEPIC; }
    
    void reset() override;
    size_t stateSize() override;
    void didLoadFromBuffer(uint8_t **buffer) override;
    void didSaveToBuffer(uint8_t **buffer) override;
    
    bool hasSwitch() override { return true; }
    const char *getSwitchDescription(int8_t pos) override;
    void setSwitch(int8_t pos) override;
    bool switchInOffPosition() { return switchIsLeft(); }
    bool switchInOnPosition() { return switchIsRight(); }

    bool cartIsVisible() { return switchInOnPosition(); }
    bool cartIsHidden() { return !cartIsVisible(); }

    void updatePeekPokeLookupTables() override;
    uint8_t peek(uint16_t addr) override;
    uint8_t peekIO1(uint16_t addr) override;
    // uint8_t peekIO2(uint16_t addr) override;
    void poke(uint16_t addr, uint8_t value) override;
    void pokeIO1(uint16_t addr, uint8_t value) override;
    // void pokeIO2(uint16_t addr, uint8_t value) override;
};


#endif
