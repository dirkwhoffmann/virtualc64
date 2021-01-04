// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class Ocean : public Cartridge {
    
public:
    
    Ocean(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() override { return "Ocean"; }
    CartridgeType getCartridgeType() override { return CRT_OCEAN; }

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void pokeIO1(u16 addr, u8 value) override;
};
