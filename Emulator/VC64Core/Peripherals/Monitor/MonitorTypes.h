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
enum class Palette : long
{
    COLOR,                      ///< Standard C64 color palette
    BLACK_WHITE,                ///< Black and white monochrome palette
    PAPER_WHITE,                ///< Paper white monochrome palette
    GREEN,                      ///< Green monochrome palette
    AMBER,                      ///< Amber monochrome palette
    SEPIA                       ///< Sepia monochrome palette
};

struct PaletteEnum : util::Reflection<PaletteEnum, Palette> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Palette::SEPIA);

    static const char *_key(Palette value)
    {
        switch (value) {
                
            case Palette::COLOR:        return "COLOR";
            case Palette::BLACK_WHITE:  return "BLACK_WHITE";
            case Palette::PAPER_WHITE:  return "PAPER_WHITE";
            case Palette::GREEN:        return "GREEN";
            case Palette::AMBER:        return "AMBER";
            case Palette::SEPIA:        return "SEPIA";
        }
        return "???";
    }
    static const char *help(Palette value)
    {
        switch (value) {
                
            case Palette::COLOR:        return "Color palette";
            case Palette::BLACK_WHITE:  return "Black and white palette";
            case Palette::PAPER_WHITE:  return "Paper white palette";
            case Palette::GREEN:        return "Green palette";
            case Palette::AMBER:        return "Amber palette";
            case Palette::SEPIA:        return "Sepia palette";
        }
        return "???";
    }
};

/// Pixel upscaler
enum class Upscaler : long
{
    NONE,                      ///< No upscaler
    EPX_2X,                    ///< EPX upscaler (2x)
    XBR_4X                     ///< XBR upscaler (4x)
};

struct UpscalerEnum : util::Reflection<UpscalerEnum, Upscaler> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Upscaler::XBR_4X);

    static const char *_key(Upscaler value)
    {
        switch (value) {

            case Upscaler::NONE:     return "NONE";
            case Upscaler::EPX_2X:   return "EPX_2X";
            case Upscaler::XBR_4X:   return "XBR_4X";
        }
        return "???";
    }
    
    static const char *help(Upscaler value)
    {
        return "";
    }
};

/// Dotmask
enum class Dotmask : long
{
    NONE,                       ///< No dotmask
    BISECTED,                   ///< Dotmask pattern 1
    TRISECTED,                  ///< Dotmask pattern 2
    BISECTED_SHIFTED,           ///< Dotmask pattern 3
    TRISECTED_SHIFTED           ///< Dotmask pattern 4
};

struct DotmaskEnum : util::Reflection<DotmaskEnum, Dotmask> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Dotmask::TRISECTED_SHIFTED);

    static const char *_key(Dotmask value)
    {
        switch (value) {

            case Dotmask::NONE:              return "NONE";
            case Dotmask::BISECTED:          return "BISECTED";
            case Dotmask::TRISECTED:         return "TRISECTED";
            case Dotmask::BISECTED_SHIFTED:  return "BISECTED_SHIFTED";
            case Dotmask::TRISECTED_SHIFTED: return "TRISECTED_SHIFTED";
        }
        return "???";
    }
    
    static const char *help(Dotmask value)
    {
        return "";
    }
};

/// Scanlines
enum class Scanlines : long
{
    NONE,                     ///< No scanlines
    EMBEDDED,                 ///< Embed scanlines in the emulator texture
    SUPERIMPOSE               ///< Emulate scanlines in the fragment shader
};

struct ScanlinesEnum : util::Reflection<ScanlinesEnum, Scanlines> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Scanlines::SUPERIMPOSE);

    static const char *_key(Scanlines value)
    {
        switch (value) {

            case Scanlines::NONE:            return "NONE";
            case Scanlines::EMBEDDED:        return "EMBEDDED";
            case Scanlines::SUPERIMPOSE:     return "SUPERIMPOSE";
        }
        return "???";
    }
    
    static const char *help(Scanlines value)
    {
        return "";
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
