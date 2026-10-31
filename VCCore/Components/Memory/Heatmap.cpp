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

#include "vcconfig.h"
#include "Heatmap.h"

namespace vc64 {

void
Heatmap::reset(const AccessTrace &trace)
{
    for (isize y = 0; y < 256; y++) {
        for (isize x = 0; x < 256; x++) heatmap[y][x] = 0;
    }
    for (isize i = 0; i < 65536; i++) history[i] = trace.reads[i] + trace.writes[i];
}

void
Heatmap::update(const AccessTrace &trace)
{
    for (isize i = 0; i < 65536; i++) {

        // Interleave the address bits: even bits form x, odd bits form y
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

        u32 totalAccesses = trace.reads[i] + trace.writes[i];
        bool accessed = totalAccesses != history[i];
        history[i] = totalAccesses;

        heatmap[y][x] = accessed ? 1.0f : 0.9f * heatmap[y][x];
    }
}

void
Heatmap::compute(float (&levels)[rows][cols]) const
{
    float max = 0;

    // Accumulate values
    for (isize y = 0; y < rows; y++) for (isize x = 0; x < cols; x++) levels[y][x] = 0;
    for (isize y = 0; y < 256; y++) {
        for (isize x = 0; x < 256; x++) {
            levels[y / 8][x / 4] += heatmap[y][x];
        }
    }

    // Normalize
    for (isize y = 0; y < rows; y++) for (isize x = 0; x < cols; x++) max = std::max(max, levels[y][x]);
    if (max <= 0) return;
    for (isize y = 0; y < rows; y++) for (isize x = 0; x < cols; x++) levels[y][x] /= max;
}

}
