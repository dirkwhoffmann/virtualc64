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


PixelEngine::PixelEngine(C64 *c64)
{
    name = "PixelEngine";
    
    debug(2, "  Creating PixelEngine at address %p...\n", this);
    
    this->c64 = c64;
    
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
PixelEngine::reset()
{
    debug(2, "  Resetting PixelEngine...\n");
    
    // Establish bindungs
    vic = c64->vic;
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
    
    // TODO: GET RID OF THESE TWO:
    zBufferTmp[0] = zBufferTmp[1] = 0;
    pixelSourceTmp[0] = pixelSourceTmp[1] = 0;
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
//                                           Drawing
// -----------------------------------------------------------------------------------------------

void
PixelEngine::setFramePixel(unsigned offset, int rgba)
{
    assert(offset < 512);
    /*
    printf("(%d,%d) ",vic->yCounter, vic->xCounter);
    printf("pixelBuffer: %p ",pixelBuffer);
    printf("screenBuffer1,2: %p -- %p %p -- %p ",
           screenBuffer1,screenBuffer1+sizeof(screenBuffer1)-1,screenBuffer2,screenBuffer2+sizeof(screenBuffer2)-1);
    printf("currentScreenBuffer: %p ",currentScreenBuffer);
    printf("offset: %d\n",offset);
    */
    
    zBuffer[offset] = BORDER_LAYER_DEPTH;
    pixelBuffer[offset] = rgba;
    pixelSource[offset] &= (~0x80); // disable sprite/foreground collision detection in border
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

// DEPRECATED
void
PixelEngine::setForegroundPixel(unsigned offset, int rgba)
{
    if (FOREGROUND_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = FOREGROUND_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
        pixelSource[offset] |= 0x80;
    }
}

void
PixelEngine::renderForegroundPixel(unsigned offset, int rgba)
{
    assert(offset == 0 || offset == 1);
    
    zBufferTmp[offset] = FOREGROUND_LAYER_DEPTH;
    pixelBufferTmp[offset] = rgba;
    pixelSourceTmp[offset] |= 0x80;
}


// DEPRECATED
void
PixelEngine::setBackgroundPixel(unsigned offset, int rgba)
{
    if (BACKGROUD_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = BACKGROUD_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
    }
}

void
PixelEngine::renderBackgroundPixel(unsigned offset, int rgba)
{
    assert(offset == 0 || offset == 1);
    
    zBufferTmp[offset] = BACKGROUD_LAYER_DEPTH;
    pixelBufferTmp[offset] = rgba;
    pixelSourceTmp[offset] = 0;
}

// DEPRECATED
void
PixelEngine::setBehindBackgroundPixel(unsigned offset, int rgba)
{
    if (BEIND_BACKGROUND_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = BEIND_BACKGROUND_DEPTH;
        pixelBuffer[offset] = rgba;
    }
}

// FIND MORE SUITABLE NAME
void
PixelEngine::drawEightBehindBackgroudPixels(unsigned offset)
{
    /* "Der Sequenzer gibt die Grafikdaten in jeder Rasterzeile im Bereich der
     Anzeigespalte aus, sofern das vertikale Rahmenflipflop gelöscht ist (siehe
     Abschnitt 3.9.). Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    
    int bg_rgba = vic->gs_last_bg_color;
    for (unsigned i = 0; i < 8; i++) {
        setBehindBackgroundPixel(offset++, bg_rgba);
    }
}

void
PixelEngine::renderSingleColorPixel(uint8_t bit)
{
    assert(bit <= 1);
    int rgba = vic->col_rgba[bit];
    
    if (dirktrace == 1 && vic->yCounter == DIRK_DEBUG_LINE) {
        printf("      VIC::renderSingleColorPixel(%d) rgba:%d\n",bit,rgba);
    }
    
    if (bit)
        renderForegroundPixel(0, rgba);
    else
        renderBackgroundPixel(0, rgba);
}

// DEPRECATED
inline void
PixelEngine::renderTwoSingleColorPixels(uint8_t bits)
{
    if (bits & 0x02)
        renderForegroundPixel(0, vic->col_rgba[1]);
    else
        renderBackgroundPixel(0, vic->col_rgba[0]);
    
    if (bits & 0x01)
        renderForegroundPixel(1, vic->col_rgba[1]);
    else
        renderBackgroundPixel(1, vic->col_rgba[0]);
}

// EVEN MORE DEPRECATED
inline void
PixelEngine::drawTwoSingleColorPixels(unsigned offset, uint8_t bits)
{
    if (bits & 0x02)
        setForegroundPixel(offset++, vic->col_rgba[1]);
    else
        setBackgroundPixel(offset++, vic->col_rgba[0]);
    
    if (bits & 0x01)
        setForegroundPixel(offset, vic->col_rgba[1]);
    else
        setBackgroundPixel(offset, vic->col_rgba[0]);
}

// DEPRECATED
inline void
PixelEngine::drawSingleColorCharacter(unsigned offset)
{
    // int fg_rgba = colors[gs_fg_color];
    // int bg_rgba = colors[gs_bg_color];
    
    assert(offset >= 0 && offset+7 < MAX_VIEWABLE_PIXELS);
    
    drawTwoSingleColorPixels(offset, vic->gs_data >> 6);
    drawTwoSingleColorPixels(offset + 2, vic->gs_data >> 4);
    drawTwoSingleColorPixels(offset + 4, vic->gs_data >> 2);
    drawTwoSingleColorPixels(offset + 6, vic->gs_data);
}

void
PixelEngine::renderMultiColorPixel(uint8_t color_bits)
{
    assert(color_bits <= 3);
    int rgba = vic->col_rgba[color_bits];
    
    if (color_bits & 0x02)
        renderForegroundPixel(0, rgba);
    else
        renderBackgroundPixel(0, rgba);
}

// DEPRECATED
void
PixelEngine::renderTwoMultiColorPixels(uint8_t bits)
{
    int rgba = vic->col_rgba[bits & 0x03];
    
    if (bits & 0x02) {
        renderForegroundPixel(0, rgba);
        renderForegroundPixel(1, rgba);
    } else {
        renderBackgroundPixel(0, rgba);
        renderBackgroundPixel(1, rgba);
    }
}

void
PixelEngine::drawTwoMultiColorPixels(unsigned offset, uint8_t bits)
{
    int rgba = vic->col_rgba[bits & 0x03];
    
    if (bits & 0x02) {
        setForegroundPixel(offset++, rgba);
        setForegroundPixel(offset++, rgba);
    } else {
        setBackgroundPixel(offset++, rgba);
        setBackgroundPixel(offset++, rgba);
    }
}

void
PixelEngine::drawMultiColorCharacter(unsigned offset)
{
    assert(offset+7 < MAX_VIEWABLE_PIXELS);
    
    drawTwoMultiColorPixels(offset, vic->gs_data >> 6);
    drawTwoMultiColorPixels(offset + 2, vic->gs_data >> 4);
    drawTwoMultiColorPixels(offset + 4, vic->gs_data >> 2);
    drawTwoMultiColorPixels(offset + 6, vic->gs_data);
}

void
PixelEngine::drawTwoInvalidSingleColorPixels(unsigned offset, uint8_t bits)
{
    drawTwoSingleColorPixels(offset, bits);
}

void
PixelEngine::drawInvalidSingleColorCharacter(unsigned offset)
{
    assert(offset+7 < MAX_VIEWABLE_PIXELS);
    
    drawTwoInvalidSingleColorPixels(offset, vic->gs_data >> 6);
    drawTwoInvalidSingleColorPixels(offset + 2, vic->gs_data >> 4);
    drawTwoInvalidSingleColorPixels(offset + 4, vic->gs_data >> 2);
    drawTwoInvalidSingleColorPixels(offset + 6, vic->gs_data);
}

void
PixelEngine::drawTwoInvalidMultiColorPixels(unsigned offset, uint8_t bits)
{
    drawTwoMultiColorPixels(offset, bits);
}

void
PixelEngine::drawInvalidMultiColorCharacter(unsigned offset)
{
    assert(offset+7 < MAX_VIEWABLE_PIXELS);
    
    drawTwoInvalidMultiColorPixels(offset, vic->gs_data >> 6);
    drawTwoInvalidMultiColorPixels(offset + 2, vic->gs_data >> 4);
    drawTwoInvalidMultiColorPixels(offset + 4, vic->gs_data >> 2);
    drawTwoInvalidMultiColorPixels(offset + 6, vic->gs_data);
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

void
PixelEngine::markLine(uint8_t color, unsigned start, unsigned end)
{
    assert (end <= 512);
    
    int rgba = colors[color];
    for (unsigned i = start; i < end; i++) {
        pixelBuffer[start + i] = rgba;
    }	
}
