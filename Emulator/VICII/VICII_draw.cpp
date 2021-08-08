// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"
#include "C64.h"

void
VICII::drawBorder()
{
    if (flipflops.delayed.main) {
        
        SET_FRAME_PIXEL(0, reg.delayed.colors[COLREG_BORDER]);

        for (isize pixel = 1; pixel <= 7; pixel++) {
            SET_FRAME_PIXEL(pixel, reg.current.colors[COLREG_BORDER]);
        }
    }
}

void
VICII::drawBorder17()
{
    if (flipflops.delayed.main && !flipflops.current.main) {
        
        // 38 column mode (only pixels 0...6 are drawn)
        SET_FRAME_PIXEL(0, reg.delayed.colors[COLREG_BORDER]);
        for (isize pixel = 1; pixel <= 6; pixel++) {
            SET_FRAME_PIXEL(pixel, reg.current.colors[COLREG_BORDER]);
        }
        
    } else {

        // 40 column mode (all eight pixels are drawn)
        drawBorder();
    }
}

void
VICII::drawBorder55()
{
    if (!flipflops.delayed.main && flipflops.current.main) {
        
        // 38 column mode (border starts at pixel 7)
        SET_FRAME_PIXEL(7, reg.delayed.colors[COLREG_BORDER]);
  
    } else {
        
        drawBorder();
    }
}

void
VICII::drawCanvas()
{
    if ((delay & VICUpdateRegisters) || VIC_SAFE_MODE == 1) {
        drawCanvasSlowPath();
    } else {
        drawCanvasFastPath();
    }
}

void
VICII::drawCanvasFastPath()
{
    if (VIC_STATS) stats.canvasFastPath++;
            
    u8 xscroll = reg.delayed.xscroll;
    
    switch (reg.delayed.mode) {
    
        case DISPLAY_MODE_STANDARD_TEXT:

            for (isize i = 0; i < 8; i++) {
                
                if (i == xscroll) loadShiftRegister();
                sr.colorbits = (sr.data >> 7);
                
                if (sr.colorbits) {
                    SET_FG_PIXEL(i, sr.latchedCol);
                } else {
                    SET_BG_PIXEL(i, reg.delayed.colors[COLREG_BG0]);
                }
                
                sr.data <<= 1;
                sr.mcFlop = !sr.mcFlop;
            }
            return;
            
        case DISPLAY_MODE_MULTICOLOR_TEXT:
            
            for (isize i = 0; i < 8; i++) {
                
                if (i == xscroll) loadShiftRegister();
                bool mc = sr.latchedCol & 0x8;
                
                if (mc) {
                    
                    if (sr.mcFlop) sr.colorbits = sr.data >> 6;
                    
                    switch (sr.colorbits) {
                        case 0: SET_BG_PIXEL(i, reg.delayed.colors[COLREG_BG0]); break;
                        case 1: SET_BG_PIXEL(i, reg.delayed.colors[COLREG_BG1]); break;
                        case 2: SET_FG_PIXEL(i, reg.delayed.colors[COLREG_BG2]); break;
                        case 3: SET_FG_PIXEL(i, sr.latchedCol & 0x07); break;
                    }
                    
                } else {
                    
                    sr.colorbits = (sr.data >> 7);
                    
                    if (sr.colorbits) {
                        SET_FG_PIXEL(i, sr.latchedCol);
                    } else {
                        SET_BG_PIXEL(i, reg.delayed.colors[COLREG_BG0]);
                    }
                }
                
                sr.data <<= 1;
                sr.mcFlop = !sr.mcFlop;
            }
            return;

        case DISPLAY_MODE_STANDARD_BITMAP:
            
            for (isize i = 0; i < 8; i++) {
                
                if (i == xscroll) loadShiftRegister();
                sr.colorbits = (sr.data >> 7);
                
                if (sr.colorbits) {
                    SET_FG_PIXEL(i, HI_NIBBLE(sr.latchedChr));
                } else {
                    SET_BG_PIXEL(i, LO_NIBBLE(sr.latchedChr));
                }

                sr.data <<= 1;
                sr.mcFlop = !sr.mcFlop;
            }
            return;

        case DISPLAY_MODE_MULTICOLOR_BITMAP:
            
            for (isize i = 0; i < 8; i++) {
                
                if (i == xscroll) loadShiftRegister();
                if (sr.mcFlop) sr.colorbits = sr.data >> 6;
                
                switch (sr.colorbits) {
                    case 0: SET_BG_PIXEL(i, reg.delayed.colors[COLREG_BG0]); break;
                    case 1: SET_BG_PIXEL(i, HI_NIBBLE(sr.latchedChr)); break;
                    case 2: SET_FG_PIXEL(i, LO_NIBBLE(sr.latchedChr)); break;
                    case 3: SET_FG_PIXEL(i, sr.latchedCol); break;
                }
                    
                sr.data <<= 1;
                sr.mcFlop = !sr.mcFlop;
            }
            return;

        case DISPLAY_MODE_EXTENDED_BG_COLOR:
            
            for (isize i = 0; i < 8; i++) {
                
                if (i == xscroll) loadShiftRegister();
                sr.colorbits = (sr.data >> 7);
                
                if (sr.colorbits) {
                    SET_FG_PIXEL(i, sr.latchedCol);
                } else {
                    isize regnr = COLREG_BG0 + (sr.latchedChr >> 6);
                    SET_BG_PIXEL(i, reg.delayed.colors[regnr]);
                }

                sr.data <<= 1;
                sr.mcFlop = !sr.mcFlop;
            }
            return;

        default:

            // Invalid color modes (no speedup necessary)
            drawCanvasSlowPath();
            break;
    }
}

void
VICII::drawCanvasSlowPath()
{
    if (VIC_STATS) stats.canvasSlowPath++;

    /* "The graphics data sequencer is capable of 8 different graphics modes
     *  that are selected by the bits ECM, BMM and MCM (Extended Color Mode,
     *  Bit Map Mode and Multi Color Mode) in the registers $d011 and
     *  $d016." [C.B.]
     */
    
    u8 d011 = reg.delayed.ctrl1;
    u8 d016 = reg.delayed.ctrl2;
    u8 mode = (d011 & 0x60) | (d016 & 0x10);
        
    //
    // Pixel 0
    //
    
    drawCanvasPixel(0, mode, d016);
    
    // After the first pixel, color register changes show up
    reg.delayed.colors[COLREG_BG0] = reg.current.colors[COLREG_BG0];
    reg.delayed.colors[COLREG_BG1] = reg.current.colors[COLREG_BG1];
    reg.delayed.colors[COLREG_BG2] = reg.current.colors[COLREG_BG2];
    reg.delayed.colors[COLREG_BG3] = reg.current.colors[COLREG_BG3];

    //
    // Pixel 1, 2, 3
    //

    drawCanvasPixel(1, mode, d016);
    drawCanvasPixel(2, mode, d016);
    drawCanvasPixel(3, mode, d016);

    /* After pixel 4, a change in D016 affects the display mode. In older
     * VICIIs, the one bits of D011 show up, too.
     */
    u8 newD016 = reg.current.ctrl2;
    if (is656x) d011 |= reg.current.ctrl1;
    mode = (d011 & 0x60) | (newD016 & 0x10);
    
    //
    // Pixel 4, 5
    //

    drawCanvasPixel(4, mode, d016);
    drawCanvasPixel(5, mode, d016);
    
    // In older VICIIs, the zero bits of D011 show up here.
    if (is656x) {
        d011 = reg.current.ctrl1;
        mode = (d011 & 0x60) | (newD016 & 0x10);
    }

    //
    // Pixel 6
    //

    drawCanvasPixel(6, mode, d016);
    
    /* Before the last pixel is drawn, a change in D016 is fully detected.
     * If the multicolor bit is set, the mc flip flop resets immediately.
     */
    if (d016 != newD016) {
        if (RISING_EDGE(d016 & 0x10, newD016 & 0x10)) sr.mcFlop = false;
        d016 = newD016;
    }
 
    //
    // Pixel 7
    //

    drawCanvasPixel(7, mode, d016);
}

void
VICII::drawCanvasPixel(u8 pixel, u8 mode, u8 d016)
{
    /* "The heart of the sequencer is a 8 bit shift register that is shifted
     *  by 1 bit every pixel and reloaded with new graphics data after every
     *  g-access. With XSCROLL from register $d016 the reloading can be delayed
     *  by 0-7 pixels, thus shifting the display up to 7 pixels to the right."
     */

    // Load shift register if condition holds
    if (pixel == reg.delayed.xscroll) loadShiftRegister();
                
    // Determine the render mode and the drawing mode for this pixel
    u8 mcBit = sr.latchedCol & 0x8;
    bool multicolorDisplayMode = (mode & 0x10) && ((mode & 0x20) || mcBit);
    bool generateMulticolorPixel = (d016 & 0x10) && ((mode & 0x20) || mcBit);
    
    // Run the shift register
    if (generateMulticolorPixel) {

        // Only update every other cycle
        if (sr.mcFlop) sr.colorbits = (sr.data >> 6) >> !multicolorDisplayMode;
        
    } else {
        
        // Update every cycle
        sr.colorbits = (sr.data >> 7) << multicolorDisplayMode;
    }
    sr.data <<= 1;
    sr.mcFlop = !sr.mcFlop;

    
    // Synthesize color
    u8 color;
    switch ((mode | mcBit) >> 1 | sr.colorbits) {
            
        // DISPLAY_MODE_STANDARD_TEXT
        case 0x00: case 0x04: color = reg.delayed.colors[COLREG_BG0]; break;
        case 0x01: case 0x05: color = sr.latchedCol; break;
        case 0x02: case 0x06: color = reg.delayed.colors[COLREG_BG0];  break;
        case 0x03: case 0x07: color = sr.latchedCol; break;
            
        // DISPLAY_MODE_MULTICOLOR_TEXT (MC = 0)
        case 0x08: color = reg.delayed.colors[COLREG_BG0]; break;
        case 0x09: color = sr.latchedCol; break;
        case 0x0A: color = reg.delayed.colors[COLREG_BG0]; break;
        case 0x0B: color = sr.latchedCol; break;
            
        // DISPLAY_MODE_MULTICOLOR_TEXT (MC = 1)
        case 0x0C: color = reg.delayed.colors[COLREG_BG0]; break;
        case 0x0D: color = reg.delayed.colors[COLREG_BG1]; break;
        case 0x0E: color = reg.delayed.colors[COLREG_BG2]; break;
        case 0x0F: color = sr.latchedCol & 0x07; break;
            
        // DISPLAY_MODE_STANDARD_BITMAP
        case 0x10: case 0x14: color = LO_NIBBLE(sr.latchedChr); break;
        case 0x11: case 0x15: color = HI_NIBBLE(sr.latchedChr); break;
        case 0x12: case 0x16: color = LO_NIBBLE(sr.latchedChr); break;
        case 0x13: case 0x17: color = HI_NIBBLE(sr.latchedChr); break;
            
        // DISPLAY_MODE_MULTICOLOR_BITMAP
        case 0x18: case 0x1C: color = reg.delayed.colors[COLREG_BG0]; break;
        case 0x19: case 0x1D: color = HI_NIBBLE(sr.latchedChr); break;
        case 0x1A: case 0x1E: color = LO_NIBBLE(sr.latchedChr); break;
        case 0x1B: case 0x1F: color = sr.latchedCol; break;
            
        // DISPLAY_MODE_EXTENDED_BG_COLOR
        case 0x20: case 0x24: color = reg.delayed.colors[COLREG_BG0 + (sr.latchedChr >> 6)]; break;
        case 0x21: case 0x25: color = sr.latchedCol; break;
        case 0x22: case 0x26: color = reg.delayed.colors[COLREG_BG0 + (sr.latchedChr >> 6)]; break;
        case 0x23: case 0x27: color = sr.latchedCol; break;
            
        // INVALID VIDEO MODES
        default: color = 0;
    }
        
    // Determine pixel depth
    bool foreground = multicolorDisplayMode ? (sr.colorbits & 0x2) : sr.colorbits;

    // Set pixel
    if (foreground) {
        SET_FG_PIXEL(pixel, color);
    } else {
        SET_BG_PIXEL(pixel, color);
    }
}

void
VICII::loadShiftRegister()
{
    if (!flipflops.delayed.vertical && sr.canLoad) {

        u32 gAccess = gAccessResult.delayed();

        sr.data = BYTE0(gAccess);
        sr.latchedChr = BYTE2(gAccess);
        sr.latchedCol = BYTE1(gAccess);
        sr.mcFlop = true;
    }
}
