// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _DINAMIC_H
#define _DINAMIC_H

#include "Cartridge.h"

class Dinamic : public Cartridge {
    
public:
    
    Dinamic(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() override { return "Dinamic"; }
    CartridgeType getCartridgeType() override { return CRT_DINAMIC; }

private:
    
    void _reset() override;

    
    //
    // Accessing the cartridge
    //
    
public:
        
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override;
    /*
    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) override;
    */
};

#endif
