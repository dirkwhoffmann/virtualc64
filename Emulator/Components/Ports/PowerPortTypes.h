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

/// Power grip characteristics
enum_long(GRID)
{
    GRID_STABLE_50HZ,   ///< 50Hz constant
    GRID_UNSTABLE_50HZ, ///< 50Hz with jitter
    GRID_STABLE_60HZ,   ///< 60Hz constant
    GRID_UNSTABLE_60HZ  ///< 60Hz with jitter
};
typedef GRID PowerGrid;

struct PowerGridEnum : util::Reflection<PowerGridEnum, PowerGrid> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = GRID_UNSTABLE_60HZ;

    static const char *prefix() { return "GRID"; }
    static const char *_key(long value)
    {
        switch (value) {

            case GRID_STABLE_50HZ:    return "STABLE_50HZ";
            case GRID_UNSTABLE_50HZ:  return "UNSTABLE_50HZ";
            case GRID_STABLE_60HZ:    return "STABLE_60HZ";
            case GRID_UNSTABLE_60HZ:  return "UNSTABLE_60HZ";
        }
        return "???";
    }
};

//
// Structures
//

typedef struct
{
    PowerGrid powerGrid;
}
PowerPortConfig;

}
