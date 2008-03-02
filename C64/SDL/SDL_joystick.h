/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
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

/* Include file for SDL joystick event handling */

#ifndef _SDL_joystick_h
#define _SDL_joystick_h

#include "SDL_sysjoystick.h"

#include <stdlib.h>
#include <strings.h>

#ifdef __cplusplus
#define C_LINKAGE	"C"
#else
#define C_LINKAGE
#endif /* __cplusplus */

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* In order to use these functions, SDL_Init() must have been called
   with the SDL_INIT_JOYSTICK flag.  This causes SDL to scan the system
   for joysticks, and load appropriate drivers.
*/

//! Initialize the SDL joystick functions
int SDL_JoystickInit(void);

//! Qiut the SDL joystick functions.
/*! This means to close all devices and after that close the SDL ifself. */
void SDL_JoystickQuit(void);


//! Returns the number of joysticks attached to the system
int SDL_NumJoysticks(void);

//! Get the implementation dependent name of a joystick.
/*! This can be called before any joysticks are opened.
	\param device_index The number of the device
    \return The name of the device, if no name can be found, this function returns NULL.
*/
const char *SDL_JoystickName(int device_index);

//! Open a joystick for use.
/*! The index passed as an argument refers to the N'th joystick on the system.  
	This index is the value which will identify this joystick in future joystick events.
	\param device_index The number of the device
	\returns A joystick identifier, or NULL if an error occurred.
*/
SDL_Joystick *SDL_JoystickOpen(int device_index);

//! Check if the specific joystick is still opened.
/*! \return 1 if the joystick has been opened, or 0 if it has not.
*/
int SDL_JoystickOpened(int device_index);

//! Get the device index of an opened joystick.
/*! \param joystick A pointer to the joystick device for which the index should be returned.
	\return The device index if the device is a valid joystick, or -1 if the device index is invalid.
*/
int SDL_JoystickIndex(SDL_Joystick *joystick);

//! Get the number of general axis controls on a joystick.
/*! \param joystick A pointer to the joystick device.
	\return The number of axis the joystick has, or -1 if the device is invalid.
*/
int SDL_JoystickNumAxes(SDL_Joystick *joystick);

//! Get the number of trackballs on a joystick.
/*! Joystick trackballs have only relative motion events associated	with them and their 
	state cannot be polled.
	\param joystick A pointer to the joystick device.
	\return The number of balls the joystick has, or -1 if the device is invalid.
*/
int SDL_JoystickNumBalls(SDL_Joystick *joystick);

//! Get the number of POV hats on a joystick.
/*! \param joystick A pointer to the joystick device.
	\return The number of hats the joystick has, or -1 if the device is invalid.
*/
int SDL_JoystickNumHats(SDL_Joystick *joystick);

//! Get the number of buttons on a joystick.
/*! \param joystick A pointer to the joystick device.
	\return The number of buttons the joystick has, or -1 if the device is invalid.
*/
int SDL_JoystickNumButtons(SDL_Joystick *joystick);

//! Update the current state of the open joysticks.
/*! This is called automatically by the event loop if any joystick events are enabled. */
void SDL_JoystickUpdate(void);

//! Get the current state of an axis control on a joystick
/*! The state is a value ranging from -32768 to 32767. The axis indices start at index 0.
	\param joystick A pointer to the joystick device.
	\param axis The axis to get information about. Axis means left/right, up/down...
	\return The state of the axis, or 0 if the device is invalid.
*/
int SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis);

/*
 * Get the current state of a POV hat on a joystick
 * The return value is one of the following positions:
 */
#define SDL_HAT_CENTERED	0x00
#define SDL_HAT_UP			0x01
#define SDL_HAT_RIGHT		0x02
#define SDL_HAT_DOWN		0x04
#define SDL_HAT_LEFT		0x08
#define SDL_HAT_RIGHTUP		(SDL_HAT_RIGHT|SDL_HAT_UP)
#define SDL_HAT_RIGHTDOWN	(SDL_HAT_RIGHT|SDL_HAT_DOWN)
#define SDL_HAT_LEFTUP		(SDL_HAT_LEFT|SDL_HAT_UP)
#define SDL_HAT_LEFTDOWN	(SDL_HAT_LEFT|SDL_HAT_DOWN)

//! The hat indices start at index 0.
/*! \param joystick A pointer to the joystick device.
	\param hat The hat to get information about.
	\return The state of the hat, or 0 if the device is invalid.
*/
unsigned short int SDL_JoystickGetHat(SDL_Joystick *joystick, int hat);

//! Get the ball axis change since the last poll.
/*! The ball indices start at index 0.
	\param joystick A pointer to the joystick device.
	\param ball The ball to get information about.
	\param dx A pointer where the x-direction should be saved.
	\param dy A pointer where the y-direction should be saved.
	\return 0, or -1 if you passed it invalid parameters.
	*/
int SDL_JoystickGetBall(SDL_Joystick *joystick, int ball, int *dx, int *dy);

//! Get the current state of a button on a joystick.
/*! The button indices start at index 0.
	\param joystick A pointer to the joystick device.
	\param button The button to get information about.
	\return State of the button, or 0 if the device is invalid
*/
unsigned short int SDL_JoystickGetButton(SDL_Joystick *joystick, int button);

//! Close a joystick previously opened with SDL_JoystickOpen().
/*! \param joystick A pointer to the joystick device.
*/
void SDL_JoystickClose(SDL_Joystick *joystick);

//! Write debugging message
/*! Syntax is similar to the printf function. The function only takes effect in debug mode.  */
void SDL_SetError(char *fmt, ...);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* _SDL_joystick_h */
