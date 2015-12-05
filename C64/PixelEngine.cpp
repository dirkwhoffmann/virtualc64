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
    name = "PixelEngine";
    
    debug(2, "  Creating PixelEngine at address %p...\n", this);
    
    currentScreenBuffer = screenBuffer1[0];
    pixelBuffer = currentScreenBuffer;
    pxbuf = currentScreenBuffer;
    zbuf = zBuffer;
    srcbuf = pixelSource;
    bufshift = 0;
    
    // Register snapshot items
    SnapshotItem items[] = {
        
        // VIC state latching
        { &dc.yCounter,             sizeof(dc.yCounter),            CLEAR_ON_RESET },
        { &dc.xCounter,             sizeof(dc.xCounter),            CLEAR_ON_RESET },
        { &dc.verticalFrameFF,      sizeof(dc.verticalFrameFF),     CLEAR_ON_RESET },
        { &dc.mainFrameFF,          sizeof(dc.mainFrameFF),         CLEAR_ON_RESET },
        { &dc.character,            sizeof(dc.character),           CLEAR_ON_RESET },
        { &dc.color,                sizeof(dc.color),               CLEAR_ON_RESET },
        { &dc.mode,                 sizeof(dc.mode),                CLEAR_ON_RESET },
        { &dc.delay,                sizeof(dc.delay),               CLEAR_ON_RESET },
        { dc.spriteX,               sizeof(dc.spriteX),             CLEAR_ON_RESET | WORD_FORMAT },
        { &dc.spriteXexpand,        sizeof(dc.spriteXexpand),       CLEAR_ON_RESET },
        { &dc.D011,                 sizeof(dc.D011),                CLEAR_ON_RESET },
        { &dc.D016,                 sizeof(dc.D016),                CLEAR_ON_RESET },
        { &dc.borderColor,          sizeof(dc.borderColor),         CLEAR_ON_RESET },
        { dc.backgroundColor,       sizeof(dc.backgroundColor),     CLEAR_ON_RESET | BYTE_FORMAT },
        { dc.spriteColor,           sizeof(dc.spriteColor),         CLEAR_ON_RESET | BYTE_FORMAT },
        { &dc.spriteExtraColor1,    sizeof(dc.spriteExtraColor1),   CLEAR_ON_RESET },
        { &dc.spriteExtraColor2,    sizeof(dc.spriteExtraColor2),   CLEAR_ON_RESET },
        { NULL,                     0,                              0 }};
    
    registerSnapshotItems(items, sizeof(items));
}

PixelEngine::~PixelEngine()
{
    debug(2, "  Releasing PixelEngine...\n");
}

void
PixelEngine::reset()
{
    VirtualComponent::reset();
    
    // Establish bindings
    vic = c64->vic;
    
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
    // Set pxbuf, zbuf, scrcbuf to the beginning of the corresponding buffers plus some horizontal shift
    if (c64->isPAL()) {
        bufshift = PAL_LEFT_BORDER_WIDTH - 28;
        pxbuf = pixelBuffer + bufshift;
        zbuf = zBuffer + bufshift;
        srcbuf = pixelSource + bufshift;
    } else {
        bufshift = NTSC_LEFT_BORDER_WIDTH - 28;
        pxbuf = pixelBuffer + bufshift;
        zbuf = zBuffer + bufshift;
        srcbuf = pixelSource + bufshift;
    }
}

void
PixelEngine::beginRasterline()
{
    // Clear z buffer. The buffer is initialized with the highest positive 8-bit value (meaning the pixel is far away)
    memset(zBuffer, SCHAR_MAX, sizeof(zBuffer));
    
    // Clear pixel source
    memset(pixelSource, 0x00, sizeof(pixelSource));

    // Prepare sprite pixel shift register
    for (unsigned i = 0; i < 8; i++) {
        sprite_sr[i].remaining_bits = -1;
        sprite_sr[i].mcol_bits = sprite_sr[i].scol_bit = 0;
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
            pxbuf = pixelBuffer + bufshift;
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

void
PixelEngine::updateBorderColorRegister()
{
    dc.borderColor = vic->getBorderColor();
}

void
PixelEngine::updateColorRegisters()
{
    dc.backgroundColor[0] = vic->getBackgroundColor();
    dc.backgroundColor[1] = vic->getExtraBackgroundColor(1);
    dc.backgroundColor[2] = vic->getExtraBackgroundColor(2);
    dc.backgroundColor[3] = vic->getExtraBackgroundColor(3);
}

void
PixelEngine::updateSpriteColorRegisters()
{
    for (unsigned i = 0; i < 8; i++)
        dc.spriteColor[i] = vic->spriteColor(i);
    dc.spriteExtraColor1 = vic->spriteExtraColor1();
    dc.spriteExtraColor2 = vic->spriteExtraColor2();
}

// -----------------------------------------------------------------------------------------------
//                          High level drawing (canvas, sprites, border)
// -----------------------------------------------------------------------------------------------

void
PixelEngine::draw()
{
    if (vic->vblank)
        return;
        
    drawBorder();
    drawCanvas();
    drawSprites();
}

void
PixelEngine::draw17()
{
    if (vic->vblank)
        return;
    
    drawBorder17();
    drawCanvas();
    drawSprites();
}

void
PixelEngine::draw55()
{
    if (vic->vblank)
        return;
    
    drawBorder55();
    drawCanvas();
    drawSprites();
}

inline void
PixelEngine::drawBorder()
{
    if (dc.mainFrameFF) {
        
        int16_t xCoord = dc.xCounter;
        setFramePixel(xCoord++, colors[dc.borderColor]);
        
        // After the first pixel has been drawn, color register changes show up
        updateBorderColorRegister();
        
        int rgba = colors[dc.borderColor];
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
    }
}

inline void
PixelEngine::drawBorder17()
{
    if (dc.mainFrameFF && !vic->mainFrameFF) {
        
        int16_t xCoord = dc.xCounter;

        // 38 column mode
        setFramePixel(xCoord++, colors[dc.borderColor]);
        
        // After the first pixel has been drawn, color register changes show up
        updateBorderColorRegister();
        
        int rgba = colors[dc.borderColor];
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord++, rgba);
        setFramePixel(xCoord, rgba);
        // No pixel 8, we draw 7 pixels, only.
        
    } else {

        // 40 column mode
        drawBorder();
    }
}

inline void
PixelEngine::drawBorder55()
{
    if (!dc.mainFrameFF && vic->mainFrameFF) {
        
        // 38 column mode
        setFramePixel(dc.xCounter+7, colors[dc.borderColor]);
        
    } else {
        
        // 40 column mode
        drawBorder();
    }
}

inline void
PixelEngine::drawCanvas()
{
    int16_t xCoord = dc.xCounter;
    
    /* "Der Sequenzer gibt die Grafikdaten in jeder Rasterzeile im Bereich der
     Anzeigespalte aus, sofern das vertikale Rahmenflipflop gelöscht ist (siehe
     Abschnitt 3.9.). Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    
    if (!dc.verticalFrameFF) {
        
        drawCanvasPixel(xCoord++, 0);
        
        // After the first pixel has been drawn, color register changes show up
        updateColorRegisters();
        
        drawCanvasPixel(xCoord++, 1);
        drawCanvasPixel(xCoord++, 2);
        drawCanvasPixel(xCoord++, 3);
        
        // After pixel 4, the one and zero bits in D016 and the one bits in D011 show up
        // This corresponds to the behavior of the color latency chip model in VICE
        dc.D016 = vic->iomem[0x16] & 0x10;  // latch 0s and 1s
        dc.D011 |= vic->iomem[0x11] & 0x60; // latch 1s
        
        drawCanvasPixel(xCoord++, 4);
        drawCanvasPixel(xCoord++, 5);
        
        // After pixel 6, the zero bits in D011 show up
        // This corresponds to the behavior of the color latency chip model in VICE
        dc.D011 &= vic->iomem[0x11] & 0x60; // latch 0s
        
        drawCanvasPixel(xCoord++, 6);
        drawCanvasPixel(xCoord, 7);
        
    } else {
        
        // "... bei gesetztem Flipflop wird die letzte aktuelle Hintergrundfarbe dargestellt."
        uint8_t bgcol = vic->getBackgroundColor();
        setEightBackgroundPixels(xCoord, colors[bgcol]);
    }
}

inline void
PixelEngine::drawCanvasPixel(int16_t offset, uint8_t pixel)
{
    // assert(pixel < 8);
    
    if (pixel == dc.delay) {
        
        // Load shift register
        sr.data = dc.data;
        
        // Remember how to synthesize pixels
        sr.latchedCharacter = dc.character;
        sr.latchedColor = dc.color;
        
        // Reset the multicolor synchronization flipflop
        sr.mc_flop = true;
    }
    
    // Determine display mode and colors
    DisplayMode mode = (DisplayMode)((dc.D011 & 0x60) | (dc.D016 & 0x10));
    loadColors(mode, sr.latchedCharacter, sr.latchedColor);
    
    // Render pixel
    if (multicol) {
        if (sr.mc_flop)
            sr.colorbits = (sr.data >> 6);
        setMultiColorPixel(offset, sr.colorbits);
    } else {
        setSingleColorPixel(offset, sr.data >> 7);
    }
    
    // Shift register and toggle multicolor flipflop
    sr.data <<= 1;
    sr.mc_flop = !sr.mc_flop;
}


inline void
PixelEngine::drawSprites()
{
    uint8_t active = vic->spriteOnOff;
    
    if (!active)
        return; // Quick exit (sprite free rasterline) 
    
    int16_t xCoord = dc.xCounter;
    updateSpriteColorRegisters();

    frozen = vic->isSecondDMAcycle;
    drawSpritePixel(xCoord++, 0);
    drawSpritePixel(xCoord++, 1);
    
    // running &= ~vic->isSecondDMAcycle;
    drawSpritePixel(xCoord++, 2);
    
    frozen = vic->isFirstDMAcycle | vic->isSecondDMAcycle;
    drawSpritePixel(xCoord++, 3);
    drawSpritePixel(xCoord++, 4);
    drawSpritePixel(xCoord++, 5);
    drawSpritePixel(xCoord++, 6);

    frozen = vic->isFirstDMAcycle;
    drawSpritePixel(xCoord, 7);
}

inline void
PixelEngine::drawSpritePixel(int16_t offset, uint8_t pixel)
{
    uint8_t active = vic->spriteOnOff;

    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(active,i)) {
            drawSpritePixel(i, offset, pixel);
        }
    }
}

void
PixelEngine::drawSpritePixel(unsigned nr, int16_t offset, uint8_t pixel)
{
    assert(nr < 8);
    assert(sprite_sr[nr].remaining_bits >= -1);
    assert(sprite_sr[nr].remaining_bits <= 24);

    if (GET_BIT(frozen, nr))
        goto draw;

    // Check for horizontal trigger condition
    if (dc.xCounter + pixel == dc.spriteX[nr] + 4) {
    
        // Make sure that shift register is only activated once per rasterline
        if (sprite_sr[nr].remaining_bits == -1) {
            sprite_sr[nr].remaining_bits = 24;
            sprite_sr[nr].exp_flop = true;
            sprite_sr[nr].mc_flop = true;
        }
    }
    
    // Clear bit buffers if shift register is empty
    if (sprite_sr[nr].remaining_bits == 0) {
        sprite_sr[nr].mcol_bits = sprite_sr[nr].scol_bit = 0;
    }
    
    // Run shift register if there are remaining pixels to draw
    if (sprite_sr[nr].remaining_bits > 0) {

        // Determine render mode (single color /multi color) and colors
        if ((sprite_sr[nr].mcol = vic->spriteIsMulticolor(nr))) { // TODO: Latch value at proper cycles. Add dc. multicol
            // Muti-color mode: Secure color bits every other cycle
            if (sprite_sr[nr].mc_flop)
                sprite_sr[nr].mcol_bits = (sprite_sr[nr].data >> 22) & 0x03;
        } else {
            // Single-color mode: Secure color bit every cycle
            sprite_sr[nr].scol_bit = (sprite_sr[nr].data >> 23) & 0x01;
        }
        
        // Toggle horizontal expansion flipflop for stretched sprites
        if (GET_BIT(dc.spriteXexpand, nr)) {
            sprite_sr[nr].exp_flop = !sprite_sr[nr].exp_flop;
        }

        // Run shift register and toggle multicolor flipflop
        if (sprite_sr[nr].exp_flop) {
            sprite_sr[nr].data <<= 1;
            sprite_sr[nr].mc_flop = !sprite_sr[nr].mc_flop;
            sprite_sr[nr].remaining_bits--;
        }
    }
    
draw:
    
    // Draw pixel (if mcol_bits and scol_bits got cleared in the code above, drawing has no effect)
    if (sprite_sr[nr].mcol)
        setMultiColorSpritePixel(nr, offset, sprite_sr[nr].mcol_bits);
    else
        setSingleColorSpritePixel(nr, offset, sprite_sr[nr].scol_bit);

    /*
    if (vic->isFirstDMAcycle)
        setSingleColorSpritePixel(3, offset, 1);
    if (vic->isSecondDMAcycle)
        setSingleColorSpritePixel(2, offset, 1);
    */
}


// -----------------------------------------------------------------------------------------------
//                         Mid level drawing (semantic pixel rendering)
// -----------------------------------------------------------------------------------------------

inline void
PixelEngine::loadColors(DisplayMode mode, uint8_t characterSpace, uint8_t colorSpace)
{
    switch (mode) {
            
        case STANDARD_TEXT:
            
            col_rgba[0] = colors[dc.backgroundColor[0]];
            col_rgba[1] = colors[colorSpace];
            multicol = false;
            break;
            
        case MULTICOLOR_TEXT:
            if (colorSpace & 0x8 /* MC flag */) {
                col_rgba[0] = colors[dc.backgroundColor[0]];
                col_rgba[1] = colors[dc.backgroundColor[1]];
                col_rgba[2] = colors[dc.backgroundColor[2]];
                col_rgba[3] = colors[colorSpace & 0x07];
                multicol = true;
            } else {
                col_rgba[0] = colors[dc.backgroundColor[0]];
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
            col_rgba[0] = colors[dc.backgroundColor[0]];
            col_rgba[1] = colors[characterSpace >> 4];
            col_rgba[2] = colors[characterSpace & 0x0F];
            col_rgba[3] = colors[colorSpace];
            multicol = true;
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            col_rgba[0] = colors[dc.backgroundColor[characterSpace >> 6]];
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
PixelEngine::setSingleColorPixel(int offset, uint8_t bit /* valid: 0, 1 */)
{
    int rgba = col_rgba[bit];
    
    if (bit)
        setForegroundPixel(offset, rgba);
    else
        setBackgroundPixel(offset, rgba);
}

inline void
PixelEngine::setMultiColorPixel(int offset, uint8_t two_bits /* valid: 00, 01, 10, 11 */)
{
    int rgba = col_rgba[two_bits];
    
    if (two_bits & 0x02)
        setForegroundPixel(offset, rgba);
    else
        setBackgroundPixel(offset, rgba);
}

inline void
PixelEngine::setSingleColorSpritePixel(unsigned nr, int offset, uint8_t bit)
{
    if (bit) {
        int rgba = colors[dc.spriteColor[nr]];
        setSpritePixel(offset, rgba, nr);
    }
}

inline void
PixelEngine::setMultiColorSpritePixel(unsigned nr, int offset, uint8_t two_bits)
{
    int rgba;
    
    switch (two_bits) {
        case 0x01:
            rgba = colors[dc.spriteExtraColor1];
            setSpritePixel(offset, rgba, nr);
            break;
            
        case 0x02:
            rgba = colors[dc.spriteColor[nr]];
            setSpritePixel(offset, rgba, nr);
            break;
            
        case 0x03:
            rgba = colors[dc.spriteExtraColor2];
            setSpritePixel(offset, rgba, nr);
            break;
    }
}


inline void
PixelEngine::setSpritePixel(int offset, int color, int nr)
{
    uint8_t mask = (1 << nr);
    
    // Check sprite/sprite collision
    if (vic->spriteSpriteCollisionEnabled && (srcbuf[offset] & 0x7F)) {
        vic->iomem[0x1E] |= ((srcbuf[offset] & 0x7F) | mask);
        vic->triggerIRQ(4);
    }
        
    // Check sprite/background collision
    if (vic->spriteBackgroundCollisionEnabled && (srcbuf[offset] & 0x80)) {
        vic->iomem[0x1F] |= mask;
        vic->triggerIRQ(2);
    }
        
    if (nr == 7)
        mask = 0;
        
    setSpritePixel(offset, color, vic->spriteDepth(nr), mask);
}


// -----------------------------------------------------------------------------------------------
//                        Low level drawing (pixel buffer access)
// -----------------------------------------------------------------------------------------------

inline void
PixelEngine::setFramePixel(int offset, int rgba)
{
    assert(offset + bufshift < NTSC_PIXELS);
    assert(&pxbuf[offset] >= &screenBuffer1[0][0] && &pxbuf[offset] < &screenBuffer1[PAL_RASTERLINES][NTSC_PIXELS] ||
           &pxbuf[offset] >= &screenBuffer2[0][0] && &pxbuf[offset] < &screenBuffer2[PAL_RASTERLINES][NTSC_PIXELS]);
    
    zbuf[offset] = BORDER_LAYER_DEPTH;
    pxbuf[offset] = rgba;
    // SPEEDUP: THE FOLLOWING LINE SHOULD NOT BE NECESSARY WHEN THE BORDER IS DRAWN FIRST
    srcbuf[offset] &= (~0x80); // disable sprite/foreground collision detection in border
}

inline void
PixelEngine::setForegroundPixel(int offset, int rgba)
{
    assert(offset + bufshift < NTSC_PIXELS);
    assert(&pxbuf[offset] >= &screenBuffer1[0][0] && &pxbuf[offset] < &screenBuffer1[PAL_RASTERLINES][NTSC_PIXELS] ||
           &pxbuf[offset] >= &screenBuffer2[0][0] && &pxbuf[offset] < &screenBuffer2[PAL_RASTERLINES][NTSC_PIXELS]);

    if (FOREGROUND_LAYER_DEPTH <= zbuf[offset]) {
        zbuf[offset] = FOREGROUND_LAYER_DEPTH;
        pxbuf[offset] = rgba;
        srcbuf[offset] |= 0x80;
    }
}

inline void
PixelEngine::setBackgroundPixel(int offset, int rgba)
{
    assert(offset + bufshift < NTSC_PIXELS);
    assert(&pxbuf[offset] >= &screenBuffer1[0][0] && &pxbuf[offset] < &screenBuffer1[PAL_RASTERLINES][NTSC_PIXELS] ||
           &pxbuf[offset] >= &screenBuffer2[0][0] && &pxbuf[offset] < &screenBuffer2[PAL_RASTERLINES][NTSC_PIXELS]);

    if (BACKGROUD_LAYER_DEPTH <= zbuf[offset]) {
        zbuf[offset] = BACKGROUD_LAYER_DEPTH;
        pxbuf[offset] = rgba;
    }
}

void
PixelEngine::setSpritePixel(int offset, int rgba, int depth, int source)
{
    assert (depth >= SPRITE_LAYER_FG_DEPTH && depth <= SPRITE_LAYER_BG_DEPTH + 8);
    assert(offset + bufshift < NTSC_PIXELS);
    assert(&pxbuf[offset] >= &screenBuffer1[0][0] && &pxbuf[offset] < &screenBuffer1[PAL_RASTERLINES][NTSC_PIXELS] ||
           &pxbuf[offset] >= &screenBuffer2[0][0] && &pxbuf[offset] < &screenBuffer2[PAL_RASTERLINES][NTSC_PIXELS]);
    
    /*
    if (offset + bufshift >= NTSC_PIXELS)
        return;
    */
    
    if (depth <= zbuf[offset]) {
        zbuf[offset] = depth;
        pxbuf[offset] = rgba;
    }
    srcbuf[offset] |= source;
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
