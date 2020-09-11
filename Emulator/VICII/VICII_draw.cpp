// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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
    u8 d011, d016, newD016, mode, oldMode, xscroll;
    
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

    // In older VICIIs, the one bits of D011 show up, too.
    if (is656x()) {
        d011 |= reg.current.ctrl1;
    }
    oldMode = mode;
    mode = (d011 & 0x60) | (newD016 & 0x10);
    
    drawCanvasPixel(4, mode, d016, xscroll == 4, oldMode != mode);
    drawCanvasPixel(5, mode, d016, xscroll == 5, false);
    
    // In older VICIIs, the zero bits of D011 show up here.
    if (is656x()) {
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
VICII::drawCanvasPixel(u8 pixel,
                       u8 mode,
                       u8 d016,
                       bool loadShiftReg,
                       bool updateColors)
{
    assert(pixel < 8);
    
    /* "The heart of the sequencer is an 8 bit shift register that is shifted by
     *  1 bit every pixel and reloaded with new graphics data after every
     *  g-access. With XSCROLL from register $d016 the reloading can be delayed
     *  by 0-7 pixels, thus shifting the display up to 7 pixels to the right."
     */
    if (loadShiftReg && sr.canLoad) {
        
        u32 result = gAccessResult.delayed();
     
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
            SET_FOREGROUND_PIXEL(pixel, col[sr.colorbits]);
        } else {
            SET_BACKGROUND_PIXEL(pixel, col[sr.colorbits]);
        }
        
    } else {
        
        // Set single-color pixel
        if (sr.colorbits) {
            SET_FOREGROUND_PIXEL(pixel, col[sr.colorbits]);
        } else {
            SET_BACKGROUND_PIXEL(pixel, col[sr.colorbits]);
        }
    }
    
    // Shift register and toggle multicolor flipflop
    sr.data <<= 1;
    sr.mcFlop = !sr.mcFlop;
    sr.remainingBits -= 1;
}

void
VICII::drawSprites()
{
    u8 firstDMA = isFirstDMAcycle;
    u8 secondDMA = isSecondDMAcycle;
    
    // Pixel 0
    drawSpritePixel(0, spriteDisplayDelayed, secondDMA);
    
    // After the first pixel, color register changes show up
    reg.delayed.colors[COLREG_SPR_EX1] = reg.current.colors[COLREG_SPR_EX1];
    reg.delayed.colors[COLREG_SPR_EX2] = reg.current.colors[COLREG_SPR_EX2];
    for (unsigned i = 0; i < 8; i++) {
        reg.delayed.colors[COLREG_SPR0 + i] = reg.current.colors[COLREG_SPR0 + i];
    }
    
    // Pixel 1, Pixel 2, Pixel 3
    drawSpritePixel(1, spriteDisplayDelayed, secondDMA);
    
    // Stop shift register on the second DMA cycle
    spriteSrActive &= ~secondDMA;
    
    drawSpritePixel(2, spriteDisplayDelayed, secondDMA);
    drawSpritePixel(3, spriteDisplayDelayed, firstDMA | secondDMA);
    
    // If a shift register is loaded, the new data appears here.
    updateSpriteShiftRegisters();

    // Pixel 4, Pixel 5
    drawSpritePixel(4, spriteDisplay, firstDMA | secondDMA);
    drawSpritePixel(5, spriteDisplay, firstDMA | secondDMA);
    
    // Changes of the X expansion bits and the priority bits show up here
    reg.delayed.sprExpandX = reg.current.sprExpandX;
    reg.delayed.sprPriority = reg.current.sprPriority;

    // Update multicolor bits if a new VICII is emulated
    u8 toggle = reg.delayed.sprMC ^ reg.current.sprMC;
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
    drawSpritePixel(6, spriteDisplay, firstDMA | secondDMA);
    
    // Update multicolor bits if an old VICII is emulated
    if (toggle && is656x()) {
        
        reg.delayed.sprMC = reg.current.sprMC;
        for (unsigned i = 0; i < 8; i++) {
            if (GET_BIT(toggle,i))
                spriteSr[i].mcFlop = 0;
        }
    }
    
    // Pixel 7
    drawSpritePixel(7, spriteDisplay, firstDMA);
    
    // Check for collisions
    for (unsigned i = 0; i < 8; i++) {
        
        int index = bufferoffset + i;
        
        // Check if two or more bits are set in pixelSource
        if (pixelSource[index] & (pixelSource[index] - 1)) {
            
            // Is it a sprite/sprite collision?
            if ((pixelSource[index] & 0xFF) & ((pixelSource[index] & 0xFF) - 1)) {
                
                // Trigger an IRQ if this is the first detected collision
                if (!spriteSpriteCollision) {
                    triggerIrq(4);
                }
                spriteSpriteCollision |= (pixelSource[index] & 0xFF);
            }
            
            // Is it a sprite/background collision?
            if ((pixelSource[index] & 0x100) && config.checkSBCollisions) {
                
                // Trigger an IRQ if this is the first detected collision
                if (!spriteBackgroundColllision) {
                    triggerIrq(2);
                }
                spriteBackgroundColllision |= (pixelSource[index] & 0xFF);
            }
        }
    }
}

void
VICII::drawSpritePixel(unsigned pixel,
                     u8 enableBits,
                     u8 freezeBits)
{
    // Quick exit condition
    if (!enableBits && !spriteSrActive) {
        return;
    }
    
    // Iterate over all sprites
    for (unsigned sprite = 0; sprite < 8; sprite++) {
        
        bool enable = GET_BIT(enableBits, sprite);
        bool freeze = GET_BIT(freezeBits, sprite);
        bool mCol = GET_BIT(reg.delayed.sprMC, sprite);
        bool xExp = GET_BIT(reg.delayed.sprExpandX, sprite);
        bool active = GET_BIT(spriteSrActive, sprite);
        
        // If a sprite is enabled, activate it's shift register if the
        // horizontal trigger condition holds.
        if (enable) {
            if (!active && xCounter + pixel == reg.delayed.sprX[sprite] && !freeze) {
                
                SET_BIT(spriteSrActive, sprite);
                active = true;
                spriteSr[sprite].expFlop = true;
                spriteSr[sprite].mcFlop = true;
            }
        }
        
        // Run shift register if it is activated
        if (active && !freeze) {
            
            // Only proceed if the expansion flipflop is set
            if (spriteSr[sprite].expFlop) {
                
                // Extract color bits from the shift register
                if (mCol) {
                    
                    // In multi-color mode, get 2 bits every second pixel
                    if (spriteSr[sprite].mcFlop) {
                        spriteSr[sprite].colBits = (spriteSr[sprite].data >> 22) & 0x03;
                    }
                    spriteSr[sprite].mcFlop = !spriteSr[sprite].mcFlop;
                    
                } else {
                    
                    // In single-color mode, get a new bit for each pixel
                    spriteSr[sprite].colBits = (spriteSr[sprite].data >> 22) & 0x02;
                }
                
                // Perform the shift operation
                spriteSr[sprite].data <<= 1;
                
                // Inactivate shift register if everything is pumped out
                if (!spriteSr[sprite].data && !spriteSr[sprite].colBits) {
                    active = false;
                    CLR_BIT(spriteSrActive, sprite);
                }
            }
            
            // Toggle expansion flipflop for horizontally stretched sprites
            spriteSr[sprite].expFlop = !spriteSr[sprite].expFlop || !xExp;
        }
        
        // Draw pixel
        if (active && !config.hideSprites) {
            
            switch (spriteSr[sprite].colBits) {
                    
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
    }
}

void
VICII::loadColors(u8 mode)
{
    u8 character = sr.latchedCharacter;
    u8 color = sr.latchedColor;
    
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
            
            assert(false);
            break;
    }
}


//
// Low level drawing (pixel buffer access)
//

void
VICII::setSpritePixel(unsigned sprite, unsigned pixel, u8 color)
{
    u8 depth = spriteDepth(sprite);
    u8 source = (1 << sprite);
    int index = bufferoffset + pixel;
    
    if (depth <= zBuffer[index]) {
        
        /* "the interesting case is when eg sprite 1 and sprite 0 overlap, and
         *  sprite 0 has the priority bit set (and sprite 1 has not). in this
         *  case 10/11 background bits show in front of whole sprite 0."
         * Test program: VICII/spritePriorities
         */
        if (!(pixelSource[index] & 0xFF)) {
            if (isVisibleColumn) COLORIZE(index, color);
            zBuffer[index] = depth;
        }
    }
    pixelSource[index] |= source;
}

void
VICII::markLine(u8 color, unsigned start, unsigned end)
{
    assert (end < TEX_WIDTH);
    
    int rgba = rgbaTable[color];
    for (unsigned i = start; i <= end; i++) {
        emuTexturePtr[start + i] = rgba;
    }
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
