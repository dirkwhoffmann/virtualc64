/*!
 * @file        Mouse1351.h
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
#include "Mouse1351.h"

Mouse1351::Mouse1351() {
    
    setDescription("Mouse1351");
    debug(3, "    Creating Mouse1351 at address %p...\n", this);
}

Mouse1351::~Mouse1351()
{
}

void
Mouse1351::reset()
{
    VirtualComponent::reset();
    
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    targetX = 0;
    targetY = 0;
}

/*
void
Mouse1351::setXY(int64_t x, int64_t y)
{
    targetX = x / dividerX;
    targetY = y / dividerY;
}
*/

uint8_t
Mouse1351::readControlPort()
{
    uint8_t result = 0xFF;
    
    if (leftButton) CLR_BIT(result, 4);
    if (rightButton) CLR_BIT(result, 0);

    return result;
}

void
Mouse1351::execute(int64_t targetX, int64_t targetY)
{
    targetX /= dividerX;
    targetY /= dividerY;
    
    // Jump directly to target coordinates if they are more than 8 shifts away.
    if (abs(targetX - mouseX) / 8 > shiftX) mouseX = targetX;
    if (abs(targetY - mouseY) / 8 > shiftY) mouseY = targetY;
    
    // Move mouse coordinates towards target coordinates
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, shiftX);
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, shiftY);
}
