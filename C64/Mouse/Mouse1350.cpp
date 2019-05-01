/*!
 * @file        Mouse1350.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"
#include "Mouse1350.h"

Mouse1350::Mouse1350() {
    
    setDescription("Mouse1350");
    debug(3, "    Creating Mouse1350 at address %p...\n", this);
}

Mouse1350::~Mouse1350()
{
}

void
Mouse1350::reset()
{
    VirtualComponent::reset();
    
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    controlPort = 0xFF;
    
    for (unsigned i = 0; i < 3; i++)
        latchedX[i] = latchedY[i] = 0;
}

uint8_t
Mouse1350::readPotX()
{
    return rightButton ? 0x00 : 0xFF;
}

uint8_t
Mouse1350::readPotY()
{
    return 0xFF;
}

uint8_t
Mouse1350::readControlPort()
{
    return controlPort & (leftButton ? 0xEF : 0xFF);
}

void
Mouse1350::execute(int64_t targetX, int64_t targetY)
{
    mouseX = targetX / dividerX;
    mouseY = targetY / dividerY;
 
    debug("targetX = %d targetY = %d\n", targetX, targetY);
    
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

