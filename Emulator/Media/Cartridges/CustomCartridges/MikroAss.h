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

class MikroAss final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_MIKRO_ASSEMBLER,
        .title      = "Mikro Assembler",
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;

    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
};

}
