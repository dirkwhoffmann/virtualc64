/*!
 * @header      Mouse.cpp
 * @author      Dirk W. Hoffmann
 * @copyright   2018. All rights reserved.
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

Mouse::Mouse()
{
    // reset();
}

Mouse::~Mouse()
{
}

void Mouse::reset()
{
    leftButton = false;
    rightButton = false;
    mouseX = 0;
    mouseY = 0;
    targetX = 0;
    targetY = 0;
}

void
Mouse::setXY(int64_t x, int64_t y)
{
    targetX = x;
    targetY = y;
    
    if (abs(targetX - mouseX) > 255) mouseX = targetX;
    if (abs(targetY - mouseY) > 255) mouseY = targetY;
}

void
Mouse::execute()
{
    if (mouseX == targetX && mouseY == targetY)
        return;
    
    if (targetX < mouseX) mouseX -= MIN(mouseX - targetX, shiftX);
    else if (targetX > mouseX) mouseX += MIN(targetX - mouseX, shiftX);
    if (targetY < mouseY) mouseY -= MIN(mouseY - targetY, shiftY);
    else if (targetY > mouseY) mouseY += MIN(targetY - mouseY, shiftY);
}

