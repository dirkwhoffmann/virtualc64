// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MACH5_INC
#define _MACH5_INC

#include "Cartridge.h"

class Mach5 : public Cartridge {

public:

    Mach5(C64 *c64) : Cartridge(c64, "Mach5") { };
    CartridgeType getCartridgeType() { return CRT_MACH5; }

    //
    //! @functiongroup Methods from Cartridge
    //

    void reset();
    uint8_t peekIO1(u16 addr);
    uint8_t peekIO2(u16 addr);
    void pokeIO1(u16 addr, uint8_t value);
    void pokeIO2(u16 addr, uint8_t value);
};

#endif
