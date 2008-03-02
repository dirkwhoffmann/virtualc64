/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is SDL_free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#ifndef SDL_SYSJOYSTICK_H
#define SDL_SYSJOYSTICK_H

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


//! The joystick structure used to identify an SDL joystick.
struct _SDL_Joystick {
	unsigned short int index;		/*! Device index */
	const char *name;				/*! Joystick name - system dependent */

	int naxes;						/*! Number of axis controls on the joystick */
	int *axes;						/*! Current axis states */

	int nhats;						/*! Number of hats on the joystick */
	unsigned short int *hats;		/*! Current hat states */
	
	int nballs;						/*! Number of trackballs on the joystick */
	struct balldelta {
		int dx;
		int dy;
	} *balls;						/*! Current ball motion deltas */
	
	int nbuttons;					/*! Number of buttons on the joystick */
	unsigned short int *buttons;	/*! Current button states */
	
	struct joystick_hwdata *hwdata;	/*! Driver dependent information */

	int ref_count;					/*! Reference count for multiple opens */
};

typedef struct _SDL_Joystick SDL_Joystick;

//! Function to scan the system for joysticks.
/*! Joystick 0 should be the system default joystick.
	\return The number of available joysticks, or -1 on an unrecoverable fatal error.
*/
int SDL_SYS_JoystickInit(void);

//! Function to get the device-dependent name of a joystick.
/*! \param index The device number of the joystick.
	\return The name of the joystick.
*/
const char *SDL_SYS_JoystickName(int index);

//! Function to open a joystick for use.
/*! The joystick to open is specified by the index field of the joystick. This should fill the 
	nbuttons and naxes fields of the joystick structure.
   \param joystick A pointer to the joystick device.
   \return It returns 0, or -1 if there is an error.
*/
int SDL_SYS_JoystickOpen(SDL_Joystick *joystick);

//! Function to update the state of a joystick - called as a device poll.
/*! This function shouldn't update the joystick structure directly, but instead should 
	call SDL_PrivateJoystick*() to deliver events and update joystick device state.
	\param joystick A pointer to the joystick device
*/
void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick);

//! Function to close a joystick after use 
/*! \param joystick A pointer to the joystick device.
*/
void SDL_SYS_JoystickClose(SDL_Joystick *joystick);

//! Function to perform any system-specific joystick related cleanup.
void SDL_SYS_JoystickQuit(void);



//! Internal event queueing function to update the joystick axis
int SDL_PrivateJoystickAxis(SDL_Joystick *joystick, unsigned short int axis, int value);

//! Internal event queueing function to update the joystick ball
int SDL_PrivateJoystickBall(SDL_Joystick *joystick, unsigned short int ball, int xrel, int yrel);

//! Internal event queueing function to update the joystick hat
int SDL_PrivateJoystickHat(SDL_Joystick *joystick, unsigned short int hat, unsigned short int value);

//! Internal event queueing function to update the joystick button
int SDL_PrivateJoystickButton(SDL_Joystick *joystick, unsigned short int button, unsigned short int state);


extern SDL_Joystick **SDL_joysticks;
extern unsigned short int SDL_numjoysticks;

extern void SDL_SetError(char *fmt, ...);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif


#endif