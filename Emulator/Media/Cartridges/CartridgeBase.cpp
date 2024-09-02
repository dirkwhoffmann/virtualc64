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
#include "Cartridge.h"
#include "C64.h"

namespace vc64 {

void
Cartridge::operator << (SerResetter &worker)
{
    // Reset external RAM if no battery is present
    if (!getCartridgeTraits().battery) eraseRAM();

    // Reset all chip packets
    for (isize i = 0; i < numPackets; i++) *packet[i] << worker;

    // Bank in visibile chips (chips with low numbers show up first)
    for (int i = MAX_PACKETS - 1; i >= 0; i--) bankIn(i);
}

void
Cartridge::operator << (SerCounter &worker)
{
    serialize(worker);

    // Add ROM size
    for (isize i = 0; i < numPackets; i++) *packet[i] << worker;

    // Add RAM size
    worker.count += ramCapacity;
}

void
Cartridge::operator << (SerReader &worker)
{
    dealloc();

    serialize(worker);

    // Load ROM
    for (isize i = 0; i < numPackets; i++) {

        assert(packet[i] == nullptr);
        packet[i] = new CartridgeRom(c64);
        *packet[i] << worker;
    }

    // Load RAM
    if (ramCapacity) {

        assert(externalRam == nullptr);
        externalRam = new u8[ramCapacity];
        worker.copy(externalRam, ramCapacity);
    }
}

void
Cartridge::operator << (SerWriter &worker)
{
    serialize(worker);

    // Save ROM
    for (isize i = 0; i < numPackets; i++) {

        assert(packet[i] != nullptr);
        *packet[i] << worker;
    }

    // Save RAM
    if (ramCapacity) {

        assert(externalRam != nullptr);
        worker.copy(externalRam, ramCapacity);
    }
}

void
Cartridge::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("Game line in CRT");
        os << bol(gameLineInCrtFile) << std::endl;
        os << tab("Exrom line in CRT");
        os << bol(exromLineInCrtFile) << std::endl;
        os << tab("Number of packets");
        os << dec(numPackets) << std::endl;

        for (isize i = 0; i < numPackets; i++) {

            os << tab("Packet " + std::to_string(i));
            os << dec(packet[i]->size / 1024) << " KB starting at ";
            os << hex(packet[i]->loadAddress) << std::endl;
        }

        if (getRamCapacity()) {

            os << tab("On-Board RAM");
            os << dec(getRamCapacity() / 1024) << " KB" << std::endl;
            os << tab("Battery");
            os << bol(getCartridgeTraits().battery) << std::endl;
        }
    }
}

}
