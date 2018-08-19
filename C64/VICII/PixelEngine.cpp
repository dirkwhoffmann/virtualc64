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
        { &pipe.registerCTRL1,      sizeof(pipe.registerCTRL1),     CLEAR_ON_RESET },
        { &pipe.previousCTRL1,      sizeof(pipe.previousCTRL1),     CLEAR_ON_RESET },
        { &pipe.registerCTRL2,      sizeof(pipe.registerCTRL2),     CLEAR_ON_RESET },
        { &pipe.g_data,             sizeof(pipe.g_data),            CLEAR_ON_RESET },
        { &pipe.g_character,        sizeof(pipe.g_character),       CLEAR_ON_RESET },
        { &pipe.g_color,            sizeof(pipe.g_color),           CLEAR_ON_RESET },
        { &pipe.mainFrameFF,        sizeof(pipe.mainFrameFF),       CLEAR_ON_RESET },
        { &pipe.verticalFrameFF,    sizeof(pipe.verticalFrameFF),   CLEAR_ON_RESET },
        
        { &pipe.borderColor,        sizeof(pipe.borderColor),       CLEAR_ON_RESET },
        { cpipe.backgroundColor,    sizeof(cpipe.backgroundColor),  CLEAR_ON_RESET | BYTE_FORMAT },
        { &displayMode,             sizeof(displayMode),            CLEAR_ON_RESET },
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

uint8_t
PixelEngine::readColorRegister(uint16_t addr)
{
    switch (addr) {
            
        case REG_BORDER_COL:
            return pipe.borderColor;
            
        case REG_BG_COL:
            return cpipe.backgroundColor[0];
            
        case REG_EXT1_COL:
            return cpipe.backgroundColor[1];
            
        case REG_EXT2_COL:
            return cpipe.backgroundColor[2];
            
        case REG_EXT3_COL:
            return cpipe.backgroundColor[3];
            
        case REG_SPR_MC1_COL:
            return vic->spriteExtraColor1;
            
        case REG_SPR_MC2_COL:
            return vic->spriteExtraColor2;
            
        case REG_SPR1_COL:
            return vic->spriteColor[0];
            
        case REG_SPR2_COL:
            return vic->spriteColor[1];
            
        case REG_SPR3_COL:
            return vic->spriteColor[2];
            
        case REG_SPR4_COL:
            return vic->spriteColor[3];
            
        case REG_SPR5_COL:
            return vic->spriteColor[4];
            
        case REG_SPR6_COL:
            return vic->spriteColor[5];
            
        case REG_SPR7_COL:
            return vic->spriteColor[6];
            
        case REG_SPR8_COL:
            return vic->spriteColor[7];
            
        default:
            assert(false);
    }
}

uint8_t
PixelEngine::readColorRegister(uint16_t addr, unsigned pixelNr)
{
    assert(pixelNr < 8);
    return (pixelNr == 0 && grayDot(addr)) ? VICII_LIGHT_GREY : readColorRegister(addr);
}

bool
PixelEngine::grayDot(uint16_t addr)
{
    assert(addr >= REG_BORDER_COL && addr <= REG_SPR8_COL);
    
    return
    c64->vic.isCurrentlyWrittenTo(addr) &&
    c64->vic.hasGrayDotBug() &&
    c64->vic.emulateGrayDotBug;
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
        
        // Determine color for pixel 0 (old register value is taken)
        uint64_t oldRegValue = vic->borderColor.read();
        int rgb0 = rgbaTable[oldRegValue & 0x0F];
        
        // Determine color for pixels 1 to 7 (new register value shows up)
        uint64_t newRegValue = vic->borderColor.readWithDelay(0);
        int rgba1234567 = rgbaTable[(newRegValue >> 8) & 0x0F];
        
        drawFramePixel(0, rgb0);
        drawFramePixels(1, 7, rgba1234567);
    }
}

void
PixelEngine::drawBorder17()
{
    if (pipe.mainFrameFF && !vic->p.mainFrameFF) {
        
        // 38 column mode
        int rgba = rgbaTable[pipe.borderColor];
        // setFramePixel(0, rgba);
        drawFramePixel(0, rgba);
        
        // After the first pixel has been drawn, color register changes show up
        rgba = rgbaTable[vic->p.borderColor];
        
        /*
        setFramePixel(1, rgba);
        setFramePixel(2, rgba);
        setFramePixel(3, rgba);
        setFramePixel(4, rgba);
        setFramePixel(5, rgba);
        setFramePixel(6, rgba);
        */
        drawFramePixels(1, 6, rgba);
        // That's all, we only draw 7 pixels here
        
    } else {

        // 40 column mode
        drawBorder();
    }
}

void
PixelEngine::drawBorder55()
{
    if (!pipe.mainFrameFF && vic->p.mainFrameFF) {
        
        // 38 column mode
        int rgba = readColorRegisterRGBA(REG_BORDER_COL);
        assert(rgba == rgbaTable[pipe.borderColor]);
        // setFramePixel(7, rgba);
        drawFramePixel(7, rgba);
        
    } else {
        
        // 40 column mode
        drawBorder();
    }
}

void
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
        
        if (!(pipe.registerCTRL2 & 0x10) && (vic->p.registerCTRL2 & 0x10)) {
            sr.mc_flop = false;
        }
        // After pixel 7, D016 changes fully show up
        pipe.registerCTRL2 |= D016;
        pipe.registerCTRL2 &= D016 | 0xEF;

        drawCanvasPixel(7);
        
    } else {
        
        // "... bei gesetztem Flipflop wird die letzte aktuelle Hintergrundfarbe dargestellt."
        int col = rgbaTable[vic->getBackgroundColor()];
        // The following fix (which was done for border-bm-idle is wrong)
        // int col = col_rgba[0];
        setEightBackgroundPixels(col);
    }
}

void
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
        
        sr.remaining_bits = 8;
    }
    
    // Clear any outstanding multicolor bit that shouldn't actually be drawn
    // TODO: VICE doesn't use a counter for this, but doesn't have the same issue,
    // figure out what magic they are doing
    if (!sr.remaining_bits) {
        sr.colorbits = 0;
    }
    
    // Load colors
    loadColors((DisplayMode)displayMode, sr.latchedCharacter, sr.latchedColor);
    
    // Render pixel
    bool multicolorDisplayMode = (displayMode & 0x10) && ((displayMode & 0x20) || (sr.latchedColor & 0x8));
    bool generateMulticolorPixel = (pipe.registerCTRL2 & 0x10) && ((displayMode & 0x20) || (sr.latchedColor & 0x8));
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
        setMultiColorPixel(pixelnr, sr.colorbits);
    } else {
        setSingleColorPixel(pixelnr, sr.colorbits);
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
PixelEngine::loadColors(DisplayMode mode, uint8_t characterSpace, uint8_t colorSpace)
{
    switch (mode) {
            
        assert(rgbaTable[cpipe.backgroundColor[0]] == readColorRegisterRGBA(REG_BG_COL));
        assert(rgbaTable[cpipe.backgroundColor[1]] == readColorRegisterRGBA(REG_EXT1_COL));
        assert(rgbaTable[cpipe.backgroundColor[2]] == readColorRegisterRGBA(REG_EXT2_COL));
        assert(rgbaTable[cpipe.backgroundColor[3]] == readColorRegisterRGBA(REG_EXT3_COL));

        case STANDARD_TEXT:
            
            // col_rgba[0] = colors[cpipe.backgroundColor[0]];
            col_rgba[0] = readColorRegisterRGBA(REG_BG_COL);
            col_rgba0[0] = readColorRegisterRGBA(REG_BG_COL, 0);
            col_rgba[1] = col_rgba0[1] = rgbaTable[colorSpace];
            break;
            
        case MULTICOLOR_TEXT:
            if (colorSpace & 0x8 /* MC flag */) {
                // col_rgba[0] = colors[cpipe.backgroundColor[0]];
                // col_rgba[1] = colors[cpipe.backgroundColor[1]];
                // col_rgba[2] = colors[cpipe.backgroundColor[2]];
                col_rgba[0] = readColorRegisterRGBA(REG_BG_COL);
                col_rgba0[0] = readColorRegisterRGBA(REG_BG_COL, 0);
                col_rgba[1] = readColorRegisterRGBA(REG_EXT1_COL);
                col_rgba0[1] = readColorRegisterRGBA(REG_EXT1_COL, 0);
                col_rgba[2] = readColorRegisterRGBA(REG_EXT2_COL);
                col_rgba0[2] = readColorRegisterRGBA(REG_EXT2_COL, 0);
                col_rgba[3] = col_rgba0[3] = rgbaTable[colorSpace & 0x07];
            } else {
                // col_rgba[0] = colors[cpipe.backgroundColor[0]];
                col_rgba[0] = readColorRegisterRGBA(REG_BG_COL);
                col_rgba0[0] = readColorRegisterRGBA(REG_BG_COL, 0);
                col_rgba[1] = col_rgba0[1] = rgbaTable[colorSpace];
            }
            break;
            
        case STANDARD_BITMAP:
            col_rgba[0] = rgbaTable[characterSpace & 0x0F]; // color of '0' pixels
            col_rgba[1] = rgbaTable[characterSpace >> 4]; // color of '1' pixels
            break;
            
        case MULTICOLOR_BITMAP:
            // col_rgba[0] = colors[cpipe.backgroundColor[0]];
            col_rgba[0] = readColorRegisterRGBA(REG_BG_COL);
            col_rgba0[0] = readColorRegisterRGBA(REG_BG_COL, 0);
            col_rgba[1] = col_rgba0[1] = rgbaTable[characterSpace >> 4];
            col_rgba[2] = col_rgba0[2] = rgbaTable[characterSpace & 0x0F];
            col_rgba[3] = col_rgba0[3] = rgbaTable[colorSpace];
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            // col_rgba[0] = colors[cpipe.backgroundColor[characterSpace >> 6]];
            col_rgba[0] = readColorRegisterRGBA(REG_BG_COL + (characterSpace >> 6));
            col_rgba0[0] = readColorRegisterRGBA(REG_BG_COL + (characterSpace >> 6), 0);
            assert(col_rgba[0] == rgbaTable[cpipe.backgroundColor[characterSpace >> 6]]);
            col_rgba[1] = col_rgba0[1] = rgbaTable[colorSpace];
            break;
            
        case INVALID_TEXT:
            col_rgba[0] = rgbaTable[VICII_BLACK];
            col_rgba[1] = rgbaTable[VICII_BLACK];
            col_rgba[2] = rgbaTable[VICII_BLACK];
            col_rgba[3] = rgbaTable[VICII_BLACK];
            break;
            
        case INVALID_STANDARD_BITMAP:
            col_rgba[0] = rgbaTable[VICII_BLACK];
            col_rgba[1] = rgbaTable[VICII_BLACK];
            break;
            
        case INVALID_MULTICOLOR_BITMAP:
            col_rgba[0] = rgbaTable[VICII_BLACK];
            col_rgba[1] = rgbaTable[VICII_BLACK];
            col_rgba[2] = rgbaTable[VICII_BLACK];
            col_rgba[3] = rgbaTable[VICII_BLACK];
            break;
            
        default:
            assert(0);
            break;
    }
}

void
PixelEngine::setSingleColorPixel(unsigned pixelnr, uint8_t bit /* valid: 0, 1 */)
{
    int rgba = (pixelnr == 0) ? col_rgba0[bit] : col_rgba[bit];
    
    if (bit)
        setForegroundPixel(pixelnr, rgba);
    else
        setBackgroundPixel(pixelnr, rgba);
}

void
PixelEngine::setMultiColorPixel(unsigned pixelnr, uint8_t two_bits /* valid: 00, 01, 10, 11 */)
{
    int rgba = (pixelnr == 0) ? col_rgba0[two_bits] : col_rgba[two_bits];
    
    if (two_bits & 0x02)
        setForegroundPixel(pixelnr, rgba);
    else
        setBackgroundPixel(pixelnr, rgba);
}

void
PixelEngine::setSingleColorSpritePixel(unsigned spritenr, unsigned pixelnr, uint8_t bit)
{
    if (bit) {
        int oldrgba = rgbaTable[vic->spriteColor[spritenr]];
        int rgba = readColorRegisterRGBA(REG_SPR1_COL + spritenr, pixelnr);
        assert(oldrgba == rgba);
        setSpritePixel(pixelnr, rgba, spritenr);
    }
}

void
PixelEngine::setMultiColorSpritePixel(unsigned spritenr, unsigned pixelnr, uint8_t two_bits)
{
    int rgba, oldrgba;
   
    switch (two_bits) {
        case 0x01:
            oldrgba = rgbaTable[vic->spriteExtraColor1];
            rgba = readColorRegisterRGBA(REG_SPR_MC1_COL);
            assert(oldrgba == rgba);
            setSpritePixel(pixelnr, rgba, spritenr);
            break;
            
        case 0x02:
            oldrgba = rgbaTable[vic->spriteColor[spritenr]];
            rgba = readColorRegisterRGBA(REG_SPR1_COL + spritenr, pixelnr);
            assert(oldrgba == rgba);
            setSpritePixel(pixelnr, rgba, spritenr);
            break;
            
        case 0x03:
            oldrgba = rgbaTable[vic->spriteExtraColor2];
            rgba = readColorRegisterRGBA(REG_SPR_MC2_COL);
            assert(oldrgba == rgba);
            setSpritePixel(pixelnr, rgba, spritenr);
            break;
    }
}

void
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


//
// Low level drawing (pixel buffer access)
//

void
PixelEngine::drawFramePixels(unsigned first, unsigned last, int rgba)
{
    assert(bufferoffset + last < NTSC_PIXELS);
    
    for (unsigned pixelNr = first; pixelNr <= last; pixelNr++) {
        
        pixelBuffer[bufferoffset + pixelNr] = rgba;
        zBuffer[pixelNr] = BORDER_LAYER_DEPTH;

        // Disable sprite/foreground collision detection in border
        pixelSource[pixelNr] &= (~0x80);
    }
}

/*
void
PixelEngine::setFramePixel(unsigned pixelnr, int rgba)
{
    unsigned offset = bufferoffset + pixelnr;
    assert(offset < NTSC_PIXELS);
    
    pixelBuffer[offset] = rgba;
    zBuffer[pixelnr] = BORDER_LAYER_DEPTH;
    pixelSource[pixelnr] &= (~0x80); // disable sprite/foreground collision detection in border
}
*/

void
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

void
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
