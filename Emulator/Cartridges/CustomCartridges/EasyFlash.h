// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _EASYFLASH_H
#define _EASYFLASH_H

#include "Cartridge.h"

class EasyFlash : public Cartridge {
    
    // Flash Rom mapping to ROML ($8000 - $9FFF)
    FlashRom flashRomL = FlashRom(c64);

    // Flash Rom mapping to ROMH ($A000 - $B000 or $E000 - $FFFF)
    FlashRom flashRomH = FlashRom(c64);
    
    // Registers register
    u8 bankReg = 0;
    u8 modeReg = 0;
    
    // Selected memory bank
    u8 bank = 0;
    
    // Jumper for controlling the Game line
    bool jumper = false;

public:
    
    //
    // Initializing
    //
    
    EasyFlash(C64 &ref);
    CartridgeType getCartridgeType() override { return CRT_EASYFLASH; }
    
    void resetCartConfig() override;

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
        
        & jumper;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & bankReg
        & modeReg
        & bank;
    }
    
    size_t __size() { COMPUTE_SNAPSHOT_SIZE }
    size_t __load(u8 *buffer) { LOAD_SNAPSHOT_ITEMS }
    size_t __save(u8 *buffer) { SAVE_SNAPSHOT_ITEMS }
    
    size_t _size() override { return Cartridge::_size() + __size(); }
    size_t _load(u8 *buf) override { return Cartridge::_load(buf) + __load(buf); }
    size_t _save(u8 *buf) override { return Cartridge::_save(buf) + __save(buf); }
    
    
    //
    // Handling ROM packets
    //
    
    void loadChip(unsigned nr, CRTFile *c) override;

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peek(u16 addr) override;
    void poke(u16 addr, u8 value) override;
    void pokeRomL(u16 addr, u8 value) override;
    void pokeRomH(u16 addr, u8 value) override;
    u8 peekIO1(u16 addr) override;
    u8 peekIO2(u16 addr) override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    void pokeBankReg(u8 value);
    void pokeModeReg(u8 value);
    bool hasLED() override { return true; }
};

#endif
