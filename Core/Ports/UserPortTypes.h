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

#pragma once

#include "RS232Types.h"

namespace vc64 {

//
// Enumerations
//

enum class UserPortDevice : long
{
    NONE,
    PARCABLE,
    PARCABLE_DOLPHIN,
    RS232
};

struct UserPortDeviceEnum : Reflection<UserPortDeviceEnum, UserPortDevice>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(UserPortDevice::RS232);

    static const char *_key(UserPortDevice value)
    {
        switch (value) {

            case UserPortDevice::NONE:              return "NONE";
            case UserPortDevice::PARCABLE:          return "PARCABLE";
            case UserPortDevice::PARCABLE_DOLPHIN:  return "PARCABLE_DOLPHIN";
            case UserPortDevice::RS232:             return "RS232";
        }
        return "???";
    }
    
    static const char *help(UserPortDevice value)
    {
        return "";
    }
};


//
// Structures
//

typedef struct
{
    UserPortDevice device;
}
UserPortConfig;

typedef struct
{

}
UserPortInfo;

}
