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

class Reu : public Cartridge {

private:

    //
    // REU registers
    //

    // Status register
    u8 sr;

    // Command register
    u8 cr;

    // Base address registers (C64 memory and REU memory)
    u16 c64Base;
    u32 reuBase;

    // Memory bank register
    // u8 bank;

    // Transfer length register
    u16 tlen;

    // Interrupt mask
    u8 imr;
    
    // Address control register
    u8 acr;


    //
    // Emulation specific variables
    //

    // Remembers the memory type of the uppermost memory bank
    MemoryType memTypeF;


    //
    // Initializing
    //

public:

    Reu(C64 &ref) : Cartridge(ref) { };
    Reu(C64 &ref, isize kb) : Reu(ref) { setRamCapacity(kb * 1024); }
    const char *getDescription() const override { return "REU"; }
    CartridgeType getCartridgeType() const override { return CRT_REU; }

private:

    void _reset(bool hard) override;


    //
    // Analyzing
    //

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Serializing
    //

private:

    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker

        << sr
        << cr
        << c64Base
        << reuBase
        << tlen
        << imr
        << acr
        << memTypeF;
    }

    template <class T>
    void applyToResetItems(T& worker, bool hard = true)
    {
    }

    isize __size() override { COMPUTE_SNAPSHOT_SIZE }
    u64 __checksum() override { COMPUTE_SNAPSHOT_CHECKSUM }
    isize __load(const u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    isize __save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }


    //
    // Accessing REU registers
    //

    bool autoloadEnabled() const { return GET_BIT(cr, 5); }
    bool ff00Enabled() const { return !GET_BIT(cr, 4); }
    bool ff00Disabled() const { return GET_BIT(cr, 4); }

    bool irqEnabled() const { return GET_BIT(imr, 7); }
    bool irqOnEndOfBlock() const { return GET_BIT(imr, 6); }
    bool irqOnVerifyError() const { return GET_BIT(imr, 5); }

    bool isArmed() const { return GET_BIT(cr, 7) && ff00Enabled(); }

    isize memStep() const { return GET_BIT(acr,7) ? 0 : 1; }
    isize reuStep() const { return GET_BIT(acr,6) ? 0 : 1; }


    //
    // Accessing cartridge memory
    //

public:

    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO2(u16 addr, u8 value) override;
    void poke(u16 addr, u8 value) override;


    //
    // Performing DMA
    //

private:

    void incMemAddr(u16 &addr) { addr = U16_ADD(addr, 1); }
    void incReuAddr(u32 &addr) { addr = U32_ADD(addr, 1) & 0x3FFFF; }

    void doDma();
    void stash(u16 memAddr, u32 reuAddr, u32 len);
    void fetch(u16 memAddr, u32 reuAddr, u32 len);
    void swap(u16 memAddr, u32 reuAddr, u32 len);
    void verify(u16 memAddr, u32 reuAddr, u32 len);


    //
    // Managing interrupts
    //

    void triggerEndOfBlockIrq();
    void triggerVerifyErrorIrq();


    //
    // Handling delegation calls
    //

public:

    void updatePeekPokeLookupTables() override;

};
