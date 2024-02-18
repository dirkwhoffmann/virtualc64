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

class StarDos : public Cartridge {
        
    CartridgeTraits traits = {

        .type       = CRT_STARDOS,
        .title      = "StarDOS",
    };

    virtual const CartridgeTraits &getTraits() const override { return traits; }

    i64 voltage = 5000000;
    i64 latestVoltageUpdate = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    StarDos(C64 &ref) : Cartridge(ref) { };

private:
    
    void _reset(bool hard) override;


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //
    
private:
    
    template <class T>
    void serialize(T& worker)
    {
        worker
        
        << voltage
        << latestVoltageUpdate;
    }
    
    CARTRIDGE_SERIALIZERS
 
    
    //
    // Accessing cartridge memory
    //
    
    u8 peekIO1(u16 addr) override { charge(); return 0; }
    u8 spypeekIO1(u16 addr) const override { return 0; }
    u8 peekIO2(u16 addr) override { discharge(); return 0; }
    u8 spypeekIO2(u16 addr) const override { return 0; }
    void pokeIO1(u16 addr, u8 value) override { charge(); }
    void pokeIO2(u16 addr, u8 value) override { discharge(); }
    
    //
    // Handling delegation calls
    //
    
public:
    
    void updatePeekPokeLookupTables() override;
    
    
    //
    // Working with the capacitor
    //
    
private:
    
    void updateVoltage();
    void charge();
    void discharge();
    void enableROML();
    void disableROML();
};
