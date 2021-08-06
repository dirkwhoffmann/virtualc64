// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class Ocean : public Cartridge {
    
public:
    
    Ocean(C64 &ref);
    const string getDescription() const override { return "Ocean"; }
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
