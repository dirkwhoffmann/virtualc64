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
    zBuffer[offset] = BORDER_LAYER_DEPTH;
    pixelBuffer[offset] = rgba;
    pixelSource[offset] &= (~0x80); // disable sprite/foreground collision detection in border
}

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
PixelEngine::setBackgroundPixel(unsigned offset, int rgba)
{
    if (BACKGROUD_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = BACKGROUD_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
    }
}

void
PixelEngine::setSingleColorPixel(unsigned offset, uint8_t bit)
{
    assert(bit <= 1);
    int rgba = vic->col_rgba[bit];
    
    if (bit)
        setForegroundPixel(offset, rgba);
    else
        setBackgroundPixel(offset, rgba);
}

void
PixelEngine::setMultiColorPixel(unsigned offset, uint8_t two_bits)
{
    assert(two_bits <= 3);
    int rgba = vic->col_rgba[two_bits];
    
    if (two_bits & 0x02)
        setForegroundPixel(offset, rgba);
    else
        setBackgroundPixel(offset, rgba);
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
