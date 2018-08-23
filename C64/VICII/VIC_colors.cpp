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

// This implementation is mainly based on the following articles by pepto:
// http://www.pepto.de/projects/colorvic/
// http://unusedino.de/ec64/technical/misc/vic656x/colors/

#include "VIC.h"

double gammaCorrect(double value, double source, double target)
{
    // Reverse gamma correction of source
    double factor = pow(255.0, 1.0 - source);
    value = MIN(MAX(factor * pow(value, source), 0), 255);
    
    // Correct gamma for target
    factor = pow(255.0, 1.0 - (1.0 / target));
    value = MIN(MAX(factor * pow(value, 1 / target), 0), 255);
    
    return round(value);
}

uint32_t
VIC::getColor(unsigned nr)
{
    assert(nr < 16);
    return pixelEngine.rgbaTable[nr];
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
    #define LUMA_VICE(x,y,z) ((double)(x - y) * 256)/((double)(z - y))
    #define LUMA_COLORES(x) (x * 7.96875)
    
    double luma_vice_6569_r1[16] = { /* taken from VICE 3.2 */
        LUMA_VICE( 630,630,1850), LUMA_VICE(1850,630,1850),
        LUMA_VICE( 900,630,1850), LUMA_VICE(1560,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE(1260,630,1850),
        LUMA_VICE( 900,630,1850), LUMA_VICE(1560,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE( 900,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE( 900,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE(1560,630,1850),
        LUMA_VICE(1260,630,1850), LUMA_VICE(1560,630,1850)
    };
    
    double luma_vice_6569_r3[16] = { /* taken from VICE 3.2 */
        LUMA_VICE( 700,700,1850), LUMA_VICE(1850,700,1850),
        LUMA_VICE(1090,700,1850), LUMA_VICE(1480,700,1850),
        LUMA_VICE(1180,700,1850), LUMA_VICE(1340,700,1850),
        LUMA_VICE(1020,700,1850), LUMA_VICE(1620,700,1850),
        LUMA_VICE(1180,700,1850), LUMA_VICE(1020,700,1850),
        LUMA_VICE(1340,700,1850), LUMA_VICE(1090,700,1850),
        LUMA_VICE(1300,700,1850), LUMA_VICE(1620,700,1850),
        LUMA_VICE(1300,700,1850), LUMA_VICE(1480,700,1850),
    };
    
    double luma_vice_6567[16] = { /* taken from VICE 3.2 */
        LUMA_VICE( 590,590,1825), LUMA_VICE(1825,590,1825),
        LUMA_VICE( 950,590,1825), LUMA_VICE(1380,590,1825),
        LUMA_VICE(1030,590,1825), LUMA_VICE(1210,590,1825),
        LUMA_VICE( 860,590,1825), LUMA_VICE(1560,590,1825),
        LUMA_VICE(1030,590,1825), LUMA_VICE( 860,590,1825),
        LUMA_VICE(1210,590,1825), LUMA_VICE( 950,590,1825),
        LUMA_VICE(1160,590,1825), LUMA_VICE(1560,590,1825),
        LUMA_VICE(1160,590,1825), LUMA_VICE(1380,590,1825)
    };
    
    double luma_vice_6567_r65a[16] = { /* taken from VICE 3.2 */
        LUMA_VICE( 560,560,1825), LUMA_VICE(1825,560,1825),
        LUMA_VICE( 840,560,1825), LUMA_VICE(1500,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE(1180,560,1825),
        LUMA_VICE( 840,560,1825), LUMA_VICE(1500,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE( 840,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE( 840,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE(1500,560,1825),
        LUMA_VICE(1180,560,1825), LUMA_VICE(1500,560,1825),
    };
    
    double luma_pepto[16] = { /* taken from Pepto's Colodore palette */
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
    switch(chipModel) {
        case PAL_6569_R1:
            luma = luma_vice_6569_r1;
            break;
        case PAL_6569_R3:
            luma = luma_vice_6569_r3;
            break;
        case NTSC_6567:
            luma = luma_vice_6567;
            break;
        case NTSC_6567_R56A:
            luma = luma_vice_6567_r65a;
            break;
        case PAL_8565:
        case NTSC_8562:
            luma = luma_pepto;
            break;
        default:
            assert(false);
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
    double brightness = this->brightness - 50.0;
    double contrast = this->contrast / 100.0 + 0.2;
    double saturation = this->saturation / 1.25;
    
    // debug("bri = %f con = %f sat = %f\n", brightness, contrast, saturation);
    
    // Compute all sixteen colors
    for (unsigned i = 0; i < 16; i++) {
        
        // Compute YUV values
        double ang = angle[i];
        double y = luma[i];
        double u = isnan(ang) ? 0 : cos(ang) * saturation;
        double v = isnan(ang) ? 0 : sin(ang) * saturation;
        
        // Apply brightness and contrast
        y *= contrast;
        u *= contrast;
        v *= contrast;
        y += brightness;
        // debug("%d: angle = %f y = %f u = %f v = %f\n", i, angle[i], y, u, v);

        // Convert YUV to RGB
        double r = y             + 1.140 * v;
        double g = y - 0.396 * u - 0.581 * v;
        double b = y + 2.029 * u;
        r = MAX(MIN(r, 255), 0);
        g = MAX(MIN(g, 255), 0);
        b = MAX(MIN(b, 255), 0);
        // debug("%d: r = %f g = %f b = %f\n", i, r, g, b);
        
        // Apply Gamma correction for PAL models
        if (isPAL()) {
            r = gammaCorrect(r, 2.8, 2.2);
            g = gammaCorrect(g, 2.8, 2.2);
            b = gammaCorrect(b, 2.8, 2.2);
        }
        
        // Clamp values to avoid jumping colors
        
        // debug("%d: r = %f g = %f b = %f\n", i, r, g, b);
        // debug("%d: R = %d G = %d B = %d\n", i, (int)r, (int)g, (int)b);
        
        // Store result
        uint32_t rgba = LO_LO_HI_HI((uint8_t)r, (uint8_t)g, (uint8_t)b, 0xFF);
        pixelEngine.rgbaTable[i] = rgba;
    }
}

