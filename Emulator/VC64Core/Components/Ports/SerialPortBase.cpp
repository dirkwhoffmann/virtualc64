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
#include "SerialPort.h"
#include "Drive.h"

namespace vc64 {

void
SerialPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("VIA1::DDRB (Drive8)");
        os << hex(drive8.via1.getDDRB()) << std::endl;
        os << tab("VIA1::DDRB (Drive9)");
        os << hex(drive9.via1.getDDRB()) << std::endl;
        os << tab("Idle");
        os << dec(stats.idle) << " frames" << std::endl;
    }
}

}
