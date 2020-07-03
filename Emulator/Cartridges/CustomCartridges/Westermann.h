// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _WESTERMANN_INC
#define _WESTERMANN_INC

#include "Cartridge.h"

class Westermann : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_WESTERMANN; }
    
    uint8_t peekIO2(uint16_t addr);
    uint8_t spypeekIO2(uint16_t addr) { return 0; }
};

#endif
