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
#include "FlashRom.h"

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
    const char *getDescription() const override { return "EasyFlash"; }
    CartridgeType getCartridgeType() const override { return CRT_EASYFLASH; }
    
    void resetCartConfig() override;

private:
    
    void _reset(bool hard) override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump(Category category, std::ostream& os) const override;
    
    
    //
    // Serialization
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        << jumper;
    }
    
    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
        worker
        
        << bankReg
        << modeReg
        << bank;
    }
    
    isize __size() override { COMPUTE_SNAPSHOT_SIZE }
    isize __load(const u8 *buffer) override {  LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
    //
    // Handling ROM packets
    //
    
    void loadChip(isize nr, const CRTFile &crt) override;

    
    //
    // Accessing cartridge memory
    //
    
public:
    
    u8 peek(u16 addr) override;
    u8 spypeek(u16 addr) const override;
    void poke(u16 addr, u8 value) override;
    void pokeRomL(u16 addr, u8 value) override;
    void pokeRomH(u16 addr, u8 value) override;
    u8 peekIO1(u16 addr) override;
    u8 spypeekIO1(u16 addr) const override;
    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO1(u16 addr, u8 value) override;
    void pokeIO2(u16 addr, u8 value) override;
    void pokeBankReg(u8 value);
    void pokeModeReg(u8 value);
    bool hasLED() const override { return true; }
};
