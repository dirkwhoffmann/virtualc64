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
enum class MouseModel : long
{
    C1350,    ///< Joystick mouse (Commodore)
    C1351,    ///< Analog mouse (Commodore)
    NEOS,     ///< Analog mouse (Neos)
    PADDLE_X, ///< Paddle (POTX)
    PADDLE_Y, ///< Paddle (POTY)
    PADDLE_XY ///< Paddle (POTX + POTX)
};

struct MouseModelEnum : util::Reflection<MouseModelEnum, MouseModel> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(MouseModel::PADDLE_XY);

    static const char *_key(MouseModel value)
    {
        switch (value) {

            case MouseModel::C1350:       return "C1350";
            case MouseModel::C1351:       return "C1351";
            case MouseModel::NEOS:        return "NEOS";
            case MouseModel::PADDLE_X:    return "PADDLE_X";
            case MouseModel::PADDLE_Y:    return "PADDLE_Y";
            case MouseModel::PADDLE_XY:   return "PADDLE_XY";
        }
        return "???";
    }
    
    static const char *help(MouseModel value)
    {
        return "";
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
