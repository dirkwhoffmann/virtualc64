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

class SuperGames : public Cartridge {
    
    // Write protection latch
    bool protect = false;
    
public:
    
    SuperGames(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "Supergames"; }
    CartridgeType getCartridgeType() const override { return CRT_SUPER_GAMES; }
        
    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        worker
        
        << protect;
    }
    
    isize __size() override { COMPUTE_SNAPSHOT_SIZE }
    isize __load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }


    //
    // Accessing cartridge memory
    //
    
public:
    
    void pokeIO2(u16 addr, u8 value) override;
};
