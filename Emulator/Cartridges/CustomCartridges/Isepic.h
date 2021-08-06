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
    const string getDescription() const override { return "Isepic"; }
    CartridgeType getCartridgeType() const override { return CRT_ISEPIC; }
    
private:
    
    void _reset(bool hard) override;

    
    //
    // Serializing
    //

private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << oldPeekSource
        << oldPokeTarget;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        worker
        
        << page;
    }
    
    isize __size() { COMPUTE_SNAPSHOT_SIZE }
    isize __load(const u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    isize _size() override { return Cartridge::_size() + __size(); }
    isize _load(const u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    isize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    

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
    
    bool hasSwitch() const override { return true; }
    const char *getSwitchDescription(i8 pos) const override;
    void setSwitch(i8 pos) override;
    bool switchInOffPosition() const { return switchIsLeft(); }
    bool switchInOnPosition() const { return switchIsRight(); }
    
    bool cartIsVisible() const { return switchInOnPosition(); }
    bool cartIsHidden() const { return !cartIsVisible(); }
    
    
    //
    // Handling delegation calls
    //
    
    void updatePeekPokeLookupTables() override;
};
