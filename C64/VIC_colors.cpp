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

