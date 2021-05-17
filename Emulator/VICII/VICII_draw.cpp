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
VICII::draw()
{
    drawCanvas();
    drawBorder();
}

void
VICII::draw17()
{
    drawCanvas();
    drawBorder17();
}

void
VICII::draw55()
{
    drawCanvas();
    drawBorder55();
}

void
VICII::drawBorder()
{
    if (flipflops.delayed.main) {
        
        SET_FRAME_PIXEL(0, reg.delayed.colors[COLREG_BORDER]);

        for (unsigned pixel = 1; pixel <= 7; pixel++) {
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
        for (unsigned pixel = 1; pixel <= 6; pixel++) {
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
    u8 d011, d016, newD016, mode, xscroll;
    
    // Check if we can take the fast path (VICII registers are stable)
    if (reg.delayed.ctrl1 != reg.current.ctrl1 ||
        reg.delayed.ctrl1 != reg.current.ctrl1 ||
        reg.delayed.colors[COLREG_BG0] != reg.current.colors[COLREG_BG0] ||
        reg.delayed.colors[COLREG_BG1] != reg.current.colors[COLREG_BG1] ||
        reg.delayed.colors[COLREG_BG2] != reg.current.colors[COLREG_BG2] ||
        reg.delayed.colors[COLREG_BG3] != reg.current.colors[COLREG_BG3])
    {
        assert((delay & VICUpdateRegisters) != 0);
        stats.slowPath++;
    } else {
        stats.fastPath++;
    }

    /* "The sequencer outputs the graphics data in every raster line in the area
     *  of the display column as long as the vertical border flip-flop is reset
     *  (see section 3.9.)." [C.B.]
     */
    
    if (flipflops.delayed.vertical) {
        
        /* "Outside of the display column and if the flip-flop is set, the last
         *  current background color is displayed (this area is normally covered
         *  by the border)." [C.B.]
         */
        /*
        SET_BACKGROUND_PIXEL(0, col[0]);
        for (unsigned pixel = 1; pixel < 8; pixel++) {
            SET_BACKGROUND_PIXEL(pixel, col[0]);
        }
        return;
        */
    }
    
    /* "The graphics data sequencer is capable of 8 different graphics modes
     *  that are selected by the bits ECM, BMM and MCM (Extended Color Mode,
     *  Bit Map Mode and Multi Color Mode) in the registers $d011 and
     *  $d016." [C.B.]
     */
    
    d011 = reg.delayed.ctrl1;
    d016 = reg.delayed.ctrl2;
    xscroll = d016 & 0x07;
    mode = (d011 & 0x60) | (d016 & 0x10); // -xxx ----

    drawCanvasPixel(0, mode, d016, xscroll == 0);
    
    // After the first pixel, color register changes show up
    reg.delayed.colors[COLREG_BG0] = reg.current.colors[COLREG_BG0];
    reg.delayed.colors[COLREG_BG1] = reg.current.colors[COLREG_BG1];
    reg.delayed.colors[COLREG_BG2] = reg.current.colors[COLREG_BG2];
    reg.delayed.colors[COLREG_BG3] = reg.current.colors[COLREG_BG3];

    drawCanvasPixel(1, mode, d016, xscroll == 1);
    drawCanvasPixel(2, mode, d016, xscroll == 2);
    drawCanvasPixel(3, mode, d016, xscroll == 3);

    // After pixel 4, a change in D016 affects the display mode.
    newD016 = reg.current.ctrl2;

    // In older VICIIs, the one bits of D011 show up, too.
    if (is656x()) {
        d011 |= reg.current.ctrl1;
    }
    mode = (d011 & 0x60) | (newD016 & 0x10);
    
    drawCanvasPixel(4, mode, d016, xscroll == 4);
    drawCanvasPixel(5, mode, d016, xscroll == 5);
    
    // In older VICIIs, the zero bits of D011 show up here.
    if (is656x()) {
        d011 = reg.current.ctrl1;
        mode = (d011 & 0x60) | (newD016 & 0x10);
    }

    drawCanvasPixel(6, mode, d016, xscroll == 6);
    
    // Before the last pixel is drawn, a change in D016 is fully detected.
    // If the multicolor bit gets set, the mc flip flop is also reset.
    if (d016 != newD016) {
        if (RISING_EDGE(d016 & 0x10, newD016 & 0x10))
            sr.mcFlop = false;
        d016 = newD016;
    }
 
    drawCanvasPixel(7, mode, d016, xscroll == 7);
}

void
VICII::drawCanvasPixel(u8 pixel,
                       u8 mode,
                       u8 d016,
                       bool loadShiftReg)
{
    assert(pixel < 8);
    
    /* "The heart of the sequencer is an 8 bit shift register that is shifted by
     *  1 bit every pixel and reloaded with new graphics data after every
     *  g-access. With XSCROLL from register $d016 the reloading can be delayed
     *  by 0-7 pixels, thus shifting the display up to 7 pixels to the right."
     */
    if (loadShiftReg && !flipflops.delayed.vertical && sr.canLoad) {
        
        u32 result = gAccessResult.delayed();
     
        // Load shift register
        sr.data = BYTE0(result);
        
        // Remember how to synthesize pixels
        sr.latchedCharacter = BYTE2(result);
        sr.latchedColor = BYTE1(result);
        
        // Reset the multicolor synchronization flipflop
        sr.mcFlop = true;
    }
        
    // Determine the render mode and the drawing mode for this pixel
    u8 mcBit = sr.latchedColor & 0x8;
    bool multicolorDisplayMode = (mode & 0x10) && ((mode & 0x20) || mcBit);
    bool generateMulticolorPixel = (d016 & 0x10) && ((mode & 0x20) || mcBit);
    
    // Determine the colorbits
    if (generateMulticolorPixel) {
        if (sr.mcFlop) {
            sr.colorbits = (sr.data >> 6) >> !multicolorDisplayMode;
        }
    } else {
        sr.colorbits = (sr.data >> 7) << multicolorDisplayMode;
    }
        
    // Draw pixel
    assert(sr.colorbits < 4);

    // Lookup how the color should be synthesized
    assert(((mode | mcBit) >> 1 | sr.colorbits) < 64);
    ColorSource source = colSrcTable[(mode | mcBit) >> 1 | sr.colorbits];
    
    u8 color;
    switch (source) {
            
        case COLSRC_D021:
            color = reg.delayed.colors[COLREG_BG0];
            break;
            
        case COLSRC_D022:
            color = reg.delayed.colors[COLREG_BG1];
            break;
            
        case COLSRC_D023:
            color = reg.delayed.colors[COLREG_BG2];
            break;

        case COLSRC_CHAR_LO:
            color = LO_NIBBLE(sr.latchedCharacter);
            break;

        case COLSRC_CHAR_HI:
            color = HI_NIBBLE(sr.latchedCharacter);
            break;

        case COLSRC_COLRAM3:
            color = sr.latchedColor & 0x07;
            break;
            
        case COLSRC_COLRAM4:
            color = sr.latchedColor;
            break;
            
        case COLSRC_INDEXED:
            color = reg.delayed.colors[COLREG_BG0 + (sr.latchedCharacter >> 6)];
            break;
            
        default:
            color = 0;
            break;
    }
    
    // Determine pixel depth
    bool foreground = multicolorDisplayMode ? (sr.colorbits & 0x2) : sr.colorbits;

    // Set pixel
    if (foreground) {
        SET_FOREGROUND_PIXEL(pixel, color);
    } else {
        SET_BACKGROUND_PIXEL(pixel, color);
    }
    
    // Shift register and toggle multicolor flipflop
    sr.data <<= 1;
    sr.mcFlop = !sr.mcFlop;
}

void
VICII::cutLayers()
{
    for (int i = 0; i < TEX_WIDTH; i++) {
        
        bool cut;

        switch (zBuffer[i]) {

            case BORDER_LAYER_DEPTH:
                cut = config.cutLayers & 0x800;
                break;
                                
            case FOREGROUND_LAYER_DEPTH:
                cut = config.cutLayers & 0x400;
                break;
                
            case BACKGROUD_LAYER_DEPTH:
                cut = config.cutLayers & 0x200;
                break;
                
            default:
                cut = pixelSource[i] & config.cutLayers & 0xFF;
                if (!(config.cutLayers & 0x100)) cut = false;
                break;
        }
        
        if (cut) {
            
            u8 r = emuTexturePtr[i] & 0xFF;
            u8 g = (emuTexturePtr[i] >> 8) & 0xFF;
            u8 b = (emuTexturePtr[i] >> 16) & 0xFF;

            double scale = config.cutOpacity / 255.0;
            u8 bg = (rasterline() / 4) % 2 == (i / 4) % 2 ? 0x22 : 0x44;
            u8 newr = (u8)(r * (1 - scale) + bg * scale);
            u8 newg = (u8)(g * (1 - scale) + bg * scale);
            u8 newb = (u8)(b * (1 - scale) + bg * scale);
            
            emuTexturePtr[i] = 0xFF000000 | newb << 16 | newg << 8 | newr;
        }
    }
}
