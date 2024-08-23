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

    static const char *prefix() { return "PALETTE"; }
    static const char *_key(long value)
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

/// Pixel upscaler
enum_long(UPSCALER)
{
    UPSCALER_NONE,                      ///< No upscaler
    UPSCALER_EPX_2X,                    ///< EPX upscaler (2x)
    UPSCALER_XBR_4X                     ///< XBR upscaler (4x)
};
typedef UPSCALER Upscaler;

struct UpscalerEnum : util::Reflection<UpscalerEnum, Upscaler> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = UPSCALER_XBR_4X;

    static const char *prefix() { return "UPSCALER"; }
    static const char *_key(long value)
    {
        switch (value) {

            case UPSCALER_NONE:     return "NONE";
            case UPSCALER_EPX_2X:   return "EPX_2X";
            case UPSCALER_XBR_4X:   return "XBR_4X";
        }
        return "???";
    }
};

/// Dotmask
enum_long(DOTMASK)
{
    DOTMASK_NONE,                       ///< No dotmask
    DOTMASK_BISECTED,                   ///< Dotmask pattern 1
    DOTMASK_TRISECTED,                  ///< Dotmask pattern 2
    DOTMASK_BISECTED_SHIFTED,           ///< Dotmask pattern 3
    DOTMASK_TRISECTED_SHIFTED           ///< Dotmask pattern 4
};
typedef DOTMASK Dotmask;

struct DotmaskEnum : util::Reflection<DotmaskEnum, Dotmask> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DOTMASK_TRISECTED_SHIFTED;

    static const char *prefix() { return "DOTMASK"; }
    static const char *_key(long value)
    {
        switch (value) {

            case DOTMASK_NONE:              return "NONE";
            case DOTMASK_BISECTED:          return "BISECTED";
            case DOTMASK_TRISECTED:         return "TRISECTED";
            case DOTMASK_BISECTED_SHIFTED:  return "BISECTED_SHIFTED";
            case DOTMASK_TRISECTED_SHIFTED: return "TRISECTED_SHIFTED";
        }
        return "???";
    }
};

/// Scanlines
enum_long(SCANLINES)
{
    SCANLINES_NONE,                     ///< No scanlines
    SCANLINES_EMBEDDED,                 ///< Embed scanlines in the emulator texture
    SCANLINES_SUPERIMPOSE               ///< Emulate scanlines in the fragment shader
};
typedef SCANLINES Scanlines;

struct ScanlinesEnum : util::Reflection<ScanlinesEnum, Scanlines> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = SCANLINES_SUPERIMPOSE;

    static const char *prefix() { return "SCANLINES"; }
    static const char *_key(long value)
    {
        switch (value) {

            case SCANLINES_NONE:            return "NONE";
            case SCANLINES_EMBEDDED:        return "EMBEDDED";
            case SCANLINES_SUPERIMPOSE:     return "SUPERIMPOSE";
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

    // Geometry
    isize hCenter;
    isize vCenter;
    isize hZoom;
    isize vZoom;

    // Effects
    Upscaler upscaler;

    bool blur;
    isize blurRadius;

    bool bloom;
    isize bloomRadius;
    isize bloomBrightness;
    isize bloomWeight;

    Dotmask dotmask;
    isize dotMaskBrightness;

    Scanlines scanlines;
    isize scanlineBrightness;
    isize scanlineWeight;

    bool disalignment;
    isize disalignmentH;
    isize disalignmentV;
}
MonitorConfig;

}
