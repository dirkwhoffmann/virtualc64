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

class GameKiller : public Cartridge {
    
public:
    
    GameKiller(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "GameKiller"; }
    CartridgeType getCartridgeType() const override { return CRT_GAME_KILLER; }
    
private:
    
    //
    // Accessing cartridge memory
    //
    
    void resetCartConfig() override;
    
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override { return 0; }
    u8 spypeekIO1(u16 addr) const override { return 0; }
    u8 peekIO2(u16 addr) override { return 0; }
    u8 spypeekIO2(u16 addr) const override { return 0; }
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
    
    //
    // Operating buttons
    //
    
    isize numButtons() const override { return 1; }
    const char *getButtonTitle(isize nr) const override;
    void pressButton(isize nr) override;
    void releaseButton(isize nr) override;

    
    //
    // Handling delegation calls
    //

    void updatePeekPokeLookupTables() override;
    void nmiWillTrigger() override;
};

