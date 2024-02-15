// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Cartridge.h"
#include "FlashRom.h"

class EasyFlash : public Cartridge {
    
    CartridgeTraits traits = {

        .type       = CRT_EASYFLASH,
        .title      = "EasyFlash",

        .led        = "LED"
    };

    virtual const CartridgeTraits &getTraits() const override { return traits; }

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
    
    void resetCartConfig() override;

private:
    
    void _reset(bool hard) override;

    
    //
    // Methods from CoreObject
    //
    
private:
    
    void _dump(Category category, std::ostream& os) const override;
    
    
    //
    // Methods from CoreComponent
    //
    
private:

    template <class T>
    void serialize(T& worker)
    {
        worker
        
        << bankReg
        << modeReg
        << bank;

        if (util::isResetter(worker)) return;

        worker

        << jumper;
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
};
