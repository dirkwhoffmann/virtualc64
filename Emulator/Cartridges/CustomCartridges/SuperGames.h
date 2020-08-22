// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SUPERGAMES_H
#define _SUPERGAMES_H

#include "Cartridge.h"

class Supergames : public Cartridge {
    
public:
    
    Supergames(C64 &ref) : Cartridge(ref, "Supergames") { };
    CartridgeType getCartridgeType() override { return CRT_SUPER_GAMES; }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void pokeIO2(u16 addr, u8 value) override;
};

#endif
