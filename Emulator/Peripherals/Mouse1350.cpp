// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "Mouse1350.h"

void
Mouse1350::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    controlPort = 0xFF;
    
    for (unsigned i = 0; i < 3; i++)
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
 
    // debug(PORT_DEBUG, "targetX = %lld targetY = %lld\n", targetX, targetY);
    
    controlPort = 0xFF;
    
    double deltaX = (mouseX - latchedX[0]);
    double deltaY = (latchedY[0] - mouseY);
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
    for (unsigned i = 0; i < 2; i++) {
        latchedX[i] = latchedX[i+1];
        latchedY[i] = latchedY[i+1];
    }
    latchedX[2] = mouseX;
    latchedY[2] = mouseY;
}

