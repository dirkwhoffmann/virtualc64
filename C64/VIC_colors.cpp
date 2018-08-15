/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, 2018
 */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

// This implementation is mainly based on the following articly by pepto:
// http://www.pepto.de/projects/colorvic/

#include "VIC.h"

double gammaCorrect(double value, double source, double target)
{
    double result = MAX(MIN(value, 255.0), 0.0);
    result = pow(255, 1 - source)     * pow(result, source);
    result = pow(255, 1 - 1 / target) * pow(result, 1 / target);
    return result;
}

uint32_t
VIC::getColor(unsigned nr)
{
    assert(nr < 16);
    return pixelEngine.colors[nr];
}

void
VIC::setBrightness(double value)
{
    brightness = value;
    updatePalette();
}

void
VIC::setContrast(double value)
{
    contrast = value;
    updatePalette();
}

void
VIC::setSaturation(double value)
{
    saturation = value;
    updatePalette();
}

void
VIC::updatePalette()
{
    // LUMA levels (varies between VICII models)
    #define LUMA(x,y,z) ((double)(x - y) * 32.0)/((double)(z - y))
    double luma[6][16] = {
        
        {   /* PAL_6569_R1 (taken from VICE 3.2) */
            LUMA( 630,630,1850),
            LUMA(1850,630,1850),
            LUMA( 900,630,1850),
            LUMA(1560,630,1850),
            LUMA(1260,630,1850),
            LUMA(1260,630,1850),
            LUMA( 900,630,1850),
            LUMA(1560,630,1850),
            LUMA(1260,630,1850),
            LUMA( 900,630,1850),
            LUMA(1260,630,1850),
            LUMA( 900,630,1850),
            LUMA(1260,630,1850),
            LUMA(1560,630,1850),
            LUMA(1260,630,1850),
            LUMA(1560,630,1850)
        },
        
        {   /* PAL_6569_R3 (taken from VICE 3.2) */
            LUMA( 700,700,1850),
            LUMA(1850,700,1850),
            LUMA(1090,700,1850),
            LUMA(1480,700,1850),
            LUMA(1180,700,1850),
            LUMA(1340,700,1850),
            LUMA(1020,700,1850),
            LUMA(1620,700,1850),
            LUMA(1180,700,1850),
            LUMA(1020,700,1850),
            LUMA(1340,700,1850),
            LUMA(1090,700,1850),
            LUMA(1300,700,1850),
            LUMA(1620,700,1850),
            LUMA(1300,700,1850),
            LUMA(1480,700,1850),
        },
        
        {   /* PAL_8565 (taken from Pepto's web page) */
            0.0, 32.0, 10.0, 20.0, 12.0, 16.0,  8.0, 24.0,
            12.0, 8.0, 16.0, 10.0, 15.0, 24.0, 15.0, 20.0
        },
        
        {   /* NTSC_6567 */
            LUMA( 590,590,1825),
            LUMA(1825,590,1825),
            LUMA( 950,590,1825),
            LUMA(1380,590,1825),
            LUMA(1030,590,1825),
            LUMA(1210,590,1825),
            LUMA( 860,590,1825),
            LUMA(1560,590,1825),
            LUMA(1030,590,1825),
            LUMA( 860,590,1825),
            LUMA(1210,590,1825),
            LUMA( 950,590,1825),
            LUMA(1160,590,1825),
            LUMA(1560,590,1825),
            LUMA(1160,590,1825),
            LUMA(1380,590,1825)
        },
        
        {   /* NTSC_6567_R56A (taken from VICE 3.2) */
            LUMA( 560,560,1825),
            LUMA(1825,560,1825),
            LUMA( 840,560,1825),
            LUMA(1500,560,1825),
            LUMA(1180,560,1825),
            LUMA(1180,560,1825),
            LUMA( 840,560,1825),
            LUMA(1500,560,1825),
            LUMA(1180,560,1825),
            LUMA( 840,560,1825),
            LUMA(1180,560,1825),
            LUMA( 840,560,1825),
            LUMA(1180,560,1825),
            LUMA(1500,560,1825),
            LUMA(1180,560,1825),
            LUMA(1500,560,1825),
        },
    
        {   /*  NTSC_8562 (taken from Pepto's web page) */
            0.0, 32.0, 10.0, 20.0, 12.0, 16.0,  8.0, 24.0,
            12.0, 8.0, 16.0, 10.0, 15.0, 24.0, 15.0, 20.0
        }
    };
    
    // Angles in the color plane
    // http://unusedino.de/ec64/technical/misc/vic656x/colors/ and VICE 3.2
    /*
    double angle[16] = {
        NAN,   NAN,   112.5, 292.5, 45.0, 225.0,   0.0, 180.0,
        135.0, 157.5, 112.5, NAN,   NAN,  225.0,   0.0, NAN
    };
    */
    
    // Angles in the color plane (Pepto's web site)
    // http://www.pepto.de/projects/colorvic/
    double angle[16] = {
        NAN,    NAN,    101.25, 281.25, 56.25, 236.25, 348.25, 168.75,
        123.75, 146.25, 101.25, NAN,    NAN,   236.25, 348.75, NAN
    };
    
    //
    // Compute YUV values (adapted from Pepto)
    //
    
    // Constants
    double screen = 1.0 / 5.0;
    
    // Normalize
    double brightness = this->brightness - 50;
    double contrast = this->contrast / 100;
    double saturation = this->saturation * (1 - screen);
    
    // Compute all sixteen colors
    assert(chipModel < 6);
    debug("Chip model = %d\n", chipModel);
    debug("brightness %f contrast %f sat %f\n", brightness, contrast, saturation);
    
    for (unsigned i = 0; i < 16; i++) {
        
        // Compute YUV values
        double ang = isnan(angle[i]) ? NAN : angle[i] * (M_PI / 180.0);
        double y = 8 * luma[chipModel][i] + brightness;
        double u = isnan(ang) ? 0 : cos(ang) * saturation;
        double v = isnan(ang) ? 0 : sin(ang) * saturation;
        u *= contrast + screen;
        v *= contrast + screen;
        
        // Convert YUV to RGB
        double r = y             + 1.140 * v;
        double g = y - 0.396 * u - 0.581 * v;
        double b = y + 2.029 * u;
        
        // Apply Gamma correction for PAL models
        if (isPAL()) {
            r = gammaCorrect(r, 2.8, 2.2);
            g = gammaCorrect(g, 2.8, 2.2);
            b = gammaCorrect(b, 2.8, 2.2);
        }
        
        // Clamp values to avoid jumping colors
        uint8_t r_clamped = MAX(MIN(r, 255.0), 0.0);
        uint8_t g_clamped = MAX(MIN(g, 255.0), 0.0);
        uint8_t b_clamped = MAX(MIN(b, 255.0), 0.0);

        // Store result
        uint32_t rgba = LO_LO_HI_HI(r_clamped, g_clamped, b_clamped, 0xFF);
        pixelEngine.colors[i] = rgba;
    }
}

//! @brief    Sets one of the sixteen C64 colors in RGBA format.
/*
void
VIC::setColor(unsigned nr, uint32_t rgba)
{
    assert(nr < 16);
    pixelEngine.colors[nr] = rgba;
}
*/

//! @brief    Returns the currently used color scheme
ColorScheme
VIC::getColorScheme()
{
    return pixelEngine.colorScheme;
}

void
VIC::setColorScheme(ColorScheme scheme)
{
    // List of predefined color schemes
    uint8_t rgb[][16][3] = {
        
        // DEFAULT (placeholder)
        {
            { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
            { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
            { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 },
            { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }
        },
        
        /* VICE (PAL) (RGB values from VICE 3.1) */
        {
            { 0x00, 0x00, 0x00 },
            { 0xff, 0xff, 0xff },
            { 0xAE, 0x59, 0x3F }, // { 0x91, 0x4a, 0x40 },
            { 0x9C, 0xE9, 0xFC }, // { 0x86, 0xc5, 0xcc },
            { 0xAF, 0x5B, 0xEC }, // { 0x93, 0x4e, 0xb6 },
            { 0x88, 0xD6, 0x3E }, // { 0x73, 0xb2, 0x4b },
            { 0x55, 0x3E, 0xE5 }, // { 0x4a, 0x35, 0xaa },
            { 0xFE, 0xFF, 0x75 }, // { 0xd4, 0xe0, 0x7c },
            { 0xB6, 0x81, 0x19 }, // { 0x98, 0x6a, 0x2d },
            { 0x7A, 0x66, 0x00 }, // { 0x66, 0x53, 0x00 },
            { 0xE7, 0x9A, 0x84 }, // { 0xc0, 0x81, 0x78 },
            { 0x72, 0x72, 0x72 }, // { 0x60, 0x60, 0x60 },
            { 0xA4, 0xA4, 0xA4 }, // { 0x8a, 0x8a, 0x8a },
            { 0xD5, 0xFF, 0x97 }, // { 0xb4, 0xed, 0x91 },
            { 0x9F, 0x8B, 0xFF }, // { 0x87, 0x77, 0xde },
            { 0xD5, 0xD5, 0xD5 }  // { 0xb3, 0xb3, 0xb3 }
        },
        
        /* VICE_NTSC (RGB values from VICE 3.1) */
        {
            { 0x00, 0x00, 0x00 },
            { 0xff, 0xff, 0xff },
            { 0xA2, 0x47, 0x42 },
            { 0x84, 0xDF, 0xE4 },
            { 0x88, 0x5C, 0xB4 },
            { 0x89, 0xB5, 0x5D },
            { 0x25, 0x3F, 0x9B },
            { 0xFF, 0xFD, 0xA2 },
            { 0xB6, 0x65, 0x3A },
            { 0x7B, 0x41, 0x00 },
            { 0xE6, 0x8A, 0x85 },
            { 0x5C, 0x5C, 0x5C },
            { 0x92, 0x92, 0x92 },
            { 0xE2, 0xFF, 0xB6 },
            { 0x71, 0x8C, 0xE7 },
            { 0xCB, 0xCB, 0xCB }
        },
        
        /* CCS64 */
        {
            { 0x10, 0x10, 0x10 },
            { 0xff, 0xff, 0xff },
            { 0xe0, 0x40, 0x40 },
            { 0x60, 0xff, 0xff },
            { 0xe0, 0x60, 0xe0 },
            { 0x40, 0xe0, 0x40 },
            { 0x40, 0x40, 0xe0 },
            { 0xff, 0xff, 0x40 },
            { 0xe0, 0xa0, 0x40 },
            { 0x9c, 0x74, 0x48 },
            { 0xff, 0xa0, 0xa0 },
            { 0x54, 0x54, 0x54 },
            { 0x88, 0x88, 0x88 },
            { 0xa0, 0xff, 0xa0 },
            { 0xa0, 0xa0, 0xff },
            { 0xc0, 0xc0, 0xc0 }
        },
        
        /* FRODO */
        {
            { 0x00, 0x00, 0x00 },
            { 0xff, 0xff, 0xff },
            { 0xcc, 0x00, 0x00 },
            { 0x00, 0xff, 0xcc },
            { 0xff, 0x00, 0xff },
            { 0x00, 0xcc, 0x00 },
            { 0x00, 0x00, 0xcc },
            { 0xff, 0xff, 0x00 },
            { 0xff, 0x88, 0x00 },
            { 0x88, 0x44, 0x00 },
            { 0xff, 0x88, 0x88 },
            { 0x44, 0x44, 0x44 },
            { 0x88, 0x88, 0x88 },
            { 0x88, 0xff, 0x88 },
            { 0x88, 0x88, 0xff },
            { 0xcc, 0xcc, 0xcc }
        },
        
        /* PC64 */
        {
            { 0x21, 0x21, 0x21 },
            { 0xff, 0xff, 0xff },
            { 0xb5, 0x21, 0x21 },
            { 0x73, 0xff, 0xff },
            { 0xb5, 0x21, 0xb5 },
            { 0x21, 0xb5, 0x21 },
            { 0x21, 0x21, 0xb5 },
            { 0xff, 0xff, 0x21 },
            { 0xb5, 0x73, 0x21 },
            { 0x94, 0x42, 0x21 },
            { 0xff, 0x73, 0x73 },
            { 0x73, 0x73, 0x73 },
            { 0x94, 0x94, 0x94 },
            { 0x73, 0xff, 0x73 },
            { 0x73, 0x73, 0xff },
            { 0xb5, 0xb5, 0xb5 }
        },
        
        /* C64S */
        {
            { 0x00, 0x00, 0x00 },
            { 0xfc, 0xfc, 0xfc },
            { 0xa8, 0x00, 0x00 },
            { 0x54, 0xfc, 0xfc },
            { 0xa8, 0x00, 0xa8 },
            { 0x00, 0xa8, 0x00 },
            { 0x00, 0x00, 0xa8 },
            { 0xfc, 0xfc, 0x00 },
            { 0xa8, 0x54, 0x00 },
            { 0x80, 0x2c, 0x00 },
            { 0xfc, 0x54, 0x54 },
            { 0x54, 0x54, 0x54 },
            { 0x80, 0x80, 0x80 },
            { 0x54, 0xfc, 0x54 },
            { 0x54, 0x54, 0xfc },
            { 0xa8, 0xa8, 0xa8 }
        },
        
        /* ALEC64 */
        {
            { 0x00, 0x00, 0x00 },
            { 0xfc, 0xfc, 0xfc },
            { 0x9c, 0x00, 0x00 },
            { 0x00, 0xbc, 0xbc },
            { 0xbc, 0x00, 0xbc },
            { 0x00, 0x9c, 0x00 },
            { 0x00, 0x00, 0x9c },
            { 0xfc, 0xfc, 0x00 },
            { 0xfc, 0x58, 0x00 },
            { 0x78, 0x38, 0x00 },
            { 0xfc, 0x00, 0x00 },
            { 0x3c, 0x3c, 0x3c },
            { 0x7c, 0x7c, 0x7c },
            { 0x00, 0xfc, 0x00 },
            { 0x00, 0x00, 0xfc },
            { 0xbc, 0xbc, 0xbc }
        },
        
        /* WIN64 */
        {
            { 0x00, 0x00, 0x00 },
            { 0xff, 0xff, 0xff },
            { 0x68, 0x00, 0x14 },
            { 0x00, 0xc0, 0xac },
            { 0x94, 0x00, 0x98 },
            { 0x5c, 0x98, 0x5e }, // exchanged with LTGREEN
            { 0x04, 0x10, 0xb0 },
            { 0xfc, 0xfc, 0x00 },
            { 0xf9, 0x9a, 0x1a },
            { 0x50, 0x20, 0x14 },
            { 0xfc, 0x50, 0x80 },
            { 0x46, 0x46, 0x46 },
            { 0x73, 0x73, 0x73 },
            { 0x24, 0xf0, 0x00 }, // exchanged with GREEN
            { 0x5e, 0x70, 0xf2 },
            { 0xac, 0xac, 0xac }
        },
        
        /* C64ALIVE */
        {
            { 0x00, 0x00, 0x00 },
            { 0xfc, 0xfc, 0xfc },
            { 0xb0, 0x00, 0x00 },
            { 0x00, 0xb8, 0xb8 },
            { 0xa0, 0x00, 0xa0 },
            { 0x00, 0xbc, 0x00 },
            { 0x00, 0x00, 0xa0 },
            { 0xf8, 0xfc, 0x50 },
            { 0xcc, 0x64, 0x00 },
            { 0x98, 0x4c, 0x28 },
            { 0xf4, 0x88, 0x90 },
            { 0x58, 0x58, 0x58 },
            { 0x94, 0x94, 0x94 },
            { 0x68, 0xfc, 0x80 },
            { 0x68, 0x80, 0xf8 },
            { 0xd8, 0xd8, 0xd8 }
        },
        
        /* GODOT */
        {
            { 0x00, 0x00, 0x00 },
            { 0xff, 0xff, 0xff },
            { 0x88, 0x00, 0x00 },
            { 0xaa, 0xff, 0xee },
            { 0xcc, 0x44, 0xcc },
            { 0x00, 0xcc, 0x55 },
            { 0x00, 0x00, 0xaa },
            { 0xee, 0xee, 0x77 },
            { 0xdd, 0x88, 0x55 },
            { 0x66, 0x44, 0x00 },
            { 0xfe, 0x77, 0x77 },
            { 0x33, 0x33, 0x33 },
            { 0x77, 0x77, 0x77 },
            { 0xaa, 0xff, 0x66 },
            { 0x00, 0x88, 0xff },
            { 0xbb, 0xbb, 0xbb }
        },
        
        /* C64SALLY */
        {
            { 0x00, 0x00, 0x00 },
            { 0xfc, 0xfc, 0xfc },
            { 0xc8, 0x00, 0x00 },
            { 0x00, 0xfc, 0xfc },
            { 0xfc, 0x00, 0xfc },
            { 0x00, 0xc8, 0x00 },
            { 0x00, 0x00, 0xc8 },
            { 0xfc, 0xfc, 0x00 },
            { 0xfc, 0x64, 0x00 },
            { 0xc0, 0x64, 0x00 },
            { 0xfc, 0x64, 0x64 },
            { 0x40, 0x40, 0x40 },
            { 0x80, 0x80, 0x80 },
            { 0x64, 0xfc, 0x64 },
            { 0x64, 0x64, 0xf0 },
            { 0xc0, 0xc0, 0xc0 }
        },
        
        /* PEPTO */
        {
            { 0x00, 0x00, 0x00 },
            { 0xff, 0xff, 0xff },
            { 0x68, 0x37, 0x2b },
            { 0x70, 0xa4, 0xb2 },
            { 0x6f, 0x3d, 0x86 },
            { 0x58, 0x8d, 0x43 },
            { 0x35, 0x28, 0x79 },
            { 0xb8, 0xc7, 0x6f },
            { 0x6f, 0x4f, 0x25 },
            { 0x43, 0x39, 0x00 },
            { 0x9A, 0x67, 0x59 },
            { 0x44, 0x44, 0x44 },
            { 0x6c, 0x6c, 0x6c },
            { 0x9a, 0xd2, 0x84 },
            { 0x6c, 0x5e, 0xb5 },
            { 0x95, 0x95, 0x95 }
        }
    };
    
    unsigned numSchemes = sizeof(rgb) / (16*3);
    debug(2, "Using color scheme %ld from %ld available schemes\n", scheme, numSchemes);

    // Check if value is out of bounds
    if (scheme >= numSchemes)
        scheme = CS_DEFAULT;
    
    // Remember scheme
    pixelEngine.colorScheme = scheme;
    
    // Resolve the default color scheme (VICII model dependent)
    if (scheme == CS_DEFAULT)
        scheme = isPAL() ? CS_VICE_PAL : CS_VICE_NTSC;

    // Set colors
    for (unsigned i = 0; i < 16; i++) {
        int rgba = LO_LO_HI_HI(rgb[scheme][i][0],
                               rgb[scheme][i][1],
                               rgb[scheme][i][2],
                               0xFF);
        pixelEngine.colors[i] = rgba;  // setColor(i, rgba);
    }
    
    
    // NEW APPROACH
    updatePalette();
}
