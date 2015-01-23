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

Joystick::Joystick(C64 *c64) {

    name = "JOYSTICK";
    debug(2, "    Creating joystick at address %p...\n", this);
    this->c64 = c64;
}

Joystick::~Joystick()
{
}

void
Joystick::reset()
{
    debug(2, "  Resetting Joystick...\n");
    
    _buttonPressed = false;
    _axisX = JOYSTICK_AXIS_NONE;
    _axisY = JOYSTICK_AXIS_NONE;
}

uint32_t
Joystick::stateSize()
{
    return 3;
}

void
Joystick::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    _buttonPressed = (uint8_t)read8(buffer);
    _axisX = (JoystickAxisState)read8(buffer);
    _axisY = (JoystickAxisState)read8(buffer);

    debug(2, "  Joystick state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
Joystick::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

    write8(buffer,(uint8_t)_buttonPressed);
    write8(buffer,(uint8_t)_axisX);
    write8(buffer,(uint8_t)_axisY);

    debug(2, "  Joystick state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
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
