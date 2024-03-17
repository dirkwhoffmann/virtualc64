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

namespace vc64 {

class Expert final : public Cartridge {

    CartridgeTraits traits = {

        .type           = CRT_EXPERT,
        .title          = "Expert",

        .memory         = KB(8),
        .battery        = true,

        .buttons        = 2,
        .button1        = "Reset",
        .button2        = "ESM",

        .switches       = 1,
        .switchLeft     = "Prg",
        .switchNeutral  = "Off",
        .switchRight    = "On"
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

    // Flipflop deciding whether the cartridge is enabled or disabled
    bool active = false;


    //
    // Initializing
    //

public:

    using Cartridge::Cartridge;


    //
    // Methods from CoreObject
    //

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from CoreComponent
    //

public:

    Expert& operator= (const Expert& other) {

        Cartridge::operator=(other);

        CLONE(active)

        return *this;
    }
    virtual void clone(const Cartridge &other) override { *this = (const Expert &)other; }

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << active;

    } CARTRIDGE_SERIALIZERS(serialize);


    //
    // Handling ROM packets
    //

    void loadChip(isize nr, const CRTFile &crt) override;


    //
    // Accessing cartridge memory
    //

    void updatePeekPokeLookupTables() override;
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;

    bool cartridgeRamIsVisible(u16 addr) const;
    bool cartridgeRamIsWritable(u16 addr) const;


    //
    // Operating buttons
    //

    isize numButtons() const override { return 2; }
    const char *getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;


    //
    // Operating switches
    //

    const char *getSwitchDescription(isize pos) const override;
    bool switchInPrgPosition() const { return switchIsLeft(); }
    bool switchInOffPosition() const { return switchIsNeutral(); }
    bool switchInOnPosition() const { return switchIsRight(); }


    //
    // Handling delegation calls
    //

    void nmiWillTrigger() override;
};

}
