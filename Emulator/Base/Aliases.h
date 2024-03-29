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

#include "Macros.h"

//
// Type aliases
//

typedef i64 Cycle;

typedef isize Cylinder;
typedef isize Head;
typedef isize Track;
typedef isize Halftrack;
typedef isize Sector;
typedef isize Block;

typedef isize HeadPos;


//
// Conversion macros
//

// Converts a time span to an (approximate) cycle count
#define USEC(delay)           (Cycle)((delay) * 1)
#define MSEC(delay)           (Cycle)((delay) * 1000)
#define SEC(delay)            (Cycle)((delay) * 1000000)

// Converts kilo and mega bytes to bytes
#define KB(x) ((x) << 10)
#define MB(x) ((x) << 20)
#define GB(x) ((x) << 30)

// Converts kilo and mega Hertz to Hertz
#define KHz(x) ((x) * 1000)
#define MHz(x) ((x) * 1000000)
