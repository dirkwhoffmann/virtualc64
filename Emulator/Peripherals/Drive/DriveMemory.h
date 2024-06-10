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
#include "DriveTypes.h"

namespace vc64 {

class DriveMemory final : public SubComponent {
    
    Descriptions descriptions = {{

        .name           = "DriveMemory",
        .description    = "Drive Memory"
    }};

    // Reference to the connected disk drive
    class Drive &drive;
    
public:
    
    /* RAM:
     *
     *          VC1541 : 2KB at $0000 - $07FF
     *   Dolphin DOS 2 : 2KB at $0000 - $07FF, 8KB at $8000 - $9FFF
     *   Dolphin DOS 3 : 2KB at $0000 - $07FF, 8KB at $6000 - $7FFF
     *
     * ROM:
     *
     *          VC1541 : 16KB at $C000 - $FFFF, Mirror at $8000
     *   Dolphin DOS 2 : 24KB at $A000 - $FFFF
     *   Dolphin DOS 3 : 32KB at $A000 - $FFFF (???)
     */
    u8 ram[0xA000];
    u8 rom[0x8000] = {};

    // Memory usage table (one entry for each KB)
    DrvMemType usage[64];
    
    
    //
    // Methods
    //
    
public:
    
    DriveMemory(C64 &ref, Drive &drive);

    DriveMemory& operator= (const DriveMemory& other) {

        CLONE_ARRAY(ram)
        CLONE_ARRAY(rom)
        CLONE_ARRAY(usage)

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

        << ram
        << usage;
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
    void _reset(bool hard) override;


    //
    // Working with Roms
    //
    
public:
    
    // Returns the size and start address of the Rom
    u16 romSize() const;
    u16 romAddr() const;

    // Indicates if a Rom is installed
    bool hasRom() const { return romSize() != 0; }

    // Computes a Rom checksum
    u32 romCRC32() const;
    u64 romFNV64() const;

    // Removes the currently installed Rom
    void deleteRom();
    
    // Loads a Rom
    void loadRom(const class RomFile &file);
    void loadRom(const u8 *buf, isize size);
    
    // Saves the currently installed Rom
    void saveRom(const fs::path &path) throws;

    
    //
    // Accessing memory
    //
    
public:

    // Reads a value from memory
    u8 peek(u16 addr);
    u8 peekZP(u8 addr) { return ram[addr]; }
    u8 peekStack(u8 sp) { return ram[0x100 + sp]; }
    
    // Emulates an idle read access
    void peekIdle(u16 addr) { }
    void peekZPIdle(u8 addr) { }
    void peekStackIdle(u8 sp) { }
    void peekIOIdle(u16 addr) { }
    
    // Reads a value from memory without side effects
    u8 spypeek(u16 addr) const;

    // Writes a value into memory
    void poke(u16 addr, u8 value);
    void pokeZP(u8 addr, u8 value) { ram[addr] = value; }
    void pokeStack(u8 sp, u8 value) { ram[0x100 + sp] = value; }

    // Updates the bank map
    void updateBankMap();
};

}
