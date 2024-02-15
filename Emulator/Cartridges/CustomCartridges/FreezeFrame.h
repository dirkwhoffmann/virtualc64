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

class FreezeFrame : public Cartridge {

    virtual CartridgeTraits traits() const override {

        return CartridgeTraits {

            .type       = CRT_FREEZE_FRAME,
            .title      = "Freeze Frame",

            .button1    = "Freeze"
        };
    }

public:

    FreezeFrame(C64 &ref) : Cartridge(ref) { };
    
private:

    void _reset(bool hard) override;

    
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
