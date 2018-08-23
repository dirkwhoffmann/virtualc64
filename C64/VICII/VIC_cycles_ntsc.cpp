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

void
VIC::cycle1ntsc()
{
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
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    
    // Phi2.5 Fetch
    sThirdAccess(3);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle2ntsc()
{
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
    BA_LINE(spriteDmaOnOff & (SPR4 | SPR5));
    
    // Phi2.5 Fetch
    sFirstAccess(4);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle3ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(4);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    
    // Phi2.5 Fetch
    sThirdAccess(4);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle4ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(4);
    pixelEngine.loadShiftRegister(4);
    pAccess(5);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR5 | SPR6));
    
    // Phi2.5 Fetch
    sFirstAccess(5);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle5ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(5);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    
    // Phi2.5 Fetch
    sThirdAccess(5);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle6ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(5);
    pixelEngine.loadShiftRegister(5);
    pAccess(6);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR6 | SPR7));
    
    // Phi2.5 Fetch
    sFirstAccess(6);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle7ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(6);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR6 | SPR7));
    
    // Phi2.5 Fetch
    sThirdAccess(6);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle8ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(6);
    pixelEngine.loadShiftRegister(6);
    pAccess(7);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR7);
    
    // Phi2.5 Fetch
    sFirstAccess(7);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle9ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(7);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR7);
    
    // Phi2.5 Fetch
    sThirdAccess(7);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle10ntsc()
{
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
    BA_LINE(false);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle11ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    preparePixelEngine();
    
    // Phi1.3 Fetch (first out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(false);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle55ntsc()
{
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
    BA_LINE(false);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle57ntsc()
{
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
    BA_LINE(spriteDmaOnOff & SPR0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle58ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (first column of right border)
    preparePixelEngine(); // Prepare for next cycle (column 2 of right border)
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.2 Sprite logic
    turnSpritesOnOrOff();
    
    // Phi2.3 VC/RC logic
    
    /* "In the first phase of cycle 58, the VIC checks if RC=7. If so, the video
     *  logic goes to idle state and VCBASE is loaded from VC (VC->VCBASE)."
     *  [C.B.]
     */
    if (registerRC == 7) {
        displayState = false;
        registerVCBASE = registerVC;
    }
    
    updateDisplayState();
    
    /* "If the video logic is in display state afterwards (this is always the
     *  case if there is a Bad Line Condition), RC is incremented." [C.B.]
     */
    if (displayState) {
        registerRC = (registerRC + 1) & 0x07;
    }
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle59ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (column 2 of right border)
    preparePixelEngine(); // Prepare for next cycle (column 3 of right border)
    
    // Phi1.3 Fetch
    pAccess(0);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    sFirstAccess(0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle60ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw (last visible cycle)
    pixelEngine.draw(); // Draw previous cycle (column 3 of right border)
    preparePixelEngine(); // Prepare for next cycle (last column of right border)
    
    // Phi1.3 Fetch
    sSecondAccess(0);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sThirdAccess(0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle61ntsc()
{
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
    BA_LINE(spriteDmaOnOff & (SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sFirstAccess(1);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle62ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sThirdAccess(1);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle63ntsc()
{
    PROCESS_DELAYED_ACTIONS
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(1);
    pixelEngine.loadShiftRegister(1);
    pAccess(2);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sFirstAccess(2);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle64ntsc()
{
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
    BA_LINE(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sThirdAccess(2);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle65ntsc()
{
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
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sFirstAccess(3);
    
    // Finalize
    updateDisplayState();
    countX();
}



