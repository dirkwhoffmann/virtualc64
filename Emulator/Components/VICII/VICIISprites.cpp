// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"

namespace vc64 {

void
VICII::drawSprites()
{
    assert(!isFirstDMAcycle);
    assert(!isSecondDMAcycle);
    
    if ((delay & VICUpdateRegisters) || VICII_SAFE_MODE == 1) {
        drawSpritesSlowPath();
    } else {
        drawSpritesFastPath();
    }
}

//
// Fast path
//

void
VICII::drawSpritesFastPath()
{    
    if (VICII_STATS) stats.spriteFastPath++;
    
    // Prepare for collision detection
    for (isize i = 0; i < 8; i++) collision[i] = 0;
    
    // Iterate through all 8 sprites
    for (isize i = 0; i < 8; i++) {

        bool enable = GET_BIT(spriteDisplay, i);
        bool active = GET_BIT(spriteSrActive, i);
        
        // Skip this sprite if there is nothing to draw
        if (!enable && !active) continue;

        if (GET_BIT(reg.delayed.sprMC, i)) {
            
            // Draw multicolor sprite
            drawSpriteNr <true> (i, enable, active);
            
        } else {
            
            // Draw monocolor sprite
            drawSpriteNr <false> (i, enable, active);
        }
    }

    // Perform collision checks
    checkCollisions();
}

template <bool multicolor> void
VICII::drawSpriteNr(isize nr, bool enable, bool active)
{
    bool xExp = GET_BIT(reg.delayed.sprExpandX, nr);

    for (isize pixel = 0; pixel < 8; pixel++) {

        /* If a sprite is enabled, activate the shift register if the
         * horizontal trigger condition holds.
         */
        if (enable) {
            if (!active && xCounter + pixel == reg.delayed.sprX[nr]) {
                
                SET_BIT(spriteSrActive, nr);
                active = true;
                spriteSr[nr].expFlop = true;
                spriteSr[nr].mcFlop = true;
            }
        }
        
        if (active) {
            
            // Only proceed if the expansion flipflop is set
            if (spriteSr[nr].expFlop) {
                
                // Extract color bits from the shift register
                if (multicolor) {
                    
                    // In multi-color mode, get 2 bits every second pixel
                    if (spriteSr[nr].mcFlop) {
                        spriteSr[nr].colBits = (spriteSr[nr].data >> 22) & 0x03;
                    }
                    spriteSr[nr].mcFlop = !spriteSr[nr].mcFlop;
                    
                } else {
                    
                    // In single-color mode, get a new bit for each pixel
                    spriteSr[nr].colBits = (spriteSr[nr].data >> 22) & 0x02;
                }
                
                // Perform the shift operation
                spriteSr[nr].data <<= 1;
                
                // Inactivate shift register if everything is pumped out
                if (!spriteSr[nr].data && !spriteSr[nr].colBits) {
                    active = false;
                    CLR_BIT(spriteSrActive, nr);
                }
            }
            
            // Toggle expansion flipflop for horizontally stretched sprites
            spriteSr[nr].expFlop = !spriteSr[nr].expFlop || !xExp;
            
            // Draw pixel
            if (spriteSr[nr].colBits && !config.hideSprites) {
                
                // Only draw the pixel if no other sprite pixel has been drawn yet
                if (!collision[pixel]) {
                    
                    u8 color =
                    spriteSr[nr].colBits == 1 ? reg.delayed.colors[VICIIColorReg::SPR_EX1] :
                    spriteSr[nr].colBits == 2 ? reg.delayed.colors[VICIIColorReg::SPR_0 + nr] :
                    reg.delayed.colors[VICIIColorReg::SPR_EX2];
                    
                    SET_SPRITE_PIXEL(nr, pixel, color);
                }
                collision[pixel] |= (1 << nr);
            }
        }
    }
}

//
// Slow path
//

void
VICII::drawSpritesSlowPath()
{
    if (VICII_STATS) stats.spriteSlowPath++;
    
    // Prepare for collision detection
    for (isize i = 0; i < 8; i++) collision[i] = 0;

    u8 firstDMA = isFirstDMAcycle;
    u8 secondDMA = isSecondDMAcycle;

    //
    // Pixel 0
    //
    
    drawSpritePixel(0, spriteDisplayDelayed, secondDMA);
    
    // After the first pixel, color register changes show up
    reg.delayed.colors[VICIIColorReg::SPR_EX1] = reg.current.colors[VICIIColorReg::SPR_EX1];
    reg.delayed.colors[VICIIColorReg::SPR_EX2] = reg.current.colors[VICIIColorReg::SPR_EX2];
    for (isize i = 0; i < 8; i++) {
        reg.delayed.colors[VICIIColorReg::SPR_0 + i] = reg.current.colors[VICIIColorReg::SPR_0 + i];
    }
    
    //
    // Pixel 1, 2, 3
    //

    drawSpritePixel(1, spriteDisplayDelayed, secondDMA);
    
    // Stop shift register on the second DMA cycle
    spriteSrActive &= ~secondDMA;
    
    drawSpritePixel(2, spriteDisplayDelayed, secondDMA);
    drawSpritePixel(3, spriteDisplayDelayed, firstDMA | secondDMA);
    
    // If a shift register is loaded, the new data appears here
    updateSpriteShiftRegisters();

    //
    // Pixel 4, 5
    //

    drawSpritePixel(4, spriteDisplay, firstDMA | secondDMA);
    drawSpritePixel(5, spriteDisplay, firstDMA | secondDMA);
    
    // Changes of the X expansion bits and the priority bits show up here
    reg.delayed.sprExpandX = reg.current.sprExpandX;
    reg.delayed.sprPriority = reg.current.sprPriority;

    // Update multicolor bits if a new VICII is emulated
    u8 toggle = reg.delayed.sprMC ^ reg.current.sprMC;
    if (toggle && is856x) {
        
        // VICE:
        // BYTE next_mc_bits = vicii.regs[0x1c];
        // BYTE toggled = next_mc_bits ^ sprite_mc_bits;
        // sbuf_mc_flops ^= toggled & (~sbuf_expx_flops);
        // sprite_mc_bits = next_mc_bits;
        
        reg.delayed.sprMC = reg.current.sprMC;
        for (isize i = 0; i < 8; i++) {
            if (GET_BIT(toggle,i))
                spriteSr[i].mcFlop ^= !spriteSr[i].expFlop;
        }
    }
    
    //
    // Pixel 6
    //

    drawSpritePixel(6, spriteDisplay, firstDMA | secondDMA);
    
    // Update multicolor bits if an old VICII is emulated
    if (toggle && is656x) {
        
        reg.delayed.sprMC = reg.current.sprMC;
        for (isize i = 0; i < 8; i++) {
            if (GET_BIT(toggle,i))
                spriteSr[i].mcFlop = 0;
        }
    }
    
    // Pixel 7
    drawSpritePixel(7, spriteDisplay, firstDMA);
    
    // Perform collision checks
    checkCollisions();
}

void
VICII::drawSpritePixel(isize pixel, u8 enableBits, u8 freezeBits)
{
    if (!enableBits && !spriteSrActive) return;
    
    // Iterate over all sprites
    for (isize sprite = 0; sprite < 8; sprite++) {
        
        bool enable = GET_BIT(enableBits, sprite);
        bool active = GET_BIT(spriteSrActive, sprite);

        if (!enable && !active) {
            if (VICII_STATS) stats.quickExitHit++;
            continue;
        }
        if (VICII_STATS) stats.quickExitMiss++;
        
        bool freeze = GET_BIT(freezeBits, sprite);
        bool mCol = GET_BIT(reg.delayed.sprMC, sprite);
        bool xExp = GET_BIT(reg.delayed.sprExpandX, sprite);
        
        /* If a sprite is enabled, activate the shift register if the
         * horizontal trigger condition holds.
         */
        if (enable) {
            if (!active && xCounter + pixel == reg.delayed.sprX[sprite] && !freeze) {
                
                SET_BIT(spriteSrActive, sprite);
                active = true;
                spriteSr[sprite].expFlop = true;
                spriteSr[sprite].mcFlop = true;
            }
        }
        
        if (active) {
            
            // Run shift register
            if (!freeze) {
                
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
            if (spriteSr[sprite].colBits && !config.hideSprites) {
                
                // Only draw the pixel if no other sprite pixel has been drawn yet
                if (!collision[pixel]) {
                    
                    u8 color =
                    spriteSr[sprite].colBits == 1 ? reg.delayed.colors[VICIIColorReg::SPR_EX1] :
                    spriteSr[sprite].colBits == 2 ? reg.delayed.colors[VICIIColorReg::SPR_0 + sprite] :
                    reg.delayed.colors[VICIIColorReg::SPR_EX2];
                    
                    SET_SPRITE_PIXEL(sprite, pixel, color);
                }
                collision[pixel] |= (1 << sprite);
            }
        }
    }
}

void
VICII::checkCollisions()
{
    // Only proceed if there was any collision at all
    for (isize i = 0; i < 8; i++) {
        
        if (collision[i]) {
            
            // Check for sprite-sprite collisions (at least 2 bits must be set)
            if ((collision[i] & (collision[i] - 1)) && config.checkSSCollisions) {
                
                // Trigger an IRQ if this is the first detected collision
                if (!spriteSpriteCollision) triggerIrq(4);
                
                spriteSpriteCollision |= collision[i];
            }
            
            // Check for sprite-background collisions (z buffer bit 4 must be set)
            if ((zBuffer[bufferoffset + i] & 0x10) && config.checkSBCollisions) {
                
                // Trigger an IRQ if this is the first detected collision
                if (!spriteBackgroundColllision) triggerIrq(2);
                
                spriteBackgroundColllision |= collision[i];
            }
        }
    }
}

}
