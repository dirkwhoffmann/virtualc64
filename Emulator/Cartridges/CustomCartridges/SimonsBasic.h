// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _SIMONS_BASIC_H
#define _SIMONS_BASIC_H

#include "Cartridge.h"

class SimonsBasic : public Cartridge {
    
public:

    SimonsBasic(C64 &ref) : Cartridge(ref, "SimonsBasic") { };
    const char *getDescriptionNew() override { return "SimonBasic"; }
    CartridgeType getCartridgeType() override { return CRT_SIMONS_BASIC; }

private:
    
    void _reset() override;

    
    //
    // Accessing cartridge memory
    //
    
public:

    u8 peekIO1(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;
};

#endif

