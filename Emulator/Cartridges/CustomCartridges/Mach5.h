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

class Mach5 : public Cartridge {

public:

    Mach5(C64 &ref) : Cartridge(ref) { };
    const string getDescription() const override { return "Mach5"; }
    CartridgeType getCartridgeType() const override { return CRT_MACH5; }

private:
    
    void _reset(bool hard) override;

    
    //
    // Accessing cartridge memory
    //

public:
    
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
};
