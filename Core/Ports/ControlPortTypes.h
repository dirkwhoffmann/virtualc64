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
enum class ControlPortDevice
{
    NONE,      ///< No device
    MOUSE,     ///< Mouse
    JOYSTICK   ///< Joystick
};

struct ControlPortDeviceEnum : Reflection<ControlPortDeviceEnum, ControlPortDevice> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ControlPortDevice::JOYSTICK);

    static const char *_key(ControlPortDevice value)
    {
        switch (value) {

            case ControlPortDevice::NONE:      return "NONE";
            case ControlPortDevice::MOUSE:     return "MOUSE";
            case ControlPortDevice::JOYSTICK:  return "JOYSTICK";
        }
        return "???";
    }
    
    static const char *help(ControlPortDevice value)
    {
        return "";
    }
};

}
