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
    void applyToResetItems(T& worker)
    {
        worker
        
        << protect;
    }
    
    usize __size() { COMPUTE_SNAPSHOT_SIZE }
    usize __load(const u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    usize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    isize _size() override { return Cartridge::_size() + __size(); }
    isize _load(const u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    isize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void pokeIO2(u16 addr, u8 value) override;
};
