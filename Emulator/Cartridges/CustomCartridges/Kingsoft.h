// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _KINGSOFT_H
#define _KINGSOFT_H

#include "Cartridge.h"

class Kingsoft : public Cartridge {
    
public:
    
    Kingsoft(C64 *c64, C64 &ref) : Cartridge(c64, ref, "Kingsoft") { };
    CartridgeType getCartridgeType() { return CRT_KINGSOFT; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();

    u8 peekIO1(u16 addr);
    u8 spyPeek(u16 addr) { return 0; }
    void pokeIO1(u16 addr, u8 value);

    void updatePeekPokeLookupTables();
};



#endif
