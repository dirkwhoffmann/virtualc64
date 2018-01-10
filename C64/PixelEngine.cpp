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

// DIRK DEBUG, REMOVE ASAP
extern unsigned dirktrace;
extern unsigned dirkcnt;


PixelEngine::PixelEngine() // C64 *c64)
{
    setDescription("PixelEngine");
    
    debug(3, "  Creating PixelEngine at address %p...\n", this);
    
    currentScreenBuffer = screenBuffer1[0];
    pixelBuffer = currentScreenBuffer;
    bufferoffset = 0;

    // Register snapshot items
    SnapshotItem items[] = {
        
        // VIC state latching
        { &pipe.xCounter,            sizeof(pipe.xCounter),           CLEAR_ON_RESET },
        { pipe.spriteX,              sizeof(pipe.spriteX),            CLEAR_ON_RESET | WORD_FORMAT },
        { &pipe.spriteXexpand,       sizeof(pipe.spriteXexpand),      CLEAR_ON_RESET },
        { &pipe.registerCTRL1,       sizeof(pipe.registerCTRL1),      CLEAR_ON_RESET },
        { &pipe.registerCTRL2,       sizeof(pipe.registerCTRL2),      CLEAR_ON_RESET },
        { &pipe.g_data,              sizeof(pipe.g_data),             CLEAR_ON_RESET },
        { &pipe.g_character,         sizeof(pipe.g_character),        CLEAR_ON_RESET },
        { &pipe.g_color,             sizeof(pipe.g_color),            CLEAR_ON_RESET },
        { &pipe.mainFrameFF,         sizeof(pipe.mainFrameFF),        CLEAR_ON_RESET },
        { &pipe.verticalFrameFF,     sizeof(pipe.verticalFrameFF),    CLEAR_ON_RESET },
        
        { &bpipe.borderColor,        sizeof(bpipe.borderColor),       CLEAR_ON_RESET },
        { cpipe.backgroundColor,     sizeof(cpipe.backgroundColor),   CLEAR_ON_RESET | BYTE_FORMAT },
        { spipe.spriteColor,         sizeof(spipe.spriteColor),       CLEAR_ON_RESET | BYTE_FORMAT },
        { &spipe.spriteExtraColor1,  sizeof(spipe.spriteExtraColor1), CLEAR_ON_RESET },
        { &spipe.spriteExtraColor2,  sizeof(spipe.spriteExtraColor2), CLEAR_ON_RESET },

        { &displayMode,              sizeof(displayMode),             CLEAR_ON_RESET },
        { NULL,                      0,                               0 }};
    
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
}

void
PixelEngine::resetScreenBuffers()
{
    for (unsigned line = 0; line < PAL_RASTERLINES; line++) {
        for (unsigned i = 0; i < NTSC_PIXELS; i++) {
            screenBuffer1[line][i] = screenBuffer2[line][i] = (line % 2) ? colors[8] : colors[9];
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
    // Clear z buffer. The buffer is initialized with the highest positive 8-bit value (meaning the pixel is far away)
    // Not necessary, because canvas pixels are drawn first
    // memset(zBuffer, SCHAR_MAX, sizeof(zBuffer));
    
    // Clear pixel source
    // Not necessary, because canvas pixels are drawn first
    // memset(pixelSource, 0x00, sizeof(pixelSource));

    // Adjust position of first pixel in buffer (make sure that screen is always centered)
    if (c64->isPAL()) {
        bufferoffset = PAL_LEFT_BORDER_WIDTH - 32;
    } else {
        bufferoffset = NTSC_LEFT_BORDER_WIDTH - 32;
    }
        
    // Prepare sprite pixel shift register
    for (unsigned i = 0; i < 8; i++) {
        sprite_sr[i].remaining_bits = -1;
        // sprite_sr[i].mcol_bits = sprite_sr[i].scol_bit = 0;
        sprite_sr[i].col_bits = 0;
    }
    
    // Clear pixel buffer (has same size as pixelSource and zBuffer)
    // FOR DEBUGGING ONLY, 0xBB is a randomly chose debug color
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

// -----------------------------------------------------------------------------------------------
//                                   VIC state latching
// -----------------------------------------------------------------------------------------------

inline void
PixelEngine::updateSpriteOnOff()
{
    dc.spriteOnOff = dc.spriteOnOffPipe;
    dc.spriteOnOffPipe = vic->spriteOnOff;
}

// -----------------------------------------------------------------------------------------------
//                          High level drawing (canvas, sprites, border)
// -----------------------------------------------------------------------------------------------

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

inline void
PixelEngine::drawBorder()
{
    if (pipe.mainFrameFF) {
        
        setFramePixel(0, colors[bpipe.borderColor]);
        
        // After the first pixel has been drawn, color register changes show up
        bpipe = vic->bp;
        
        int rgba = colors[bpipe.borderColor];
        setFramePixel(1, rgba);
        setFramePixel(2, rgba);
        setFramePixel(3, rgba);
        setFramePixel(4, rgba);
        setFramePixel(5, rgba);
        setFramePixel(6, rgba);
        setFramePixel(7, rgba);
    }
}

inline void
PixelEngine::drawBorder17()
{
    if (pipe.mainFrameFF && !vic->p.mainFrameFF) {
        
        // 38 column mode
        setFramePixel(0, colors[bpipe.borderColor]);
        
        // After the first pixel has been drawn, color register changes show up
        bpipe = vic->bp;
        
        int rgba = colors[bpipe.borderColor];
        setFramePixel(1, rgba);
        setFramePixel(2, rgba);
        setFramePixel(3, rgba);
        setFramePixel(4, rgba);
        setFramePixel(5, rgba);
        setFramePixel(6, rgba);
        // That's all, we only draw 7 pixels here
        
    } else {

        // 40 column mode
        drawBorder();
    }
}

inline void
PixelEngine::drawBorder55()
{
    if (!pipe.mainFrameFF && vic->p.mainFrameFF) {
        
        // 38 column mode
        setFramePixel(7, colors[bpipe.borderColor]);
        
    } else {
        
        // 40 column mode
        drawBorder();
    }
}

inline void
PixelEngine::drawCanvas()
{
    /* "Der Sequenzer gibt die Grafikdaten in jeder Rasterzeile im Bereich der
     Anzeigespalte aus, sofern das vertikale Rahmenflipflop gelöscht ist (siehe
     Abschnitt 3.9.). Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    
    if (!pipe.verticalFrameFF) {
        
        uint8_t D011 = vic->p.registerCTRL1 & 0x60; // -xx- ----
        uint8_t D016 = vic->p.registerCTRL2 & 0x10; // ---x ----
        
        drawCanvasPixel(0);
        
        // After the first pixel has been drawn, color register changes show up
        cpipe = vic->cp;
        
        drawCanvasPixel(1);
        drawCanvasPixel(2);
        drawCanvasPixel(3);
        
        // After pixel 4, the one and zero bits in D016 and the one bits in D011 show up
        // This corresponds to the behavior of the color latency chip model in VICE
        displayMode |= D016;        // latch 1s of D016
        displayMode &= D016 | 0xEF; // latch 0s of D016
        displayMode |= D011;        // latch 1s of D011
        
        drawCanvasPixel(4);
        drawCanvasPixel(5);
        
        // After pixel 6, the zero bits in D011 show up
        // This corresponds to the behavior of the color latency chip model in VICE
        displayMode &= D011 | 0x9F; // latch 0s of D011

        drawCanvasPixel(6);
        
        // TODO (seen in VICE)
        // if D016 had a rising edge, clear the multicolor flipflop
        /*
         if (rising_edge_d016)
            sr.mc_flop = false;
        */
        drawCanvasPixel(7);
        
    } else {
        
        // "... bei gesetztem Flipflop wird die letzte aktuelle Hintergrundfarbe dargestellt."
        int col = colors[vic->getBackgroundColor()];
        // The following fix (which was done for border-bm-idle is wrong)
        // int col = col_rgba[0];
        setEightBackgroundPixels(col);
    }
}

inline void
PixelEngine::drawCanvasPixel(uint8_t pixelnr)
{
    assert(pixelnr < 8);
    
    if (pixelnr == (pipe.registerCTRL2 & 0x07) /* horizontal raster scroll */ && sr.canLoad) {
        
        // Load shift register
        sr.data = pipe.g_data;
        
        // Remember how to synthesize pixels
        sr.latchedCharacter = pipe.g_character;
        sr.latchedColor = pipe.g_color;
        
        // Reset the multicolor synchronization flipflop
        sr.mc_flop = true;
    }
    
    // Load colors
    loadColors((DisplayMode)displayMode, sr.latchedCharacter, sr.latchedColor);
    
    // Render pixel
    if (multicol) {
        if (sr.mc_flop)
            sr.colorbits = (sr.data >> 6);
        setMultiColorPixel(pixelnr, sr.colorbits);
    } else {
        setSingleColorPixel(pixelnr, sr.data >> 7);
    }
    
    // Shift register and toggle multicolor flipflop
    sr.data <<= 1;
    sr.mc_flop = !sr.mc_flop;
}


inline void
PixelEngine::drawSprites()
{
    uint8_t firstDMA = vic->isFirstDMAcycle;
    uint8_t secondDMA = vic->isSecondDMAcycle;

    if (!dc.spriteOnOff && !dc.spriteOnOffPipe && !firstDMA && !secondDMA) // Quick exit
        return;
    
    // Update sprite color registers
    spipe = vic->sp;
    
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
        if (pipe.xCounter + pixelnr == pipe.spriteX[spritenr] && sprite_sr[spritenr].remaining_bits == -1) {
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

// -----------------------------------------------------------------------------------------------
//                         Mid level drawing (semantic pixel rendering)
// -----------------------------------------------------------------------------------------------

inline void
PixelEngine::loadColors(DisplayMode mode, uint8_t characterSpace, uint8_t colorSpace)
{
    switch (mode) {
            
        case STANDARD_TEXT:
            
            col_rgba[0] = colors[cpipe.backgroundColor[0]];
            col_rgba[1] = colors[colorSpace];
            multicol = false;
            break;
            
        case MULTICOLOR_TEXT:
            if (colorSpace & 0x8 /* MC flag */) {
                col_rgba[0] = colors[cpipe.backgroundColor[0]];
                col_rgba[1] = colors[cpipe.backgroundColor[1]];
                col_rgba[2] = colors[cpipe.backgroundColor[2]];
                col_rgba[3] = colors[colorSpace & 0x07];
                multicol = true;
            } else {
                col_rgba[0] = colors[cpipe.backgroundColor[0]];
                col_rgba[1] = colors[colorSpace];
                multicol = false;
            }
            break;
            
        case STANDARD_BITMAP:
            col_rgba[0] = colors[characterSpace & 0x0F]; // color of '0' pixels
            col_rgba[1] = colors[characterSpace >> 4]; // color of '1' pixels
            multicol = false;
            break;
            
        case MULTICOLOR_BITMAP:
            col_rgba[0] = colors[cpipe.backgroundColor[0]];
            col_rgba[1] = colors[characterSpace >> 4];
            col_rgba[2] = colors[characterSpace & 0x0F];
            col_rgba[3] = colors[colorSpace];
            multicol = true;
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            col_rgba[0] = colors[cpipe.backgroundColor[characterSpace >> 6]];
            col_rgba[1] = colors[colorSpace];
            multicol = false;
            break;
            
        case INVALID_TEXT:
            col_rgba[0] = colors[PixelEngine::BLACK];
            col_rgba[1] = colors[PixelEngine::BLACK];
            col_rgba[2] = colors[PixelEngine::BLACK];
            col_rgba[3] = colors[PixelEngine::BLACK];
            multicol = (colorSpace & 0x8 /* MC flag */);
            break;
            
        case INVALID_STANDARD_BITMAP:
            col_rgba[0] = colors[PixelEngine::BLACK];
            col_rgba[1] = colors[PixelEngine::BLACK];
            multicol = false;
            break;
            
        case INVALID_MULTICOLOR_BITMAP:
            col_rgba[0] = colors[PixelEngine::BLACK];
            col_rgba[1] = colors[PixelEngine::BLACK];
            col_rgba[2] = colors[PixelEngine::BLACK];
            col_rgba[3] = colors[PixelEngine::BLACK];
            multicol = true;
            break;
            
        default:
            assert(0);
            break;
    }
}

inline void
PixelEngine::setSingleColorPixel(unsigned pixelnr, uint8_t bit /* valid: 0, 1 */)
{
    int rgba = col_rgba[bit];
    
    if (bit)
        setForegroundPixel(pixelnr, rgba);
    else
        setBackgroundPixel(pixelnr, rgba);
}

inline void
PixelEngine::setMultiColorPixel(unsigned pixelnr, uint8_t two_bits /* valid: 00, 01, 10, 11 */)
{
    int rgba = col_rgba[two_bits];
    
    if (two_bits & 0x02)
        setForegroundPixel(pixelnr, rgba);
    else
        setBackgroundPixel(pixelnr, rgba);
}

inline void
PixelEngine::setSingleColorSpritePixel(unsigned spritenr, unsigned pixelnr, uint8_t bit)
{
    if (bit) {
        int rgba = colors[spipe.spriteColor[spritenr]];
        setSpritePixel(pixelnr, rgba, spritenr);
    }
}

inline void
PixelEngine::setMultiColorSpritePixel(unsigned spritenr, unsigned pixelnr, uint8_t two_bits)
{
    int rgba;
    
    switch (two_bits) {
        case 0x01:
            rgba = colors[spipe.spriteExtraColor1];
            setSpritePixel(pixelnr, rgba, spritenr);
            break;
            
        case 0x02:
            rgba = colors[spipe.spriteColor[spritenr]];
            setSpritePixel(pixelnr, rgba, spritenr);
            break;
            
        case 0x03:
            rgba = colors[spipe.spriteExtraColor2];
            setSpritePixel(pixelnr, rgba, spritenr);
            break;
    }
}


inline void
PixelEngine::setSpritePixel(unsigned pixelnr, int color, int nr)
{
    uint8_t mask = (1 << nr);
    
    // Check sprite/sprite collision
    if (vic->spriteSpriteCollisionEnabled && (pixelSource[pixelnr] & 0x7F)) {
        vic->iomem[0x1E] |= ((pixelSource[pixelnr] & 0x7F) | mask);
        vic->triggerIRQ(4);
    }
        
    // Check sprite/background collision
    if (vic->spriteBackgroundCollisionEnabled && (pixelSource[pixelnr] & 0x80)) {
        vic->iomem[0x1F] |= mask;
        vic->triggerIRQ(2);
    }
        
    if (nr == 7)
        mask = 0;
        
    setSpritePixel(pixelnr, color, vic->spriteDepth(nr), mask);
}


// -----------------------------------------------------------------------------------------------
//                        Low level drawing (pixel buffer access)
// -----------------------------------------------------------------------------------------------

inline void
PixelEngine::setFramePixel(unsigned pixelnr, int rgba)
{
    unsigned offset = bufferoffset + pixelnr;
    assert(offset < NTSC_PIXELS);
    
    pixelBuffer[offset] = rgba;
    zBuffer[pixelnr] = BORDER_LAYER_DEPTH;
    pixelSource[pixelnr] &= (~0x80); // disable sprite/foreground collision detection in border
}

inline void
PixelEngine::setForegroundPixel(unsigned pixelnr, int rgba)
{
    unsigned offset = bufferoffset + pixelnr;
    assert(offset < NTSC_PIXELS);

    // The zBuffer check is not necessary as the canvas pixels are the first to draw
    // if (FOREGROUND_LAYER_DEPTH <= zBuffer[offset])
    {
        pixelBuffer[offset] = rgba;
        zBuffer[pixelnr] = FOREGROUND_LAYER_DEPTH;
        pixelSource[pixelnr] = 0x80;
    }
}

inline void
PixelEngine::setBackgroundPixel(unsigned pixelnr, int rgba)
{
    unsigned offset = bufferoffset + pixelnr;
    assert(offset < NTSC_PIXELS);

    // The zBuffer check is not necessary as the canvas pixels are the first to draw
    // if (BACKGROUD_LAYER_DEPTH <= zBuffer[offset])
    {
        pixelBuffer[offset] = rgba;
        zBuffer[pixelnr] = BACKGROUD_LAYER_DEPTH;
        pixelSource[pixelnr] = 0x00;
    }

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
    
    if (c64->isPAL()) {
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
    
    int rgba = colors[color];
    for (unsigned i = start; i < end; i++) {
        pixelBuffer[start + i] = rgba;
    }	
}
