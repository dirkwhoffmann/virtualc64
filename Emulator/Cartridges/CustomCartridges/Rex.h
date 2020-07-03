// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _REX_INC
#define _REX_INC

#include "Cartridge.h"

class Rex : public Cartridge {
    
public:
    
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_REX; }
    
    uint8_t peekIO2(u16 addr);
    uint8_t spypeekIO2(u16 addr) { return 0; }
};

#endif
