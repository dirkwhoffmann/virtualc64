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
#include "ControlPort.h"
#include "IOUtils.h"

namespace vc64 {

void
ControlPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("Port Nr");
        os << dec(objid) << std::endl;
        os << tab("Detetected device");
        os << ControlPortDeviceEnum::key(device) << std::endl;
    }
}

}
