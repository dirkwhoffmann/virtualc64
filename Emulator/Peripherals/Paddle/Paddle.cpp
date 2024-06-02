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

Paddle::Paddle(C64& ref, ControlPort& pref) : SubComponent(ref, pref.objid), port(pref)
{
};

u8
Paddle::getControlPort() const
{
    u8 result = 0xFF;

    if (button)      CLR_BIT(result, 4);

    return result;
}

void 
Paddle::setX(double x)
{
    assert(x >= -1.0 && x <= 1.0);
    pos = x;

    port.device = CPDEVICE_PADDLE;
}

void 
Paddle::setX(isize x)
{
    assert(x >= 0 && x <= 255);
    setX(x / 127.5 - 1.0);
}

void 
Paddle::setDx(double dx)
{
    double newPos = pos + dx;
    if (newPos < -1.0) newPos = -1.0;
    if (newPos > 1.0) newPos = 1.0;

    setX(newPos);
}

void 
Paddle::setDx(isize dx)
{
    setDx(double(dx) / 127.5);
}

void
Paddle::trigger(GamePadAction event)
{
    debug(PRT_DEBUG, "Port %ld: %s\n", objid, GamePadActionEnum::key(event));

    switch (event) {

        case PRESS_FIRE:

            button = true;
            break;

        case RELEASE_FIRE:

            button = false;
            break;

        default:
            fatalError;
    }

    port.device = CPDEVICE_PADDLE;
}

void
Paddle::updatePotX()
{

}

void
Paddle::updatePotY()
{

}

void updatePotY();

u8
Paddle::readPotX() const
{
    return u8((pos + 1.0) * 127.5);
}

u8
Paddle::readPotY() const
{
    // return u8((pos + 1.0) * 127.5);
    return 0;
}

}
