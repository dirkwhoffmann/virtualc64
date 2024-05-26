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
#include "CIA.h"

namespace vc64 {

i64
CIA::getOption(Option option) const
{
    switch (option) {

        case OPT_CIA_REVISION:      return config.revision;
        case OPT_CIA_TIMER_B_BUG:   return config.timerBBug;

        default:
            fatalError;
    }
}

void
CIA::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_CIA_REVISION:

            if (!CIARevisionEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, CIARevisionEnum::keyList());
            }
            return;

        case OPT_CIA_TIMER_B_BUG:

            return;

        default:
            throw VC64Error(ERROR_OPT_UNSUPPORTED);
    }
}

void
CIA::setOption(Option opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case OPT_CIA_REVISION:

            config.revision = (CIARevision)value;
            return;

        case OPT_CIA_TIMER_B_BUG:

            config.timerBBug = value;
            return;

        default:
            fatalError;
    }
}

}
