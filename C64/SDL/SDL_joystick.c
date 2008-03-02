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

//! This is the joystick API for Simple DirectMedia Layer
#include "SDL_joystick.h"

unsigned short int SDL_numjoysticks = 0;
SDL_Joystick **SDL_joysticks = NULL;
static SDL_Joystick *default_joystick = NULL;

int SDL_JoystickInit(void)
{
	int arraylen;
	int status;

	SDL_numjoysticks = 0;
	status = SDL_SYS_JoystickInit();
	if ( status >= 0 ) {
		arraylen = (status+1)*sizeof(*SDL_joysticks);
		SDL_joysticks = (SDL_Joystick **)malloc(arraylen);
		if ( SDL_joysticks == NULL ) {
			SDL_numjoysticks = 0;
		} else {
			memset(SDL_joysticks, 0, arraylen);
			SDL_numjoysticks = status;
		}
		status = 0;
	}
	default_joystick = NULL;
	return(status);
}

int SDL_NumJoysticks(void)
{
	return SDL_numjoysticks;
}

const char *SDL_JoystickName(int device_index)
{
	if ( (device_index < 0) || (device_index >= SDL_numjoysticks) ) {
		SDL_SetError("There are %d joysticks available",
		             SDL_numjoysticks);
		return(NULL);
	}
	return(SDL_SYS_JoystickName(device_index));
}

SDL_Joystick *SDL_JoystickOpen(int device_index)
{
	int i;
	SDL_Joystick *joystick;

	if ( (device_index < 0) || (device_index >= SDL_numjoysticks) ) {
		SDL_SetError("There are %d joysticks available",
		             SDL_numjoysticks);
		return(NULL);
	}

	/* If the joystick is already open, return it */
	for ( i=0; SDL_joysticks[i]; ++i ) {
		if ( device_index == SDL_joysticks[i]->index ) {
			joystick = SDL_joysticks[i];
			++joystick->ref_count;
			return(joystick);
		}
	}

	/* Create and initialize the joystick */
	joystick = (SDL_Joystick *)malloc((sizeof *joystick));
	if ( joystick != NULL ) {
		memset(joystick, 0, (sizeof *joystick));
		joystick->index = device_index;
		if ( SDL_SYS_JoystickOpen(joystick) < 0 ) {
			free(joystick);
			joystick = NULL;
		} else {
			if ( joystick->naxes > 0 ) {
				joystick->axes = (int *)malloc
					(joystick->naxes*sizeof(int));
			}
			if ( joystick->nhats > 0 ) {
				joystick->hats = (unsigned short int *)malloc
					(joystick->nhats*sizeof(unsigned short int));
			}
			if ( joystick->nballs > 0 ) {
				joystick->balls = (struct balldelta *)malloc
				  (joystick->nballs*sizeof(*joystick->balls));
			}
			if ( joystick->nbuttons > 0 ) {
				joystick->buttons = (unsigned short int *)malloc
					(joystick->nbuttons*sizeof(unsigned short int));
			}
			if ( ((joystick->naxes > 0) && !joystick->axes)
			  || ((joystick->nhats > 0) && !joystick->hats)
			  || ((joystick->nballs > 0) && !joystick->balls)
			  || ((joystick->nbuttons > 0) && !joystick->buttons)) {
				// Only a macro which calls SDL_Error -> SDL_SetError to report error messages
				//SDL_OutOfMemory();
				SDL_JoystickClose(joystick);
				joystick = NULL;
			}
			if ( joystick->axes ) {
				memset(joystick->axes, 0,
					joystick->naxes*sizeof(int));
			}
			if ( joystick->hats ) {
				memset(joystick->hats, 0,
					joystick->nhats*sizeof(unsigned short int));
			}
			if ( joystick->balls ) {
				memset(joystick->balls, 0,
				  joystick->nballs*sizeof(*joystick->balls));
			}
			if ( joystick->buttons ) {
				memset(joystick->buttons, 0,
					joystick->nbuttons*sizeof(unsigned short int));
			}
		}
	}
	if ( joystick ) {
		/* Add joystick to list */
		++joystick->ref_count;
		for ( i=0; SDL_joysticks[i]; ++i )
			/* Skip to next joystick */;
		SDL_joysticks[i] = joystick;
	}
	return(joystick);
}

int SDL_JoystickOpened(int device_index)
{
	int i, opened;

	opened = 0;
	for ( i=0; SDL_joysticks[i]; ++i ) {
		if ( SDL_joysticks[i]->index == (unsigned short int)device_index ) {
			opened = 1;
			break;
		}
	}
	return(opened);
}

static int ValidJoystick(SDL_Joystick **joystick)
{
	int valid;

	if ( *joystick == NULL ) {
		*joystick = default_joystick;
	}
	if ( *joystick == NULL ) {
		SDL_SetError("Joystick hasn't been opened yet");
		valid = 0;
	} else {
		valid = 1;
	}
	return valid;
}

int SDL_JoystickIndex(SDL_Joystick *joystick)
{
	if ( ! ValidJoystick(&joystick) ) {
		return(-1);
	}
	return(joystick->index);
}

int SDL_JoystickNumAxes(SDL_Joystick *joystick)
{
	if ( ! ValidJoystick(&joystick) ) {
		return(-1);
	}
	return(joystick->naxes);
}

int SDL_JoystickNumHats(SDL_Joystick *joystick)
{
	if ( ! ValidJoystick(&joystick) ) {
		return(-1);
	}
	return(joystick->nhats);
}

int SDL_JoystickNumBalls(SDL_Joystick *joystick)
{
	if ( ! ValidJoystick(&joystick) ) {
		return(-1);
	}
	return(joystick->nballs);
}

int SDL_JoystickNumButtons(SDL_Joystick *joystick)
{
	if ( ! ValidJoystick(&joystick) ) {
		return(-1);
	}
	return(joystick->nbuttons);
}

int SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis)
{
	int state;

	if ( ! ValidJoystick(&joystick) ) {
		return(0);
	}
	if ( axis < joystick->naxes ) {
		state = joystick->axes[axis];
	} else {
		SDL_SetError("Joystick only has %d axes", joystick->naxes);
		state = 0;
	}
	return(state);
}

unsigned short int SDL_JoystickGetHat(SDL_Joystick *joystick, int hat)
{
	unsigned short int state;

	if ( ! ValidJoystick(&joystick) ) {
		return(0);
	}
	if ( hat < joystick->nhats ) {
		state = joystick->hats[hat];
	} else {
		SDL_SetError("Joystick only has %d hats", joystick->nhats);
		state = 0;
	}
	return(state);
}

int SDL_JoystickGetBall(SDL_Joystick *joystick, int ball, int *dx, int *dy)
{
	int retval;

	if ( ! ValidJoystick(&joystick) ) {
		return(-1);
	}

	retval = 0;
	if ( ball < joystick->nballs ) {
		if ( dx ) {
			*dx = joystick->balls[ball].dx;
		}
		if ( dy ) {
			*dy = joystick->balls[ball].dy;
		}
		joystick->balls[ball].dx = 0;
		joystick->balls[ball].dy = 0;
	} else {
		SDL_SetError("Joystick only has %d balls", joystick->nballs);
		retval = -1;
	}
	return(retval);
}

unsigned short int SDL_JoystickGetButton(SDL_Joystick *joystick, int button)
{
	unsigned short int state;

	if ( ! ValidJoystick(&joystick) ) {
		return(0);
	}
	if ( button < joystick->nbuttons ) {
		state = joystick->buttons[button];
	} else {
		SDL_SetError("Joystick only has %d buttons",joystick->nbuttons);
		state = 0;
	}
	return(state);
}

void SDL_JoystickClose(SDL_Joystick *joystick)
{
	int i;

	if ( ! ValidJoystick(&joystick) ) {
		return;
	}

	/* First decrement ref count */
	if ( --joystick->ref_count > 0 ) {
		return;
	}

	if ( joystick == default_joystick ) {
		default_joystick = NULL;
	}
	SDL_SYS_JoystickClose(joystick);

	/* Remove joystick from list */
	for ( i=0; SDL_joysticks[i]; ++i ) {
		if ( joystick == SDL_joysticks[i] ) {
			memcpy(&SDL_joysticks[i], &SDL_joysticks[i+1],
			       (SDL_numjoysticks-i)*sizeof(joystick));
			break;
		}
	}

	/* Free the data associated with this joystick */
	if ( joystick->axes ) {
		free(joystick->axes);
	}
	if ( joystick->hats ) {
		free(joystick->hats);
	}
	if ( joystick->balls ) {
		free(joystick->balls);
	}
	if ( joystick->buttons ) {
		free(joystick->buttons);
	}
	free(joystick);
}

void SDL_JoystickQuit(void)
{
	SDL_numjoysticks = 0;

	/* Quit the joystick setup */
	SDL_SYS_JoystickQuit();
	if ( SDL_joysticks ) {
		free(SDL_joysticks);
		SDL_joysticks = NULL;
	}
}


/* These are global for SDL_sysjoystick.c and SDL_events.c */
#if 0
int SDL_PrivateJoystickAxis(SDL_Joystick *joystick, unsigned short int axis, int value)
{
	int posted;

	/* Update internal joystick state */
	joystick->axes[axis] = value;

	/* Post the event, if desired */
	posted = 0;
	return(posted);
}

int SDL_PrivateJoystickHat(SDL_Joystick *joystick, unsigned short int hat, unsigned short int value)
{
	int posted;

	/* Update internal joystick state */
	joystick->hats[hat] = value;

	/* Post the event, if desired */
	posted = 0;
	return(posted);
}

int SDL_PrivateJoystickBall(SDL_Joystick *joystick, unsigned short int ball,
					int xrel, int yrel)
{
	int posted;

	/* Update internal mouse state */
	joystick->balls[ball].dx += xrel;
	joystick->balls[ball].dy += yrel;

	/* Post the event, if desired */
	posted = 0;
	return(posted);
}

int SDL_PrivateJoystickButton(SDL_Joystick *joystick, unsigned short int button, unsigned short int state)
{
	int posted;

	/* Update internal joystick state */
	joystick->buttons[button] = state;

	/* Post the event, if desired */
	posted = 0;
	return(posted);
}

void SDL_JoystickUpdate(void)
{
	int i;

	for ( i=0; SDL_joysticks[i]; ++i ) {
		SDL_SYS_JoystickUpdate(SDL_joysticks[i]);
	}
}
#endif
/*! \todo It would be better to use debug() from basic.cpp, but cause we're in the C and not C++ part we can't reach this function. Replace it when
	the whole joystick-part is written in C++.
*/
void 
SDL_SetError(char *fmt, ...) {
#ifdef DEBUG
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
#endif
}

