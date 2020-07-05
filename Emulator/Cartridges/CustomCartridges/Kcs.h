// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _KCS_H
#define _KCS_H

#include "Cartridge.h"

class KcsPower : public Cartridge {
    
public:
    KcsPower(C64 *c64, C64 &ref);
    CartridgeType getCartridgeType() { return CRT_KCS_POWER; }
    
    void _reset();
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    u8 peekIO1(u16 addr);
    u8 spypeekIO1(u16 addr);
    u8 peekIO2(u16 addr);
    void pokeIO1(u16 addr, u8 value);
    void pokeIO2(u16 addr, u8 value);
    
    unsigned numButtons() { return 1; }
    const char *getButtonTitle(unsigned nr) { return (nr == 1) ? "Freeze" : NULL; }
    void pressButton(unsigned nr);
    void releaseButton(unsigned nr);
};

#endif

