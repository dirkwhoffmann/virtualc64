// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/gfx/Images.h"
#include "utl/gfx/Colors.h"
#include <bit>
#include <algorithm>
#include <vector>
#include <fstream>

#define MINIZ_HEADER_FILE_ONLY
#include "miniz.h"

namespace utl {

namespace {

// The texel format whose bytes appear as R,G,B,A in memory
constexpr TexelFormat nativeFormat =
    std::endian::native == std::endian::little ? TexelFormat::ABGR : TexelFormat::RGBA;

constexpr mz_uint pngLevel = 6;

// Repacks a buffer of texels from format F into the native format
template <TexelFormat F> void
convert(const u32 *src, u32 *dst, size_t count)
{
    for (size_t i = 0; i < count; i++) {

        GpuColor<F> c(src[i]);
        dst[i] = GpuColor<nativeFormat>(c.r(), c.g(), c.b(), c.a()).rawValue;
    }
}

}

void
exportPNG(const fs::path &path, const void *data, isize w, isize h,
           TexelFormat texelFormat)
{
    exportPNG(path, data, w, h, w, 1, texelFormat);
}

void
exportPNG(const fs::path &path, const void *data, isize w, isize h, isize stride,
          isize vscale, TexelFormat texelFormat)
{
    if (!data || w <= 0 || h <= 0 || stride < w || vscale < 1)
        throw Error(0, "exportPNG: invalid image data or dimensions");

    auto *pixels = static_cast<const u32 *>(data);

    // Copy into a contiguous buffer if the rows aren't packed, have to be
    // repeated, or the texels aren't in the RGBA byte layout the encoder expects
    std::vector<u32> tmp;
    if (stride != w || vscale != 1 || texelFormat != nativeFormat) {

        tmp.resize(size_t(w) * size_t(h * vscale));
        for (isize y = 0; y < h; y++) {

            auto *src = pixels + y * stride;
            auto *dst = tmp.data() + y * vscale * w;

            switch (texelFormat) {

                case TexelFormat::ABGR: convert<TexelFormat::ABGR>(src, dst, size_t(w)); break;
                case TexelFormat::ARGB: convert<TexelFormat::ARGB>(src, dst, size_t(w)); break;
                case TexelFormat::RGBA: convert<TexelFormat::RGBA>(src, dst, size_t(w)); break;
            }

            // Repeat the converted row
            for (isize i = 1; i < vscale; i++)
                std::copy(dst, dst + w, dst + i * w);
        }
        pixels = tmp.data();
    }

    // Encode into memory and write the file ourselves, which keeps
    // non-ASCII paths working
    size_t len = 0;
    void *png = tdefl_write_image_to_png_file_in_memory_ex(pixels, int(w), int(h * vscale), 4, &len,
                                                           pngLevel, MZ_FALSE);
    if (!png)
        throw Error(0, "exportPNG: image encoding failed");

    std::ofstream file(path, std::ios::binary);
    file.write(static_cast<const char *>(png), std::streamsize(len));
    mz_free(png);
    if (!file)
        throw Error(0, "exportPNG: can't write " + path.string());
}

}
