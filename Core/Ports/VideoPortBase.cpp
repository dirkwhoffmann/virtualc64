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
#include "VideoPort.h"

namespace vc64 {

void
VideoPort::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

    }
}

i64
VideoPort::getOption(Opt option) const
{
    switch (option) {

        case Opt::VID_WHITE_NOISE:   return config.whiteNoise;

        default:
            fatalError;
    }
}

void
VideoPort::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::VID_WHITE_NOISE:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
VideoPort::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::VID_WHITE_NOISE:

            config.whiteNoise = (bool)value;
            return;

        default:
            fatalError;
    }
}

}
