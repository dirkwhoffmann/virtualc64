// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MIKROASS_H
#define _MIKROASS_H

#include "Cartridge.h"

class MikroAss : public Cartridge {

public:

    MikroAss(C64 *c64, C64 &ref) : Cartridge(c64, ref, "Mikro Assembler") { };
    CartridgeType getCartridgeType() override { return CRT_MIKRO_ASS; }

    
    //
    // Accessing cartridge memory
    //

public:
    
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
};

#endif
