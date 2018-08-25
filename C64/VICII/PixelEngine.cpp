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

PixelEngine::PixelEngine()
{
    setDescription("PixelEngine");
    
    debug(3, "  Creating PixelEngine at address %p...\n", this);
}

PixelEngine::~PixelEngine()
{
    debug(3, "  Releasing PixelEngine...\n");
}

void
PixelEngine::reset()
{
    VirtualComponent::reset();
    
    // Establish bindings
    vic = &c64->vic;
    
    currentScreenBuffer = screenBuffer1;
    pixelBuffer = currentScreenBuffer;
    bufferoffset = 0;
    
    memset(&sr, 0, sizeof(sr));
    memset(&sprite_sr, 0, sizeof(sprite_sr));
}

void
PixelEngine::resetScreenBuffers()
{
    for (unsigned line = 0; line < PAL_RASTERLINES; line++) {
        for (unsigned i = 0; i < NTSC_PIXELS; i++) {
            screenBuffer1[line * NTSC_PIXELS + i] = screenBuffer2[line * NTSC_PIXELS + i] = (line % 2) ? rgbaTable[8] : rgbaTable[9];
        }
    }
}

void
PixelEngine::beginFrame()
{
    visibleColumn = false;
}

void
PixelEngine::beginRasterline()
{
    // We adjust the position of the first pixel in the pixel buffer to make
    // sure that the screen always appears centered.
    if (c64->vic.isPAL()) {
        bufferoffset = PAL_LEFT_BORDER_WIDTH - 32;
    } else {
        bufferoffset = NTSC_LEFT_BORDER_WIDTH - 32;
    }
        
    // Prepare sprite pixel shift register
    for (unsigned i = 0; i < 8; i++) {
        sprite_sr[i].remaining_bits = -1;
        sprite_sr[i].col_bits = 0;
    }
    
    // Clear pixel buffer (has the same size as pixelSource and zBuffer)
    // 0xBB is a randomly chosen debug color
    if (!vic->vblank)
        memset(pixelBuffer, 0xBB, sizeof(pixelSource));
}

void
PixelEngine::endRasterline()
{
    if (!vic->vblank) {
        
        // Make the border look nice (evetually, we should get rid of this)
        expandBorders();
        
        // Advance pixelBuffer
        uint16_t nextline = c64->getRasterline() - PAL_UPPER_VBLANK + 1;
        if (nextline < PAL_RASTERLINES) {
            pixelBuffer = currentScreenBuffer + (nextline * NTSC_PIXELS);
        }
        
    }
}
void
PixelEngine::endFrame()
{
    // Switch active screen buffer
    currentScreenBuffer = (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1;
    pixelBuffer = currentScreenBuffer;    
}

void
PixelEngine::draw()
{
    // if (vic->vblank) return;
        
    drawCanvas();
    drawBorder();
    drawSprites();
    copyPixels();
}

void
PixelEngine::draw17()
{
    // if (vic->vblank) return;
    
    drawCanvas();
    drawSprites();
    drawBorder17();
    copyPixels();
}

void
PixelEngine::draw55()
{
    // if (vic->vblank) return;
    
    drawCanvas();
    drawSprites();
    drawBorder55();
    copyPixels();
}

void
PixelEngine::drawOutsideBorder()
{
    if (vic->vblank)
        return;
    
    drawSprites();
}

void
PixelEngine::drawBorder()
{
    assert(vic->mainFrameFF.delayed() == vic->flipflops.main);
    
    if (vic->mainFrameFF.delayed()) {
        
        assert(vic->borderColor.delayed() == vic->regValue.delayed.colors[COLREG_BORDER]);
        assert(vic->borderColor.current() == vic->regValue.current.colors[COLREG_BORDER]);
        drawFramePixel(0, vic->borderColor.delayed());
        drawFramePixels(1, 7, vic->borderColor.current());
    }
}

void
PixelEngine::drawBorder17()
{
    
    assert(vic->borderColor.delayed() == vic->regValue.delayed.colors[COLREG_BORDER]);
    assert(vic->borderColor.current() == vic->regValue.current.colors[COLREG_BORDER]);
    assert(vic->mainFrameFF.delayed() == vic->flipflops.main);
    assert(vic->mainFrameFF.current() == vic->newFlipflops.main);

    if (vic->mainFrameFF.delayed() && !vic->mainFrameFF.current()) {
        
        // 38 column mode (only pixels 0...6 are drawn)
        drawFramePixel(0, vic->borderColor.delayed());
        drawFramePixels(1, 6, vic->borderColor.current());
        
    } else {

        // 40 column mode (all eight pixels are drawn)
        drawBorder();
    }
}

void
PixelEngine::drawBorder55()
{
    assert(vic->mainFrameFF.delayed() == vic->flipflops.main);
    assert(vic->mainFrameFF.current() == vic->newFlipflops.main);
    
    if (!vic->mainFrameFF.delayed() && vic->mainFrameFF.current()) {
        
        // 38 column mode (border starts at pixel 7)
        drawFramePixel(7, vic->borderColor.delayed());
  
    } else {
        
        drawBorder();
    }
}

void
PixelEngine::drawCanvas()
{
    uint8_t d011, d016, newD016, mode, oldMode, xscroll;
    
    /* "The sequencer outputs the graphics data in every raster line in the area
     *  of the display column as long as the vertical border flip-flop is reset
     *  (see section 3.9.)." [C.B.]
     */
    
    assert(vic->verticalFrameFF.delayed() == vic->flipflops.vertical);
    if (vic->verticalFrameFF.delayed()) {
        
        /* "Outside of the display column and if the flip-flop is set, the last
         *  current background color is displayed (this area is normally covered
         *  by the border)." [C.B.]
         */
        // TODO: This is wrong, border-bm-idle test fails
        drawEightBackgroundPixels(vic->bgColor[0].current());
        return;
    }
    
    /* "The graphics data sequencer is capable of 8 different graphics modes
     *  that are selected by the bits ECM, BMM and MCM (Extended Color Mode,
     *  Bit Map Mode and Multi Color Mode) in the registers $d011 and
     *  $d016." [C.B.]
     */
    
    d011 = vic->control1.delayed();
    d016 = vic->control2.delayed();
    xscroll = d016 & 0x07;
    mode = (d011 & 0x60) | (d016 & 0x10); // -xxx ----

    drawCanvasPixel(0, mode, d016, xscroll == 0, true);
    
    // After the first pixel, color register changes show up
    vic->regValue.delayed.colors[COLREG_BG0] = vic->regValue.current.colors[COLREG_BG0];
    vic->regValue.delayed.colors[COLREG_BG1] = vic->regValue.current.colors[COLREG_BG1];
    vic->regValue.delayed.colors[COLREG_BG2] = vic->regValue.current.colors[COLREG_BG2];
    vic->regValue.delayed.colors[COLREG_BG3] = vic->regValue.current.colors[COLREG_BG3];

    drawCanvasPixel(1, mode, d016, xscroll == 1, true);
    drawCanvasPixel(2, mode, d016, xscroll == 2, false);
    drawCanvasPixel(3, mode, d016, xscroll == 3, false);

    // After pixel 4, a change in D016 affects the display mode.
    newD016 = vic->control2.current();

    // In newer VICIIs, the one bits of D011 show up, too.
    if (!vic->is856x()) {
        d011 |= vic->control1.current();
    }
    oldMode = mode;
    mode = (d011 & 0x60) | (newD016 & 0x10);
    
    drawCanvasPixel(4, mode, d016, xscroll == 4, oldMode != mode);
    drawCanvasPixel(5, mode, d016, xscroll == 5, false);
    
    // In newer VICIIs, the zero bits of D011 show up here.
    if (!vic->is856x()) {
        d011 = vic->control1.current();
        oldMode = mode;
        mode = (d011 & 0x60) | (newD016 & 0x10);
    }

    drawCanvasPixel(6, mode, d016, xscroll == 6, oldMode != mode);
    
    // Before the last pixel is drawn, a change is D016 is fully detected.
    // If the multicolor bit get set, the mc flip flop is also reset.
    if (d016 != newD016) {
        if (RISING_EDGE(d016 & 0x10, newD016 & 0x10))
            sr.mc_flop = false;
        d016 = newD016;
    }
 
    drawCanvasPixel(7, mode, d016, xscroll == 7, false);
}


void
PixelEngine::drawCanvasPixel(uint8_t pixelNr,
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
        
        uint32_t result = vic->gAccessResult.delayed();
        
        // Load shift register
        sr.data = result & 0xFF;
        
        // Remember how to synthesize pixels
        sr.latchedCharacter = (result >> 16) & 0xFF;
        sr.latchedColor = (result >> 8) & 0xFF;
        
        // Reset the multicolor synchronization flipflop
        sr.mc_flop = true;
        
        // Make sure that colors get updated
        updateColors = true;
        
        sr.remaining_bits = 8;
    }
    
    // Clear any outstanding multicolor bit that shouldn't actually be drawn
    // TODO: VICE doesn't use a counter for this, but doesn't have the same issue,
    // figure out what magic they are doing
    if (!sr.remaining_bits) {
        sr.colorbits = 0;
    }
    
    // Update colors if necessary
    if (updateColors)
        loadColors(pixelNr, mode, sr.latchedCharacter, sr.latchedColor);
    
    // Render pixel
    bool multicolorDisplayMode =
    (mode & 0x10) && ((mode & 0x20) || (sr.latchedColor & 0x8));
    
    bool generateMulticolorPixel =
    (d016 & 0x10) && ((mode & 0x20) || (sr.latchedColor & 0x8));
    
    // Generate pixel
    if (generateMulticolorPixel) {
        if (sr.mc_flop) {
            sr.colorbits = (sr.data >> 6) >> !multicolorDisplayMode;
        }
    } else {
        sr.colorbits = (sr.data >> 7) << multicolorDisplayMode;
    }
    
    // Draw pixel
    if (multicolorDisplayMode) {
        setMultiColorPixel(pixelNr, sr.colorbits);
    } else {
        setSingleColorPixel(pixelNr, sr.colorbits);
    }
    
    // Shift register and toggle multicolor flipflop
    sr.data <<= 1;
    sr.mc_flop = !sr.mc_flop;
    sr.remaining_bits -= 1;
}


void
PixelEngine::drawSprites()
{
    uint8_t oldSpriteOnOff = vic->spriteOnOff.delayed();
    uint8_t newSpriteOnOff = vic->spriteOnOff.readWithDelay(2);


    // Quick exit
    if (!oldSpriteOnOff && !newSpriteOnOff)
        return;
    
    uint8_t firstDMA = vic->isFirstDMAcycle;
    uint8_t secondDMA = vic->isSecondDMAcycle;
    
    // Get sprite expansion bits
    uint8_t oldSprXExpand = vic->sprXExpand.delayed();
    uint8_t newSprXExpand = vic->sprXExpand.current();
    
    // Get color values (values may change after the first pixel has been drawn)
    uint8_t oldExtraColor1 = vic->sprExtraColor1.delayed();
    uint8_t oldExtraColor2 = vic->sprExtraColor2.delayed();
    uint8_t newExtraColor1 = vic->sprExtraColor1.current();
    uint8_t newExtraColor2 = vic->sprExtraColor2.current();

    // uint8_t oldSprXExpansion = vic->sprXExpansion.delayed();
    // uint8_t newSprXExpansion = vic->sprXExpansion.current();
    
    // For all sprites ...
    for (unsigned i = 0; i < 8; i++) {
        
        bool oldOnOff = GET_BIT(oldSpriteOnOff, i);
        bool newOnOff = GET_BIT(newSpriteOnOff, i);
        
        if (!oldOnOff && !newOnOff)
            continue;
        
        spriteXCoord = vic->sprXCoord[i].delayed(); // GET RID OF THIS VAR
        assert(spriteXCoord == vic->regValue.delayed.sprX[i]);
        spriteXExpand = GET_BIT(oldSprXExpand, i);
        assert(oldSprXExpand == vic->regValue.delayed.sprExpandX);
        
        bool firstDMAi = GET_BIT(firstDMA, i);
        bool secondDMAi = GET_BIT(secondDMA, i);
        
        // Load colors for the first pixel
        assert(oldExtraColor1 == vic->regValue.delayed.colors[COLREG_SPR_EX1]);
        assert(oldExtraColor2 == vic->regValue.delayed.colors[COLREG_SPR_EX2]);
        sprExtraCol1 = oldExtraColor1;
        sprExtraCol2 = oldExtraColor2;
        sprCol[i] = vic->sprColor[i].delayed();
        assert(sprCol[i] == vic->regValue.delayed.colors[COLREG_SPR0 + i]);
        
        // Draw first pixel
        if (oldOnOff) {
            drawSpritePixel(i, 0, secondDMAi, 0, 0);
        }
        
        // Load colors for the other pixel
        assert(newExtraColor1 == vic->regValue.current.colors[COLREG_SPR_EX1]);
        assert(newExtraColor2 == vic->regValue.current.colors[COLREG_SPR_EX2]);
        sprExtraCol1 = newExtraColor1;
        sprExtraCol2 = newExtraColor2;
        sprCol[i] = vic->sprColor[i].current();
        assert(sprCol[i] == vic->regValue.current.colors[COLREG_SPR0 + i]);
        
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
            
            // If spriteXexpand has changed, it shows up at this point in time.
            spriteXExpand = GET_BIT(newSprXExpand, i);
            assert(newSprXExpand == vic->regValue.current.sprExpandX);

            drawSpritePixel(i, 6, firstDMAi | secondDMAi, 0, 0);
            drawSpritePixel(i, 7, firstDMAi,              0, 0);
        }
    }
}

void
PixelEngine::drawSpritePixel(unsigned spriteNr,
                             unsigned pixelNr,
                             bool freeze,
                             bool halt,
                             bool load)
{
    assert(spriteNr < 8);
    assert(sprite_sr[spriteNr].remaining_bits >= -1);
    assert(sprite_sr[spriteNr].remaining_bits <= 26);
    
    assert(vic->iomem[0x1C] == vic->regValue.current.sprMC);
    bool multicol = GET_BIT(vic->iomem[0x1C], spriteNr);

    // Load shift register if applicable
    if (load) {
        loadShiftRegister(spriteNr);
    }
    
    // Stop shift register if applicable
    if (halt) {
        sprite_sr[spriteNr].remaining_bits = -1;
        sprite_sr[spriteNr].col_bits = 0;
    }
    
    // Run shift register if applicable
    if (!freeze) {
        
        // Check for horizontal trigger condition
        if (vic->xCounter + pixelNr == spriteXCoord && sprite_sr[spriteNr].remaining_bits == -1) {
            sprite_sr[spriteNr].remaining_bits = 26; // 24 data bits + 2 clearing zeroes
            sprite_sr[spriteNr].exp_flop = true;
            sprite_sr[spriteNr].mc_flop = true;
        }

        // Run shift register if there are remaining pixels to draw
        if (sprite_sr[spriteNr].remaining_bits > 0) {

            // Determine render mode (single color /multi color) and colors
            // TODO: Latch multicolor value at proper cycles. Add dc. multicol
            // sprite_sr[nr].mcol = vic->spriteIsMulticolor(nr);
            sprite_sr[spriteNr].col_bits = sprite_sr[spriteNr].data >> (multicol && sprite_sr[spriteNr].mc_flop ? 22 : 23);
                        
            // Toggle horizontal expansion flipflop for stretched sprites
            if (spriteXExpand)
                sprite_sr[spriteNr].exp_flop = !sprite_sr[spriteNr].exp_flop;
            else
                sprite_sr[spriteNr].exp_flop = true;

            // Run shift register and toggle multicolor flipflop
            if (sprite_sr[spriteNr].exp_flop) {
                sprite_sr[spriteNr].data <<= 1;
                sprite_sr[spriteNr].mc_flop = !sprite_sr[spriteNr].mc_flop;
                sprite_sr[spriteNr].remaining_bits--;
            }
        }
    }
    
    // Draw pixel
    if (visibleColumn && !vic->hideSprites) {
        if (multicol)
            setMultiColorSpritePixel(spriteNr, pixelNr, sprite_sr[spriteNr].col_bits & 0x03);
        else
            setSingleColorSpritePixel(spriteNr, pixelNr, sprite_sr[spriteNr].col_bits & 0x01);
    }
}

void
PixelEngine::loadColors(uint8_t pixelNr, uint8_t mode,
                        uint8_t characterSpace, uint8_t colorSpace)
{
    bool old = (pixelNr == 0);
    
    switch (mode) {
            
        case STANDARD_TEXT:
            
            col[0] = old ? vic->bgColor[0].delayed() : vic->bgColor[0].current();
            assert(col[0] == vic->regValue.delayed.colors[COLREG_BG0]);
            col[1] = colorSpace;
            break;
            
        case MULTICOLOR_TEXT:
            
            if (colorSpace & 0x8 /* MC flag */) {
                
                col[0] = old ? vic->bgColor[0].delayed() : vic->bgColor[0].current();
                col[1] = old ? vic->bgColor[1].delayed() : vic->bgColor[1].current();
                col[2] = old ? vic->bgColor[2].delayed() : vic->bgColor[2].current();
                col[3] = colorSpace & 0x07;

                assert(col[0] == vic->regValue.delayed.colors[COLREG_BG0]);
                assert(col[1] == vic->regValue.delayed.colors[COLREG_BG1]);
                assert(col[2] == vic->regValue.delayed.colors[COLREG_BG2]);

            } else {
                
                col[0] = old ? vic->bgColor[0].delayed() : vic->bgColor[0].current();
                assert(col[0] == vic->regValue.delayed.colors[COLREG_BG0]);
                col[1] = colorSpace;

            }
            break;
            
        case STANDARD_BITMAP:
            
            col[0] = characterSpace & 0xF;
            col[1] = characterSpace >> 4;
            break;
            
        case MULTICOLOR_BITMAP:
            
            col[0] = old ? vic->bgColor[0].delayed() : vic->bgColor[0].current();
            assert(col[0] == vic->regValue.delayed.colors[COLREG_BG0]);
            col[1] = characterSpace >> 4;
            col[2] = characterSpace & 0x0F;
            col[3] = colorSpace;
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            
            col[0] = old ?
            vic->bgColor[characterSpace >> 6].delayed() :
            vic->bgColor[characterSpace >> 6].current();
            assert(col[0] == vic->regValue.delayed.colors[COLREG_BG0 + (characterSpace >> 6)]);
            col[1] = colorSpace;
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
PixelEngine::setSingleColorPixel(unsigned pixelNr, uint8_t bit /* valid: 0, 1 */)
{
    // int oldrgba = (pixelNr == 0) ? col_rgba0[bit] : col_rgba[bit];
    // int rgba = rgbaTable[(col[bit] >> (pixelNr * 8)) & 0xF];
    // assert(rgba == oldrgba);
    
    if (bit) {
        // setForegroundPixel(pixelNr, rgba);
        drawForegroundPixel(pixelNr, col[bit]);
    } else {
        // setBackgroundPixel(pixelNr, rgba);
        drawBackgroundPixel(pixelNr, col[bit]);
    }
}

void
PixelEngine::setMultiColorPixel(unsigned pixelNr, uint8_t two_bits /* valid: 00, 01, 10, 11 */)
{
    // int oldrgba = (pixelNr == 0) ? col_rgba0[two_bits] : col_rgba[two_bits];
    // int rgba = rgbaTable[(col[two_bits] >> (pixelNr * 8)) & 0xF];
    // assert(rgba == oldrgba);
    
    if (two_bits & 0x02) {
        // setForegroundPixel(pixelNr, rgba);
        drawForegroundPixel(pixelNr, col[two_bits]);
    } else {
        // setBackgroundPixel(pixelNr, rgba);
        drawBackgroundPixel(pixelNr, col[two_bits]);
    }
}

void
PixelEngine::setSingleColorSpritePixel(unsigned spriteNr, unsigned pixelNr, uint8_t bit)
{
    if (bit) {
        drawSpritePixel(pixelNr, sprCol[spriteNr], spriteNr);
    }
}

void
PixelEngine::setMultiColorSpritePixel(unsigned spriteNr, unsigned pixelNr, uint8_t two_bits)
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
PixelEngine::drawSpritePixel(unsigned pixelNr, uint8_t color, int nr)
{
    uint8_t mask = (1 << nr);
    
    // Check for collision
    if (pixelSource[pixelNr]) {
        
        // Is it a sprite/sprite collision?
        if ((pixelSource[pixelNr] & 0x7F) && vic->spriteSpriteCollisionEnabled) {
            
            vic->spriteSpriteCollision |= ((pixelSource[pixelNr] & 0x7F) | mask);
            vic->iomem[0x1E] |= ((pixelSource[pixelNr] & 0x7F) | mask);
            assert(vic->iomem[0x1E] == vic->spriteSpriteCollision);
            vic->triggerIRQ(4);
        }
        
        // Is it a sprite/background collision?
        if ((pixelSource[pixelNr] & 0x80) && vic->spriteBackgroundCollisionEnabled) {
            
            vic->spriteBackgroundColllision |= mask;
            vic->iomem[0x1F] |= mask;
            assert(vic->iomem[0x1F] == vic->spriteBackgroundColllision);
            vic->triggerIRQ(2);
        }
    }
    
    // Bit 7 indicates background as source
    if (nr == 7) mask = 0;
    
    putSpritePixel(pixelNr, color, vic->spriteDepth(nr), mask);
}


//
// Low level drawing (pixel buffer access)
//

void
PixelEngine::drawFramePixels(unsigned first, unsigned last, uint8_t color)
{
    assert(bufferoffset + last < NTSC_PIXELS);
    
    for (unsigned pixelNr = first; pixelNr <= last; pixelNr++) {
        
        // pixelBuffer[bufferoffset + pixelNr] = rgbaTable[color & 0xF];
        colBuffer[pixelNr] = color;
        zBuffer[pixelNr] = BORDER_LAYER_DEPTH;

        // Disable sprite/foreground collision detection in border
        pixelSource[pixelNr] &= (~0x80);
    }
}

void
PixelEngine::drawForegroundPixel(unsigned pixelNr, uint8_t color)
{
    // unsigned offset = bufferoffset + pixelNr;
    // assert(offset < NTSC_PIXELS);
    
    // pixelBuffer[offset] = rgbaTable[color];
    colBuffer[pixelNr] = color;
    zBuffer[pixelNr] = FOREGROUND_LAYER_DEPTH;
    pixelSource[pixelNr] = 0x80;
}

void
PixelEngine::drawBackgroundPixel(unsigned pixelNr, uint8_t color)
{
    // unsigned offset = bufferoffset + pixelNr;
    // assert(offset < NTSC_PIXELS);
    
    // pixelBuffer[offset] = rgbaTable[color];
    colBuffer[pixelNr] = color;
    zBuffer[pixelNr] = BACKGROUD_LAYER_DEPTH;
    pixelSource[pixelNr] = 0x00;
}

void
PixelEngine::putSpritePixel(unsigned pixelNr, uint8_t color, int depth, int source)
{
    // unsigned offset = bufferoffset + pixelNr;
    // assert(offset < NTSC_PIXELS);
    
    if (depth <= zBuffer[pixelNr]) {
        
        /* "the interesting case is when eg sprite 1 and sprite 0 overlap, and
         *  sprite 0 has the priority bit set (and sprite 1 has not). in this
         *  case 10/11 background bits show in front of whole sprite 0."
         * Test program: VICII/spritePriorities
         */
        if (!(pixelSource[pixelNr] & 0x7F)) {
            // pixelBuffer[offset] = rgbaTable[color];
            colBuffer[pixelNr] = color;
            zBuffer[pixelNr] = depth;
        }
    }
    pixelSource[pixelNr] |= source;
}

/*
void
PixelEngine::setSpritePixel(unsigned pixelnr, int rgba, int depth, int source)
{
    unsigned offset = bufferoffset + pixelnr;
    assert(offset < NTSC_PIXELS);
    
    if (depth <= zBuffer[pixelnr] && !(pixelSource[pixelnr] & 0x7F)) {
        pixelBuffer[offset] = rgba;
        zBuffer[pixelnr] = depth;
    }
    pixelSource[pixelnr] |= source;
}
*/

void
PixelEngine::copyPixels() {
    
    assert(bufferoffset + 7 < NTSC_PIXELS);
    
    for (unsigned i = 0; i < 8; i++) {
        pixelBuffer[bufferoffset++] = rgbaTable[colBuffer[i]];
    }
}

void
PixelEngine::expandBorders()
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
PixelEngine::markLine(uint8_t color, unsigned start, unsigned end)
{
    assert (end <= NTSC_PIXELS);
    
    int rgba = rgbaTable[color];
    for (unsigned i = start; i < end; i++) {
        pixelBuffer[start + i] = rgba;
    }	
}
