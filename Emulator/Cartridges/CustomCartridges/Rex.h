// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _REX_H
#define _REX_H

#include "Cartridge.h"

class Rex : public Cartridge {
    
public:
    
    Rex(C64 *c64, C64 &ref) : Cartridge(c64, ref, "Rex") { };
    CartridgeType getCartridgeType() override { return CRT_REX; }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) override { return 0; }
};

#endif
