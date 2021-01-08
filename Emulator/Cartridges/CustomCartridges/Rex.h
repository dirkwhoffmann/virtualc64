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

class Rex : public Cartridge {
    
public:
    
    Rex(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "Rex"; }
    CartridgeType getCartridgeType() const override { return CRT_REX; }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) override { return 0; }
};
