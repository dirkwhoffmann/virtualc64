// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MAGICDESK_INC
#define _MAGICDESK_INC

#include "Cartridge.h"

class MagicDesk : public CartridgeWithRegister {
    
public:
    
    MagicDesk(C64 *c64) : CartridgeWithRegister(c64, "MagicDesk") { };
    CartridgeType getCartridgeType() { return CRT_MAGIC_DESK; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    uint8_t peekIO1(u16 addr);
    void pokeIO1(u16 addr, uint8_t value);
};


#endif
