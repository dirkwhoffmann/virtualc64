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

/// Mouse model
enum_long(MOUSE_MODEL)
{
    MOUSE_C1350,    ///< Joystick mouse (Commodore)
    MOUSE_C1351,    ///< Analog mouse (Commodore)
    MOUSE_NEOS      ///< Analog mouse (Neos)
};
typedef MOUSE_MODEL MouseModel;

struct MouseModelEnum : util::Reflection<MouseModelEnum, MouseModel> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = MOUSE_NEOS;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "MOUSE"; }
    static const char *key(long value)
    {
        switch (value) {
                
            case MOUSE_C1350:  return "C1350";
            case MOUSE_C1351:  return "C1351";
            case MOUSE_NEOS:   return "NEOS";
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
