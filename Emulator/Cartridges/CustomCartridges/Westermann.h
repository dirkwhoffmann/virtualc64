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
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_WESTERMANN; }
    
    u8 peekIO2(u16 addr);
    u8 spypeekIO2(u16 addr) { return 0; }
};

#endif
