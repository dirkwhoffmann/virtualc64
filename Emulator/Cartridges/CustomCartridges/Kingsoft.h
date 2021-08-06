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

class Kingsoft : public Cartridge {
    
public:
    
    Kingsoft(C64 &ref) : Cartridge(ref) { };
    const string getDescription() const override { return "Kingsoft"; }
    CartridgeType getCartridgeType() const override { return CRT_KINGSOFT; }

    void resetCartConfig() override;

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;

    
    //
    // Handling delegation calls
    //
    
public:
    
    void updatePeekPokeLookupTables() override;
};
