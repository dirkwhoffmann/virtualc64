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
#include "RegressionTester.h"

namespace vc64 {

void
RegressionTester::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

i64
RegressionTester::getOption(Option option) const
{
    switch (option) {

        case OPT_DBG_DEBUGCART:     return (i64)config.debugcart;
        case OPT_DBG_WATCHDOG:      return (i64)config.watchdog;

        default:
            fatalError;
    }
}

void
RegressionTester::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_DBG_DEBUGCART:
        case OPT_DBG_WATCHDOG:

            return;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED);
    }
}

void
RegressionTester::setOption(Option opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case OPT_DBG_DEBUGCART:

            config.debugcart = (bool)value;
            return;

        case OPT_DBG_WATCHDOG:

            config.watchdog = (isize)value;
            setWatchdog(config.watchdog);
            return;

        default:
            fatalError;
    }
}

}
