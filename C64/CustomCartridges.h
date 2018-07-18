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

//! @brief    Type 1 cartridges
class ActionReplay : public Cartridge {
    
    //! @brief    Indicates if the cartridge is disabled
    /*! @details  The ActionReplay cartridge is controlled by poking into
     *            I/O space 1 (no matter which register). If the cartridge is
     *            deactivated, poking into I/O space 1 has no effect.
     */
    bool hide;
    
    //! @brief   Decides whether RAM or ROM shows up at $8000-$9FFF.
    bool ramShowsUp;

    //! @brief   Currently visible ROM chip (0 ... 3)
    uint8_t selectedChip;

public:
    ActionReplay(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_ACTION_REPLAY; }
    void reset();
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    uint8_t peek(uint16_t addr); 
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
    void pressFirstButton();
    void pressSecondButton();

    //! @brief   Clears the control register
    /*! @details The ActionReplay cartridge has one control register which
     *           can be accessed by writing at an arbitrary address in
     *           I/O space 1.
     */
    void clearControlReg(); 
};

//! @brief    Type 35 cartridges
class ActionReplay3 : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    // ActionReplay3(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_ACTION_REPLAY3; }
    uint8_t peek(uint16_t addr);
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pressFirstButton();
    void pressSecondButton();
    void releaseFirstButton();
    
    //! @brief   Sets the cartridge's control register
    /*! @details This function triggers all side effects that take place when
     *           the control register value changes.
     */
    void setControlReg(uint8_t value);

    unsigned bank() { return regValue & 0x01; }
    bool game() { return !!(regValue & 0x02); }
    bool exrom() { return !(regValue & 0x08); }
    bool disabled() { return !!(regValue & 0x04); }
};

//! @brief    Type 3 cartridges
class FinalIII : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_FINAL_III; }
    void reset();
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO2(uint16_t addr, uint8_t value);
    void pressFirstButton();
    void pressSecondButton();
};


//! @brief    Type 4 cartridges
class SimonsBasic : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_SIMONS_BASIC; }
    void reset();
    uint8_t peekIO1(uint16_t addr);
    uint8_t readIO1(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
};


//! @brief    Type 5 cartridges
class Ocean : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_OCEAN; }
    void pokeIO1(uint16_t addr, uint8_t value);
};


//! @brief    Type 7 cartridges
class Funplay : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_FUNPLAY; }
    void pokeIO1(uint16_t addr, uint8_t value);
};


//! @brief    Type 8 cartridges
class Supergames : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_SUPER_GAMES; }
    void pokeIO2(uint16_t addr, uint8_t value);
};


//! @brief    Type 10 cartridges
class EpyxFastLoad : public Cartridge {
    
private:
    
    //! @brief    Discharges the cartridge's capacitor
    /*! @details  The Epyx cartridge utilizes a capacitor to switch the ROM on and off.
     *            During normal operation, the capacitor slowly charges. When it is
     *            completely charged, the ROM gets disabled. When the cartridge is attached,
     *            the capacitor is discharged and the ROM visible. To avoid the ROM to be
     *            disabled, the cartridge can either read from ROML or I/O space 1. Both
     *            operations discharge the capacitor and keep the ROM alive.
     */
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
    uint8_t read(uint16_t addr);
    uint8_t peekIO1(uint16_t addr);
    uint8_t readIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
};

//! @brief    Type 11 cartridges
class Westermann : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_WESTERMANN; }
    uint8_t peekIO2(uint16_t addr);
    uint8_t readIO2(uint16_t addr);
};

//! @brief    Type 12 cartridges
class Rex : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_REX; }
    uint8_t peekIO2(uint16_t addr);
    uint8_t readIO2(uint16_t addr);
};

//! @brief    Type 18 cartridges
class Zaxxon : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_ZAXXON; }
    uint8_t peek(uint16_t addr);
    uint8_t read(uint16_t addr);
};

//! @brief    Type 19 cartridges
class MagicDesk : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_MAGIC_DESK; }
    uint8_t peekIO1(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
};

//! @brief    Type 21 cartridges
class Comal80 : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_COMAL80; }
    void reset();
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
};

//! @brief    GeoRAM cartridge
class GeoRAM : public Cartridge {
    
private:
    
    //! @brief   Selected RAM bank
    uint8_t bank;
    
    //! @brief   Selected page inside the selected RAM bank.
    uint8_t page;
        
    //! @brief   Computes the offset for accessing the cartridge RAM
    unsigned offset(uint8_t addr);
    
public:
    GeoRAM(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_GEO_RAM; }
    void reset();
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
};

#endif
