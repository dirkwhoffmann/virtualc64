// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _ACTIONREPLAY_H
#define _ACTIONREPLAY_H

#include "Cartridge.h"

//
// Action Replay (hardware version 3)
//

class ActionReplay3 : public CartridgeWithRegister {
    
public:
    
    ActionReplay3(C64 *c64, C64 &ref) : CartridgeWithRegister(c64, ref, "AR3") { };
    CartridgeType getCartridgeType() { return CRT_ACTION_REPLAY3; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    u8 peek(u16 addr);
    u8 peekIO1(u16 addr);
    u8 peekIO2(u16 addr);
    
    void pokeIO1(u16 addr, u8 value);
    
    unsigned numButtons() { return 2; }
    const char *getButtonTitle(unsigned nr);
    void pressButton(unsigned nr);
    void releaseButton(unsigned nr);
    
    //! @brief   Sets the cartridge's control register
    /*! @details This function triggers all side effects that take place when
     *           the control register value changes.
     */
    void setControlReg(u8 value);
    
    unsigned bank() { return control & 0x01; }
    bool game() { return !!(control & 0x02); }
    bool exrom() { return !(control & 0x08); }
    bool disabled() { return !!(control & 0x04); }
};


//
// Action Replay (hardware version 4 and above)
//

class ActionReplay : public CartridgeWithRegister {
    
public:
    
    ActionReplay(C64 *c64, C64 &ref);
    CartridgeType getCartridgeType() override { return CRT_ACTION_REPLAY; }
    
    
    //
    // Methods from HardwareComponent
    //
    
private:
    
    void _reset() override;
    size_t oldStateSize() override {
        return Cartridge::oldStateSize() + 1; }
    void oldDidLoadFromBuffer(u8 **buffer) override {
        Cartridge::oldDidLoadFromBuffer(buffer); control = read8(buffer); }
    void oldDidSaveToBuffer(u8 **buffer) override {
        Cartridge::oldDidSaveToBuffer(buffer); write8(buffer, control); }
    
    //
    // Methods from Cartridge
    //
    
public:
    
    void resetCartConfig() override;
    
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
    unsigned numButtons() override { return 2; }
    const char *getButtonTitle(unsigned nr) override;
    void pressButton(unsigned nr) override;
    void releaseButton(unsigned nr) override;
    
    
    /* Sets the cartridge's control register. This function triggers all side
     * effects that take place when the control register value changes.
     */
    void setControlReg(u8 value);
    
    virtual unsigned bank() { return (control >> 3) & 0x03; }
    virtual bool game() { return (control & 0x01) == 0; }
    virtual bool exrom() { return (control & 0x02) != 0; }
    virtual bool disabled() { return (control & 0x04) != 0; }
    virtual bool resetFreezeMode() { return (control & 0x40) != 0; }
    
    // Returns true if the cartridge RAM shows up at addr
    virtual bool ramIsEnabled(u16 addr); 
};


//
// Atomic Power (a derivation of the Action Replay cartridge)
//

class AtomicPower : public ActionReplay {
    
public:
    
    AtomicPower(C64 *c64, C64 &ref);
    CartridgeType getCartridgeType() { return CRT_ATOMIC_POWER; }
    
    /* Indicates if special ROM / RAM config has to be used. In contrast to
     * the Action Replay cartridge, Atomic Power has the ability to map the
     * on-board RAM to the ROMH area at $A000 - $BFFF. To enable this special
     * configuration, the control register has to be configured as follows:
     *
     *            Bit 0b10000000 (Extra ROM)    is 0.
     *            Bit 0b01000000 (Freeze clear) is 0.
     *            Bit 0b00100000 (RAM enable)   is 1.
     *            Bit 0b00000100 (Disable)      is 0.
     *            Bit 0b00000010 (Exrom)        is 1.
     *            Bit 0b00000001 (Game)         is 0.
     */
    bool specialMapping() { return (control & 0b11100111) == 0b00100010; }
    
    bool game();
    bool exrom();
    bool ramIsEnabled(u16 addr);
};

#endif
