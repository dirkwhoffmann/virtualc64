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

#include "Aliases.h"
#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// Color palette
enum_long(PALETTE)
{
    PALETTE_COLOR,                      ///< Standard C64 color palette
    PALETTE_BLACK_WHITE,                ///< Black and white monochrome palette
    PALETTE_PAPER_WHITE,                ///< Paper white monochrome palette
    PALETTE_GREEN,                      ///< Green monochrome palette
    PALETTE_AMBER,                      ///< Amber monochrome palette
    PALETTE_SEPIA                       ///< Sepia monochrome palette
};
typedef PALETTE Palette;

struct PaletteEnum : util::Reflection<PaletteEnum, Palette> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = PALETTE_SEPIA;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "PALETTE"; }
    static const char *key(long value)
    {
        switch (value) {

            case PALETTE_COLOR:        return "COLOR";
            case PALETTE_BLACK_WHITE:  return "BLACK_WHITE";
            case PALETTE_PAPER_WHITE:  return "PAPER_WHITE";
            case PALETTE_GREEN:        return "GREEN";
            case PALETTE_AMBER:        return "AMBER";
            case PALETTE_SEPIA:        return "SEPIA";
        }
        return "???";
    }
};


//
// Structures
//

typedef struct
{
    // Colors
    Palette palette;
    isize brightness;
    isize contrast;
    isize saturation;
}
MonitorConfig;

}
