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
    shiftX = INT_MAX;
    shiftY = INT_MAX;
    dividerX = 1;
    dividerY = 1;
}

void
Mouse::setXY(int64_t x, int64_t y)
{
    targetX = x / dividerX;
    targetY = y / dividerY;
    
    // Sync mouse coords with target coords if more than 8 shifts would
    // be needed to reach target coords
    if (abs(targetX - mouseX) / 8 > shiftX) {
        // printf("Resetting x\n");
        mouseX = targetX;
    }
    if (abs(targetY - mouseY) / 8 > shiftY) {
        // printf("Resetting y\n");
        mouseY = targetY;
    }
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

