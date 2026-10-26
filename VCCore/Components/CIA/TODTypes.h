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

#include "BasicTypes.h"

namespace vc64 {

//
// Structures
//

/// Time-of-day value in BCD format
typedef union
{
    struct {
        u8 tenth;                        ///< Tenths of a second
        u8 sec;                          ///< Seconds
        u8 min;                          ///< Minutes
        u8 hour;                         ///< Hours
    };
    u32 value;                           ///< The time as a single 32-bit value
}
TimeOfDay;

/// State of the time-of-day clock
typedef struct
{
    TimeOfDay time;                         ///< Current time
    TimeOfDay latch;                        ///< Latched time
    TimeOfDay alarm;                        ///< Alarm time
}
TODInfo;

}
