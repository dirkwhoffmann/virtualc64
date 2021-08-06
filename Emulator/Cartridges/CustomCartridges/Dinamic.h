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

class Dinamic : public Cartridge {
    
public:
    
    Dinamic(C64 &ref) : Cartridge(ref) { };
    const string getDescription() const override { return "Dinamic"; }
    CartridgeType getCartridgeType() const override { return CRT_DINAMIC; }

private:
    
    void _reset(bool hard) override;

    
    //
    // Accessing the cartridge
    //
    
public:
        
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
};
