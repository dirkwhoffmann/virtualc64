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

#include "BasicTypes.h"

namespace vc64 {

//
// Enumerations
//

/// Gamepad / Joystick event
enum class GamePadAction : long
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

struct GamePadActionEnum : Reflectable<GamePadActionEnum, GamePadAction> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(GamePadAction::RELEASE_RIGHT);

    static const char *_key(GamePadAction value)
    {
        switch (value) {

            case GamePadAction::PULL_UP:        return "PULL_UP";
            case GamePadAction::PULL_DOWN:      return "PULL_DOWN";
            case GamePadAction::PULL_LEFT:      return "PULL_LEFT";
            case GamePadAction::PULL_RIGHT:     return "PULL_RIGHT";
            case GamePadAction::PRESS_FIRE:     return "PRESS_FIRE";
            case GamePadAction::PRESS_LEFT:     return "PRESS_LEFT";
            case GamePadAction::PRESS_RIGHT:    return "PRESS_RIGHT";
            case GamePadAction::RELEASE_X:      return "RELEASE_X";
            case GamePadAction::RELEASE_Y:      return "RELEASE_Y";
            case GamePadAction::RELEASE_XY:     return "RELEASE_XY";
            case GamePadAction::RELEASE_FIRE:   return "RELEASE_FIRE";
            case GamePadAction::RELEASE_LEFT:   return "RELEASE_LEFT";
            case GamePadAction::RELEASE_RIGHT:  return "RELEASE_RIGHT";
        }
        return "???";
    }
    
    static const char *help(GamePadAction value)
    {
        return "";
    }
};

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

typedef struct
{
    bool button;
    int axisX;
    int axisY;
}
JoystickInfo;

}
