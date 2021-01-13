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

class SimonsBasic : public Cartridge {
    
public:

    SimonsBasic(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "SimonBasic"; }
    CartridgeType getCartridgeType() const override { return CRT_SIMONS_BASIC; }

private:
    
    void _reset() override;

    
    //
    // Accessing cartridge memory
    //
    
public:

    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
};
