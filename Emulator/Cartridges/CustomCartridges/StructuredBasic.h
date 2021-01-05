// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"

class StructuredBasic : public Cartridge {
    
    // Write protection latch
    // bool protect = false;
    
public:
    
    StructuredBasic(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() override { return "StructuredBasic"; }
    CartridgeType getCartridgeType() override { return CRT_STRUCTURED_BASIC; }
        
    
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
        /*
        worker
        
        & protect;
        */
    }
    
    usize __size() { COMPUTE_SNAPSHOT_SIZE }
    usize __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    usize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    usize _size() override { return Cartridge::_size() + __size(); }
    usize _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    usize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    void pokeIO1(u16 addr, u8 value) override;
};
