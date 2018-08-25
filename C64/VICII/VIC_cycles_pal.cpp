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

/* All cycles are processed in this order:
 *
 *   Phi1.1 Frame logic
 *   Phi1.2 Draw
 *   Phi1.3 Fetch
 *   Phi2.1 Rasterline interrupt
 *   Phi2.2 Sprite logic
 *   Phi2.3 VC/RC logic
 *   Phi2.4 BA logic
 *   Phi2.5 Fetch
 */

void
VIC::processDelayedActions()
{
    if (delay & VICTriggerIrq) {
        c64->cpu.pullDownIrqLine(CPU::INTSRC_VIC);
    }
    if (delay & VICReleaseIrq) {
        c64->cpu.releaseIrqLine(CPU::INTSRC_VIC);
    }
    if (delay & VICLpTransition) {
        triggerLightpenInterrupt();
    }
    if (delay & VICUpdateFlipflops) {
        flipflops = newFlipflops;
    }
    if (delay & VICUpdateRegisters) {
        reg.delayed = reg.current; 
    }
    if (delay & VICSetDisplayState) {
        displayState = true;
    }

    
    delay = (delay << 1) & VICClearanceMask;
}

void
VIC::cycle1pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    if (verticalFrameFFsetCond) {
        verticalFrameFF.write(true);
        setVerticalFrameFF(true);
    }
    
    // Phi1.3 Fetch
    sFinalize(2);
    pixelEngine.loadShiftRegister(2);
    pAccess(3);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (c64->getRasterline() != 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIRQ(1);
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sFirstAccess(3);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle2pal()
{
    // Check for yCounter overflows
    if (yCounterOverflow())
        yCounter = 0;
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sSecondAccess(3);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (yCounter == 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIRQ(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    
    // Phi2.5 Fetch
    sThirdAccess(3);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle3pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(3);
    pixelEngine.loadShiftRegister(3);
    pAccess(4);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR4 | SPR5));
    
    // Phi2.5 Fetch
    sFirstAccess(4);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle4pal()
{
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
    
    END_CYCLE
}


void
VIC::cycle5pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle6pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle7pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle8pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle9pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle10pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle11pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    
    // Phi1.3 Fetch (first out of five DRAM refreshs)
    sFinalize(7);
    pixelEngine.loadShiftRegister(7);
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(false);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle12()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, onl
    
    // Phi1.3 Fetch (second out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    
    /* "3. Liegt in den Zyklen 12-54 ein Bad-Line-Zustand vor, wird BA auf Low
     gelegt und die c-Zugriffe gestartet. Einmal gestartet, findet in der
     zweiten Phase jedes Taktzyklus im Bereich 15-54 ein c-Zugriff statt. Die
     gelesenen Daten werden in der Videomatrix-/Farbzeile an der durch VMLI
     angegebenen Position abgelegt. Bei jedem g-Zugriff im Display-Zustand
     werden diese Daten ebenfalls an der durch VMLI spezifizierten Position
     wieder intern gelesen." [C.B.] */
    
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle13() // X Coordinate -3 - 4 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    
    // Phi1.3 Fetch (third out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle14() // SpriteX: 0 - 7 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.visibleColumn = true; // We have reach the first visible column
    DRAW
    
    // Phi1.3 Fetch (forth out of five DRAM refreshs)
    rAccess();
    
    // Phi2.3 VC/RC logic
    
    // "2. In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
    //     (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
    //     Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt." [C.B.]
    
    registerVC = registerVCBASE;
    registerVMLI = 0;
    if (badLine)
        registerRC = 0;
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    
    END_CYCLE
    xCounter = 0;
}

void
VIC::cycle15() // SpriteX: 8 - 15 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch (last DRAM refresh)
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    C_ACCESS
    
    // Finalize
    cleared_bits_in_d017 = 0;
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle16() // SpriteX: 16 - 23 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
  
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOff();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    C_ACCESS
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle17() // SpriteX: 24 - 31 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(24);
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    C_ACCESS
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle18() // SpriteX: 32 - 39
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(31);
    
    // Phi1.2 Draw
    pixelEngine.sr.canLoad = true; // Entering canvas area
    DRAW17
  
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    C_ACCESS
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle19to54()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    C_ACCESS
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle55pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
  
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOn();
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR0);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle56()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(335);
    
    // Phi1.2 Draw
    DRAW55
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOn();
    toggleExpansionFlipflop();
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR0);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle57pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(344);
    
    // Phi1.2 Draw (border starts here)
    DRAW
    pixelEngine.sr.canLoad = false; // Leaving canvas area
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle58pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch
    pAccess(0);
    
    // Phi2.2 Sprite logic
    turnSpritesOnOrOff();
    
    // Phi2.3 VC/RC logic
    
    /* "In the first phase of cycle 58, the VIC checks if RC=7. If so, the video
     *  logic goes to idle state and VCBASE is loaded from VC (VC->VCBASE)."
     *  [C.B.]
     */
    if (registerRC == 7) {

        if (!badLine) {
            displayState = false;
        } else {
            assert(displayState);
        }
        
        oldDisplayState = false;

        registerVCBASE = registerVC;
    }
    
    updateDisplayState();
    
    /* "If the video logic is in display state afterwards (this is always the
     *  case if there is a Bad Line Condition), RC is incremented." [C.B.]
     */
    assert(oldDisplayState == displayState);
    if (oldDisplayState) {
        registerRC = (registerRC + 1) & 0x07;
    }
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    sFirstAccess(0);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle59pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch
    sSecondAccess(0);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sThirdAccess(0);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle60pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw (last visible cycle)
    DRAW
    
    // Phi1.3 Fetch
    sFinalize(0);
    pAccess(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sFirstAccess(1);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle61pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    pixelEngine.visibleColumn = false; // This was the last visible column
    
    // Phi1.3 Fetch
    sSecondAccess(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sThirdAccess(1);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}

void
VIC::cycle62pal()
{
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
    
    END_CYCLE
}

void
VIC::cycle63pal()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi1.3 Fetch
    sSecondAccess(2);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sThirdAccess(2);
    
    // Finalize
    updateDisplayState();
    
    END_CYCLE
}
