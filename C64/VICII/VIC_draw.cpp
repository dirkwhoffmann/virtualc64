//
//  PixelEngine.cpp
/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
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

#include "C64.h"

void
VIC::draw()
{
    if (vblank) {
        drawSprites();
    } else {
        drawCanvas();
        drawBorder();
        drawSprites();
        copyPixels();
    }
}

void
VIC::draw17()
{
    if (vblank) {
        drawSprites();
    } else {
        drawCanvas();
        drawSprites();
        drawBorder17();
        copyPixels();
    }
}

void
VIC::draw55()
{
    if (vblank) {
        drawSprites();
    } else {
        drawCanvas();
        drawSprites();
        drawBorder55();
        copyPixels();
    }
}

void
VIC::drawOutsideBorder()
{
    // if (!vblank) drawSprites();
    drawSprites();
}

void
VIC::drawBorder()
{
    if (flipflops.delayed.main) {
        SET_FRAME_PIXEL(0, reg.delayed.colors[COLREG_BORDER]);
        for (unsigned pixel = 1; pixel <= 7; pixel++) {
            SET_FRAME_PIXEL(pixel, reg.current.colors[COLREG_BORDER]);
        }
    }
}

void
VIC::drawBorder17()
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
VIC::drawBorder55()
{
    if (!flipflops.delayed.main && flipflops.current.main) {
        
        // 38 column mode (border starts at pixel 7)
        SET_FRAME_PIXEL(7, reg.delayed.colors[COLREG_BORDER]);
  
    } else {
        
        drawBorder();
    }
}

void
VIC::drawCanvas()
{
    uint8_t d011, d016, newD016, mode, oldMode, xscroll;
    
    /* "The sequencer outputs the graphics data in every raster line in the area
     *  of the display column as long as the vertical border flip-flop is reset
     *  (see section 3.9.)." [C.B.]
     */
    
    if (flipflops.delayed.vertical) {
        
        /* "Outside of the display column and if the flip-flop is set, the last
         *  current background color is displayed (this area is normally covered
         *  by the border)." [C.B.]
         */
        // TODO: This is wrong, border-bm-idle test fails
        SET_FRAME_PIXEL(0, reg.delayed.colors[COLREG_BG0]);
        for (unsigned pixel = 1; pixel < 8; pixel++) {
            SET_FRAME_PIXEL(pixel, reg.current.colors[COLREG_BG0]);
        }
        return;
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

    drawCanvasPixel(0, mode, d016, xscroll == 0, true);
    
    // After the first pixel, color register changes show up
    reg.delayed.colors[COLREG_BG0] = reg.current.colors[COLREG_BG0];
    reg.delayed.colors[COLREG_BG1] = reg.current.colors[COLREG_BG1];
    reg.delayed.colors[COLREG_BG2] = reg.current.colors[COLREG_BG2];
    reg.delayed.colors[COLREG_BG3] = reg.current.colors[COLREG_BG3];

    drawCanvasPixel(1, mode, d016, xscroll == 1, true);
    drawCanvasPixel(2, mode, d016, xscroll == 2, false);
    drawCanvasPixel(3, mode, d016, xscroll == 3, false);

    // After pixel 4, a change in D016 affects the display mode.
    newD016 = reg.current.ctrl2;

    // In newer VICIIs, the one bits of D011 show up, too.
    if (!is856x()) {
        d011 |= reg.current.ctrl1;
    }
    oldMode = mode;
    mode = (d011 & 0x60) | (newD016 & 0x10);
    
    drawCanvasPixel(4, mode, d016, xscroll == 4, oldMode != mode);
    drawCanvasPixel(5, mode, d016, xscroll == 5, false);
    
    // In newer VICIIs, the zero bits of D011 show up here.
    if (!is856x()) {
        d011 = reg.current.ctrl1;
        oldMode = mode;
        mode = (d011 & 0x60) | (newD016 & 0x10);
    }

    drawCanvasPixel(6, mode, d016, xscroll == 6, oldMode != mode);
    
    // Before the last pixel is drawn, a change is D016 is fully detected.
    // If the multicolor bit get set, the mc flip flop is also reset.
    if (d016 != newD016) {
        if (RISING_EDGE(d016 & 0x10, newD016 & 0x10))
            sr.mcFlop = false;
        d016 = newD016;
    }
 
    drawCanvasPixel(7, mode, d016, xscroll == 7, false);
}


void
VIC::drawCanvasPixel(uint8_t pixelNr,
                             uint8_t mode,
                             uint8_t d016,
                             bool loadShiftReg,
                             bool updateColors)
{
    assert(pixelNr < 8);
    
    /* "The heart of the sequencer is an 8 bit shift register that is shifted by
     *  1 bit every pixel and reloaded with new graphics data after every
     *  g-access. With XSCROLL from register $d016 the reloading can be delayed
     *  by 0-7 pixels, thus shifting the display up to 7 pixels to the right."
     */
    if (loadShiftReg && sr.canLoad) {
        
        uint32_t result = gAccessResult.delayed();
     
        // Load shift register
        sr.data = BYTE0(result);
        
        // Remember how to synthesize pixels
        sr.latchedCharacter = BYTE2(result);
        sr.latchedColor = BYTE1(result);
        
        // Reset the multicolor synchronization flipflop
        sr.mcFlop = true;
        
        // Make sure that colors get updated
        updateColors = true;
        
        sr.remainingBits = 8;
    }
    
    // Clear any outstanding multicolor bit that shouldn't actually be drawn
    // TODO: VICE doesn't use a counter for this, but doesn't have the same issue,
    // figure out what magic they are doing
    if (!sr.remainingBits) {
        sr.colorbits = 0;
    }
    
    // Determine the render mode and the drawing mode for this pixel
    bool multicolorDisplayMode =
    (mode & 0x10) && ((mode & 0x20) || (sr.latchedColor & 0x8));
    
    bool generateMulticolorPixel =
    (d016 & 0x10) && ((mode & 0x20) || (sr.latchedColor & 0x8));
    
    // Determine the colorbits
    if (generateMulticolorPixel) {
        if (sr.mcFlop) {
            sr.colorbits = (sr.data >> 6) >> !multicolorDisplayMode;
        }
    } else {
        sr.colorbits = (sr.data >> 7) << multicolorDisplayMode;
    }
    
    // Load colors
    if (updateColors) loadColors(mode);
    
    // Draw pixel
    assert(sr.colorbits < 4);
    if (multicolorDisplayMode) {
        
        // Set multi-color pixel
        if (sr.colorbits & 0x02) {
            SET_FOREGROUND_PIXEL(pixelNr, col[sr.colorbits]);
        } else {
            SET_BACKGROUND_PIXEL(pixelNr, col[sr.colorbits]);
        }
        
    } else {
        
        // Set single-color pixel
        if (sr.colorbits) {
            SET_FOREGROUND_PIXEL(pixelNr, col[sr.colorbits]);
        } else {
            SET_BACKGROUND_PIXEL(pixelNr, col[sr.colorbits]);
        }
    }
    
    // Shift register and toggle multicolor flipflop
    sr.data <<= 1;
    sr.mcFlop = !sr.mcFlop;
    sr.remainingBits -= 1;
}


void
VIC::drawSprites()
{
    uint8_t oldSpriteOnOff = spriteOnOff.delayed();
    uint8_t newSpriteOnOff = spriteOnOff.readWithDelay(2);


    // Quick exit
    if (!oldSpriteOnOff && !newSpriteOnOff)
        return;
    
    uint8_t firstDMA = isFirstDMAcycle;
    uint8_t secondDMA = isSecondDMAcycle;
        
    // For all sprites ...
    for (unsigned i = 0; i < 8; i++) {
        
        bool oldOnOff = GET_BIT(oldSpriteOnOff, i);
        bool newOnOff = GET_BIT(newSpriteOnOff, i);
        
        if (!oldOnOff && !newOnOff)
            continue;
        
        bool firstDMAi = GET_BIT(firstDMA, i);
        bool secondDMAi = GET_BIT(secondDMA, i);
        
        // Load colors for the first pixel
        sprExtraCol1 = reg.delayed.colors[COLREG_SPR_EX1];
        sprExtraCol2 = reg.delayed.colors[COLREG_SPR_EX2];
        sprCol[i] = reg.delayed.colors[COLREG_SPR0 + i];
        
        // Prepare sequencer
        spriteSr[i].exp = GET_BIT(reg.delayed.sprExpandX, i);
        spriteSr[i].mc = GET_BIT(reg.delayed.sprMC, i);
        
        // Draw first pixel
        if (oldOnOff) {
            drawSpritePixel(i, 0, secondDMAi, 0, 0);
        }
        
        // Load colors for the other pixel
        sprExtraCol1 = reg.current.colors[COLREG_SPR_EX1];
        sprExtraCol2 = reg.current.colors[COLREG_SPR_EX2];
        sprCol[i] = reg.current.colors[COLREG_SPR0 + i];
        
        // Draw the next three pixels
        if (oldOnOff) {
            drawSpritePixel(i, 1, secondDMAi,             0,          0);
            drawSpritePixel(i, 2, secondDMAi,             secondDMAi, 0);
            drawSpritePixel(i, 3, firstDMAi | secondDMAi, 0,          0);
        }
        
        // Draw the remaining four pixels
        if (newOnOff) {
            drawSpritePixel(i, 4, firstDMAi | secondDMAi, 0, secondDMAi);
            drawSpritePixel(i, 5, firstDMAi | secondDMAi, 0, 0);
            
            // Update X expansion bit
            spriteSr[i].exp = GET_BIT(reg.current.sprExpandX, i);
            
            // Update sprite priority bits
            COPY_BIT(reg.current.sprPriority, reg.delayed.sprPriority, i);
            
            // Update multicolor bit (new VICIIs)
            if (is856x()) {
                bool newMC = GET_BIT(reg.current.sprMC, i);
                if (spriteSr[i].mc != newMC) {
                    spriteSr[i].mc = newMC;
                    spriteSr[i].mcFlop ^= ~spriteSr[i].expFlop;
                }
            }
            
            drawSpritePixel(i, 6, firstDMAi | secondDMAi, 0, 0);

            // Update multicolor bit (old VICIIs)
            if (is656x()) {
                bool newMC = GET_BIT(reg.current.sprMC, i);
                if (spriteSr[i].mc != newMC) {
                    spriteSr[i].mc = newMC;
                    spriteSr[i].mcFlop = 0;
                }
            }

            drawSpritePixel(i, 7, firstDMAi,              0, 0);
        }
    }
}

void
VIC::drawSpritePixel(unsigned spriteNr,
                             unsigned pixelNr,
                             bool freeze,
                             bool halt,
                             bool load)
{
    assert(spriteNr < 8);
    assert(spriteSr[spriteNr].remaining_bits >= -1);
    assert(spriteSr[spriteNr].remaining_bits <= 26);
    
    bool multicol = spriteSr[spriteNr].mc; 

    // Load shift register if applicable
    if (load) {
        loadShiftRegister(spriteNr);
    }
    
    // Stop shift register if applicable
    if (halt) {
        spriteSr[spriteNr].remaining_bits = -1;
        spriteSr[spriteNr].colBits = 0;
    }
    
    // Run shift register if applicable
    if (!freeze) {
        
        // Check for horizontal trigger condition
        if (xCounter + pixelNr == reg.delayed.sprX[spriteNr]) {
            if (spriteSr[spriteNr].remaining_bits == -1) {
                spriteSr[spriteNr].remaining_bits = 26; // 24 data bits + 2 clearing zeroes
                spriteSr[spriteNr].expFlop = true;
                spriteSr[spriteNr].mcFlop = true;
            }
        }

        // Run shift register if there are remaining pixels to draw
        if (spriteSr[spriteNr].remaining_bits > 0) {

            // Determine render mode (single color /multi color) and colors
            
            // Get color bits by shifting sr data by 22 bits if a multi color
            // is rendered or by 23, if single color mode is used.
            unsigned shift = 22 + (!multicol || !spriteSr[spriteNr].mcFlop);
            spriteSr[spriteNr].colBits = spriteSr[spriteNr].data >> shift;
      
            // Toggle horizontal expansion flipflop for stretched sprites
            if (spriteSr[spriteNr].exp)
                spriteSr[spriteNr].expFlop = !spriteSr[spriteNr].expFlop;
            else
                spriteSr[spriteNr].expFlop = true;

            // Run shift register and toggle multicolor flipflop
            if (spriteSr[spriteNr].expFlop) {
                spriteSr[spriteNr].data <<= 1;
                spriteSr[spriteNr].mcFlop = !spriteSr[spriteNr].mcFlop;
                spriteSr[spriteNr].remaining_bits--;
            }
        }
    }
    
    // Draw pixel
    if (isVisibleColumn && !hideSprites) {
        if (multicol)
            setMultiColorSpritePixel(spriteNr, pixelNr, spriteSr[spriteNr].colBits & 0x03);
        else
            setSingleColorSpritePixel(spriteNr, pixelNr, spriteSr[spriteNr].colBits & 0x01);
    }
}

void
VIC::loadColors(uint8_t mode)
{
    uint8_t character = sr.latchedCharacter;
    uint8_t color = sr.latchedColor;
    
    switch (mode) {
            
        case STANDARD_TEXT:
            
            col[0] = reg.delayed.colors[COLREG_BG0];
            col[1] = color;
            break;
            
        case MULTICOLOR_TEXT:
            
            if (color & 0x8 /* MC flag */) {
                
                col[0] = reg.delayed.colors[COLREG_BG0];
                col[1] = reg.delayed.colors[COLREG_BG1];
                col[2] = reg.delayed.colors[COLREG_BG2];
                col[3] = color & 0x07;

            } else {
                
                col[0] = reg.delayed.colors[COLREG_BG0];
                col[1] = color;

            }
            break;
            
        case STANDARD_BITMAP:
            
            col[0] = character & 0xF;
            col[1] = character >> 4;
            break;
            
        case MULTICOLOR_BITMAP:
            
            col[0] = reg.delayed.colors[COLREG_BG0];
            col[1] = character >> 4;
            col[2] = character & 0x0F;
            col[3] = color;
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            
            col[0] = reg.delayed.colors[COLREG_BG0 + (character >> 6)];
            col[1] = color;
            break;
            
        case INVALID_TEXT:
        case INVALID_STANDARD_BITMAP:
        case INVALID_MULTICOLOR_BITMAP:
            
            col[0] = 0;
            col[1] = 0;
            col[2] = 0;
            col[3] = 0;
            break;
            
        default:
            
            assert(0);
            break;
    }
}

void
VIC::setSingleColorSpritePixel(unsigned spriteNr, unsigned pixelNr, uint8_t bit)
{
    if (bit) {
        drawSpritePixel(pixelNr, sprCol[spriteNr], spriteNr);
    }
}

void
VIC::setMultiColorSpritePixel(unsigned spriteNr, unsigned pixelNr, uint8_t two_bits)
{
    switch (two_bits) {
            
        case 0x01:
           
            drawSpritePixel(pixelNr, sprExtraCol1, spriteNr);
            break;
            
        case 0x02:
            
            drawSpritePixel(pixelNr, sprCol[spriteNr], spriteNr);
            break;
            
        case 0x03:
            
            drawSpritePixel(pixelNr, sprExtraCol2, spriteNr);
            break;
    }
}

void
VIC::drawSpritePixel(unsigned pixelNr, uint8_t color, int nr)
{
    uint8_t mask = (1 << nr);
    
    // Check for collision
    if (pixelSource[pixelNr]) {
        
        // Is it a sprite/sprite collision?
        if ((pixelSource[pixelNr] & 0x7F) && spriteSpriteCollisionEnabled) {
            
            spriteSpriteCollision |= ((pixelSource[pixelNr] & 0x7F) | mask);
            triggerIrq(4);
        }
        
        // Is it a sprite/background collision?
        if ((pixelSource[pixelNr] & 0x80) && spriteBackgroundCollisionEnabled) {
            
            spriteBackgroundColllision |= mask;
            triggerIrq(2);
        }
    }
    
    // Bit 7 indicates background as source
    if (nr == 7) mask = 0;
    
    setSpritePixel(pixelNr, color, spriteDepth(nr), mask);
}


//
// Low level drawing (pixel buffer access)
//

/*
void
VIC::setFramePixels(unsigned first, unsigned last, uint8_t color)
{
    assert(bufferoffset + last < NTSC_PIXELS);
    
    for (unsigned pixelNr = first; pixelNr <= last; pixelNr++) {
        SET_FRAME_PIXEL(pixelNr, color);
    }
}
*/
    
/*
void
VIC::setFramePixels(uint8_t color)
{
    assert(bufferoffset + last < NTSC_PIXELS);
    
    for (unsigned pixelNr = 0; pixelNr < 8; pixelNr++) {
        SET_FRAME_PIXEL(pixelNr, color);
    }
}
*/
    
void
VIC::setSpritePixel(unsigned pixel, uint8_t color, int depth, int source)
{
    // unsigned offset = bufferoffset + pixelNr;
    // assert(offset < NTSC_PIXELS);
    
    if (depth <= zBuffer[pixel]) {
        
        /* "the interesting case is when eg sprite 1 and sprite 0 overlap, and
         *  sprite 0 has the priority bit set (and sprite 1 has not). in this
         *  case 10/11 background bits show in front of whole sprite 0."
         * Test program: VICII/spritePriorities
         */
        if (!(pixelSource[pixel] & 0x7F)) {
            COLORIZE(pixel, color);
            zBuffer[pixel] = depth;
        }
    }
    pixelSource[pixel] |= source;
}

#ifdef WRITE_THROUGH
void
VIC::copyPixels()
{
    bufferoffset += 8;
}
#else
void
VIC::copyPixels() {
    
    assert(bufferoffset + 7 < NTSC_PIXELS);
    
    for (unsigned i = 0; i < 8; i++) {
        pixelBuffer[bufferoffset++] = rgbaTable[colBuffer[i]];
    }
}
#endif

void
VIC::expandBorders()
{
    int color, lastX;
    unsigned leftPixelPos;
    unsigned rightPixelPos;
    
    if (c64->vic.isPAL()) {
        leftPixelPos = PAL_LEFT_BORDER_WIDTH - (4*8);
        rightPixelPos = PAL_LEFT_BORDER_WIDTH + PAL_CANVAS_WIDTH + (4*8) - 1;
        lastX = PAL_PIXELS;
    } else {
        leftPixelPos = NTSC_LEFT_BORDER_WIDTH - (4*8);
        rightPixelPos = NTSC_LEFT_BORDER_WIDTH + NTSC_CANVAS_WIDTH + (4*8) - 1;
        lastX = NTSC_PIXELS;
    }
    
    // Make picked pixels visible for debugging
    // pixelBuffer[leftPixelPos + 1] = colors[5];
    // pixelBuffer[rightPixelPos - 1] = colors[5];
    
    color = pixelBuffer[leftPixelPos];
    for (unsigned i = 0; i < leftPixelPos; i++) {
        pixelBuffer[i] = color;
        // pixelBuffer[i] = colors[5]; // for debugging
    }
    color = pixelBuffer[rightPixelPos];
    for (unsigned i = rightPixelPos+1; i < lastX; i++) {
        pixelBuffer[i] = color;
        // pixelBuffer[i] = colors[5]; // for debugging
    }

    /*
    // Draw grid lines
    for (unsigned i = 0; i < NTSC_PIXELS; i += 10)
    pixelBuffer[i] = 0xFFFFFFFF;
    */
}

void
VIC::markLine(uint8_t color, unsigned start, unsigned end)
{
    assert (end <= NTSC_PIXELS);
    
    int rgba = rgbaTable[color];
    for (unsigned i = start; i < end; i++) {
        pixelBuffer[start + i] = rgba;
    }
}
