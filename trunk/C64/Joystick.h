/*
 * Authors: Benjamin Klein
 *          Dirk Hoffmann
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

enum JoystickAxisState
{
	JOYSTICK_AXIS_NONE = 0,
	
	JOYSTICK_AXIS_X_LEFT = -1,
	JOYSTICK_AXIS_X_RIGHT = 1,
	JOYSTICK_AXIS_X_NONE = 0,
	
	JOYSTICK_AXIS_Y_UP = -1,
	JOYSTICK_AXIS_Y_DOWN = 1,
	JOYSTICK_AXIS_Y_NONE = 0
};

class Joystick : public VirtualComponent {

private:
    
    bool _buttonPressed;
    JoystickAxisState _axisX;
    JoystickAxisState _axisY;
    
public:

    //! Constructor
    Joystick(C64 *c64);
    
    //! Destructor
    ~Joystick();
    
    //! Reset
    void reset();
    
    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
    //! Save state
    void saveToBuffer(uint8_t **buffer);

    bool GetButtonPressed();
	JoystickAxisState GetAxisX();
	JoystickAxisState GetAxisY();
		
	void SetButtonPressed(bool pressed);
	void SetAxisX(JoystickAxisState state);
	void SetAxisY(JoystickAxisState state);

};

#endif
