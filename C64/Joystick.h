/*
 * Originally written by Benjamin Klein
 * Rewritten by Dirk W. Hoffmann
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

#ifndef JOYSTICK_H
#define JOYSTICK_H

enum JoystickDirection
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_FIRE,
    
    JOYSTICK_RELEASED
};

class Joystick : public VirtualComponent {

private:
    
    bool _buttonPressed;
    JoystickDirection _axisX;
    JoystickDirection _axisY;
    
public:

    //! Constructor
    Joystick();
    
    //! Destructor
    ~Joystick();
    
    //! Reset
    void reset();

    //! Dump internal state to console
    void dumpState();

    inline bool getButtonPressed() { return _buttonPressed; }
    inline JoystickDirection getAxisX() { return _axisX; }
    inline JoystickDirection getAxisY() { return _axisY; }
		
    inline void setButtonPressed(bool pressed) { _buttonPressed = pressed; }
    inline void setAxisX(JoystickDirection state) { _axisX = state; }
    inline void setAxisY(JoystickDirection state) { _axisY = state; }

};

#endif
