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
#include "CoreObject.h"
#include <iostream>
#include <format>

namespace vc64 {

isize
CoreObject::verbosity = 3;

string
CoreObject::prefix(LogLevel level, const std::source_location &loc) const
{
    const isize verbosity = 2;

    switch (verbosity) {

        case 0:  return "";
        case 1:  return std::format("{}: ", objectName());

        default:
            return std::format("{}:{}: ", objectName(), loc.line());
    }
}

}
