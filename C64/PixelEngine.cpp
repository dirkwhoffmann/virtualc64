//
//  PixelEngine.cpp
/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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
    
    // this->c64 = c64;
    
    // Delete screen buffers
    for (unsigned i = 0; i < sizeof(screenBuffer1) / sizeof(int); i++) {
        screenBuffer1[i] = colors[BLUE];
    }
    for (unsigned i = 0; i < sizeof(screenBuffer2) / sizeof(int); i++) {
        screenBuffer2[i] = colors[BLUE];
    }
    currentScreenBuffer = screenBuffer1;
    pixelBuffer = currentScreenBuffer;
    
    // Initialize colors
    setColorScheme(CCS64);
}


PixelEngine::~PixelEngine()
{
    debug(2, "  Releasing PixelEngine...\n");
}

void
PixelEngine::reset(C64 *c64)
{
    this->c64 = c64;
    reset();
}

void
PixelEngine::reset()
{
    debug(2, "  Resetting PixelEngine...\n");
    
    // Establish bindungs
    vic = c64->vic;
    
    // Shift register
    memset(&sr, 0x00, sizeof(sr));
}


void
PixelEngine::beginFrame()
{
    
}

void
PixelEngine::beginRasterline()
{
    // Clear z buffer. The buffer is initialized with a high, positive value (meaning the pixel is far away)
    // TODO: WHY DON'T WE USE 0xFF? PLEASE CHECK
    memset(zBuffer, 0x7f, sizeof(zBuffer));
    
    // Clear pixel source
    memset(pixelSource, 0x00, sizeof(pixelSource));
    
    // Clear pixel buffer
    // TODO: THIS MIGHT NOT BE NECESSARY AS EACH PIXEL GETS OVERWRITTEN
    memset(pixelBuffer, 0x00, sizeof(pixelSource));
    
    // Reset shift register ( TODO: DO WE REALLY NEED THIS?)
    sr.data = 0;
}

void
PixelEngine::endRasterline()
{
    // Advance pixelBuffer one line
    pixelBuffer += vic->totalScreenWidth;
    
    // Hopefully, we never write outside one of the two screen buffers
    assert(pixelBuffer - screenBuffer1 < 511*512 || pixelBuffer - screenBuffer2 < 511*512);
}

void
PixelEngine::endFrame()
{
    // Switch active screen buffer
    currentScreenBuffer = (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1;
    pixelBuffer = currentScreenBuffer;
   
}

// -----------------------------------------------------------------------------------------------
//                                   VIC state latching
// -----------------------------------------------------------------------------------------------

#if 0
void
PixelEngine::prepareForCycle(uint8_t cycle)
{
    dc.cycle = cycle;
    dc.yCounter = vic->yCounter;
    dc.xCounter = vic->xCounter;
    dc.verticalFrameFF = vic->verticalFrameFF;
    dc.mainFrameFF = vic->mainFrameFF;
    dc.data = vic->g_data;
    dc.character = vic->g_character;
    dc.color = vic->g_color;
    dc.mode = vic->g_mode;
    dc.delay = vic->getHorizontalRasterScroll();
}
#endif

void
PixelEngine::updateColorRegisters()
{
    dc.borderColor = vic->getBorderColor();
    dc.backgroundColor[0] = vic->getBackgroundColor();
    dc.backgroundColor[1] = vic->getExtraBackgroundColor(1);
    dc.backgroundColor[2] = vic->getExtraBackgroundColor(2);
    dc.backgroundColor[3] = vic->getExtraBackgroundColor(3);
}

// -----------------------------------------------------------------------------------------------
//                          High level drawing (canvas, sprites, border)
// -----------------------------------------------------------------------------------------------

void
PixelEngine::draw()
{
    drawCanvas();
    // TODO: drawSprites()
    drawBorder();
}

inline void
PixelEngine::drawCanvas()
{    
    uint16_t xCoord = (dc.xCounter - 28) + vic->leftBorderWidth;
    
    /* "Der Sequenzer gibt die Grafikdaten in jeder Rasterzeile im Bereich der
     Anzeigespalte aus, sofern das vertikale Rahmenflipflop gelöscht ist (siehe
     Abschnitt 3.9.). Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    
    if (!dc.verticalFrameFF) {
        
        drawCanvasPixel(xCoord, 0);
        
        // After pixel 1, color register changes show up
        updateColorRegisters();
        
        drawCanvasPixel(xCoord + 1, 1);
        drawCanvasPixel(xCoord + 2, 2);
        drawCanvasPixel(xCoord + 3, 3);
        
        // After pixel 4, the one and zero bits in D016 and the one bits in D011 show up
        // This corresponds the behavior of the color latency chip model in VICE
        dc.D016 = vic->iomem[0x16] & 0x10;  // latch 0s and 1s
        dc.D011 |= vic->iomem[0x11] & 0x60; // latch 1s
        
        drawCanvasPixel(xCoord + 4, 4);
        drawCanvasPixel(xCoord + 5, 5);
        
        // After pixel 6, the zero bits in D011 show up
        // This corresponds the behavior of the color latency chip model in VICE
        dc.D011 &= vic->iomem[0x11] & 0x60; // latch 0s
        
        drawCanvasPixel(xCoord + 6, 6);
        drawCanvasPixel(xCoord + 7, 7);
        
    } else {
        
        // "... bei gesetztem Flipflop wird die letzte aktuelle Hintergrundfarbe dargestellt."
        uint8_t bgcol = vic->getBackgroundColor();
        setEightBackgroundPixels(xCoord, colors[bgcol]);
    }
}

inline void
PixelEngine::drawCanvasPixel(uint16_t offset, uint8_t pixel)
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
    
    // Shift register and toggle flipflop
    sr.data <<= 1;
    sr.mc_flop = !sr.mc_flop;
}

void
PixelEngine::drawAllSprites()
{
    if (vic->drawSprites) {
        for (int i = 0; i < 8; i++) {
            if (vic->oldSpriteOnOff & (1 << i)) {
                drawSprite(i);
            }
        }
    }
}

void
PixelEngine::drawSprite(uint8_t nr)
{
    assert(nr < 8);
    
    int spriteX, offset;
    spriteX = vic->getSpriteX(nr);
    
    if (spriteX < 488)
        offset = spriteX + (vic->leftBorderWidth - 24);
    else
        offset = spriteX + (vic->leftBorderWidth - 24) - 488;
    
    if (vic->spriteIsMulticolor(nr)) {
        
        int colorLookup[4] = {
            0x00,
            colors[vic->spriteExtraColor1()],
            colors[vic->spriteColor(nr)],
            colors[vic->spriteExtraColor2()]
        };
        
        for (int i = 0; i < 3; i++) {
            uint8_t pattern = vic->spriteShiftReg[nr][i];
            
            uint8_t col;
            if (vic->spriteWidthIsDoubled(nr)) {
                col = (pattern >> 6) & 0x03;
                if (col) {
                    setSpritePixel(offset, colorLookup[col], nr);
                    setSpritePixel(offset+1, colorLookup[col], nr);
                    setSpritePixel(offset+2, colorLookup[col], nr);
                    setSpritePixel(offset+3, colorLookup[col], nr);
                }
                col = (pattern >> 4) & 0x03;
                if (col) {
                    setSpritePixel(offset+4, colorLookup[col], nr);
                    setSpritePixel(offset+5, colorLookup[col], nr);
                    setSpritePixel(offset+6, colorLookup[col], nr);
                    setSpritePixel(offset+7, colorLookup[col], nr);
                }
                col = (pattern >> 2) & 0x03;
                if (col) {
                    setSpritePixel(offset+8, colorLookup[col], nr);
                    setSpritePixel(offset+9, colorLookup[col], nr);
                    setSpritePixel(offset+10, colorLookup[col], nr);
                    setSpritePixel(offset+11, colorLookup[col], nr);
                }
                col = pattern & 0x03;
                if (col) {
                    setSpritePixel(offset+12, colorLookup[col], nr);
                    setSpritePixel(offset+13, colorLookup[col], nr);
                    setSpritePixel(offset+14, colorLookup[col], nr);
                    setSpritePixel(offset+15, colorLookup[col], nr);
                }
                offset += 16;
            } else {
                col = (pattern >> 6) & 0x03;
                if (col) {
                    setSpritePixel(offset, colorLookup[col], nr);
                    setSpritePixel(offset+1, colorLookup[col], nr);
                }
                col = (pattern >> 4) & 0x03;
                if (col) {
                    setSpritePixel(offset+2, colorLookup[col], nr);
                    setSpritePixel(offset+3, colorLookup[col], nr);
                }
                col = (pattern >> 2) & 0x03;
                if (col) {
                    setSpritePixel(offset+4, colorLookup[col], nr);
                    setSpritePixel(offset+5, colorLookup[col], nr);
                }
                col = pattern & 0x03;
                if (col) {
                    setSpritePixel(offset+6, colorLookup[col], nr);
                    setSpritePixel(offset+7, colorLookup[col], nr);
                }
                offset += 8;
            }
        }
    } else {
        int fgcolor = colors[vic->spriteColor(nr)];
        for (int i = 0; i < 3; i++) {
            uint8_t pattern = vic->spriteShiftReg[nr][i];
            
            if (vic->spriteWidthIsDoubled(nr)) {
                if (pattern & 128) {
                    setSpritePixel(offset, fgcolor, nr);
                    setSpritePixel(offset+1, fgcolor, nr);
                }
                if (pattern & 64) {
                    setSpritePixel(offset+2, fgcolor, nr);
                    setSpritePixel(offset+3, fgcolor, nr);
                }
                if (pattern & 32) {
                    setSpritePixel(offset+4, fgcolor, nr);
                    setSpritePixel(offset+5, fgcolor, nr);
                }
                if (pattern & 16) {
                    setSpritePixel(offset+6, fgcolor, nr);
                    setSpritePixel(offset+7, fgcolor, nr);
                }
                if (pattern & 8) {
                    setSpritePixel(offset+8, fgcolor, nr);
                    setSpritePixel(offset+9, fgcolor, nr);
                }
                if (pattern & 4) {
                    setSpritePixel(offset+10, fgcolor, nr);
                    setSpritePixel(offset+11, fgcolor, nr);
                }
                if (pattern & 2) {
                    setSpritePixel(offset+12, fgcolor, nr);
                    setSpritePixel(offset+13, fgcolor, nr);
                }
                if (pattern & 1) {
                    setSpritePixel(offset+14, fgcolor, nr);
                    setSpritePixel(offset+15, fgcolor, nr);
                }
                offset += 16;
            } else {
                if (pattern & 128) {
                    setSpritePixel(offset, fgcolor, nr);
                }
                if (pattern & 64) {
                    setSpritePixel(offset+1, fgcolor, nr);
                }
                if (pattern & 32) {
                    setSpritePixel(offset+2, fgcolor, nr);
                }
                if (pattern & 16) {
                    setSpritePixel(offset+3, fgcolor, nr);
                }
                if (pattern & 8) {
                    setSpritePixel(offset+4, fgcolor, nr);
                }
                if (pattern & 4) {
                    setSpritePixel(offset+5, fgcolor, nr);
                }
                if (pattern & 2) {
                    setSpritePixel(offset+6, fgcolor, nr);
                }
                if (pattern & 1) {
                    setSpritePixel(offset+7, fgcolor, nr);
                }
                offset += 8;
            }
        }
    }
}

inline void
PixelEngine::drawBorder()
{
    uint16_t xCoord = (dc.xCounter - 28) + vic->leftBorderWidth;
    
    // Take special care of 38 column mode
    
    if (dc.cycle == 17 && dc.mainFrameFF && !vic->mainFrameFF) {
        int border_rgba = colors[dc.borderColor];
        setSevenFramePixels(xCoord, border_rgba);
        return;
    }
    
    if (dc.cycle == 55 && !dc.mainFrameFF && vic->mainFrameFF) {
        int border_rgba = colors[dc.borderColor];
        setFramePixel(xCoord+7, border_rgba);
        return;
    }
    
    // Standard case
    
    if (dc.mainFrameFF) {
        setFramePixel(xCoord, colors[dc.borderColor]);
        updateColorRegisters();
        setFramePixel(xCoord + 1, colors[dc.borderColor]);
        setFramePixel(xCoord + 2, colors[dc.borderColor]);
        setFramePixel(xCoord + 3, colors[dc.borderColor]);
        setFramePixel(xCoord + 4, colors[dc.borderColor]);
        setFramePixel(xCoord + 5, colors[dc.borderColor]);
        setFramePixel(xCoord + 6, colors[dc.borderColor]);
        setFramePixel(xCoord + 7, colors[dc.borderColor]);
        return;
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
PixelEngine::setSingleColorPixel(unsigned offset, uint8_t bit /* valid: 0, 1 */)
{
    int rgba = col_rgba[bit];
    
    if (bit)
        setForegroundPixel(offset, rgba);
    else
        setBackgroundPixel(offset, rgba);
}

inline void
PixelEngine::setMultiColorPixel(unsigned offset, uint8_t two_bits /* valid: 00, 01, 10, 11 */)
{
    int rgba = col_rgba[two_bits];
    
    if (two_bits & 0x02)
        setForegroundPixel(offset, rgba);
    else
        setBackgroundPixel(offset, rgba);
}

void
PixelEngine::setSpritePixel(unsigned offset, int color, int nr)
{
    uint8_t mask = (1 << nr);
    
    if (offset < vic->totalScreenWidth) {
        
        //int depth = spriteDepth(nr);
        //if (depth < zBuffer[offset]) {
        //	pixelBuffer[offset] = color;
        //	zBuffer[offset] = depth;
        // }
        
        // Check sprite/sprite collision
        if (vic->spriteSpriteCollisionEnabled && (pixelSource[offset] & 0x7F)) {
            vic->iomem[0x1E] |= ((pixelSource[offset] & 0x7F) | mask);
            vic->triggerIRQ(4);
        }
        
        // Check sprite/background collision
        if (vic->spriteBackgroundCollisionEnabled && (pixelSource[offset] & 0x80)) {
            vic->iomem[0x1F] |= mask;
            vic->triggerIRQ(2);
        }
        
        if (nr == 7)
            mask = 0;
        
        setSpritePixel(offset, color, vic->spriteDepth(nr), mask);
    }
}


// -----------------------------------------------------------------------------------------------
//                        Low level drawing (pixel buffer access)
// -----------------------------------------------------------------------------------------------

inline void
PixelEngine::setFramePixel(unsigned offset, int rgba)
{
    zBuffer[offset] = BORDER_LAYER_DEPTH;
    pixelBuffer[offset] = rgba;
    pixelSource[offset] &= (~0x80); // disable sprite/foreground collision detection in border
}

inline void
PixelEngine::setForegroundPixel(unsigned offset, int rgba)
{
    if (FOREGROUND_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = FOREGROUND_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
        pixelSource[offset] |= 0x80;
    }
}

inline void
PixelEngine::setBackgroundPixel(unsigned offset, int rgba)
{
    if (BACKGROUD_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = BACKGROUD_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
    }
}

void
PixelEngine::setSpritePixel(unsigned offset, int rgba, int depth, int source)
{
    assert (depth >= SPRITE_LAYER_FG_DEPTH && depth <= SPRITE_LAYER_BG_DEPTH + 8);
    
    if (depth <= zBuffer[offset]) {
        zBuffer[offset] = depth;
        pixelBuffer[offset] = rgba;
    }
    pixelSource[offset] |= source;
}

void
PixelEngine::expandBorders()
{
    int color;
    unsigned leftPixelPos = (-4-28) + vic->leftBorderWidth;
    unsigned rightPixelPos = leftPixelPos+(48*8)-1;
    
    color = pixelBuffer[leftPixelPos];
    for (unsigned i = 0; i <= leftPixelPos; i++)
        // pixelBuffer[i] = colors[5];
        pixelBuffer[i] = color;
    
    color = pixelBuffer[rightPixelPos];
    for (unsigned i = rightPixelPos+1; i < vic->totalScreenWidth; i++)
        // pixelBuffer[i] = colors[5];
        pixelBuffer[i] = color;
}

void
PixelEngine::markLine(uint8_t color, unsigned start, unsigned end)
{
    assert (end <= 512);
    
    int rgba = colors[color];
    for (unsigned i = start; i < end; i++) {
        pixelBuffer[start + i] = rgba;
    }	
}
