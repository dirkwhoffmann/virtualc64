// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PORT_PUBLIC_H
#define _PORT_PUBLIC_H

//
// Enumerations
//

enum_long(CPDEVICE)
{
    CPDEVICE_NONE,
    CPDEVICE_MOUSE,
    CPDEVICE_JOYSTICK,
    CPDEVICE_COUNT
};
typedef CPDEVICE ControlPortDevice;

enum_long(GAME_PAD_ACTION)
{
    PULL_UP,       // Pull the joystick up
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
};
typedef GAME_PAD_ACTION GamePadAction;

enum_long(CRTMODE)
{
    CRTMODE_16K,
    CRTMODE_8K,
    CRTMODE_ULTIMAX,
    CRTMODE_OFF,
    CRTMODE_COUNT
};
typedef CRTMODE CRTMode;

#endif
