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
#include "TOD.h"

namespace vc64 {

void
TOD::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("Time of Day");
        os << hex(tod.hour)   << ":" << hex(tod.min)     << ":";
        os << hex(tod.sec)    << ":" << hex(tod.tenth)   << std::endl;

        os << tab("Alarm");
        os << hex(alarm.hour) << ":" << hex(alarm.min)   << ":";
        os << hex(alarm.sec)  << ":" << hex(alarm.tenth) << std::endl;

        os << tab("Latch");
        os << hex(latch.hour) << ":" << hex(latch.min)   << ":";
        os << hex(latch.sec)  << ":" << hex(latch.tenth) << std::endl;

        os << tab("Frozen");
        os << bol(frozen) << std::endl;
        os << tab("Stopped");
        os << bol(stopped) << std::endl;
    }
}

void
TOD::cacheInfo(TODInfo &info) const
{
    {   SYNCHRONIZED

        info.time = tod;
        info.latch = latch;
        info.alarm = alarm;
    }
}

}
