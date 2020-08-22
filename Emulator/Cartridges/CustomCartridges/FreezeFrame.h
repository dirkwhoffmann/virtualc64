// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FREEZEFRAME_H
#define _FREEZEFRAME_H

#include "Cartridge.h"

class FreezeFrame : public Cartridge {
    
public:
    
    FreezeFrame(C64 *c64, C64 &ref) : Cartridge(c64, ref, "FreezeFrame") { };
    CartridgeType getCartridgeType() override { return CRT_FREEZE_FRAME; }
    
private:

    void _reset() override;

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) override { return 0; }

    
    //
    // Operating buttons
    //
    
    unsigned numButtons() override { return 1; }
    const char *getButtonTitle(unsigned nr) override;
    void pressButton(unsigned nr) override;
    void releaseButton(unsigned nr) override;
};

#endif
