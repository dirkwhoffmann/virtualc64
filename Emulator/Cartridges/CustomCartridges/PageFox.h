// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PAGEFOX_H
#define _PAGEFOX_H

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
    CartridgeType getCartridgeType() override { return CRT_PAGEFOX; }
    
private:
    
    void _reset() override;
    
    
    //
    // Analyzing
    //
    
private:
    
    void _dump() override;
    
    
    //
    // Serialization
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & ctrlReg;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
    }
    
    size_t __size() { COMPUTE_SNAPSHOT_SIZE }
    size_t __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    size_t __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    size_t _size() override { return Cartridge::_size() + __size(); }
    size_t _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    size_t _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    //
    // Intepreting the control register
    //
        
    u16 bankSelect()  { return (ctrlReg & 0b00010) >> 1; }
    u8 chipSelect()   { return (ctrlReg & 0b01100) >> 2; }
    u8 bank()         { return (ctrlReg & 0b00110) >> 1; }
    u8 disabled()     { return (ctrlReg & 0b10000) >> 4; }
    u8 ramIsVisible() { return chipSelect() == 0b10; }
    
    //
    // Accessing cartridge memory
    //
    
public:
    
    void resetCartConfig() override;
    u8 peekRomL(u16 addr) override;
    u8 peekRomH(u16 addr) override;
    void pokeRomL(u16 addr, u8 value) override;
    void pokeRomH(u16 addr, u8 value) override;
    u8 peekIO1(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;
    void updatePeekPokeLookupTables() override;
    
private:
    
    u16 ramAddrL(u16 addr) { return (bankSelect() << 14) + (addr & 0x1FFF); }
    u16 ramAddrH(u16 addr) { return 0x2000 + ramAddrL(addr); }
};

#endif
