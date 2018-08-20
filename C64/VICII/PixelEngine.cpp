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
    
    currentScreenBuffer = screenBuffer1[0];
    pixelBuffer = currentScreenBuffer;
    bufferoffset = 0;

    // Register snapshot items
    SnapshotItem items[] = {
        
        // VIC state latching
        { pipe.spriteX,             sizeof(pipe.spriteX),           CLEAR_ON_RESET | WORD_FORMAT },
        { &pipe.spriteXexpand,      sizeof(pipe.spriteXexpand),     CLEAR_ON_RESET },
        { &pipe.previousCTRL1,      sizeof(pipe.previousCTRL1),     CLEAR_ON_RESET },
        { &pipe.g_data,             sizeof(pipe.g_data),            CLEAR_ON_RESET },
        { &pipe.g_character,        sizeof(pipe.g_character),       CLEAR_ON_RESET },
        { &pipe.g_color,            sizeof(pipe.g_color),           CLEAR_ON_RESET },
        { &pipe.mainFrameFF,        sizeof(pipe.mainFrameFF),       CLEAR_ON_RESET },
        { &pipe.verticalFrameFF,    sizeof(pipe.verticalFrameFF),   CLEAR_ON_RESET },

        { &newDisplayMode,          sizeof(newDisplayMode),         CLEAR_ON_RESET },
        { NULL,                     0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
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
    
    memset(&sr, 0, sizeof(sr));
    memset(&sprite_sr, 0, sizeof(sprite_sr));
    memset(&pipe, 0, sizeof(pipe));
    // col_rgba[0] = col_rgba[1] = col_rgba[2] = col_rgba[3] = rgbaTable[0];
    // col_rgba0[0] = col_rgba0[1] = col_rgba0[2] = col_rgba0[3] = 0;
}

void
PixelEngine::resetScreenBuffers()
{
    for (unsigned line = 0; line < PAL_RASTERLINES; line++) {
        for (unsigned i = 0; i < NTSC_PIXELS; i++) {
            screenBuffer1[line][i] = screenBuffer2[line][i] = (line % 2) ? rgbaTable[8] : rgbaTable[9];
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
        
        // Make the border look nice
        expandBorders();
        
        // Advance pixelBuffer
        uint16_t nextline = c64->getRasterline() - PAL_UPPER_VBLANK + 1;
        if (nextline < PAL_RASTERLINES) {
                        
            // Old code
            // pixelBuffer += NTSC_PIXELS;
            // pxbuf += NTSC_PIXELS;
            
            // New code (slightly slower, but foolproof. Can't get outside the screen buffer)
            pixelBuffer = currentScreenBuffer + (nextline * NTSC_PIXELS);
            // pxbuf = pixelBuffer + bufshift;
            
        }
        
    }
}
void
PixelEngine::endFrame()
{
    // Switch active screen buffer
    currentScreenBuffer = (currentScreenBuffer == screenBuffer1[0]) ? screenBuffer2[0] : screenBuffer1[0];
    pixelBuffer = currentScreenBuffer;    
}

void
PixelEngine::updateSpriteOnOff()
{
    dc.spriteOnOff = dc.spriteOnOffPipe;
    dc.spriteOnOffPipe = vic->spriteOnOff;
}

void
PixelEngine::draw()
{
    if (vic->vblank)
        return;
        
    drawCanvas();
    drawBorder();
    drawSprites();
    
    bufferoffset += 8;
}

void
PixelEngine::draw17()
{
    if (vic->vblank)
        return;
    
    drawCanvas();
    drawBorder17();
    drawSprites();
    
    bufferoffset += 8;
}

void
PixelEngine::draw55()
{
    if (vic->vblank)
        return;
    
    drawCanvas();
    drawBorder55();
    drawSprites();
    
    bufferoffset += 8;
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
    if (pipe.mainFrameFF) {
        
        drawFramePixel(0, vic->borderColor.delayed());
        drawFramePixels(1, 7, vic->borderColor.current());
    }
}

void
PixelEngine::drawBorder17()
{
    if (pipe.mainFrameFF && !vic->p.mainFrameFF) {
        
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
    if (!pipe.mainFrameFF && vic->p.mainFrameFF) {
        
        // 38 column mode (border starts at pixel 7)
        drawFramePixel(7, vic->borderColor.delayed());
  
    } else {
        
        drawBorder();
    }
}

void
PixelEngine::drawCanvas()
{
    /* "The sequencer outputs the graphics data in every raster line in the area
     *  of the display column as long as the vertical border flip-flop is reset
     *  (see section 3.9.)." [C.B.]
     */
    
    if (!pipe.verticalFrameFF) {
        
        /* "The graphics data sequencer is capable of 8 different graphics modes
         *  that are selected by the bits ECM, BMM and MCM (Extended Color Mode,
         *  Bit Map Mode and Multi Color Mode) in the registers $d011 and
         *  $d016." [C.B.]
         */        
        uint64_t d011 = vic->control1.delayed();
        uint64_t d016 = vic->control2.delayed();
        
        uint64_t oldD011_mode = d011 & 0x6060606060606060; // -xx- ----
        uint64_t oldD016_mode = d016 & 0x1010101010101010; // ---x ----
        uint64_t newD011_mode = vic->control1.current() & 0x6060606060606060; // -xx- ----
        uint64_t newD016_mode = vic->control2.current() & 0x1010101010101010; // ---x ----
        
        // Timing of a $D011 register change:
        // The new one bit show up after the first four pixels are drawn.
        // The new zero bits show up after the first six pixels are drawn.
        newDisplayMode =
        (oldD011_mode & 0x0000FFFFFFFFFFFF) |
        (newD011_mode & 0xFFFFFFFF00000000);
        
        // Timing of a $D016 register change:
        // The new bits show up after the first four pixels are drawn.
        newDisplayMode |=
        (oldD016_mode & 0x00000000FFFFFFFF) |
        (newD016_mode & 0xFFFFFFFF00000000);
        
        uint8_t ctrl2 = d016 & 0xFF;
        drawCanvasPixel(0, GET_BYTE(newDisplayMode, 0), ctrl2);
        drawCanvasPixel(1, GET_BYTE(newDisplayMode, 1), ctrl2);
        drawCanvasPixel(2, GET_BYTE(newDisplayMode, 2), ctrl2);
        drawCanvasPixel(3, GET_BYTE(newDisplayMode, 3), ctrl2);
        drawCanvasPixel(4, GET_BYTE(newDisplayMode, 4), ctrl2);
        drawCanvasPixel(5, GET_BYTE(newDisplayMode, 5), ctrl2);
        drawCanvasPixel(6, GET_BYTE(newDisplayMode, 6), ctrl2);
        
        if (!(oldD016_mode & 0x10) && (newD016_mode & 0x10)) {
            sr.mc_flop = false;
        }
    
        drawCanvasPixel(7, GET_BYTE(newDisplayMode, 7), ctrl2);
        
    } else {
        
        /* "Outside of the display column and if the flip-flop is set, the last
         *  current background color is displayed (this area is normally covered
         *  by the border)." [C.B.]
         */
        // TODO: Check if border-bm-idle test passes
        drawEightBackgroundPixels(vic->bgColor[0].current());
    }
}

void
PixelEngine::drawCanvasPixel(uint8_t pixelNr, uint8_t displayMode, uint8_t regCtrl2)
{
    assert(pixelNr < 8);
    
    /* "The heart of the sequencer is an 8 bit shift register that is shifted by
     *  1 bit every pixel and reloaded with new graphics data after every
     *  g-access. With XSCROLL from register $d016 the reloading can be delayed
     *  by 0-7 pixels, thus shifting the display up to 7 pixels to the right."
     */
    if (pixelNr == (regCtrl2 & 0x07) /* XSCROLL */ && sr.canLoad) {
        
        // Load shift register
        sr.data = pipe.g_data;
        
        // Remember how to synthesize pixels
        sr.latchedCharacter = pipe.g_character;
        sr.latchedColor = pipe.g_color;
        
        // Reset the multicolor synchronization flipflop
        sr.mc_flop = true;
        
        sr.remaining_bits = 8;
    }
    
    // Clear any outstanding multicolor bit that shouldn't actually be drawn
    // TODO: VICE doesn't use a counter for this, but doesn't have the same issue,
    // figure out what magic they are doing
    if (!sr.remaining_bits) {
        sr.colorbits = 0;
    }
    
    // Load colors
    // loadColors(pixelnr, (DisplayMode)displayMode, sr.latchedCharacter, sr.latchedColor);
    loadColors(pixelNr, (DisplayMode)displayMode, sr.latchedCharacter, sr.latchedColor);
    
    // Render pixel
    bool multicolorDisplayMode =
    (displayMode & 0x10) && ((displayMode & 0x20) || (sr.latchedColor & 0x8));
    bool generateMulticolorPixel =
    (regCtrl2 & 0x10) && ((displayMode & 0x20) || (sr.latchedColor & 0x8));
    
    // During pixels 5-7 of a D016 trasition, the VIC seems to behave in a mixed
    // state, where the pixel is displayed as the new mode, but is generated in
    // the old way (shifted to the expected number of bits for the display mode)
    if (generateMulticolorPixel) {
        if (sr.mc_flop) {
            sr.colorbits = (sr.data >> 6);
            if (!multicolorDisplayMode) {
                sr.colorbits >>= 1;
            }
        }
    } else {
        sr.colorbits = sr.data >> 7;
        if (multicolorDisplayMode) {
            sr.colorbits <<= 1;
        }
    }
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
    uint8_t firstDMA = vic->isFirstDMAcycle;
    uint8_t secondDMA = vic->isSecondDMAcycle;
    
    if (!dc.spriteOnOff && !dc.spriteOnOffPipe && !firstDMA && !secondDMA) // Quick exit
        return;
    
    // Load colors from VIC registers
    loadSpriteColors();
    
    // Draw first four pixels for each sprite
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(dc.spriteOnOff, i)) {

            bool firstDMAi = GET_BIT(firstDMA, i);
            bool secondDMAi = GET_BIT(secondDMA, i);
            
            drawSpritePixel(i, 0, secondDMAi             /* freeze */, 0          /* halt */, 0         /* load */);
            drawSpritePixel(i, 1, secondDMAi             /* freeze */, 0          /* halt */, 0         /* load */);
            drawSpritePixel(i, 2, secondDMAi             /* freeze */, secondDMAi /* halt */, 0         /* load */);
            drawSpritePixel(i, 3, firstDMAi | secondDMAi /* freeze */, 0          /* halt */, 0         /* load */);
        }
    }

    updateSpriteOnOff();
    
    // Draw last four pixels for each sprite
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(dc.spriteOnOff, i)) {

            bool firstDMAi = GET_BIT(firstDMA, i);
            bool secondDMAi = GET_BIT(secondDMA, i);

            drawSpritePixel(i, 4, firstDMAi | secondDMAi /* freeze */, 0         /* halt */, secondDMAi /* load */);
            drawSpritePixel(i, 5, firstDMAi | secondDMAi /* freeze */, 0         /* halt */, 0          /* load */);
            
            // If spriteXexpand has changed, it shows up at this point in time.
            COPY_BIT(vic->p.spriteXexpand, pipe.spriteXexpand, i);
            
            drawSpritePixel(i, 6, firstDMAi | secondDMAi /* freeze */, 0         /* halt */, 0          /* load */);
            drawSpritePixel(i, 7, firstDMAi              /* freeze */, 0         /* halt */, 0          /* load */);
        }
    }
}

void
PixelEngine::drawSpritePixel(unsigned spritenr, unsigned pixelnr, bool freeze, bool halt, bool load)
{
    assert(spritenr < 8);
    assert(sprite_sr[spritenr].remaining_bits >= -1);
    assert(sprite_sr[spritenr].remaining_bits <= 26);
    
    bool multicol = vic->spriteIsMulticolor(spritenr);

    // Load shift register if applicable
    if (load) {
        loadShiftRegister(spritenr);
    }
    
    // Stop shift register if applicable
    if (halt) {
        sprite_sr[spritenr].remaining_bits = -1;
        sprite_sr[spritenr].col_bits = 0;
    }
    
    // Run shift register if applicable
    if (!freeze) {
        
        // Check for horizontal trigger condition
        if (vic->xCounter + pixelnr == pipe.spriteX[spritenr] && sprite_sr[spritenr].remaining_bits == -1) {
            sprite_sr[spritenr].remaining_bits = 26; // 24 data bits + 2 clearing zeroes
            sprite_sr[spritenr].exp_flop = true;
            sprite_sr[spritenr].mc_flop = true;
        }

        // Run shift register if there are remaining pixels to draw
        if (sprite_sr[spritenr].remaining_bits > 0) {

            // Determine render mode (single color /multi color) and colors
            // TODO: Latch multicolor value at proper cycles. Add dc. multicol
            // sprite_sr[nr].mcol = vic->spriteIsMulticolor(nr);
            sprite_sr[spritenr].col_bits = sprite_sr[spritenr].data >> (multicol && sprite_sr[spritenr].mc_flop ? 22 : 23);
                        
            // Toggle horizontal expansion flipflop for stretched sprites
            if (GET_BIT(pipe.spriteXexpand, spritenr))
                sprite_sr[spritenr].exp_flop = !sprite_sr[spritenr].exp_flop;
            else
                sprite_sr[spritenr].exp_flop = true;

            // Run shift register and toggle multicolor flipflop
            if (sprite_sr[spritenr].exp_flop) {
                sprite_sr[spritenr].data <<= 1;
                sprite_sr[spritenr].mc_flop = !sprite_sr[spritenr].mc_flop;
                sprite_sr[spritenr].remaining_bits--;
            }
        }
    }
    
    // Draw pixel
    if (visibleColumn && vic->drawSprites) {
        if (multicol)
            setMultiColorSpritePixel(spritenr, pixelnr, sprite_sr[spritenr].col_bits & 0x03);
        else
            setSingleColorSpritePixel(spritenr, pixelnr, sprite_sr[spritenr].col_bits & 0x01);
    }
}

void
PixelEngine::loadColors(uint8_t pixelNr, DisplayMode mode,
                        uint8_t characterSpace, uint8_t colorSpace)
{
    switch (mode) {

        #define MIX_COLORS(x,y) ((x & 0x0FF) | (y & ~0x0FF))
            
        case STANDARD_TEXT:
            
            col[0] = MIX_COLORS(vic->bgColor[0].delayed(),vic->bgColor[0].current());
            col[1] = pattern[colorSpace];
            break;
            
        case MULTICOLOR_TEXT:
            if (colorSpace & 0x8 /* MC flag */) {
                col[0] = MIX_COLORS(vic->bgColor[0].delayed(),vic->bgColor[0].current());
                col[1] = MIX_COLORS(vic->bgColor[1].delayed(),vic->bgColor[1].current());
                col[2] = MIX_COLORS(vic->bgColor[2].delayed(),vic->bgColor[2].current());
                col[3] = pattern[colorSpace & 0x07];
            } else {
                col[0] = MIX_COLORS(vic->bgColor[0].delayed(),vic->bgColor[0].current());
                col[1] = pattern[colorSpace];
            }
            break;
            
        case STANDARD_BITMAP:
            col[0] = pattern[characterSpace & 0xF];
            col[1] = pattern[characterSpace >> 4];
            
            break;
            
        case MULTICOLOR_BITMAP:
            col[0] = MIX_COLORS(vic->bgColor[0].delayed(),vic->bgColor[0].current());
            col[1] = pattern[characterSpace >> 4];
            col[2] = pattern[characterSpace & 0x0F];
            col[3] = pattern[colorSpace];
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            col[0] = MIX_COLORS(vic->bgColor[characterSpace >> 6].delayed(),
                                vic->bgColor[characterSpace >> 6].current());
            col[1] = pattern[colorSpace];
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
PixelEngine::loadSpriteColors()
{
    sprExtraCol1 = vic->sprExtraColor1.delayed();
    sprExtraCol2 = vic->sprExtraColor2.delayed();
    for (unsigned i = 0; i < 8; i++)
        sprCol[i] = vic->sprColor[i].delayed();
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
PixelEngine::drawSpritePixel(unsigned pixelNr, uint64_t color, int nr)
{
    uint8_t mask = (1 << nr);
    
    // Check for collision
    if (pixelSource[pixelNr]) {
        
        // Is it a sprite/sprite collision?
        if ((pixelSource[pixelNr] & 0x7F) && vic->spriteSpriteCollisionEnabled) {
            vic->iomem[0x1E] |= ((pixelSource[pixelNr] & 0x7F) | mask);
            vic->triggerIRQ(4);
        }
        
        // Is it a sprite/background collision?
        if ((pixelSource[pixelNr] & 0x80) && vic->spriteBackgroundCollisionEnabled) {
            vic->iomem[0x1F] |= mask;
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
PixelEngine::drawFramePixels(unsigned first, unsigned last, uint64_t color)
{
    assert(bufferoffset + last < NTSC_PIXELS);
    
    color >>= (8 * first);
    for (unsigned pixelNr = first; pixelNr <= last; pixelNr++, color >>= 8) {
        
        pixelBuffer[bufferoffset + pixelNr] = rgbaTable[color & 0xF];
        zBuffer[pixelNr] = BORDER_LAYER_DEPTH;

        // Disable sprite/foreground collision detection in border
        pixelSource[pixelNr] &= (~0x80);
    }
}

void
PixelEngine::drawForegroundPixel(unsigned pixelNr, uint64_t color)
{
    unsigned offset = bufferoffset + pixelNr;
    assert(offset < NTSC_PIXELS);
    
    pixelBuffer[offset] = rgbaTable[GET_BYTE(color, pixelNr)];
    zBuffer[pixelNr] = FOREGROUND_LAYER_DEPTH;
    pixelSource[pixelNr] = 0x80;
}

void
PixelEngine::drawBackgroundPixel(unsigned pixelNr, uint64_t color)
{
    unsigned offset = bufferoffset + pixelNr;
    assert(offset < NTSC_PIXELS);
    
    pixelBuffer[offset] = rgbaTable[GET_BYTE(color, pixelNr)];
    zBuffer[pixelNr] = BACKGROUD_LAYER_DEPTH;
    pixelSource[pixelNr] = 0x00;
}

void
PixelEngine::putSpritePixel(unsigned pixelNr, uint64_t color, int depth, int source)
{
    unsigned offset = bufferoffset + pixelNr;
    assert(offset < NTSC_PIXELS);
    
    if (depth <= zBuffer[pixelNr] && !(pixelSource[pixelNr] & 0x7F)) {
        pixelBuffer[offset] = rgbaTable[GET_BYTE(color, pixelNr)];
        zBuffer[pixelNr] = depth;
    }
    pixelSource[pixelNr] |= source;
}

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
