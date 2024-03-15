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

class Ocean final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_OCEAN,
        .title      = "Ocean",
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;

    void bankIn(isize nr) override;
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;
};

}
