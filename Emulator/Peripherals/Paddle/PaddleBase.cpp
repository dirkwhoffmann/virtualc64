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

namespace vc64 {

void
Paddle::_didReset(bool hard)
{
    button[0] = false;
    button[1] = false;
    pos[0] = 0.0;
    pos[1] = 0.0;
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
        os << tab("Button 0") << bol(button[0]) << std::endl;
        os << tab("Button 1") << bol(button[1]) << std::endl;
        os << tab("Position 0") << flt(pos[0]) << std::endl;
        os << tab("Position 1") << flt(pos[1]) << std::endl;
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
    switch (option) {

        case OPT_PADDLE_ORIENTATION:    return config.orientation;

        default:
            fatalError;
    }
}

void
Paddle::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_PADDLE_ORIENTATION:

            if (!PaddleOrientationEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, PaddleOrientationEnum::keyList());
            }
            return;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED);
    }
}

void
Paddle::setOption(Option opt, i64 value)
{
    switch (opt) {
            
        case OPT_PADDLE_ORIENTATION:
            
            config.orientation = PaddleOrientation(value);
            return;
            
        default:
            fatalError;
    }
}

}
