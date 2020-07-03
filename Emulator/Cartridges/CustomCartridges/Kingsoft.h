// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _KINGSOFT_INC
#define _KINGSOFT_INC

#include "Cartridge.h"

class Kingsoft : public Cartridge {
    
public:
    
    Kingsoft(C64 *c64) : Cartridge(c64, "Kingsoft") { };
    CartridgeType getCartridgeType() { return CRT_KINGSOFT; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();

    uint8_t peekIO1(u16 addr);
    uint8_t spyPeek(u16 addr) { return 0; }
    void pokeIO1(u16 addr, uint8_t value);

    void updatePeekPokeLookupTables();
};



#endif
