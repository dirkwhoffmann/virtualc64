/*
 * This file is part of Joystick.
 *
 *   Joystick is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Joystick is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Joystick; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @autor Andreas Fertig
 * @date 04.05.07
*/


               
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "SDL_joystick.h"

//! JoystickState
/*! Datatype for the axis and button state of a joystick */
typedef enum JoystickState {	
	JOY_UNTOUCHED	= 0,
	JOY_UP			= 1,
	JOY_DOWN		= 2,
	JOY_LEFT		= 4,
	JOY_RIGHT		= 8,
	JOY_BUTTON		= 16,
} JoystickState;


//! Axis
/*! The axis direction of a joystick */
enum Axis {
	AXIS_LEFTRIGHT,
	AXIS_UPDOWN
};

//! Joystick class to handle controlling with a joystick.
//class Joystick : public VirtualComponent {
class Joystick {

public:
	//! constructor
	/*! \param devNo	The device number to open, start with 0, max the max number of joysticks in the system - 1 */
	Joystick( int devNo );
	
	//! destructor
	/*! Close the joystick and when it is the last one opened then also the SDL itself. */
	~Joystick();

	//! Get the state of the axis up/down.
	/*! \return JOY_UP if the axis is pressed in the up direction, JOY_DOWN if the stick is pressed in the down 
		direction or JOY_UNTOUCHED if the state is center */
	JoystickState getAxisUpDownState();

	//! Get the state of the axis left/right.
	/*! \return JOY_LEFT if the axis is pressed in the left direction, JOY_RIGHT if the stick is pressed in 
		the right direction or JOY_UNTOUCHED if the state is center	*/
	JoystickState getAxisLeftRightState();

	//! Handle button press and release events. 
	/*! The traditional C64 joystick has had only one button, so we handle all buttons as one. That means if one of 
		them is pressed the return value is true and the value is only then false when no button is pressed.
		\return true if one button is pressed, false if no button is pressed */
	JoystickState getButtonState();
	
private:

	//! Pointer to the joystick device
	SDL_Joystick *joy;
	//! The internal joystick-number which this class handles
	int joystickNo;
	
	//! static variable initialization
	/*! Each variable occures only one time in the whole programm!
		So we can check if the SDL was initialized or not. */
	static int joystickOpenCount;
	static bool initialized;
};



#endif
