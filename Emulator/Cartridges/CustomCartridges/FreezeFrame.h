// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FREEZEFRAME_INC
#define _FREEZEFRAME_INC

#include "Cartridge.h"

class FreezeFrame : public Cartridge {
    
public:
    
    using Cartridge::Cartridge;
    CartridgeType getCartridgeType() { return CRT_FREEZE_FRAME; }

    //
    //! @functiongroup Methods from VirtualComponent
    //

    void reset();

    //
    //! @functiongroup Methods from Cartridge
    //
    
    uint8_t peekIO1(uint16_t addr);
    uint8_t spypeekIO1(uint16_t addr) { return 0; }
    uint8_t peekIO2(uint16_t addr);
    uint8_t spypeekIO2(uint16_t addr) { return 0; }

    unsigned numButtons() { return 1; }
    const char *getButtonTitle(unsigned nr) { return (nr == 1) ? "Freeze" : NULL; }
    void pressButton(unsigned nr);
    void releaseButton(unsigned nr);
};

#endif
