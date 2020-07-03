// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _WARPSPEED_INC
#define _WARPSPEED_INC

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
    uint8_t peekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
    
    unsigned numButtons() { return 1; }
    const char *getButtonTitle(unsigned nr) { return (nr == 1) ? "Reset" : NULL; }
    void pressButton(unsigned nr);
};

#endif
