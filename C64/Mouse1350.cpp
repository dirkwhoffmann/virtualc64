/*!
 * @header      Mouse1350.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "C64.h"

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
    Mouse::reset();
    
    shiftX = INT_MAX;
    shiftY = INT_MAX;
    dividerX = 64;
    dividerY = 64;
    controlPort = 0xFF;
    
    for (unsigned i = 0; i < 3; i++)
        latchedX[i] = latchedY[i] = 0;
}

uint8_t
Mouse1350::readControlPort()
{
    return controlPort & (leftButton ? 0xEF : 0xFF);
}

void
Mouse1350::execute()
{
    Mouse::execute();
    
    controlPort = 0xFF;
    
    double deltaX = (mouseX - latchedX[0]);
    double deltaY = (latchedY[0] - mouseY);
    double absDeltaX = abs(deltaX);
    double absDeltaY = abs(deltaY);
    double max = (absDeltaX > absDeltaY) ? absDeltaX : absDeltaY;
    
    if (max) {
        
        deltaX /= max;
        deltaY /= max;
        
        debug("dX = %f dY = %f\n", deltaX, deltaY);
        
        if (deltaY < -0.5) { CLR_BIT(controlPort, 0); msg("UP "); }
        if (deltaY > 0.5)  { CLR_BIT(controlPort, 1); msg("DOWN "); }
        if (deltaX < -0.5) { CLR_BIT(controlPort, 2); msg("LEFT "); }
        if (deltaX > 0.5)  { CLR_BIT(controlPort, 3); msg("RIGHT "); }
        msg("\n");
    }
    
    // Update latch pipeline
    for (unsigned i = 0; i < 2; i++) {
        latchedX[i] = latchedX[i+1];
        latchedY[i] = latchedY[i+1];
    }
    latchedX[2] = mouseX;
    latchedY[2] = mouseY;
}

