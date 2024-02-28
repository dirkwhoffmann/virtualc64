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

#include "Aliases.h"

namespace vc64 {

//
// Frequencies
//

// Clock frequency in Hz
static const long PAL_CLOCK_FREQUENCY      = 985249;
static const long NTSC_CLOCK_FREQUENCY     = 1022727;
static const long PAL_CYCLES_PER_SECOND    = PAL_CLOCK_FREQUENCY;
static const long NTSC_CYCLES_PER_SECOND   = NTSC_CLOCK_FREQUENCY;


//
// Screen parameters
//

// Pixel aspect ratios
static const double PAL_ASPECT_RATIO       = 0.9365;
static const double NTSC_ASPECT_RATIO      = 0.7500;

// Horizontal parameters (measured in cycles)
static const long HBLANK_CYCLES_LEFT       = 13;
static const long BORDER_CYCLES_LEFT       = 4;
static const long CANVAS_CYCLES            = 40;
static const long BORDER_CYCLES_RIGHT      = 4;
static const long HBLANK_CYCLES_RIGHT_PAL  = 2;
static const long HBLANK_CYCLES_RIGHT_NTSC = 4;

// Derived values
static const long PAL_CYCLES               = 63;   // 13 + 4 + 40 + 4 + 2
static const long NTSC_CYCLES              = 65;   // 13 + 4 + 40 + 4 + 4

// Horizontal parameters (measured in pixels)
static const long HBLANK_PIXELS_LEFT       = 104;
static const long BORDER_PIXELS_LEFT       = 32;
static const long CANVAS_PIXELS            = 320;
static const long BORDER_PIXELS_RIGHT      = 32;
static const long HBLANK_PIXELS_RIGHT_PAL  = 16;
static const long HBLANK_PIXELS_RIGHT_NTSC = 32;

// Derived values
static const long FIRST_VISIBLE_PIXEL      = 104;  // 104
static const long VISIBLE_PIXELS           = 384;  //       32 + 320 + 32
static const long LAST_VISIBLE_PIXEL       = 487;  // 104 + 32 + 320 + 31
static const long PAL_PIXELS               = 504;  // 104 + 32 + 320 + 32 + 16
static const long NTSC_PIXELS              = 520;  // 104 + 32 + 320 + 32 + 32

// Vertical parameters
static const long FIRST_VISIBLE_LINE       = 16;

// Width and height of the emulator texture
static const long TEX_HEIGHT               = 312;  // PAL height
static const long TEX_WIDTH                = 520;  // NTSC width

}
