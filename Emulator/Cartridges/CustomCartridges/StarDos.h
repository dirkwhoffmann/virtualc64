// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _STARDOS_H
#define _STARDOS_H

#include "Cartridge.h"

class StarDos : public Cartridge {
    
    private:
    
    u64 voltage = 5000000;
    u64 latestVoltageUpdate = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    StarDos(C64 *c64, C64 &ref) : Cartridge(c64, ref, "StarDos") { };
    CartridgeType getCartridgeType() override { return CRT_STARDOS; }

private:
    
    void _reset() override;
    
    //
    // Accessing cartridge memory
    //
    
    u8 peekIO1(u16 addr) override { charge(); return 0; }
    u8 peekIO2(u16 addr) override { discharge(); return 0; }
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

#endif
