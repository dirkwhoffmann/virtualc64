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


//! @brief    Type 10 cartridges (Epyx Fast Load)
class EpyxFastLoad : public Cartridge {
    
private:
    
    //! @brief    Indicated when the cartridge will be disabled
    /*! @details  The Epyx cartridge utilizes a capacitor to switch the ROM on and off.
     *            During normal operation, the capacitor slowly charges. When it is
     *            completely charged, the ROM gets disabled. When the cartridge is attached,
     *            the capacitor is discharged and the ROM visible. To avoid the ROM to be
     *            disabled, the cartridge can either read from ROML or I/O space 1. Both
     *            operations discharge the capacitor and keep the ROM alive.
     */
    uint64_t disable_at_cycle;
    
    //! @brief    Discharge the capacitor
    void dischargeCapacitor(); 
    
public:
    
    //! @brief    Checks the capacitor and switched off cartridge if required
    /*! @return   true if the cartridge is active, and false if the cartridge is disabled.
     */
    bool checkCapacitor();

    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_EPYX_FASTLOAD; }
    void reset();
    void execute();
    uint8_t peek(uint16_t addr);
    uint8_t peekIO(uint16_t addr);
};

#endif
