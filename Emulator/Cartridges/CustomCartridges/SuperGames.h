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

class SuperGames : public Cartridge {
    
    // Write protection latch
    bool protect = false;
    
public:
    
    SuperGames(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "Supergames"; }
    CartridgeType getCartridgeType() const override { return CRT_SUPER_GAMES; }


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
