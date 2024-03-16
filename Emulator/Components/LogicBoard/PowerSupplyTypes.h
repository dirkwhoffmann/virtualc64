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

#include "Types.h"


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


//
// Structures
//

typedef struct
{
    PowerGrid powerGrid;
}
PowerSupplyConfig;
