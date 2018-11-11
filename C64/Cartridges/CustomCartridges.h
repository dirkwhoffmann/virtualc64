/*!
 * @header      CustomCartridges.h
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

#ifndef _CUSTOM_CARTRIDGES_INC
#define _CUSTOM_CARTRIDGES_INC

#include "Cartridge.h"
#include "ActionReplay.h"
#include "EasyFlash.h"
#include "FinalIII.h"
#include "StarDos.h"
#include "Epyx.h"

//! @brief    Type 2 cartridges
class KcsPower : public Cartridge {
    
public:
    KcsPower(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_KCS_POWER; }
    void reset();
    uint8_t peekIO1(uint16_t addr);
    uint8_t spypeekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
    bool hasFreezeButton() { return false; }
    bool hasResetButton() { return true; }
    void pressResetButton();
    void releaseResetButton();
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

//! @brief    Type 11 cartridges
class Westermann : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_WESTERMANN; }
    uint8_t peekIO2(uint16_t addr);
    uint8_t spypeekIO2(uint16_t addr);
};

//! @brief    Type 12 cartridges
class Rex : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_REX; }
    uint8_t peekIO2(uint16_t addr);
    uint8_t spypeekIO2(uint16_t addr);
};

//! @brief    Type 16 cartridges
class WarpSpeed : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_WARPSPEED; }
    void resetCartConfig();
    bool hasResetButton() { return true; }
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
};

//! @brief    Type 18 cartridges
class Zaxxon : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    void reset();
    CartridgeType getCartridgeType() { return CRT_ZAXXON; }
    uint8_t peekRomL(uint16_t addr);
    uint8_t spypeekRomL(uint16_t addr);
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

//! @brief    Type 45 cartridges
class FreezeFrame : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_FREEZE_FRAME; }
    void reset();
    uint8_t peekIO1(uint16_t addr);
    uint8_t spypeekIO1(uint16_t addr) { return 0; }
    uint8_t peekIO2(uint16_t addr);
    uint8_t spypeekIO2(uint16_t addr) { return 0; }
    bool hasFreezeButton() { return true; }
    void pressFreezeButton();
    void releaseFreezeButton();
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
