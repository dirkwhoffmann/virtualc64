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

class GeoRAM : public Cartridge {
    
private:
    
    // Selected RAM bank
    u8 bank = 0;
    
    // Selected page inside the selected RAM bank
    u8 page = 0;
    
    
    //
    // Initializing
    //

public:
    
    GeoRAM(C64 &ref) : Cartridge(ref) { };
    const char *getDescription() const override { return "GeoRam"; }
    CartridgeType getCartridgeType() const override { return CRT_GEO_RAM; }
    
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
        
        << bank
        << page;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    usize __size() { COMPUTE_SNAPSHOT_SIZE }
    usize __load(const u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    usize __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    usize _size() override { return Cartridge::_size() + __size(); }
    usize _load(const u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    usize _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    //
    // Accessing cartridge memory
    //
    
public:

    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    
private:
    
    // Maps an address to the proper position in cartridge RAM
    unsigned offset(u8 addr) const;
};
