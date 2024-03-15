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

class SuperGames final : public Cartridge {

    CartridgeTraits traits = {

        .type       = CRT_SUPER_GAMES,
        .title      = "Super Games",
    };

    virtual const CartridgeTraits &getCartridgeTraits() const override { return traits; }

    // Write protection latch
    bool protect = false;

public:

    SuperGames(C64 &ref) : Cartridge(ref) { };


    //
    // Methods from CoreObject
    //

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from CoreComponent
    //

public:

    SuperGames& operator= (const SuperGames& other) {

        Cartridge::operator=(other);

        CLONE(protect)

        return *this;
    }
    virtual void clone(const Cartridge &other) override { *this = (const SuperGames &)other; }

    template <class T>
    void serialize(T& worker)
    {
        worker

        << protect;

    } CARTRIDGE_SERIALIZERS(serialize)


    //
    // Accessing cartridge memory
    //

public:

    void pokeIO2(u16 addr, u8 value) override;
};

}
