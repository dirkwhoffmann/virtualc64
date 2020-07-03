// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _COMAL80_INC
#define _COMAL80_INC

#include "Cartridge.h"

class Comal80 : public CartridgeWithRegister {
    
    //! Control register
    uint8_t control;
    
public:
    
    Comal80(C64 *c64) : CartridgeWithRegister(c64, "Comal80") { };
    CartridgeType getCartridgeType() { return CRT_COMAL80; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    
    void reset();
    uint8_t peekIO1(u16 addr) { return control; }
    uint8_t peekIO2(u16 addr) { return 0; }
    void pokeIO1(u16 addr, uint8_t value);
};

#endif
