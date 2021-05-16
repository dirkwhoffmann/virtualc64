// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class KcsPower : public Cartridge {
    
public:
    
    KcsPower(C64 &ref);
    const char *getDescription() const override { return "Ksc"; }
    CartridgeType getCartridgeType() const override { return CRT_KCS_POWER; }
    
private:
    
    void _reset(bool hard) override;

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
    
    //
    // Operating buttons
    //
    
    long numButtons() const override { return 1; }
    const char *getButtonTitle(unsigned nr) const override;
    void pressButton(unsigned nr) override;
    void releaseButton(unsigned nr) override;
};
