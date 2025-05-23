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

#include "VirtualC64Config.h"
#include "Monitor.h"
#include "VICII.h"
#include <cmath>

namespace vc64 {

/* This implementation is mainly based on the following articles by pepto:
 * http://www.pepto.de/projects/colorvic/
 * http://unusedino.de/ec64/technical/misc/vic656x/colors/
 */

// TODO: Turn into Lamda expression
static double gammaCorrect(double value, double source, double target)
{
    // Reverse gamma correction of source
    double factor = std::pow(255.0, 1.0 - source);
    value = std::clamp(factor * std::pow(value, source), 0.0, 255.0);

    // Correct gamma for target
    factor = std::pow(255.0, 1.0 - (1.0 / target));
    value = std::clamp(factor * std::pow(value, 1 / target), 0.0, 255.0);

    return std::round(value);
}

u32
Monitor::getColor(isize nr, Palette palette)
{
    double y, u, v;

    // LUMA levels (varies between VICII models)
#define LUMA_VICE(x,y,z) ((double)(x - y) * 256)/((double)(z - y))
#define LUMA_COLORES(x) (x * 7.96875)

    double luma_vice_6569_r1[16] = {

        // Taken from VICE 3.2
        LUMA_VICE( 630,630,1850), LUMA_VICE(1850,630,1850),
        LUMA_VICE( 900,630,1850), LUMA_VICE(1560,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE(1260,630,1850),
        LUMA_VICE( 900,630,1850), LUMA_VICE(1560,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE( 900,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE( 900,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE(1560,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE(1560,630,1850)
    };

    double luma_vice_6569_r3[16] = {

        // Taken from VICE 3.2
        LUMA_VICE( 700,700,1850), LUMA_VICE(1850,700,1850),
        LUMA_VICE(1090,700,1850), LUMA_VICE(1480,700,1850),
        LUMA_VICE(1180,700,1850), LUMA_VICE(1340,700,1850),
        LUMA_VICE(1020,700,1850), LUMA_VICE(1620,700,1850),
        LUMA_VICE(1180,700,1850), LUMA_VICE(1020,700,1850),
        LUMA_VICE(1340,700,1850), LUMA_VICE(1090,700,1850),
        LUMA_VICE(1300,700,1850), LUMA_VICE(1620,700,1850),
        LUMA_VICE(1300,700,1850), LUMA_VICE(1480,700,1850),
    };

    double luma_vice_6567[16] = {

        // taken from VICE 3.2
        LUMA_VICE( 590,590,1825), LUMA_VICE(1825,590,1825),
        LUMA_VICE( 950,590,1825), LUMA_VICE(1380,590,1825),
        LUMA_VICE(1030,590,1825), LUMA_VICE(1210,590,1825),
        LUMA_VICE( 860,590,1825), LUMA_VICE(1560,590,1825),
        LUMA_VICE(1030,590,1825), LUMA_VICE( 860,590,1825),
        LUMA_VICE(1210,590,1825), LUMA_VICE( 950,590,1825),
        LUMA_VICE(1160,590,1825), LUMA_VICE(1560,590,1825),
        LUMA_VICE(1160,590,1825), LUMA_VICE(1380,590,1825)
    };

    double luma_vice_6567_r65a[16] = {

        // Taken from VICE 3.2
        LUMA_VICE( 560,560,1825), LUMA_VICE(1825,560,1825),
        LUMA_VICE( 840,560,1825), LUMA_VICE(1500,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE(1180,560,1825),
        LUMA_VICE( 840,560,1825), LUMA_VICE(1500,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE( 840,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE( 840,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE(1500,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE(1500,560,1825),
    };

    double luma_pepto[16] = {

        // Taken from Pepto's Colodore palette
        LUMA_COLORES(0),  LUMA_COLORES(32),
        LUMA_COLORES(10), LUMA_COLORES(20),
        LUMA_COLORES(12), LUMA_COLORES(16),
        LUMA_COLORES(8),  LUMA_COLORES(24),
        LUMA_COLORES(12), LUMA_COLORES(8),
        LUMA_COLORES(16), LUMA_COLORES(10),
        LUMA_COLORES(15), LUMA_COLORES(24),
        LUMA_COLORES(15), LUMA_COLORES(20)
    };

    double *luma;
    switch(vic.getConfig().revision) {

        case VICIIRev::PAL_6569_R1:

            luma = luma_vice_6569_r1;
            break;

        case VICIIRev::PAL_6569_R3:

            luma = luma_vice_6569_r3;
            break;

        case VICIIRev::NTSC_6567:

            luma = luma_vice_6567;
            break;

        case VICIIRev::NTSC_6567_R56A:

            luma = luma_vice_6567_r65a;
            break;

        case VICIIRev::PAL_8565:
        case VICIIRev::NTSC_8562:

            luma = luma_pepto;
            break;

        default:
            fatalError;
    }

    // Angles in the color plane
#define ANGLE_PEPTO(x) (x * 22.5 * M_PI / 180.0)
#define ANGLE_COLORES(x) ((x * 22.5 + 11.5) * M_PI / 180.0)

    // Pepto's first approach
    // http://unusedino.de/ec64/technical/misc/vic656x/colors/

    /*
     double angle[16] = {
     NAN,            NAN,
     ANGLE_PEPTO(5), ANGLE_PEPTO(13),
     ANGLE_PEPTO(2), ANGLE_PEPTO(10),
     ANGLE_PEPTO(0), ANGLE_PEPTO(8),
     ANGLE_PEPTO(6), ANGLE_PEPTO(7),
     ANGLE_PEPTO(5), NAN,
     NAN,            ANGLE_PEPTO(10),
     ANGLE_PEPTO(0), NAN
     };
     */

    // Pepto's second approach
    // http://www.pepto.de/projects/colorvic/

    double angle[16] = {
        NAN,               NAN,
        ANGLE_COLORES(4),  ANGLE_COLORES(12),
        ANGLE_COLORES(2),  ANGLE_COLORES(10),
        ANGLE_COLORES(15), ANGLE_COLORES(7),
        ANGLE_COLORES(5),  ANGLE_COLORES(6),
        ANGLE_COLORES(4),  NAN,
        NAN,               ANGLE_COLORES(10),
        ANGLE_COLORES(15), NAN
    };

    //
    // Compute YUV values (adapted from Pepto)
    //

    // Normalize
    double brightness = config.brightness - 50.0;
    double contrast = config.contrast / 100.0 + 0.2;
    double saturation = config.saturation / 1.25;

    // Compute Y, U, and V
    double ang = angle[nr];
    y = luma[nr];
    u = std::isnan(ang) ? 0 : cos(ang) * saturation;
    v = std::isnan(ang) ? 0 : sin(ang) * saturation;

    // Apply brightness and contrast
    y *= contrast;
    u *= contrast;
    v *= contrast;
    y += brightness;

    // Translate to monochrome if applicable
    switch(palette) {

        case Palette::BLACK_WHITE:
            u = 0.0;
            v = 0.0;
            break;

        case Palette::PAPER_WHITE:
            u = -128.0 + 120.0;
            v = -128.0 + 133.0;
            break;

        case Palette::GREEN:
            u = -128.0 + 29.0;
            v = -128.0 + 64.0;
            break;

        case Palette::AMBER:
            u = -128.0 + 24.0;
            v = -128.0 + 178.0;
            break;

        case Palette::SEPIA:
            u = -128.0 + 97.0;
            v = -128.0 + 154.0;
            break;

        default:
            assert(palette == Palette::COLOR);
    }

    // Convert YUV value to RGB
    double r = y             + 1.140 * v;
    double g = y - 0.396 * u - 0.581 * v;
    double b = y + 2.029 * u;
    r = std::clamp(r, 0.0, 255.0);
    g = std::clamp(g, 0.0, 255.0);
    b = std::clamp(b, 0.0, 255.0);

    // Apply Gamma correction for PAL models
    if (vic.pal()) {

        r = gammaCorrect(r, 2.8, 2.2);
        g = gammaCorrect(g, 2.8, 2.2);
        b = gammaCorrect(b, 2.8, 2.2);
    }

    return LO_LO_HI_HI((u8)r, (u8)g, (u8)b, 0xFF);
}

}
