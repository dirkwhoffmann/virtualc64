// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"

void
VICII::drawSprites()
{
    u8 firstDMA = isFirstDMAcycle;
    u8 secondDMA = isSecondDMAcycle;
    
    bool foreground[8];
    for (isize i = 0; i < 8; i++) {
        foreground[i] = zBuffer[bufferoffset + i] == FOREGROUND_LAYER_DEPTH;
        collision[i] = 0;
    }
        
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
                
        u16 newPixelSource = collision[i] | (foreground[i] ? 0x100 : 0);
        
        // Check if two or more bits are set in pixelSource
        if (newPixelSource & (newPixelSource - 1)) {
            
            // Is it a sprite/sprite collision?
            if ((newPixelSource & 0xFF) & ((newPixelSource & 0xFF) - 1)) {
                
                if (config.checkSSCollisions) {
                    
                    // Trigger an IRQ if this is the first detected collision
                    if (!spriteSpriteCollision) triggerIrq(4);
                    
                    spriteSpriteCollision |= (newPixelSource & 0xFF);
                }
            }
            
            // Is it a sprite/background collision?
            if (newPixelSource & 0x100) {
                
                if (config.checkSBCollisions) {
                    
                    // Trigger an IRQ if this is the first detected collision
                    if (!spriteBackgroundColllision) triggerIrq(2);
                    
                    spriteBackgroundColllision |= (newPixelSource & 0xFF);
                }
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
    if (!enableBits && !spriteSrActive) return;
    
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
        if (!collision[pixel]) {
            if (isVisibleColumn) COLORIZE(index, color);
            zBuffer[index] = depth;
        }
    }
    collision[pixel] |= source;
}
