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

enum_long(COMDEV)
{
    COMDEV_NONE,
    COMDEV_LOOPBACK,
    COMDEV_RETROSHELL,
    COMDEV_COMMANDER
};
typedef COMDEV CommunicationDevice;

#ifdef __cplusplus
struct CommunicationDeviceEnum : util::Reflection<CommunicationDeviceEnum, CommunicationDevice>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = COMDEV_COMMANDER;

    static const char *prefix() { return "COMDEV"; }
    static const char *_key(long value)
    {
        switch (value) {

            case COMDEV_NONE:       return "NONE";
            case COMDEV_LOOPBACK:   return "LOOPBACK";
            case COMDEV_RETROSHELL: return "RETROSHELL";
            case COMDEV_COMMANDER:  return "COMMANDER";
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
    CommunicationDevice device;
    isize baud;
}
RS232Config;

}
