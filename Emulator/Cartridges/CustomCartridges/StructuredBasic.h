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

class StructuredBasic : public Cartridge {
        
public:
    
    StructuredBasic(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "StructuredBasic"; }
    CartridgeType getCartridgeType() const override { return CRT_STRUCTURED_BASIC; }
        

    //
    // Accessing cartridge memory
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
};
