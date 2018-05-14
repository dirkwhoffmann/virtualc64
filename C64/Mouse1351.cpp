/*!
 * @header      Mouse1351.cpp
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

Mouse1351::Mouse1351() {
    
    setDescription("Mouse1351");
    debug(3, "    Creating Mouse1351 at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        { &mouseX,          sizeof(mouseX),         CLEAR_ON_RESET },
        { &targetX,         sizeof(targetX),        CLEAR_ON_RESET },
        { &mouseY,          sizeof(mouseY),         CLEAR_ON_RESET },
        { &targetY,         sizeof(targetY),        CLEAR_ON_RESET },
        { &controlPort,     sizeof(controlPort),    CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Mouse1351::~Mouse1351()
{
}

void
Mouse1351::reset()
{
    VirtualComponent::reset();
    controlPort = 0xFF;
}

void
Mouse1351::setXY(int64_t x, int64_t y)
{
    targetX = x;
    targetY = y;
    
    if (abs(targetX - mouseX) > 255) mouseX = targetX;
    if (abs(targetY - mouseY) > 255) mouseY = targetY;
}

void
Mouse1351::setLeftButton(bool pressed)
{
    if (pressed) {
        CLR_BIT(controlPort, 4);
    } else {
        SET_BIT(controlPort, 4);
    }
}

void
Mouse1351::setRightButton(bool pressed)
{
    if (pressed) {
        SET_BIT(controlPort, 0);
    } else {
        CLR_BIT(controlPort, 0);
    }
}

void
Mouse1351::execute()
{
    if (mouseX == targetX && mouseY == targetY)
        return;
    
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, 31);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, 31);
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, 31);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, 31);
}
