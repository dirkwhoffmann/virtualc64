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
    drawCanvas();
    drawBorder();
}

void
VIC::draw17()
{
    drawCanvas();
    drawBorder17();
}

void
VIC::draw55()
{
    drawCanvas();
    drawBorder55();
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
        SET_BACKGROUND_PIXEL(0, col[0]);
        for (unsigned pixel = 1; pixel < 8; pixel++) {
            SET_BACKGROUND_PIXEL(pixel, col[0]);
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
    uint8_t firstDMA = isFirstDMAcycle;
    uint8_t secondDMA = isSecondDMAcycle;
    
    // Pixel 0
    drawSpritePixel(0, spriteDisplayDelayed, secondDMA, 0);
    
    // After the first pixel, color register changes show up
    reg.delayed.colors[COLREG_SPR_EX1] = reg.current.colors[COLREG_SPR_EX1];
    reg.delayed.colors[COLREG_SPR_EX2] = reg.current.colors[COLREG_SPR_EX2];
    for (unsigned i = 0; i < 8; i++) {
        reg.delayed.colors[COLREG_SPR0 + i] = reg.current.colors[COLREG_SPR0 + i];
    }
    
    // Pixel 1, Pixel 2, Pixel 3
    drawSpritePixel(1, spriteDisplayDelayed, secondDMA, 0);
    drawSpritePixel(2, spriteDisplayDelayed, secondDMA, secondDMA);
    drawSpritePixel(3, spriteDisplayDelayed, firstDMA | secondDMA, 0);
    
    // If a shift register is loaded, the new data appears here.
    updateSpriteShiftRegisters();

    // Pixel 4, Pixel 5
    drawSpritePixel(4, spriteDisplay, firstDMA | secondDMA, 0);
    drawSpritePixel(5, spriteDisplay, firstDMA | secondDMA, 0);
    
    // Changes of the X expansion bits and the priority bits show up here
    reg.delayed.sprExpandX = reg.current.sprExpandX;
    reg.delayed.sprPriority = reg.current.sprPriority;

    // Update multicolor bits if a new VICII is emulated
    uint8_t toggle = reg.delayed.sprMC ^ reg.current.sprMC;
    if (toggle && is856x()) {
        
        // VICE:
        // BYTE next_mc_bits = vicii.regs[0x1c];
        // BYTE toggled = next_mc_bits ^ sprite_mc_bits;
        // sbuf_mc_flops ^= toggled & (~sbuf_expx_flops);
        // sprite_mc_bits = next_mc_bits;
        
        reg.delayed.sprMC = reg.current.sprMC;
        for (unsigned i = 0; i < 8; i++) {
            if (GET_BIT(toggle,i))
                spriteSr[i].mcFlop ^= !spriteSr[i].expFlop;
        }
    }
    
    // Pixel 6
    drawSpritePixel(6, spriteDisplay, firstDMA | secondDMA, 0);
    
    // Update multicolor bits if an old VICII is emulated
    if (toggle && is656x()) {
        
        reg.delayed.sprMC = reg.current.sprMC;
        for (unsigned i = 0; i < 8; i++) {
            if (GET_BIT(toggle,i))
                spriteSr[i].mcFlop = 0;
        }
    }
    
    // Pixel 7
    drawSpritePixel(7, spriteDisplay, firstDMA, 0);
    
    // Check for collisions
    for (unsigned i = 0; i < 8; i++) {
        
        // Check if two or more bits are set in pixelSource
        if (pixelSource[i] & (pixelSource[i] - 1)) {
            
            // Is it a sprite/sprite collision?
            if ((pixelSource[i] & 0xFF) & ((pixelSource[i] & 0xFF) - 1)) {
                
                spriteSpriteCollision |= (pixelSource[i] & 0xFF);
                triggerIrq(4);
            }
            
            // Is it a sprite/background collision?
            if ((pixelSource[i] & 0x100) && spriteBackgroundCollisionEnabled) {
                
                spriteBackgroundColllision |= (pixelSource[i] & 0xFF);
                triggerIrq(2);
            }
        }
    }
}

void
VIC::drawSpritePixel(unsigned pixel,
                     uint8_t enableBits,
                     uint8_t freezeBits,
                     uint8_t haltBits)
{
    // Iterate over all sprites
    for (unsigned sprite = 0; sprite < 8; sprite++) {
        
        assert(spriteSr[sprite].remaining_bits >= -1);
        assert(spriteSr[sprite].remaining_bits <= 26);

        if (!GET_BIT(enableBits, sprite))
            continue;
        
        bool freeze = GET_BIT(freezeBits, sprite);
        bool halt = GET_BIT(haltBits, sprite);
        bool mCol = GET_BIT(reg.delayed.sprMC, sprite);
        bool xExp = GET_BIT(reg.delayed.sprExpandX, sprite);
        
        // Stop shift register if applicable
        if (halt) {
            spriteSr[sprite].remaining_bits = -1;
            spriteSr[sprite].colBits = 0;
        }
        
        // Run shift register if applicable
        if (!freeze) {
            
            // Check for horizontal trigger condition
            if (xCounter + pixel == reg.delayed.sprX[sprite]) {
                if (spriteSr[sprite].remaining_bits == -1) {
                    spriteSr[sprite].remaining_bits = 26; // 24 data bits + 2 clearing zeroes
                    spriteSr[sprite].expFlop = true;
                    spriteSr[sprite].mcFlop = true;
                }
            }
            
            // Run shift register if there are remaining pixels to draw
            if (spriteSr[sprite].remaining_bits > 0) {
                
                // Determine render mode (single color /multi color) and colors
                
                // Get color bits by shifting sr data by 22 bits if a multi color
                // is rendered or by 23, if single color mode is used.
                unsigned shift = 22 + (!mCol || !spriteSr[sprite].mcFlop);
                spriteSr[sprite].colBits = spriteSr[sprite].data >> shift;
                
                // Toggle horizontal expansion flipflop for stretched sprites
                if (xExp)
                    spriteSr[sprite].expFlop = !spriteSr[sprite].expFlop;
                else
                    spriteSr[sprite].expFlop = true;
                
                // Run shift register and toggle multicolor flipflop
                if (spriteSr[sprite].expFlop) {
                    spriteSr[sprite].data <<= 1;
                    spriteSr[sprite].mcFlop = !spriteSr[sprite].mcFlop;
                    spriteSr[sprite].remaining_bits--;
                }
            }
        }
        
        // Draw pixel
        if (!hideSprites) {
            if (mCol)
                setMultiColorSpritePixel(sprite, pixel, spriteSr[sprite].colBits & 0x03);
            else
                setSingleColorSpritePixel(sprite, pixel, spriteSr[sprite].colBits & 0x01);
        }
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
VIC::setSingleColorSpritePixel(unsigned sprite, unsigned pixel, uint8_t bit)
{
    if (bit) {
        setSpritePixel(sprite, pixel, reg.delayed.colors[COLREG_SPR0 + sprite]);
    }
}

void
VIC::setMultiColorSpritePixel(unsigned sprite, unsigned pixel, uint8_t two_bits)
{
    switch (two_bits) {
            
        case 0x01:
           
            setSpritePixel(sprite, pixel, reg.delayed.colors[COLREG_SPR_EX1]);
            break;
            
        case 0x02:
            
            setSpritePixel(sprite, pixel, reg.delayed.colors[COLREG_SPR0 + sprite]);
            break;
            
        case 0x03:
            
            setSpritePixel(sprite, pixel, reg.delayed.colors[COLREG_SPR_EX2]);
            break;
    }
}


//
// Low level drawing (pixel buffer access)
//

void
VIC::setSpritePixel(unsigned sprite, unsigned pixel, uint8_t color)
{
    // unsigned offset = bufferoffset + pixelNr;
    // assert(offset < NTSC_PIXELS);
    
    uint8_t depth = spriteDepth(sprite);
    uint8_t source = (1 << sprite);
    
    if (depth <= zBuffer[pixel]) {
        
        /* "the interesting case is when eg sprite 1 and sprite 0 overlap, and
         *  sprite 0 has the priority bit set (and sprite 1 has not). in this
         *  case 10/11 background bits show in front of whole sprite 0."
         * Test program: VICII/spritePriorities
         */
        if (!(pixelSource[pixel] & 0xFF)) {
            if (isVisibleColumn) COLORIZE(pixel, color);
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
