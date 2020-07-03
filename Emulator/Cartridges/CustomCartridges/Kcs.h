// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _KCS_INC
#define _KCS_INC

#include "Cartridge.h"

class KcsPower : public Cartridge {
    
public:
    KcsPower(C64 *c64);
    CartridgeType getCartridgeType() { return CRT_KCS_POWER; }
    
    void reset();
    
    //
    //! @functiongroup Methods from Cartridge
    //
    
    uint8_t peekIO1(uint16_t addr);
    uint8_t spypeekIO1(uint16_t addr);
    uint8_t peekIO2(uint16_t addr);
    void pokeIO1(uint16_t addr, uint8_t value);
    void pokeIO2(uint16_t addr, uint8_t value);
    
    unsigned numButtons() { return 1; }
    const char *getButtonTitle(unsigned nr) { return (nr == 1) ? "Freeze" : NULL; }
    void pressButton(unsigned nr);
    void releaseButton(unsigned nr);
};

#endif

