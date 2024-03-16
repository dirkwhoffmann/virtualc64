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
#include "Reflection.hpp"

//
// Enumerations
//

/// Mouse model
enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,    ///< Joystick mouse (Commodore)
    MOUSE_C1351,    ///< Analog mouse (Commodore)
    MOUSE_NEOS      ///< Analog mouse (Neos)
};
typedef MOUSE_MODEL MouseModel;

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
