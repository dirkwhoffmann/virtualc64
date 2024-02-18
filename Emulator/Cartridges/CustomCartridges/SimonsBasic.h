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

class SimonsBasic : public Cartridge {
   
    CartridgeTraits traits = {

        .type       = CRT_SIMONS_BASIC,
        .title      = "Simons Basic",
    };

    virtual const CartridgeTraits &getTraits() const override { return traits; }

public:

    SimonsBasic(C64 &ref) : Cartridge(ref) { };

private:
    
    void operator << (util::SerResetter &worker) override;

    
    //
    // Accessing cartridge memory
    //
    
public:

    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
};
