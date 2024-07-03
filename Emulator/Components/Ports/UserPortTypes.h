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

enum_long(USR)
{
    USR_NONE,
    USR_PARCABLE,
    USR_PARCABLE_DOLPHIN,
    USR_RS232
};
typedef USR UserPortDevice;

#ifdef __cplusplus
struct UserPortDeviceEnum : util::Reflection<UserPortDeviceEnum, UserPortDevice>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = USR_RS232;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "USR"; }
    static const char *key(long value)
    {
        switch (value) {

            case USR_NONE:              return "NONE";
            case USR_PARCABLE:          return "PARCABLE";
            case USR_PARCABLE_DOLPHIN:  return "PARCABLE_DOLPHIN";
            case USR_RS232:             return "RS232";
        }
        return "???";
    }
};
#endif


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
