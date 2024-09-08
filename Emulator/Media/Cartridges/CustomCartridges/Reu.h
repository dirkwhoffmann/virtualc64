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

namespace vc64 {

// Status register bits
namespace SR {
constexpr u8 IRQ_PENDING  = 0b10000000;
constexpr u8 END_OF_BLOCK = 0b01000000;
constexpr u8 VERIFY_ERROR = 0b00100000;
constexpr u8 CHIPS_256K   = 0b00010000;
constexpr u8 VERSION      = 0b00001111;
}

// Control register bits
namespace CR {
constexpr u8 EXECUTE      = 0b10000000;
constexpr u8 RESERVED     = 0b01001100;
constexpr u8 AUTOLOAD     = 0b00100000;
constexpr u8 FF00_DISABLE = 0b00010000;
constexpr u8 TRANSFER     = 0b00000011;
}

class Reu final : public Cartridge {

    CartridgeTraits traits = {

        .type           = CRT_REU,
        .title          = "REU",
        .battery        = true,
        .needsExecution = true
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

private:

    // REU capacity in KB
    isize kb = 0;

    //
    // REU registers
    //

    // Status register (0x00)
    u8 sr = 0;

    // Command register (0x01)
    u8 cr = 0;

    // C64 base address register (0x02 - 0x03)
    u16 c64Base = 0;
    u16 c64BaseLatched = 0;

    // REU base address register (0x04 - 0x05)
    u16 reuBase = 0;
    u16 reuBaseLatched = 0;

    // Bank register (0x06)
    u8 reuBank = 0;
    u8 reuBankLatched = 0;

    // Transfer length register (0x07 - 0x08)
    u16 tlength = 0;
    u16 tlengthLatched = 0;

    // Interrupt mask register (0x09)
    u8 imr = 0;

    // Address control register (0x0A)
    u8 acr = 0;

    // Data registers used during DMA operations
    // TODO: Merge with 'bus' eventually
    u8 c64Val = 0;
    u8 reuVal = 0;
    
    // Flipflop used to control the swap operation
    bool swapff = false;

    // Last to values of the BA line as seen by the REU
    bool ba[2];

    // Indicates a condition where BA rises too late to be seen by the REU
    bool lateBA;


    //
    // Emulation specific variables
    //

    // Upper bank bits (used by modded REUs with higher capacities)
    u32 upperBankBits = 0;

    // Latest value on the data bus
    u8 bus = 0;

    // Remembers the memory type of the uppermost memory bank
    MemoryType memTypeF = M_NONE;


    //
    // State machine
    //

    // The action to be performed next
    EventID action = EVENT_NONE;

    // Remainig wait states until the action is being performed
    isize waitStates = 0;


    //
    // Initializing
    //

public:

    Reu(C64 &ref) : Cartridge(ref) { };
    Reu(C64 &ref, isize kb);


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from CoreComponent
    //

public:

    Reu& operator= (const Reu& other) {

        Cartridge::operator=(other);

        CLONE(kb)

        CLONE(sr)
        CLONE(cr)
        CLONE(c64Base)
        CLONE(c64BaseLatched)
        CLONE(reuBase)
        CLONE(reuBaseLatched)
        CLONE(reuBank)
        CLONE(reuBankLatched)
        CLONE(tlength)
        CLONE(tlengthLatched)
        CLONE(imr)
        CLONE(acr)

        CLONE(c64Val)
        CLONE(reuVal)
        CLONE(swapff)
        CLONE_ARRAY(ba)
        CLONE(lateBA);

        CLONE(upperBankBits)
        CLONE(bus)
        CLONE(memTypeF)

        return *this;
    }
    virtual void clone(const Cartridge &other) override { *this = (const Reu &)other; }

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << kb

        << sr
        << cr
        << c64Base
        << c64BaseLatched
        << reuBase
        << reuBaseLatched
        << reuBank
        << reuBankLatched
        << tlength
        << tlengthLatched
        << imr
        << acr

        << c64Val
        << reuVal
        << swapff
        << ba
        << lateBA

        << upperBankBits
        << bus
        << memTypeF;

    } CARTRIDGE_SERIALIZERS(serialize);

    void _didReset(bool hard) override;


    //
    // Querying properties
    //

public:

    bool isREU1700() const { return getRamCapacity() == KB(128); }
    bool isREU1764() const { return getRamCapacity() == KB(256); }
    bool isREU1750() const { return getRamCapacity() >= KB(512); }

    // Returns the bitmask of the REU address register
    u32 wrapMask() const { return isREU1700() ? 0x1FFFF : 0x7FFFF; }

    // Returns true if a DMA transfer has been initiated
    bool isActive() const;

    /* Emulation speed
     *
     * This value indicates how many bytes are transfered during a single DMA
     * cycle. A value of 1 means the REU is emulated at native speed. A value
     * of 2 emulates a REU at twice the speed etc. A very high value emulates
     * a turbo REU. In that case, the entire data transfer is performed in a
     * single DMA cycle.
     */
    isize bytesPerDmaCycle() { return expansionPort.getConfig().reuSpeed; }


    //
    // Accessing REU registers
    //

private:

    bool autoloadEnabled() const { return GET_BIT(cr, 5); }
    bool ff00Enabled() const { return !GET_BIT(cr, 4); }
    bool ff00Disabled() const { return GET_BIT(cr, 4); }

    bool irqEnabled() const { return GET_BIT(imr, 7); }
    bool irqOnEndOfBlock() const { return GET_BIT(imr, 6); }
    bool irqOnVerifyError() const { return GET_BIT(imr, 5); }

    bool isArmed() const { return GET_BIT(cr, 7) && ff00Enabled(); }

    isize memStep() const { return GET_BIT(acr,7) ? 0 : 1; }
    isize reuStep() const { return GET_BIT(acr,6) ? 0 : 1; }

    void incMemAddr();
    void incReuAddr();


    //
    // Accessing memory
    //

public:

    void eraseRAM() override;

    u8 peekIO2(u16 addr) override;
    u8 spypeekIO2(u16 addr) const override;
    void pokeIO2(u16 addr, u8 value) override;
    void poke(u16 addr, u8 value) override;

private:

    u8 readFromC64Ram(u16 addr);
    void writeToC64Ram(u16 addr, u8 value);

    u8 readFromReuRam(u32 addr);
    void writeToReuRam(u32 addr, u8 value);
    void prefetch(u32 addr) { (void)readFromReuRam(addr); }

    // Checks whether a given address maps to a floating bus
    bool floating(u32 addr) const;

    // Maps an address to the (mirrored) physical REU address
    u32 mapAddr(u32 addr) const;


    //
    //Emulating the device
    //

public:

    // Executes pending actions
    void execute() override;

private:

    // Called by execute()
    void execute(EventID id);

    // Schedules a new action
    void schedule(EventID id, isize ws = 0) { action = id; waitStates = ws; }


    //
    // Performing DMA
    //

public:

    // Records the current value of the BA line
    void sniffBA();

    // Checks whether the REU can use the bus
    bool busIsBlocked(EventID id) const;

    // Initiates a DMA transfer
    void initiateDma();

    // Performs a single DMA cycle
    isize doDma(EventID id);

    void finalizeDma(EventID id);


    //
    // Managing interrupts
    //

private:

    void triggerEndOfBlockIrq();
    void triggerVerifyErrorIrq();


    //
    // Handling delegation calls
    //

public:

    void updatePeekPokeLookupTables() override;
};

}
