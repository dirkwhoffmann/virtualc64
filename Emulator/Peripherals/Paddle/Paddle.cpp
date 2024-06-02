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

void 
Paddle::setButton(isize nr, bool value)
{
    assert(nr == 0 || nr == 1);

    button[nr] = value;
}

void 
Paddle::setPosXY(isize nr, double x, double y)
{
    assert(nr == 0 || nr == 1);
    assert(x >= -1.0 && x <= 1.0);
    assert(y >= -1.0 && y <= 1.0);

    switch (config.orientation) {

        case PADDLE_HORIZONTAL:         pos[nr] = x; break;
        case PADDLE_HORIZONTAL_FLIPPED: pos[nr] = x; break;
        case PADDLE_VERTICAL:           pos[nr] = y; break;
        case PADDLE_VERTICAL_FLIPPED:   pos[nr] = y; break;

        default:
            fatalError;
    }
}

void 
Paddle::setPosDxDy(isize nr, double x, double y)
{
    assert(nr == 0 || nr == 1);

    switch (config.orientation) {

        case PADDLE_HORIZONTAL:         pos[nr] -= x; break;
        case PADDLE_HORIZONTAL_FLIPPED: pos[nr] += x; break;
        case PADDLE_VERTICAL:           pos[nr] += y; break;
        case PADDLE_VERTICAL_FLIPPED:   pos[nr] -= y; break;

        default:
            fatalError;
    }

    if (pos[nr] < -1.0) pos[nr] = -1.0;
    if (pos[nr] > 1.0) pos[nr] = 1.0;
}

u8
Paddle::readControlPort() const
{
    u8 result = 0xFF;

    if (button[0]) CLR_BIT(result, 2);
    if (button[1]) CLR_BIT(result, 3);

    return result;
}

u8
Paddle::readPotX() const
{
    return u8((pos[0] + 1.0) * 127.5);
}

u8
Paddle::readPotY() const
{
    return u8((pos[1] + 1.0) * 127.5);
}

}
