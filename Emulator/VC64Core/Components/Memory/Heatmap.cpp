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
#include "Heatmap.h"
#include "Memory.h"
#include "VICII.h"

namespace vc64 {

Heatmap::Heatmap()
{
    double r = 0.0, g = 0.0, b = 0.0, a = .25;
    double dr, dg, db, da;

    dr = (1.0 - r) / 256; dg = (0.0 - g) / 256; db = (0.0 - b) / 256; da = (1.0 - a) / 256;
    for (isize i = 0; i < 256; i++, r += dr, g += dg, b += db, a += da) {
        palette[i] = u8(a * 255) << 24 | u8(b * 255) << 16 | u8(g * 255) << 8 | u8(r * 255);
    }

    /*
    r = 0.0; g = 0.0; b = 0.0; a = .25;
    dr = (0.0 - r) / 128; dg = (0.0 - g) / 128; db = (0.0 - b) / 128; da = (1.0 - a) / 128;
    for (isize i = 0; i < 128; i++, r += dr, g += dg, b += db, a += da) {
        palette[i] = u8(a * 255) << 24 | u8(b * 255) << 16 | u8(g * 255) << 8 | u8(r * 255);
    }
    dr = (1.0 - r) / 128; dg = (0.0 - g) / 128; db = (0.0 - b) / 128; da = (1.0 - a) / 128;
    for (isize i = 128; i < 256; i++, r += dr, g += dg, b += db, a += da) {
        palette[i] = u8(a * 255) << 24 | u8(b * 255) << 16 | u8(g * 255) << 8 | u8(r * 255);
    }
    */

}

void
Heatmap::update(const class Memory &mem)
{
    for (isize i = 0; i < 65536; i++) {
        
        // isize x = i % 256, y = i / 256;

        isize x = 0, y = 0;
        if (i & (1 << 15)) y += 128;
        if (i & (1 << 14)) x += 128;
        if (i & (1 << 13)) y += 64;
        if (i & (1 << 12)) x += 64;
        if (i & (1 << 11)) y += 32;
        if (i & (1 << 10)) x += 32;
        if (i & (1 << 9)) y += 16;
        if (i & (1 << 8)) x += 16;
        if (i & (1 << 7)) y += 8;
        if (i & (1 << 6)) x += 8;
        if (i & (1 << 5)) y += 4;
        if (i & (1 << 4)) x += 4;
        if (i & (1 << 3)) y += 2;
        if (i & (1 << 2)) x += 2;
        if (i & (1 << 1)) y += 1;
        if (i & (1 << 0)) x += 1;

        auto totalAccesses = mem.stats.reads[i] + mem.stats.writes[i];
        auto accesses = totalAccesses - history[i];
        history[i] = totalAccesses;

        if (accesses) {
            heatmap[y][x] = 1.0;
        } else {
            heatmap[y][x] = 0.9f * heatmap[y][x] + 0.1f * accesses;
        }
    }
}

void 
Heatmap::draw(u32 *buffer, isize width, isize height) const
{
    assert(width == 256);
    assert(height == 256);

    float values[32][64] = { }, max = 0;

    // Accumulate values
    for (usize y = 0; y < 256; y++) {
        for (usize x = 0; x < 256; x++) {
            values[y / 8][x / 4] += heatmap[y][x];
        }
    }

    // Determine maximum value
    for (usize y = 0; y < 32; y++) {
        for (usize x = 0; x < 64; x++) {
            max = std::max(max, values[y][x]);
        }
    }

    for (isize i = 0; i < 65536; i++) buffer[i] = 0;

    max /= 255.0f;
    for (usize y = 0; y < 32; y++) {
        for (usize x = 0; x < 64; x++) {

            u8 val = u8(values[y][x] / max);
            u32 col = palette[val];
            buffer[y*256*8 + 4*x + 0] = col;
            buffer[y*256*8 + 4*x + 1] = col;
            buffer[y*256*8 + 4*x + 2] = col;
            buffer[y*256*8 + 4*x + 3] = 0;
        }
        for (isize j = 0; j < 7; j++) {
            memcpy(buffer+y*256*8+(j*256),buffer+y*256*8,256*4);
        }
        // memset(buffer+y*256*8+(7*256), 0, 256*4);
    }
}

}
