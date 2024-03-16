// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------
/// @file

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

/// Gamepad / Joystick event
enum_long(GAME_PAD_ACTION)
{
    PULL_UP,       ///< Pull the joystick up
    PULL_DOWN,     ///< Pull the joystick down
    PULL_LEFT,     ///< Pull the joystick left
    PULL_RIGHT,    ///< Pull the joystick right
    PRESS_FIRE,    ///< Press the joystick button
    PRESS_LEFT,    ///< Press the left mouse button
    PRESS_RIGHT,   ///< Press the right mouse button
    RELEASE_X,     ///< Move back to neutral horizontally
    RELEASE_Y,     ///< Move back to neutral vertically
    RELEASE_XY,    ///< Move back to neutral
    RELEASE_FIRE,  ///< Release the joystick button
    RELEASE_LEFT,  ///< Release the left mouse button
    RELEASE_RIGHT  ///< Release the right mouse button
};
typedef GAME_PAD_ACTION GamePadAction;

//
// Structures
//

typedef struct
{
    bool autofire;
    bool autofireBursts;
    isize autofireBullets;
    isize autofireDelay;
}
JoystickConfig;
