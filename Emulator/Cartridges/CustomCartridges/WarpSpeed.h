// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _WARPSPEED_H
#define _WARPSPEED_H

#include "Cartridge.h"

class WarpSpeed : public Cartridge {
    
public:
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_WARPSPEED; }
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    void resetCartConfig();
    bool hasResetButton() { return true; }
    u8 peekIO1(u16 addr);
    u8 peekIO2(u16 addr);
    void pokeIO1(u16 addr, u8 value);
    void pokeIO2(u16 addr, u8 value);
    
    unsigned numButtons() { return 1; }
    const char *getButtonTitle(unsigned nr) { return (nr == 1) ? "Reset" : NULL; }
    void pressButton(unsigned nr);
};

#endif
