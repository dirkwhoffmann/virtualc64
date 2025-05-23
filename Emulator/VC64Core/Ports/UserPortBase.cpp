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

UserPort::UserPort(C64 &ref) : SubComponent(ref)
{
    subComponents = std::vector<CoreComponent *> {

        &rs232
    };
}

void
UserPort::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

    }
}

i64
UserPort::getOption(Opt option) const
{
    switch (option) {

        case Opt::USR_DEVICE:    return (i64)config.device;

        default:
            fatalError;
    }
}

void
UserPort::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::USR_DEVICE:

            if (!UserPortDeviceEnum::isValid(value)) {
                throw Error(Fault::OPT_INV_ARG, UserPortDeviceEnum::keyList());
            }
            return;

        default:
            throw Error(Fault::OPT_UNSUPPORTED);
    }
}

void
UserPort::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::USR_DEVICE:

            config.device = (UserPortDevice)value;
            return;

        default:
            fatalError;
    }
}

}
