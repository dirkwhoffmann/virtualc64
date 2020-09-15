// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _STRUCTURED_BASIC_H
#define _STRUCTURED_BASIC_H

#include "Cartridge.h"

class StructuredBasic : public Cartridge {
    
    // Write protection latch
    // bool protect = false;
    
public:
    
    StructuredBasic(C64 &ref) : Cartridge(ref, "StructuredBasic") { };
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
    
    size_t __size() { COMPUTE_SNAPSHOT_SIZE }
    size_t __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    size_t __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    size_t _size() override { return Cartridge::_size() + __size(); }
    size_t _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    size_t _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    void pokeIO1(u16 addr, u8 value) override;
};

#endif
