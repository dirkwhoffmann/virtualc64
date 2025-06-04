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
#include "Joystick.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

void
Joystick::operator << (SerResetter &worker)
{
    serialize(worker);

    // Discard any active joystick movements
    button = false;
    axisX = 0;
    axisY = 0;
}

void
Joystick::operator << (SerReader &worker)
{
    serialize(worker);

    // Discard any active joystick movements
    button = false;
    axisX = 0;
    axisY = 0;
}

void
Joystick::_dump(Category category, std::ostream &os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }

    if (category == Category::State) {

        os << tab("Joystick nr") << dec(objid) << std::endl;
        os << tab("Button") << bol(button) << std::endl;
        os << tab("X axis") << dec(axisX) << std::endl;
        os << tab("Y axis") << dec(axisY) << std::endl;
    }
}

void
Joystick::cacheInfo(JoystickInfo &result) const
{
    {   SYNCHRONIZED

        result.button = button;
        result.axisX = axisX;
        result.axisY = axisY;
    }
}

i64
Joystick::getOption(Opt option) const
{
    switch (option) {

        case Opt::AUTOFIRE:          return (i64)config.autofire;
        case Opt::AUTOFIRE_BURSTS:   return (i64)config.autofireBursts;
        case Opt::AUTOFIRE_BULLETS:  return (i64)config.autofireBullets;
        case Opt::AUTOFIRE_DELAY:    return (i64)config.autofireDelay;

        default:
            fatalError;
    }
}

void
Joystick::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::AUTOFIRE:
        case Opt::AUTOFIRE_BURSTS:
        case Opt::AUTOFIRE_BULLETS:
        case Opt::AUTOFIRE_DELAY:

            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
Joystick::setOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::AUTOFIRE:

            config.autofire = bool(value);
            return;

        case Opt::AUTOFIRE_BURSTS:

            config.autofireBursts = bool(value);
            if (isAutofiring()) reload();
            return;

        case Opt::AUTOFIRE_BULLETS:

            config.autofireBullets = isize(value);
            if (isAutofiring()) reload();
            return;

        case Opt::AUTOFIRE_DELAY:

            config.autofireDelay = isize(value);
            return;

        default:
            fatalError;
    }
}

}
