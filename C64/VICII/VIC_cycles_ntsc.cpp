/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/* This program is free software; you can redistribute it and/or modify
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

#define PROCESS_DELAYED_ACTIONS \
if (unlikely(delay != 0)) { processDelayedActions(); }

void
VIC::cycle1ntsc()
{
    debug_cycle(1);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    if (verticalFrameFFsetCond) {
        p.verticalFrameFF = true;
    }
    
    // Phi1.3 Fetch
    sSecondAccess(3);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (c64->getRasterline() != 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIRQ(1);
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    
    // Phi2.5 Fetch
    sThirdAccess(3);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle2ntsc()
{
    debug_cycle(2);
    PROCESS_DELAYED_ACTIONS
    
    // Check for yCounter overflows
    if (yCounterOverflow())
        yCounter = 0;
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(3);
    pixelEngine.loadShiftRegister(3);
    pAccess(4);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (yCounter == 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIRQ(1);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR4 | SPR5));
    
    // Phi2.5 Fetch
    sFirstAccess(4);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle3ntsc()
{
    debug_cycle(3);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(4);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    
    // Phi2.5 Fetch
    sThirdAccess(4);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle4ntsc()
{
    debug_cycle(4);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(4);
    pixelEngine.loadShiftRegister(4);
    pAccess(5);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR5 | SPR6));
    
    // Phi2.5 Fetch
    sFirstAccess(5);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle5ntsc()
{
    debug_cycle(5);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(5);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    
    // Phi2.5 Fetch
    sThirdAccess(5);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle6ntsc()
{
    debug_cycle(6);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(5);
    pixelEngine.loadShiftRegister(5);
    pAccess(6);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR6 | SPR7));
    
    // Phi2.5 Fetch
    sFirstAccess(6);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle7ntsc()
{
    debug_cycle(7);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(6);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR6 | SPR7));
    
    // Phi2.5 Fetch
    sThirdAccess(6);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle8ntsc()
{
    debug_cycle(8);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(6);
    pixelEngine.loadShiftRegister(6);
    pAccess(7);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR7);
    
    // Phi2.5 Fetch
    sFirstAccess(7);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle9ntsc()
{
    debug_cycle(9);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(7);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR7);
    
    // Phi2.5 Fetch
    sThirdAccess(7);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle10ntsc()
{
    debug_cycle(10);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    preparePixelEngine();
    
    // Phi1.3 Fetch
    sFinalize(7);
    pixelEngine.loadShiftRegister(7);
    rIdleAccess();
    
    // Phi2.4 BA logic
    setBAlow(false);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle11ntsc()
{
    debug_cycle(11);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    preparePixelEngine();
    
    // Phi1.3 Fetch (first out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    setBAlow(false);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle55ntsc()
{
    debug_cycle(55);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (canvas column)
    preparePixelEngine(); // Prepare for next cycle (canvas column)
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOn();
    
    // Phi2.4 BA logic
    setBAlow(false);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle57ntsc()
{
    debug_cycle(57);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(344);
    
    // Phi1.2 Draw (border starts here)
    pixelEngine.draw(); // Draw previous cycle (last canvas column)
    preparePixelEngine(); // Prepare for next cycle (first column of right border)
    pixelEngine.sr.canLoad = false; // Leaving canvas area
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle58ntsc()
{
    debug_cycle(58);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (first column of right border)
    preparePixelEngine(); // Prepare for next cycle (column 2 of right border)
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.2 Sprite logic
    
    // Reset mc with mcbase for all sprites
    for (unsigned i = 0; i < 8; i++)
        mc[i] = mcbase[i];
    
    // Turn display on for all sprites with a matching y coordinate
    // Sprite display remains off if sprite DMA is off or sprite is disabled (register 0x15)
    oldSpriteOnOff |= spriteDmaOnOff & iomem[0x15] & compareSpriteY((uint8_t)yCounter);
    spriteOnOff.write(spriteOnOff.current() |
                        (spriteDmaOnOff & iomem[0x15] &
                         compareSpriteY((uint8_t)yCounter)));
    
    // Turn display off for all sprites that lost DMA.
    oldSpriteOnOff &= spriteDmaOnOff;
    spriteOnOff.write(spriteOnOff.current() & spriteDmaOnOff);
    
    // Phi2.3 VC/RC logic
    
    // "5. In der ersten Phase von Zyklus 58 wird geprüft, ob RC=7 ist. Wenn ja,
    //     geht die Videologik in den Idle-Zustand und VCBASE wird mit VC geladen
    //     (VC->VCBASE)." [C.B.]
    
    // "Der Übergang vom Display- in den Idle-Zustand erfolgt in Zyklus 58 einer Zeile,
    //  wenn der RC den Wert 7 hat und kein Bad-Line-Zustand vorliegt."
    
    
    if (registerRC == 7) {
        registerVCBASE = registerVC;
        if (!badLineCondition)
            displayState = false;
    }
    
    updateDisplayState();
    
    if (displayState) {
        // 3 bit overflow register
        registerRC = (registerRC + 1) & 0x07;
    }
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle59ntsc()
{
    debug_cycle(59);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (column 2 of right border)
    preparePixelEngine(); // Prepare for next cycle (column 3 of right border)
    
    // Phi1.3 Fetch
    pAccess(0);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    sFirstAccess(0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle60ntsc()
{
    debug_cycle(60);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw (last visible cycle)
    pixelEngine.draw(); // Draw previous cycle (column 3 of right border)
    preparePixelEngine(); // Prepare for next cycle (last column of right border)
    
    // Phi1.3 Fetch
    sSecondAccess(0);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sThirdAccess(0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle61ntsc()
{
    debug_cycle(61);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (last column of right border)
    pixelEngine.visibleColumn = false; // This was the last visible column
    
    // Phi1.3 Fetch
    sFinalize(0);
    pAccess(1);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sFirstAccess(1);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle62ntsc()
{
    debug_cycle(62);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(1);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sThirdAccess(1);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle63ntsc()
{
    debug_cycle(63);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(1);
    pixelEngine.loadShiftRegister(1);
    pAccess(2);
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sFirstAccess(2);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle64ntsc()
{
    debug_cycle(64);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi1.2 Draw
    // Phi1.3 Fetch
    sSecondAccess(2);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sThirdAccess(2);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle65ntsc()
{
    debug_cycle(65);
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi1.3 Fetch
    sFinalize(2);
    pixelEngine.loadShiftRegister(2);
    pAccess(3);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sFirstAccess(3);
    
    // Finalize
    updateDisplayState();
    countX();
}



