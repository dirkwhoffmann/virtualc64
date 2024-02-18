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

class Comal80 : public Cartridge {
       
    CartridgeTraits traits = {

        .type       = CRT_COMAL80,
        .title      = "Comal 80",
    };

    virtual const CartridgeTraits &getTraits() const override { return traits; }

public:

    Comal80(C64 &ref) : Cartridge(ref) { };
    
    void operator << (util::SerResetter &worker) override;

    u8 peekIO1(u16 addr) override { return control; }
    u8 spypeekIO1(u16 addr) const override { return control; }
    u8 peekIO2(u16 addr) override { return 0; }
    u8 spypeekIO2(u16 addr) const override { return 0; }
    void pokeIO1(u16 addr, u8 value) override;
};
