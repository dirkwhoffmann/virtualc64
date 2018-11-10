/*!
 * @header      ActionReplay.h
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

#ifndef _ACTIONREPLAY_INC
#define _ACTIONREPLAY_INC

#include "Cartridge.h"

//! @brief    Type 1 cartridges
class ActionReplay : public Cartridge {
    
    public:
    ActionReplay(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_ACTION_REPLAY; }
    void reset();
    // uint8_t peekRomL(uint16_t addr);
    uint8_t peek(uint16_t addr);
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void poke(uint16_t addr, uint8_t value);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
    bool hasFreezeButton() { return true; }
    void pressFreezeButton();
    bool hasResetButton() { return true; }
    
    //! @brief   Sets the cartridge's control register
    /*! @details This function triggers all side effects that take place when
     *           the control register value changes.
     */
    void setControlReg(uint8_t value);
    
    unsigned bank() { return (regValue >> 3) & 0x03; }
    bool game() { return !(regValue & 0x01); }
    bool exrom() { return (regValue >> 1) & 0x01; }
    bool disabled() { return regValue & 0x04; }
    bool ramIsEnabled() { return regValue & 0x20; }
    bool resetFreezeMode() { return regValue & 0x40; }
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
    bool hasFreezeButton() { return true; }
    void pressFreezeButton();
    void releaseFreezeButton();
    bool hasResetButton() { return true; }
    
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

#endif
