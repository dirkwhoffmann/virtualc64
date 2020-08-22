// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _WESTERMANN_H
#define _WESTERMANN_H

#include "Cartridge.h"

class Westermann : public Cartridge {
    
public:
    
    Westermann(C64 *c64, C64 &ref) : Cartridge(c64, ref, "Westermann") { };
    CartridgeType getCartridgeType() override { return CRT_WESTERMANN; }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) override { return 0; }
};

#endif
