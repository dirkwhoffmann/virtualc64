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

    name = "Joystick";
    debug(2, "    Creating joystick at address %p...\n", this);
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        { &_buttonPressed,  sizeof(_buttonPressed), CLEAR_ON_RESET },
        { &_axisX,          sizeof(_axisX),         CLEAR_ON_RESET },
        { &_axisY,          sizeof(_axisY),         CLEAR_ON_RESET },
        { NULL,             0,                      0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

Joystick::~Joystick()
{
}

void
Joystick::reset()
{
    VirtualComponent::reset();

    _buttonPressed = false;
    _axisX = JOYSTICK_AXIS_NONE;
    _axisY = JOYSTICK_AXIS_NONE;
}

void
Joystick::dumpState()
{
    msg("Joystick port\n");
    msg("-------------\n");
    msg("Button: %s AxisX: %d AxisY: %d\n", _buttonPressed ? "YES" : "NO", _axisX, _axisY);
}

bool Joystick::GetButtonPressed()
{
    return _buttonPressed;
}

JoystickAxisState Joystick::GetAxisX()
{
    return _axisX;
}

JoystickAxisState Joystick::GetAxisY()
{
    return _axisY;
}

void Joystick::SetButtonPressed(bool pressed)
{
    // fprintf(stderr,"%p %s", this, __PRETTY_FUNCTION__);
    _buttonPressed = pressed;
}

void Joystick::SetAxisX(JoystickAxisState state)
{
    // fprintf(stderr,"%p %s", this, __PRETTY_FUNCTION__);
    _axisX = state;
}

void Joystick::SetAxisY(JoystickAxisState state)
{
    // fprintf(stderr,"%p %s", this, __PRETTY_FUNCTION__);
    _axisY = state;
}
