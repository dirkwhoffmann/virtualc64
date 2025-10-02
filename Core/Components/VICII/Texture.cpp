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

#include "config.h"
#include "Texture.h"

namespace vc64 {

Texture::Texture()
{
    pixels.alloc(texels);
}

void
Texture::clear(Texel col1, Texel col2)
{
    auto *ptr = pixels.ptr;

    for (isize row = 0; row < height; row++, ptr += width) {
        for (isize col = 0; col < width; col++) {
            ptr[col] = ((row >> 2) & 1) == ((col >> 3) & 1) ? col1 : col2;
        }
    }
}

void
Texture::clear(isize row, Texel col1, Texel col2)
{
    auto *ptr = pixels.ptr + row * width;

    for (isize col = 0; col < width; col++) {
        ptr[col] = ((row >> 2) & 1) == ((col >> 3) & 1) ? col1 : col2;
    }
}

void
Texture::clear(isize row, isize cycle, Texel col1, Texel col2)
{
    auto *ptr = pixels.ptr + row * width + 4 * cycle;

    for (isize col = 0; col < 4; col++) {
        ptr[col] = ((row >> 2) & 1) == ((col >> 3) & 1) ? col1 : col2;
    }
}

}
