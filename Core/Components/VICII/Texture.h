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

#include "Buffer.h"
#include "Constants.h"

namespace vc64 {

typedef u32 Texel;
consteval u32 TEXEL(u32 x) { return x; }

struct Texture {

    // Texture dimensions
    static const long height = Tex::height;
    static const long width  = Tex::width;

    // Predefined colors
    static constexpr Texel black    = TEXEL(0xFF000000);
    static constexpr Texel grey2    = TEXEL(0xFF222222);
    static constexpr Texel grey4    = TEXEL(0xFF444444);
    static constexpr Texel white    = TEXEL(0xFFFFFFFF);
    static constexpr Texel red      = TEXEL(0xFF0000FF);
    static constexpr Texel green    = TEXEL(0xFF00FF00);
    static constexpr Texel blue     = TEXEL(0xFFFF0000);
    static constexpr Texel yellow   = TEXEL(0xFF00FFFF);
    static constexpr Texel magenta  = TEXEL(0xFFFF00FF);
    static constexpr Texel cyan     = TEXEL(0xFFFFFF00);

    // Color aliases
    // static constexpr Texel vblank   = grey4;    // VBLANK area
    // static constexpr Texel hblank   = grey4;    // HBLANK area
    static constexpr Texel cb1      = grey2;    // Checkerboard color 1
    static constexpr Texel cb2      = grey4;    // Checkerboard color 2

    // Frame number
    i64 nr = 0;

    // Pixel buffer
    util::Buffer <u32> pixels;

    Texture();

    // Initializes (a portion of) the frame buffer with a checkerboard pattern
    void clear();
    void clear(isize row);
    void clear(isize row, isize cycle);
};

}
