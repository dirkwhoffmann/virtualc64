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

    shiftX = 127;
    shiftY = 127;

    latchedX = 0;
    latchedY = 0;
    controlPort = 0xFF;
}

uint8_t
Mouse1350::readControlPort()
{
    return controlPort & (leftButton ? 0xEF : 0xFF);
}

void
Mouse1350::execute()
{
    int64_t deltaX = mouseX - latchedX;
    int64_t deltaY = mouseY - latchedY;
    
    debug("dX = %d dY = %d\n", deltaX, deltaY);

    latchedX = mouseX;
    latchedY = mouseY;
    
    controlPort = 0xFF;
    if (deltaY < -5) CLR_BIT(controlPort, 0);
    if (deltaY > 5)  CLR_BIT(controlPort, 1);
    if (deltaX < -5) CLR_BIT(controlPort, 2);
    if (deltaX > 5)  CLR_BIT(controlPort, 3);
}
