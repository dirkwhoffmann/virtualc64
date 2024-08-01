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

class FreezeFrame final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_FREEZE_FRAME,
        .title      = "Freeze Frame",

        .buttons    = 1,
        .button1    = "Freeze"
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;

private:


    void _didReset(bool hard) override;


    //
    // Accessing cartridge memory
    //

public:

    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;


    //
    // Operating buttons
    //

    isize numButtons() const override { return 1; }
    const char *getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;
    void releaseButton(isize nr) override;
};

}
