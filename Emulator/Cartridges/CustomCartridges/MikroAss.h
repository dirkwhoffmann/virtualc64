// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MIKROASS_INC
#define _MIKROASS_INC

#include "Cartridge.h"

class MikroAss : public Cartridge {

public:

    MikroAss(C64 *c64) : Cartridge(c64, "Mikro Assembler") { };
    CartridgeType getCartridgeType() { return CRT_MIKRO_ASS; }

    //
    //! @functiongroup Methods from Cartridge
    //

    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
};

#endif
