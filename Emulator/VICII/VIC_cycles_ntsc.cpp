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
VIC::cycle1ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sFirstAccess(3);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    if (verticalFrameFFsetCond) {
        setVerticalFrameFF(true);
    }
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(3);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (vc64.rasterLine != 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIrq(1);
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    
    END_CYCLE
}

void
VIC::cycle2ntsc()
{
    // Check for lightpen IRQ in first rasterline
    if (!lpLine && vc64.rasterLine == 0)
        checkForLightpenIrqAtStartOfFrame();
    
    // Phi2.5 Fetch (previous cycle)
    sThirdAccess(3);

    // Check for yCounter overflows
    if (yCounterOverflow())
        yCounter = 0;
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(3);
    pAccess(4);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (yCounter == 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIrq(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR4 | SPR5));
    
    END_CYCLE
}

void
VIC::cycle3ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sFirstAccess(4);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(4);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    
    END_CYCLE
}

void
VIC::cycle4ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sThirdAccess(4);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(4);
    pAccess(5);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR5 | SPR6));
    
    END_CYCLE
}

void
VIC::cycle5ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sFirstAccess(5);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(5);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    
    END_CYCLE
}

void
VIC::cycle6ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sThirdAccess(5);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(5);
    pAccess(6);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR6 | SPR7));
    
    END_CYCLE
}

void
VIC::cycle7ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sFirstAccess(6);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(6);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR6 | SPR7));
    
    END_CYCLE
}

void
VIC::cycle8ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sThirdAccess(6);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(6);
    pAccess(7);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR7);
    
    END_CYCLE
}

void
VIC::cycle9ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sFirstAccess(7);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(7);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR7);
    
    END_CYCLE
}

void
VIC::cycle10ntsc()
{
    // Phi2.5 Fetch (previous cycle)
    sThirdAccess(7);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(7);
    iAccess();
    
    // Phi2.4 BA logic
    BA_LINE(false);
    
    END_CYCLE
}

void
VIC::cycle11ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch (first out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(false);
    
    END_CYCLE
}

void
VIC::cycle55ntsc()
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
    BA_LINE(false);
    
    END_VISIBLE_CYCLE
}

void
VIC::cycle57ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(344);
    
    // Phi1.2 Draw (border starts here)
    DRAW
    sr.canLoad = false; // Leaving canvas area
    
    // Phi1.3 Fetch
    iAccess();
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR0);
    
    END_VISIBLE_CYCLE
}

void
VIC::cycle58ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW

    // Phi1.3 Fetch
    iAccess();
    
    // Phi2.2 Sprite logic
    spriteDisplayDelayed = spriteDisplay;
    turnSpritesOnOrOff();
    
    // Phi2.3 VC/RC logic
    
    /* "In the first phase of cycle 58, the VIC checks if RC=7. If so, the video
     *  logic goes to idle state and VCBASE is loaded from VC (VC->VCBASE)."
     *  [C.B.]
     */
    if (rc == 7) {
        displayState = badLine;
        vcBase = vc;
    }
    
    /* "If the video logic is in display state afterwards (this is always the
     *  case if there is a Bad Line Condition), RC is incremented." [C.B.]
     */
    if (displayState) {
        rc = (rc + 1) & 0x07;
    }
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1));
    
    END_VISIBLE_CYCLE
}

void
VIC::cycle59ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW59

    // Phi1.3 Fetch
    pAccess(0);
    
    // Phi2.2 Sprite logic
    spriteDisplayDelayed = spriteDisplay;
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    sFirstAccess(0);
    
    END_VISIBLE_CYCLE
}

void
VIC::cycle60ntsc()
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
    
    END_VISIBLE_CYCLE
}

void
VIC::cycle61ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw the last visible column
    DRAW

    // Phi1.3 Fetch
    sFinalize(0);
    pAccess(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR1 | SPR2));
    
    // Phi2.5 Fetch
    sFirstAccess(1);
 
    END_VISIBLE_CYCLE
    isVisibleColumn = false;
    // visibleColumnCnt = 0;
}

void
VIC::cycle62ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(1);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sThirdAccess(1);
    
    END_CYCLE
}

void
VIC::cycle63ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(1);
    pAccess(2);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR2 | SPR3));
    
    // Phi2.5 Fetch
    sFirstAccess(2);
    
    END_CYCLE
}

void
VIC::cycle64ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sSecondAccess(2);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));
    
    // Phi2.5 Fetch
    sThirdAccess(2);
    
    END_CYCLE
}

void
VIC::cycle65ntsc()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(2);
    pAccess(3);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4));
    
    END_CYCLE
}



