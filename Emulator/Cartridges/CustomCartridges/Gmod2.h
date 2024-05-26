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

class Gmod2 final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_GMOD2,
        .title      = "Gmod2",
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;

    u8 peekIO1(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;

private:
    
    bool exrom() const { return control & 0x40; }
    u8 mode() const { return control >> 6; }
    u8 bank() const { return control & 0x3F; }
};

}
