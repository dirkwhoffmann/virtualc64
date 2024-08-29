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

#include "SubComponent.h"
#include "CartridgeTypes.h"

namespace vc64 {

/* This class implements a Flash Rom module of type Am29F040B. Flash Roms
 * of this type are used, e.g., by the EasyFlash cartridge. The implementation
 * is based on the following ressources:
 *
 *       29F040.pdf : Data sheet published by AMD
 *   flash040core.c : Part of the VICE emulator
 */
class FlashRom final : public SubComponent {

    Descriptions descriptions = {{

        .name           = "FlashRom",
        .description    = "Flash ROM",
        .shell          = ""
    }};

    Options options = {

    };
    
    // Number of sectors in this Flash Rom
    static const isize numSectors = 8;

    // Size of a single sector in bytes (64 KB)
    static const isize sectorSize = 0x10000;

    // Total size of the Flash Rom in bytes (512 KB)
    static const isize romSize = 0x80000;

    // Current Flash Rom state
    FlashState state;

    // State taken after an operations has been completed
    FlashState baseState;

    // Flash Rom data
    u8 *rom = nullptr;


    //
    // Class methods
    //

public:

    // Checks whether the provided number is a valid bank number
    static bool isBankNumber(isize bank) { return bank < 64; }

    // Converts a Flash Rom state to a string
    static const char *getStateAsString(FlashState state);


    //
    // Methods
    //

public:

    FlashRom(C64 &ref);
    ~FlashRom();

    FlashRom& operator= (const FlashRom& other) {

        CLONE(state)
        CLONE(baseState)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << state
        << baseState;
    }

    void operator << (SerResetter &worker) override { serialize(worker); }
    void operator << (SerChecker &worker) override { serialize(worker); }
    void operator << (SerCounter &worker) override;
    void operator << (SerReader &worker) override;
    void operator << (SerWriter &worker) override;


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;
    void _didReset(bool hard) override;


    //
    // Methods from Configurable
    //

public:

    const Options &getOptions() const override { return options; }


    //
    // Loading banks
    //

public:

    /* Loads an 8 KB chunk of Rom data from a buffer. This method is used when
     * loading the contents from a CRT file.
     */
    void loadBank(isize bank, u8 *data);


    //
    // Accessing memory
    //

public:

    u8 peek(u32 addr);
    u8 peek(isize bank, u16 addr);

    u8 spypeek(u32 addr) const;
    u8 spypeek(isize bank, u16 addr) const;

    void poke(u32 addr, u8 value);
    void poke(isize bank, u16 addr, u8 value);


    //
    // Performing flash operations
    //

    // Checks if addr serves as the first command address
    bool firstCommandAddr(u32 addr) { return (addr & 0x7FF) == 0x555; }

    // Checks if addr serves as the second command address
    bool secondCommandAddr(u32 addr) { return (addr & 0x7FF) == 0x2AA; }

    // Performs a "Byte Program" operation
    bool doByteProgram(u32 addr, u8 value);
    bool doByteProgram(isize bank, u16 addr, u8 value);

    // Performs a "Sector Erase" operation
    void doSectorErase(u32 addr);
    void doSectorErase(isize bank, u16 addr);

    // Performs a "Chip Erase" operation
    void doChipErase();
};

}
