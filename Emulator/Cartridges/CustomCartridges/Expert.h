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

class Expert : public Cartridge {
    
    // Flipflop deciding whether the cartridge is enabled or disabled
    bool active = false;

    
    //
    // Initializing
    //
    
public:
    
    Expert(C64 &ref);
    const char *getDescription() override { return "Expert"; }
    CartridgeType getCartridgeType() override { return CRT_EXPERT; }

private:
    
    void _reset() override;

    
    //
    // Initializing
    //

    void _dump() override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & active;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    usize __size() { COMPUTE_SNAPSHOT_SIZE }
    usize __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    usize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    usize _size() override { return Cartridge::_size() + __size(); }
    usize _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    usize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }

   
    //
    // Handling ROM packets
    //
    
    void loadChip(unsigned nr, CRTFile &c) override;

    
    //
    // Accessing cartridge memory
    //
          
    void updatePeekPokeLookupTables() override;
    u8 peek(u16 addr) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) override { return 0; }
    void poke(u16 addr, u8 value) override;
    void pokeIO1(u16 addr, u8 value) override;
    
    bool cartridgeRamIsVisible(u16 addr);
    bool cartridgeRamIsWritable(u16 addr);

    
    //
    // Operating buttons
    //
    
    long numButtons() override { return 2; }
    const char *getButtonTitle(unsigned nr) override;
    void pressButton(unsigned nr) override;
    
    
    //
    // Operating switches
    //
    
    bool hasSwitch() override { return true; }
    const char *getSwitchDescription(i8 pos) override;
    bool switchInPrgPosition() { return switchIsLeft(); }
    bool switchInOffPosition() { return switchIsNeutral(); }
    bool switchInOnPosition() { return switchIsRight(); }
    
    
    //
    // Handling delegation calls
    //
    
    void nmiWillTrigger() override;
};
