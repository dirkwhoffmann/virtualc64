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

class Ocean : public Cartridge {
    
public:
    
    Ocean(C64 &ref);
    const char *getDescription() const override { return "Ocean"; }
    CartridgeType getCartridgeType() const override { return CRT_OCEAN; }

    //
    // Accessing cartridge memory
    //
    
public:
    
    void bankIn(isize nr) override;

    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;
};
