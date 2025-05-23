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

#include "VirtualC64Config.h"
#include "UserPort.h"

namespace vc64 {

u8 
UserPort::getPB() const
{
    switch (config.device) {

        case UserPortDevice::RS232:
            return rs232.getPB();

        default:
            return 0xFF;
    }
}

void
UserPort::setPA2(bool value)
{
    switch (config.device) {

        case UserPortDevice::RS232:

            rs232.setPA2(value);
            break;

        default:
            break;
    }
}

}
