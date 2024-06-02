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

#include "config.h"
#include "ExpansionPort.h"

namespace vc64 {

ExpansionPort&
ExpansionPort::operator= (const ExpansionPort& other)
{
    auto type = getCartridgeType();
    auto othertype = other.getCartridgeType();

    CLONE(crtType)
    CLONE(gameLine)
    CLONE(exromLine)

    if (type != othertype) {

        // Create a new cartridge of the proper type
        cartridge = std::unique_ptr<Cartridge>(Cartridge::makeWithType(c64, othertype));
    }

    // Clone the cartridge
    if (cartridge) cartridge->clone(*other.cartridge);

    return *this;
}

void
ExpansionPort::operator << (SerResetter &worker)
{
    serialize(worker);
    if (cartridge) *cartridge << worker;
}

void
ExpansionPort::operator << (SerChecker &worker)
{
    serialize(worker);
    if (cartridge) *cartridge << worker;
}

void
ExpansionPort::operator << (SerCounter &worker)
{
    serialize(worker);
    if (cartridge) *cartridge << worker;
}

void
ExpansionPort::operator << (SerReader &worker)
{
    serialize(worker);

    // Delete existing cartridge
    cartridge = nullptr;

    // Load cartridge (if any)
    if (crtType != CRT_NONE) {

        cartridge = std::unique_ptr<Cartridge>(Cartridge::makeWithType(c64, crtType));
        *cartridge << worker;
    }
}

void
ExpansionPort::operator << (SerWriter &worker)
{
    serialize(worker);

    // Save cartridge (if any)
    if (crtType != CRT_NONE) *cartridge << worker;
}

void
ExpansionPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    auto type = cartridge ? cartridge->getCartridgeType() : CRT_NONE;

    if (category == Category::State) {

        os << tab("Game line");
        os << bol(gameLine) << std::endl;
        os << tab("Exrom line");
        os << bol(exromLine) << std::endl;
        os << tab("Cartridge");
        os << CartridgeTypeEnum::key(type) << std::endl;

        if (cartridge) {

            os << std::endl;
            cartridge->dump(category, os);
        }
    }
}

void
ExpansionPort::_reset(bool hard)
{
    if (cartridge) cartridge->_reset(hard);
}

}
