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
// Constants
//

static constexpr auto PORT_1 = 0;
static constexpr auto PORT_2 = 1;


//
// Enumerations
//

/// Control port device
enum_long(CPDEVICE)
{
    CPDEVICE_NONE,      ///< No device
    CPDEVICE_MOUSE,     ///< Mouse
    CPDEVICE_JOYSTICK   ///< Joystick
};
typedef CPDEVICE ControlPortDevice;

struct ControlPortDeviceEnum : util::Reflection<ControlPortDeviceEnum, ControlPortDevice> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CPDEVICE_JOYSTICK;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CPDEVICE"; }
    static const char *key(long value)
    {
        switch (value) {

            case CPDEVICE_NONE:      return "NONE";
            case CPDEVICE_MOUSE:     return "MOUSE";
            case CPDEVICE_JOYSTICK:  return "JOYSTICK";
        }
        return "???";
    }
};

}
