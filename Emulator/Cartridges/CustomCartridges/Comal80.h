// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _COMAL80_H
#define _COMAL80_H

#include "Cartridge.h"

class Comal80 : public Cartridge {
        
public:
    
    Comal80(C64 *c64, C64 &ref) : Cartridge(c64, ref, "Comal80") { };
    CartridgeType getCartridgeType() override { return CRT_COMAL80; }
    
    void _reset() override;

    u8 peekIO1(u16 addr) override { return control; }
    u8 peekIO2(u16 addr) override { return 0; }
    void pokeIO1(u16 addr, u8 value) override;
};

#endif
