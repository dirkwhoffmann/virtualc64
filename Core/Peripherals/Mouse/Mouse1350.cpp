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
#include "Mouse1350.h"
#include "C64.h"

#include <cmath>

namespace vc64 {

void
Mouse1350::_didReset(bool hard)
{
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    controlPort = 0xFF;
    
    for (isize i = 0; i < 3; i++)
        latchedX[i] = latchedY[i] = 0;
}

u8
Mouse1350::readPotX() const
{
    return rightButton ? 0x00 : 0xFF;
}

u8
Mouse1350::readPotY() const
{
    return 0xFF;
}

u8
Mouse1350::readControlPort() const
{
    return controlPort & (leftButton ? 0xEF : 0xFF);
}

void
Mouse1350::execute(i64 targetX, i64 targetY)
{
    mouseX = targetX / dividerX;
    mouseY = targetY / dividerY;

    // debug(PRT_DEBUG, "targetX = %lld targetY = %lld\n", targetX, targetY);
    
    controlPort = 0xFF;
    
    double deltaX = double(mouseX - latchedX[0]);
    double deltaY = double(latchedY[0] - mouseY);
    double absDeltaX = abs(deltaX);
    double absDeltaY = abs(deltaY);
    double max = (absDeltaX > absDeltaY) ? absDeltaX : absDeltaY;
    
    if (max > 0) {
        deltaX /= max;
        deltaY /= max;
        if (deltaY < -0.5) { CLR_BIT(controlPort, 0); } // UP
        if (deltaY > 0.5)  { CLR_BIT(controlPort, 1); } // DOWN
        if (deltaX < -0.5) { CLR_BIT(controlPort, 2); } // LEFT
        if (deltaX > 0.5)  { CLR_BIT(controlPort, 3); } // RIGHT
    }
    
    // Update latch pipeline
    for (isize i = 0; i < 2; i++) {
        latchedX[i] = latchedX[i+1];
        latchedY[i] = latchedY[i+1];
    }
    latchedX[2] = mouseX;
    latchedY[2] = mouseY;
}

}
