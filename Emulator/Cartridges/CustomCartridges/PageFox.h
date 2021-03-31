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

class PageFox : public Cartridge {
    
    /* The cartridge has a single control register which can be accessed in
     * the $DE80 - $DEFF memory range.
     */
    u8 ctrlReg = 0;
        
public:
    
    //
    // Initializing
    //
    
    PageFox(C64 &ref);
    const char *getDescription() const override { return "PageFox"; }
    CartridgeType getCartridgeType() const override { return CRT_PAGEFOX; }
    
private:
    
    void _reset() override;
    
    
    //
    // Analyzing
    //
    
private:
    
    void _dump() const override;
    
    
    //
    // Serialization
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << ctrlReg;
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
    // Intepreting the control register
    //
        
    u16 bankSelect() const { return (ctrlReg & 0b00010) >> 1; }
    u8 chipSelect() const { return (ctrlReg & 0b01100) >> 2; }
    u8 bank() const { return (ctrlReg & 0b00110) >> 1; }
    u8 disabled() const { return (ctrlReg & 0b10000) >> 4; }
    u8 ramIsVisible() const { return chipSelect() == 0b10; }
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekRomL(u16 addr) override;
    u8 spypeekRomL(u16 addr) const override;
    u8 peekRomH(u16 addr) override;
    u8 spypeekRomH(u16 addr) const override;
    void pokeRomL(u16 addr, u8 value) override;
    void pokeRomH(u16 addr, u8 value) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void updatePeekPokeLookupTables() override;
    
private:
    
    u16 ramAddrL(u16 addr) const { return (bankSelect() << 14) + (addr & 0x1FFF); }
    u16 ramAddrH(u16 addr) const { return 0x2000 + ramAddrL(addr); }
};
