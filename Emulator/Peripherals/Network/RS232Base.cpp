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
#include "RS232.h"

namespace vc64 {

void
RS232::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

i64
RS232::getOption(Option option) const
{
    switch (option) {

        case OPT_RS232_BAUD:    return (i64)config.baud;

        default:
            fatalError;
    }
}

void
RS232::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_RS232_BAUD:
            return;

        default:
            throw Error(ERROR_OPT_UNSUPPORTED);
    }
}

void
RS232::setOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_RS232_BAUD:

            config.baud = isize(value);
            return;

        default:
            fatalError;
    }
}
}
