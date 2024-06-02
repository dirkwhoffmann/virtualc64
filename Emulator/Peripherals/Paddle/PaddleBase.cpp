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
#include "Paddle.h"
#include "Emulator.h"
// #include "IOUtils.h"

namespace vc64 {

void
Paddle::_reset(bool hard)
{
    leftButton = false;
    rightButton = false;
    pos = 0;
}


void
Paddle::operator << (SerResetter &worker)
{
    serialize(worker);
}

void
Paddle::operator << (SerReader &worker)
{
    serialize(worker);
}

void
Paddle::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Paddle nr") << dec(objid) << std::endl;
        os << tab("Left button") << bol(leftButton) << std::endl;
        os << tab("Right button") << bol(rightButton) << std::endl;
        os << tab("Pos") << flt(pos) << std::endl;
    }
}

void
Paddle::cacheInfo(PaddleInfo &result) const
{
    {   SYNCHRONIZED

    }
}

i64
Paddle::getOption(Option option) const
{
    fatalError;
}

void
Paddle::checkOption(Option opt, i64 value)
{
    throw VC64Error(ERROR_OPT_UNSUPPORTED);
}

void
Paddle::setOption(Option opt, i64 value)
{
    fatalError;
}

}
