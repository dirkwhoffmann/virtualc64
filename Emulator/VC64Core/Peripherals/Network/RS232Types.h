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

enum class CommunicationDevice : long
{
    NONE,
    LOOPBACK,
    RETROSHELL,
    COMMANDER
};

struct CommunicationDeviceEnum : util::Reflection<CommunicationDeviceEnum, CommunicationDevice>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(CommunicationDevice::COMMANDER);

    static const char *_key(CommunicationDevice value)
    {
        switch (value) {

            case CommunicationDevice::NONE:       return "NONE";
            case CommunicationDevice::LOOPBACK:   return "LOOPBACK";
            case CommunicationDevice::RETROSHELL: return "RETROSHELL";
            case CommunicationDevice::COMMANDER:  return "COMMANDER";
        }
        return "???";
    }
    
    static const char *help(CommunicationDevice value)
    {
        return "";
    }
};


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
