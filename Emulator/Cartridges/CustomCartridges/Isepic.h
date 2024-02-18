// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class Isepic : public Cartridge {
    
    CartridgeTraits traits = {

        .type           = CRT_ISEPIC,
        .title          = "Isepic",
        .memory         = KB(2),
        .switches       = 1,
        .switchLeft     = "Off",
        .switchRight    = "On"
    };

    virtual const CartridgeTraits &getTraits() const override { return traits; }

    // Selected page inside the selected RAM bank
    isize page;

    // Original mapping of the uppermost memory page
    MemoryType oldPeekSource;
    MemoryType oldPokeTarget;

    
    //
    // Initialization
    //
    
public:
    
    Isepic(C64 &ref);


    //
    // Methods from CoreObject
    //

    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

private:
        
    template <class T>
    void serialize(T& worker)
    {
        worker
        
        << page;

        if (util::isResetter(worker)) return;

        worker

        << oldPeekSource
        << oldPokeTarget;
    }
    
    void operator << (util::SerResetter &worker) override;
    void operator << (util::SerChecker &worker) override { Cartridge::operator<<(worker); serialize(worker); }
    void operator << (util::SerCounter &worker) override { Cartridge::operator<<(worker); serialize(worker); }
    void operator << (util::SerReader &worker) override { Cartridge::operator<<(worker); serialize(worker); }
    void operator << (util::SerWriter &worker) override { Cartridge::operator<<(worker); serialize(worker); }


    //
    // Accessing cartridge memory
    //
    
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
    
    //
    // Operating switches
    //
    
    const char *getSwitchDescription(isize pos) const override;
    void setSwitch(isize pos) override;
    bool switchInOffPosition() const { return switchIsLeft(); }
    bool switchInOnPosition() const { return switchIsRight(); }
    
    bool cartIsVisible() const { return switchInOnPosition(); }
    bool cartIsHidden() const { return !cartIsVisible(); }
    
    
    //
    // Handling delegation calls
    //
    
    void updatePeekPokeLookupTables() override;
};
