// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

//
// Action Replay (hardware version 3)
//

namespace vc64 {

class ActionReplay3 final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_ACTION_REPLAY3,
        .title      = "Action Replay 3",

        .buttons    = 2,
        .button1    = "Freeze",
        .button2    = "Reset"
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;
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

    isize bank() const { return control & 0x01; }
    bool game() const { return !!(control & 0x02); }
    bool exrom() const { return !(control & 0x08); }
    bool disabled() const { return !!(control & 0x04); }


    //
    // Handling buttons
    //

    isize numButtons() const override { return 2; }
    const char *getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;
    void releaseButton(isize nr) override;
};


//
// Action Replay (hardware version 4 and above)
//

class ActionReplay : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_ACTION_REPLAY,
        .title      = "Action Replay",
        .memory     = KB(8),
        .buttons    = 2,
        .button1    = "Freeze",
        .button2    = "Reset"
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;
    void operator << (SerResetter &worker) override;
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

    virtual isize bank() const { return (control >> 3) & 0x03; }
    virtual bool game() const { return (control & 0x01) == 0; }
    virtual bool exrom() const { return (control & 0x02) != 0; }
    virtual bool disabled() const { return (control & 0x04) != 0; }
    virtual bool resetFreezeMode() const { return (control & 0x40) != 0; }

    // Returns true if the cartridge RAM shows up at the provided address
    virtual bool ramIsEnabled(u16 addr) const;


    //
    // Handling buttons
    //

    isize numButtons() const override { return 2; }
    const char *getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;
    void releaseButton(isize nr) override;
};


//
// Atomic Power (a derivation of the Action Replay cartridge)
//

class AtomicPower final : public ActionReplay {

    CartridgeTraits traits = {

        .type       = CRT_ATOMIC_POWER,
        .title      = "Atomic Power",
        .memory     = KB(8),
        .buttons    = 2,
        .button1    = "Freeze",
        .button2    = "Reset"
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using ActionReplay::ActionReplay;
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

}
