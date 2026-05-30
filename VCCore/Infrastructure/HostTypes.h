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
// Enumerations
//

/// Texture format
enum class TexFormat : long
{
    ABGR,                       ///< AABBGGRR
    ARGB,                       ///< AARRGGBB
    RGBA                        ///< RRGGBBAA
};

struct TexFormatEnum : Reflectable<TexFormatEnum, TexFormat> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(TexFormat::RGBA);

    static const char *_key(TexFormat value)
    {
        switch (value) {

            case TexFormat::ABGR:       return "ABGR";
            case TexFormat::ARGB:       return "ARGB";
            case TexFormat::RGBA:       return "RGBA";
        }
        return "???";
    }
    static const char *help(TexFormat value)
    {
        switch (value) {

            case TexFormat::ABGR:       return "32 bit AABBGGRR";
            case TexFormat::ARGB:       return "32 bit AARRGGBB";
            case TexFormat::RGBA:       return "32 bit RRGGBBAA";
        }
        return "???";
    }
};


//
// Structures
//

typedef struct
{
    // Refresh rate of the host display
    isize refreshRate;
    
    // Audio sample rate of the host computer
    isize sampleRate;

    // Texture format
    TexFormat texFormat;

    // Framebuffer dimensions
    isize frameBufferWidth;
    isize frameBufferHeight;
}
HostConfig;

}
