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

using namespace vc64;

class CartridgeRom : public SubComponent {
    
    friend class Cartridge;
    
protected:
    
    // Rom data
    u8 *rom = nullptr;
    
public:
    
    // Size of the Rom data in bytes
    u16 size = 0;
    
    /* Load address. This value is taken from the .CRT file. Possible values
     * are $8000 for chips mapping into the ROML area, $A000 for chips mapping
     * into the ROMH area in 16KB game mode, and $E000 for chips mapping into
     * the ROMH area in ultimax mode.
     */
    u16 loadAddress = 0;
    
    
    //
    // Initializing
    //
    
public:
    
    CartridgeRom(C64 &ref);
    CartridgeRom(C64 &ref, u16 _size, u16 _loadAddress, const u8 *buffer = nullptr);
    ~CartridgeRom();
    const char *getDescription() const override { return "CartridgeRom"; }

    
    //
    // Methods from CoreComponent
    //
    
private:
    
    template <class T>
    void serialize(T& worker)
    {
        if (util::isResetter(worker)) return;

        worker

        << size
        << loadAddress;
    }
    
    void operator << (util::SerChecker &worker) override { serialize(worker); }
    void operator << (util::SerCounter &worker) override;
    void operator << (util::SerResetter &worker) override;
    void operator << (util::SerReader &worker) override;
    void operator << (util::SerWriter &worker) override;

    
    //
    // Accessing
    //
    
public:
    
    // Returns true if this Rom chip maps to ROML
    bool mapsToL() const;
    
    // Returns true if this Rom chip maps to ROMH
    bool mapsToH() const;

    // Returns true if this Rom chip maps to both ROML and ROMH
    bool mapsToLH() const;
    
    // Reads or writes a byte
    u8 peek(u16 addr);
    u8 spypeek(u16 addr) const;
    void poke(u16 addr, u8 value) { }
};
