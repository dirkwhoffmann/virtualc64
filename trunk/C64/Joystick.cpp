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
          
#include "C64.h"

int Joystick::joystickOpenCount = 0;
bool Joystick::initialized = false;


Joystick::Joystick( int devNo ) {
	
	// debug("Creating virtual joystick at address %p...\n", this);

	// initialize the SDL if no other joytick has made this
	if( initialized == false ) {
		joystickOpenCount = 0;
		// initialize the SDL part
		SDL_JoystickInit();
		initialized = true;
	}
	
	if( devNo < SDL_NumJoysticks() ) {
		joy = SDL_JoystickOpen( devNo );
		
		if(joy) {
			joystickNo = devNo;
			joystickOpenCount++;

			printf("Opened Joystick %d\n", devNo);
			printf("Joystick %d data:\n", devNo);
			printf("\tName: %s\n", SDL_JoystickName(devNo));
			printf("\tNumber of Axes: %d\n", SDL_JoystickNumAxes(joy));
			printf("\tNumber of Buttons: %d\n", SDL_JoystickNumButtons(joy));
			printf("\tNumber of Balls: %d\n", SDL_JoystickNumBalls(joy));
		} else {
			printf("Couldn't open joystick %d\n", devNo );  
			joy = NULL;
			throw("Couldn't open joystick %d\n", devNo );
		}

	} else {
		printf( "Joystick number to big!!!\n" );
		joy = NULL;
		throw("Joystick number to big!\n");
	}
}


Joystick::~Joystick() {
	SDL_JoystickClose( joy );
	joystickOpenCount--;
	
	if( joystickOpenCount == 0 ) {
		SDL_JoystickQuit();
		initialized = false;
		printf("close joy.coo\n");
	}
}


JoystickState Joystick::getAxisUpDownState() {
	SDL_JoystickUpdate();
	int state = SDL_JoystickGetAxis( joy, AXIS_UPDOWN );
	
	if( state == 1 )
		return JOY_UP;
	else if( state == -1 )
		return JOY_DOWN;
	else
		return JOY_UNTOUCHED;
}


JoystickState Joystick::getAxisLeftRightState() {
	SDL_JoystickUpdate();
	int state = SDL_JoystickGetAxis( joy, AXIS_LEFTRIGHT );
	
	if( state == 1 )
		return JOY_LEFT;
	else if( state == -1 )
		return JOY_RIGHT;
	else
		return JOY_UNTOUCHED;
}


JoystickState Joystick::getButtonState() {
	SDL_JoystickUpdate();
	int buttonState = 0;
	int numButtons = SDL_JoystickNumButtons( joy );

	// poll all buttons, handle them all as if they were one button only. So if one of 
	// them is pressed set it to 1;
	for( int i=0; i < numButtons; i++ )
		buttonState |= SDL_JoystickGetButton( joy, i );
	
	if( buttonState == 1 )
			return JOY_BUTTON;
	else
		return JOY_UNTOUCHED;
}

