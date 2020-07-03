// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef CONTROLPORT_TYPES_H
#define CONTROLPORT_TYPES_H

/*! @brief    Joystick directions
 */
typedef enum {
    
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_FIRE
    
} JoystickDirection;

/*! @brief    Joystick events
 */
typedef enum {
    
    PULL_UP,
    PULL_DOWN,
    PULL_LEFT,
    PULL_RIGHT,
    PRESS_FIRE,
    RELEASE_X,
    RELEASE_Y,
    RELEASE_XY,
    RELEASE_FIRE
    
} JoystickEvent;

#endif 
