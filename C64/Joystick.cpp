/*
 * Authors: Benjamin Klein (Original)
 *          Dirk W. Hoffmann (Further development) 
 *
 * This program is free software; you can redistribute it and/or modify
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

Joystick::Joystick() {

    setDescription("Joystick");
    debug(3, "    Creating joystick at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &button,  sizeof(button), 0 },
        { &axisX,   sizeof(axisX),  0 },
        { &axisY,   sizeof(axisY),  0 },
        { NULL,     0,              0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Joystick::~Joystick()
{
}

void
Joystick::reset()
{
    VirtualComponent::reset();

    button = false;
    axisX = JOYSTICK_RELEASED;
    axisY = JOYSTICK_RELEASED;
}

void
Joystick::dumpState()
{
    msg("Joystick port\n");
    msg("-------------\n");
    msg("Button: %s AxisX: %d AxisY: %d\n", button ? "YES" : "NO", axisX, axisY);
}

/*
void
Joystick::setAxis(JoystickDirection dir)
{
    switch (dir) {
        case JOYSTICK_UP:
        case JOYSTICK_DOWN:
            axisY = dir;
            break;
        case JOYSTICK_LEFT:
        case JOYSTICK_RIGHT:
            axisX = dir;
            break;
        case JOYSTICK_RELEASED:
            axisX = axisY = dir;
        default:
            break;
    }
}
*/

void
Joystick::setXAxis(int value)
{
    switch(value) {
        case -1:
            axisX = JOYSTICK_LEFT;
            break;
        case 1:
            axisX = JOYSTICK_RIGHT;
            break;
        default:
            axisX = JOYSTICK_RELEASED;
    }
}

void
Joystick::setYAxis(int value)
{
    switch(value) {
        case -1:
            axisY = JOYSTICK_UP;
            break;
        case 1:
            axisY = JOYSTICK_DOWN;
            break;
        default:
            axisY = JOYSTICK_RELEASED;
    }
}

