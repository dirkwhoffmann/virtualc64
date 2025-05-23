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
#include "PowerPort.h"

namespace vc64 {

void
PowerPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

i64
PowerPort::getOption(Opt option) const
{
    switch (option) {

        case Opt::POWER_GRID:  return (i64)config.powerGrid;

        default:
            fatalError;
    }
}

void
PowerPort::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::POWER_GRID:

            if (!PowerGridEnum::isValid(value)) {
                throw Error(Fault::OPT_INV_ARG, PowerGridEnum::keyList());
            }
            return;

        default:
            throw Error(Fault::OPT_UNSUPPORTED);
    }
}

void
PowerPort::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::POWER_GRID:

            config.powerGrid = (PowerGrid)value;
            return;

        default:
            fatalError;
    }
}

}
