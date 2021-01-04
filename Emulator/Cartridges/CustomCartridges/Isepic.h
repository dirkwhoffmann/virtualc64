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

class Isepic : public Cartridge {
    
    // Selected page inside the selected RAM bank
    u8 page;

    // Original mapping of the uppermost memory page
    MemoryType oldPeekSource;
    MemoryType oldPokeTarget;

    
    //
    // Initialization
    //
    
public:
    
    Isepic(C64 &ref);
    const char *getDescription() override { return "Isepic"; }
    CartridgeType getCartridgeType() override { return CRT_ISEPIC; }
    
private:
    
    void _reset() override;

    
    //
    // Serializing
    //

private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & oldPeekSource
        & oldPokeTarget;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & page;
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
    
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
    
    //
    // Operating switches
    //
    
    bool hasSwitch() override { return true; }
    const char *getSwitchDescription(i8 pos) override;
    void setSwitch(i8 pos) override;
    bool switchInOffPosition() { return switchIsLeft(); }
    bool switchInOnPosition() { return switchIsRight(); }
    
    bool cartIsVisible() { return switchInOnPosition(); }
    bool cartIsHidden() { return !cartIsVisible(); }
    
    
    //
    // Handling delegation calls
    //
    
    void updatePeekPokeLookupTables() override;
};
