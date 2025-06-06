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
#include "IOUtils.h"

namespace vc64 {

void
RS232::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("txdShr") << hex(txdShr) << std::endl;
        os << tab("rxdCnt") << dec(rxdCnt) << std::endl;
        os << tab("txdCnt") << dec(txdCnt) << std::endl;
        os << tab("Pending input") << input << std::endl;
    }
}

i64
RS232::getOption(Opt option) const
{
    switch (option) {

        case Opt::RS232_DEVICE:  return (i64)config.device;
        case Opt::RS232_BAUD:    return (i64)config.baud;

        default:
            fatalError;
    }
}

void
RS232::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::RS232_DEVICE:
            if (!CommunicationDeviceEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, CommunicationDeviceEnum::keyList());
            }
            return;

        case Opt::RS232_BAUD:
            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
RS232::setOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::RS232_DEVICE:

            config.device = CommunicationDevice(value);
            return;

        case Opt::RS232_BAUD:

            config.baud = isize(value);
            return;

        default:
            fatalError;
    }
}
}
