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
#include "C64Memory.h"
#include "VICII.h"

namespace vc64 {

void 
Heatmap::update(const class C64Memory &mem)
{
    float newheat[256][256];
    static u32 palette[256] = { };

    if (palette[0] == 0) {

        u8 r = 0; u8 g = 255; u8 b = 0;

        for (isize i = 0; i < 128; i++) {

            palette[i] = 0xFF << 24 | b << 16 | g << 8 | r;
            g -= 2; r += 2;
        }
        g = 0; r = 255;
        for (isize i = 128; i < 256; i++) {

            palette[i] = 0xFF << 24 | b << 16 | g << 8 | r;
            g += 2; // b += 2;
        }

        printf("initalize palette\n");
        for (isize i = 0; i < 256; i++) {

        }
    }

    for (isize i = 0; i < 65536; i++) {

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

        // y = i / 256; x = i % 256;

        auto totalAccesses = mem.stats.reads[i] + mem.stats.writes[i];
        auto accesses = totalAccesses - history[i];
        history[i] = totalAccesses;

        // heatmap[y][x] = accesses;

        // float oldTemperature = heatmap[y][x];
        float newTemperature = float(accesses * 128);
        // float mixedTemperature = 0.8f * oldTemperature + 0.2f * newTemperature;
        if (newTemperature > 255) newTemperature = 255;
        if (newTemperature < 0) newTemperature = 0;
        newheat[y][x] = newTemperature;

        if (x > 0 && x < 255 && y > 0 && y < 255) {

            // Bleed
            /*
            newheat[y-1][x-1] += newTemperature / 2;
            newheat[y-1][x] += newTemperature / 2;
            newheat[y-1][x+1] += newTemperature / 2;
            newheat[y][x-1] += newTemperature / 2;
            newheat[y][x+1] += newTemperature / 2;
            newheat[y+1][x-1] += newTemperature / 2;
            newheat[y+1][x] += newTemperature / 2;
            newheat[y+1][x+1] += newTemperature / 2;
            */
        }
    }

    for (isize i = 0; i < 65536; i++) {

        float h = newheat[0][i];
        // printf("h = %f heatmap[0][i] = %f\n", h, heatmap[0][i]);
        if (h > 255) h = 255; if (h < 0) h = 0;


        float heat;
        if (h > heatmap[0][i]) {
            heat = 0.8f * heatmap[0][i] + 0.2f * h;
        } else {
            heat = 0.95f * heatmap[0][i] + 0.05f * h;
        }
        if (heat > 256 || heat < 0) {
            printf("heatmap[0][%ld] = %f\n", i, heatmap[0][i]);
        }
        assert(heat < 256 && heat >= 0);
        if (heat > 255) heat = 255;
        if (heat < 0) heat = 0;
        heatmap[0][i] = heat;

        auto val8 = u8(heat);
        // texture[i] = 0xFF << 24 | val8 << 16 | val8 << 8 | val8;
        texture[i] = 0x88 << 24 | 0 << 16 | 0 << 8 | val8;
        // texture[i] = palette[val8];
    }

    // printf("maxval = %ld\n", maxval);

    // Experimental
    /*
    auto *t1 = mem.vic.emuTexture1;
    auto *t2 = mem.vic.emuTexture2;
    for (isize i = 0; i < 256; i++) {
        for (isize j = 0; j < 256; j++) {
            isize k = Texture::width * i + j;
            t1[k] = t2[k] = texture[i * 256 + j];
        }
    }
    */
}

}
