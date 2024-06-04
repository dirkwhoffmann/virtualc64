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

class Zaxxon final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_ZAXXON,
        .title      = "Zaxxon",
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

public:

    using Cartridge::Cartridge;

private:

    void _reset(bool hard) override;


    //
    // Accessing cartridge memory
    //

public:

    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) const override;
};

}
