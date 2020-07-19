// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PORT_TYPES_H
#define _PORT_TYPES_H

//
// Enumerations
//

typedef enum : long
{
    CPD_NONE,
    CPD_MOUSE,
    CPD_JOYSTICK
}
ControlPortDevice;

inline bool isControlPortDevice(long value) {
    return value >= 0 && value <= CPD_JOYSTICK;
}

// DEPRECATED
typedef enum {
    
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_FIRE
    
} JoystickDirection;

typedef enum
{
    PULL_UP = 0,   // Pull the joystick up
    PULL_DOWN,     // Pull the joystick down
    PULL_LEFT,     // Pull the joystick left
    PULL_RIGHT,    // Pull the joystick right
    PRESS_FIRE,    // Press the joystick button
    PRESS_LEFT,    // Press the left mouse button
    PRESS_RIGHT,   // Press the right mouse button
    RELEASE_X,     // Move back to neutral horizontally
    RELEASE_Y,     // Move back to neutral vertically
    RELEASE_XY,    // Move back to neutral
    RELEASE_FIRE,  // Release the joystick button
    RELEASE_LEFT,  // Release the left mouse button
    RELEASE_RIGHT  // Release the right mouse button
}
GamePadAction;

inline bool isGamePadAction(long value) {
    return value >= 0 && value <= RELEASE_RIGHT;
}

typedef enum
{
    CRT_16K,
    CRT_8K,
    CRT_ULTIMAX,
    CRT_OFF
    
}
CartridgeMode;

#endif
