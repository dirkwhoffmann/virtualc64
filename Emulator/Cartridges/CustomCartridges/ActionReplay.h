// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

//
// Action Replay (hardware version 3)
//

class ActionReplay3 : public Cartridge {
    
public:
    
    ActionReplay3(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "AR3"; }
    CartridgeType getCartridgeType() const override { return CRT_ACTION_REPLAY3; }

    
    //
    // Accessing cartridge data
    //
    
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override { return 0; }
    u8 spypeekIO1(u16 addr) const override { return 0; }
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;

    // Sets the control register and triggers side effects
    void setControlReg(u8 value);
    
    unsigned bank() const { return control & 0x01; }
    bool game() const { return !!(control & 0x02); }
    bool exrom() const { return !(control & 0x08); }
    bool disabled() const { return !!(control & 0x04); }
    
    
    //
    // Handling buttons
    //

    long numButtons() const override { return 2; }
    const char *getButtonTitle(unsigned nr) const override;
    void pressButton(unsigned nr) override;
    void releaseButton(unsigned nr) override;
};


//
// Action Replay (hardware version 4 and above)
//

class ActionReplay : public Cartridge {
        
public:
        
    ActionReplay(C64 &ref);
    const char *getDescription() const override { return "ActionReplay"; }
    CartridgeType getCartridgeType() const override { return CRT_ACTION_REPLAY; }

    void _reset(bool hard) override;
    void resetCartConfig() override;

    
    //
    // Accessing cartridge data
    //
    
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;

    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
    // Sets the control register and triggers side effects
    void setControlReg(u8 value);
    
    virtual unsigned bank() const { return (control >> 3) & 0x03; }
    virtual bool game() const { return (control & 0x01) == 0; }
    virtual bool exrom() const { return (control & 0x02) != 0; }
    virtual bool disabled() const { return (control & 0x04) != 0; }
    virtual bool resetFreezeMode() const { return (control & 0x40) != 0; }
    
    // Returns true if the cartridge RAM shows up at the provided address
    virtual bool ramIsEnabled(u16 addr) const;

    
    //
    // Handling buttons
    //
    
    long numButtons() const override { return 2; }
    const char *getButtonTitle(unsigned nr) const override;
    void pressButton(unsigned nr) override;
    void releaseButton(unsigned nr) override;
};


//
// Atomic Power (a derivation of the Action Replay cartridge)
//

class AtomicPower : public ActionReplay {
    
public:
    
    AtomicPower(C64 &ref) : ActionReplay(ref) { };
    const char *getDescription() const override { return "AtomicPower"; }
    CartridgeType getCartridgeType() const override { return CRT_ATOMIC_POWER; }
    
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
    bool specialMapping() const { return (control & 0b11100111) == 0b00100010; }
    
    bool game() const override;
    bool exrom() const override;
    bool ramIsEnabled(u16 addr) const override;
};
