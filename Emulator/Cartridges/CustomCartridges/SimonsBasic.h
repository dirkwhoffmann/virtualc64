// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SIMONS_BASIC_INC
#define _SIMONS_BASIC_INC

#include "Cartridge.h"

class SimonsBasic : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_SIMONS_BASIC; }
    void reset();
    uint8_t peekIO1(uint16_t addr);
    uint8_t readIO1(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
};

#endif

