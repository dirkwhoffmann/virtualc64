/*
 * Author: Dirk W. Hoffmann
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

#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

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
VIC::cycle1pal()
{
    debug_cycle(1);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    if (verticalFrameFFsetCond) {
        p.verticalFrameFF = true;
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
    setBAlow(spriteDmaOnOff & (SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sFirstAccess(3);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle1ntsc()
{
    debug_cycle(1);
    
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
VIC::cycle2pal()
{
    debug_cycle(2);
    
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
VIC::cycle3pal()
{
    debug_cycle(3);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.3 Fetch
    sFinalize(3);
    pixelEngine.loadShiftRegister(3);
    pAccess(4);
    
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
VIC::cycle4pal()
{
    debug_cycle(4);
    
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
VIC::cycle5pal()
{
    debug_cycle(5);
    
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
VIC::cycle6pal()
{
    debug_cycle(6);
    
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
VIC::cycle7pal()
{
    debug_cycle(7);
    
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
VIC::cycle8pal()
{
    debug_cycle(8);
    
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
VIC::cycle9pal()
{
    debug_cycle(9);
    
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
VIC::cycle10pal()
{
    debug_cycle(10);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    preparePixelEngine();
    
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
VIC::cycle11pal()
{
    debug_cycle(11);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    preparePixelEngine();
    
    // Phi1.3 Fetch (first out of five DRAM refreshs)
    sFinalize(7);
    pixelEngine.loadShiftRegister(7);
    rAccess();
    
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
VIC::cycle12()
{
    debug_cycle(12);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    preparePixelEngine();
    
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
    
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle13() // X Coordinate -3 - 4 (?)
{
    debug_cycle(13);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.drawOutsideBorder(); // Runs the sprite sequencer, only
    preparePixelEngine(); // Prepare for next cycle (first border column)
    // Update color registers in pixel engine to get the first pixel right
    pixelEngine.cpipe = cp;
    
    // Phi1.3 Fetch (third out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle14() // SpriteX: 0 - 7 (?)
{
    debug_cycle(14);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.visibleColumn = true; // We have reach the first visible column
    pixelEngine.draw(); // Draw previous cycle (first border column)
    preparePixelEngine(); // Prepare for next cycle (border column 2)
    
    // Phi1.3 Fetch (forth out of five DRAM refreshs)
    rAccess();
    
    // Phi2.3 VC/RC logic
    
    // "2. In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
    //     (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
    //     Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt." [C.B.]
    
    registerVC = registerVCBASE;
    registerVMLI = 0;
    if (badLineCondition)
        registerRC = 0;
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    xCounter = 0;
}

void
VIC::cycle15() // SpriteX: 8 - 15 (?)
{
    debug_cycle(15);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (border column 2)
    preparePixelEngine(); // Prepare for next cycle (border column 3)
    
    // Phi1.3 Fetch (last DRAM refresh)
    rAccess();
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    cleared_bits_in_d017 = 0;
    updateDisplayState();
    countX();
}

void
VIC::cycle16() // SpriteX: 16 - 23 (?)
{
    debug_cycle(16);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (border column 3)
    preparePixelEngine(); // Prepare for next cycle (border column 4)
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOff();
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle17() // SpriteX: 24 - 31 (?)
{
    debug_cycle(17);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(24);
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (border column 4)
    preparePixelEngine(); // Prepare for next cycle (first canvas column)
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle18() // SpriteX: 32 - 39
{
    debug_cycle(18);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(31);
    
    // Phi1.2 Draw
    pixelEngine.sr.canLoad = true; // Entering canvas area
    pixelEngine.draw17(); // Draw previous cycle (first canvas column)
    preparePixelEngine(); // Prepare for next cycle (canvas column 2)
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle19to54()
{
    debug_cycle(19);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle
    preparePixelEngine(); // Prepare for next cycle
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle55pal()
{
    debug_cycle(55);
    
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
    setBAlow(spriteDmaOnOff & SPR0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle55ntsc()
{
    debug_cycle(55);
    
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
VIC::cycle56()
{
    debug_cycle(56);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(335);
    
    // Phi1.2 Draw
    pixelEngine.draw55(); // Draw previous cycle (canvas column)
    preparePixelEngine(); // Prepare for next cycle (last canvas column)
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOn();
    toggleExpansionFlipflop();
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle57pal()
{
    debug_cycle(57);
    
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
    setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle57ntsc()
{
    debug_cycle(57);
    
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
