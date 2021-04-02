// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"

//
// Timing parameters
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
static const long PAL_PIXELS               = 504;  // 104 + 32 + 320 + 32 + 16
static const long NTSC_PIXELS              = 520;  // 104 + 32 + 320 + 32 + 32

// Width and height of the emulator texture
static const long TEX_HEIGHT               = 312;  // PAL height
static const long TEX_WIDTH                = 520;  // NTSC width

// Vertical parameters
static const long FIRST_VISIBLE_LINE       = 16;
