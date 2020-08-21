// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _MAGICDESK_H
#define _MAGICDESK_H

#include "Cartridge.h"

class MagicDesk : public Cartridge {
    
public:
    
    MagicDesk(C64 *c64, C64 &ref) : Cartridge(c64, ref, "MagicDesk") { };
    CartridgeType getCartridgeType() { return CRT_MAGIC_DESK; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    u8 peekIO1(u16 addr);
    void pokeIO1(u16 addr, u8 value);
};


#endif
