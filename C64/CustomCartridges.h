/*!
 * @header      CustomCartridges.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
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

#ifndef _POWERPLAY_INC
#define _POWERPLAY_INC

#include "Cartridge.h"


//! @brief    Type 3 cartridges (Final Cartrige III)
class FinalIII : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_FINAL_CARTRIDGE_III; }
    void reset();
    uint8_t peekIO(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
    void pressFirstButton();
    void pressSecondButton();
};


//! @brief    Type 4 cartridges (Simons Basic)
class SimonsBasic : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_SIMONS_BASIC; }
    void reset();
    uint8_t peekIO(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
};


//! @brief    Type 5 cartridges (Ocean type 1)
class OceanType1 : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_OCEAN_TYPE_1; }
    void poke(uint16_t addr, uint8_t value);
};


//! @brief    Type 7 cartridges (Fun play, Power play)
class Powerplay : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_FUN_PLAY_POWER_PLAY; }
    void poke(uint16_t addr, uint8_t value);
};


//! @brief    Type 8 cartridges (Super games)
class Supergames : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_SUPER_GAMES; }
    uint8_t peekIO(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
};

#endif
