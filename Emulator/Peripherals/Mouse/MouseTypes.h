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

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// Mouse model
enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,    ///< Joystick mouse (Commodore)
    MOUSE_C1351,    ///< Analog mouse (Commodore)
    MOUSE_NEOS,     ///< Analog mouse (Neos)
    MOUSE_PADDLE_X, ///< Paddle (POTX)
    MOUSE_PADDLE_Y, ///< Paddle (POTY)
    MOUSE_PADDLE_XY ///< Paddle (POTX + POTX)
};
typedef MOUSE_MODEL MouseModel;

struct MouseModelEnum : util::Reflection<MouseModelEnum, MouseModel> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = MOUSE_PADDLE_XY;

    static const char *prefix() { return "MOUSE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case MOUSE_C1350:       return "C1350";
            case MOUSE_C1351:       return "C1351";
            case MOUSE_NEOS:        return "NEOS";
            case MOUSE_PADDLE_X:    return "PADDLE_X";
            case MOUSE_PADDLE_Y:    return "PADDLE_Y";
            case MOUSE_PADDLE_XY:   return "PADDLE_XY";
        }
        return "???";
    }
};

//
// Structures
//

typedef struct
{
    MouseModel model;
    bool shakeDetection;
    isize velocity;
}
MouseConfig;

}
