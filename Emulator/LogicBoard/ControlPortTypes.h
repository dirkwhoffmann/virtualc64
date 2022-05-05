// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Constants
//

#define PORT_1  1
#define PORT_2  2


//
// Enumerations
//

enum_long(CPDEVICE)
{
    CPDEVICE_NONE,
    CPDEVICE_MOUSE,
    CPDEVICE_JOYSTICK
};
typedef CPDEVICE ControlPortDevice;

#ifdef __cplusplus
struct ControlPortDeviceEnum : util::Reflection<ControlPortDeviceEnum, ControlPortDevice> {
    
	static constexpr long minVal = 0;
    static constexpr long maxVal = CPDEVICE_JOYSTICK;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }
    
    static const char *prefix() { return "CPDEVICE"; }
    static const char *key(ControlPortDevice value)
    {
        switch (value) {
                
            case CPDEVICE_NONE:      return "NONE";
            case CPDEVICE_MOUSE:     return "MOUSE";
            case CPDEVICE_JOYSTICK:  return "JOYSTICK";
        }
        return "???";
    }
};
#endif
