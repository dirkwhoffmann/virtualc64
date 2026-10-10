// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include "GraphicsTypes.h"

namespace utl {

// Writes an image to disk as a PNG file
void exportPNG(const fs::path &path, const void *data, isize w, isize h,
               TexelFormat texelFormat = TexelFormat::ABGR);

/* Same as above, for an image that is part of a larger pixel array. The
 * stride is the distance between two rows, measured in texels (not bytes).
 * To export a cutout, pass a pointer to its first texel. Each row is written
 * vscale times, which stretches the image vertically for sources with
 * non-square pixels. The PNG is w texels wide and h * vscale texels high.
 */
void exportPNG(const fs::path &path, const void *data, isize w, isize h, isize stride,
               isize vscale = 1, TexelFormat texelFormat = TexelFormat::ABGR);

}
