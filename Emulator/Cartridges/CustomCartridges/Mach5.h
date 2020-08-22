// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MACH5_H
#define _MACH5_H

#include "Cartridge.h"

class Mach5 : public Cartridge {

public:

    Mach5(C64 *c64, C64 &ref) : Cartridge(c64, ref, "Mach5") { };
    CartridgeType getCartridgeType() override { return CRT_MACH5; }

private:
    
    void _reset() override;

    
    //
    // Accessing cartridge memory
    //

public:
    
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
};

#endif
