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

class PageFox final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_PAGEFOX,
        .title      = "Pagefox",

        .memory     = KB(32)
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }


    /* The cartridge has a single control register which can be accessed in
     * the $DE80 - $DEFF memory range.
     */
    u8 ctrlReg = 0;

public:

    //
    // Initializing
    //

    using Cartridge::Cartridge;


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from CoreComponent
    //

public:

    PageFox& operator= (const PageFox& other) {

        Cartridge::operator=(other);

        CLONE(ctrlReg)

        return *this;
    }
    virtual void clone(const Cartridge &other) override { *this = (const PageFox &)other; }


    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << ctrlReg;

    } CARTRIDGE_SERIALIZERS(serialize);

    void _didReset(bool hard) override;


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

    u16 ramAddrL(u16 addr) const { return (u16)(bankSelect() << 14 | (addr & 0x1FFF)); }
    u16 ramAddrH(u16 addr) const { return 0x2000 + ramAddrL(addr); }
};

}
