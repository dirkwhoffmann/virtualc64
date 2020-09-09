// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

/* All cycles are processed in this order:
 *
 *   Previous cycle  |  Phi2.5 Fetch (previous cycle)
 *  -------------------------------------------------
 *   Current cycle   |  Phi1.1 Frame logic
 *   Phi 1 phase     |  Phi1.2 Draw
 *                   |  Phi1.3 Fetch
 *                   |-------------------------------
 *   Current cycle   |  Phi2.1 Rasterline interrupt
 *   Phi 2 phase     |  Phi2.2 Sprite logic
 *                   |  Phi2.3 VC/RC logic
 *                   |  Phi2.4 BA logic
 */

#define PAL if (type == PAL_CYCLE || type == PAL_DEBUG_CYCLE)
#define NTSC if (type == NTSC_CYCLE || type == NTSC_DEBUG_CYCLE)

template <VICIICycleType type> void
VICII::cycle1()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(2); }
    NTSC { sFirstAccess(3); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    if (verticalFrameFFsetCond) {
        setVerticalFrameFF(true);
    }
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sFinalize(2); pAccess(3); }
    NTSC { sSecondAccess(3); }
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (c64.rasterLine != 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIrq(1);
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR3 | SPR4)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR3 | SPR4 | SPR5)); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle2()
{
    // Check for lightpen IRQ in first rasterline
    if (!lpLine && c64.rasterLine == 0)
        checkForLightpenIrqAtStartOfFrame();
    
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(3); }
    NTSC { sThirdAccess(3); }

    // Check for yCounter overflows
    if (yCounterOverflow())
        yCounter = 0;
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sSecondAccess(3); }
    NTSC { sFinalize(3); pAccess(4); }
        
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (yCounter == 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIrq(1);
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR3 | SPR4 | SPR5)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR4 | SPR5)); }
        
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle3()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(3); }
    NTSC { sFirstAccess(4); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sFinalize(3); pAccess(4); }
    NTSC { sSecondAccess(4); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR4 | SPR5)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR4 | SPR5 | SPR6)); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle4()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(4); }
    NTSC { sThirdAccess(4); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sSecondAccess(4); }
    NTSC { sFinalize(4); pAccess(5); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR4 | SPR5 | SPR6)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR5 | SPR6)); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle5()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(4); }
    NTSC { sFirstAccess(5); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sFinalize(4); pAccess(5); }
    NTSC { sSecondAccess(5); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR5 | SPR6)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR5 | SPR6 | SPR7)); }
     
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle6()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(5); }
    NTSC { sThirdAccess(5); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sSecondAccess(5); }
    NTSC { sFinalize(5); pAccess(6); }
        
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR5 | SPR6 | SPR7)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR6 | SPR7)); }
        
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle7()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(5); }
    NTSC { sFirstAccess(6); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sFinalize(5); pAccess(6); }
    NTSC { sSecondAccess(6); }
        
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR6 | SPR7)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR6 | SPR7)); }
        
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle8()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(6); }
    NTSC { sThirdAccess(6); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sSecondAccess(6); }
    NTSC { sFinalize(6); pAccess(7); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR6 | SPR7)); }
    NTSC { BA_LINE(spriteDmaOnOff & SPR7); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle9()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(6); }
    NTSC { sFirstAccess(7); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sFinalize(6); pAccess(7); }
    NTSC { sSecondAccess(7); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & SPR7); }
    NTSC { BA_LINE(spriteDmaOnOff & SPR7); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle10()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(7); }
    NTSC { sThirdAccess(7); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sSecondAccess(7); }
    NTSC { sFinalize(7); iAccess(); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & SPR7); }
    NTSC { BA_LINE(false); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle11()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(7); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch (first out of five DRAM refreshs)
    PAL  { sFinalize(7); }
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(false);
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle12()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

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
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle13() // X Coordinate -3 - 4 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch (third out of five DRAM refreshs)
    rAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle14() // SpriteX: 0 - 7 (?)
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw the first visible column
    isVisibleColumn = true;
    DRAW
    
    // Phi1.3 Fetch (forth out of five DRAM refreshs)
    rAccess();
    
    // Phi2.3 VC/RC logic
    
    // "2. In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
    //     (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
    //     Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt." [C.B.]
    
    vc = vcBase;
    vmli = 0;
    if (badLine)
        rc = 0;
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    END_VISIBLE_CYCLE
    xCounter = 0;
}

template <VICIICycleType type> void
VICII::cycle15() // SpriteX: 8 - 15 (?)
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
    
    cleared_bits_in_d017 = 0;
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle16() // SpriteX: 16 - 23 (?)
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
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle17() // SpriteX: 24 - 31 (?)
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
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle18() // SpriteX: 32 - 39
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(31);
    
    // Phi1.2 Draw
    sr.canLoad = true; // Entering canvas area
    DRAW17
  
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.4 BA logic
    BA_LINE(badLine);
    
    // Phi2.5 Fetch
    C_ACCESS
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle19to54()
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
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle55()
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
    PAL  { BA_LINE(spriteDmaOnOff & SPR0); }
    NTSC { BA_LINE(false); }
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle56()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(335);
    
    // Phi1.2 Draw
    DRAW55
    
    // Phi1.3 Fetch
    iAccess();
    
    // Phi2.2 Sprite logic
    turnSpriteDmaOn();
    toggleExpansionFlipflop();
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & SPR0);
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle57()
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
    PAL  { BA_LINE(spriteDmaOnOff & (SPR0 | SPR1)); }
    NTSC { BA_LINE(spriteDmaOnOff & SPR0); }
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle58()
{
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch
    PAL  { pAccess(0); }
    NTSC { iAccess(); }
    
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
    PAL  { BA_LINE(spriteDmaOnOff & (SPR0 | SPR1)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR0 | SPR1)); }
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle59()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(0); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW59
    
    // Phi1.3 Fetch
    PAL  { sSecondAccess(0); }
    NTSC { pAccess(0); }
    
    // Phi2.2 Sprite logic
    spriteDisplayDelayed = spriteDisplay;
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR0 | SPR1 | SPR2)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR0 | SPR1)); }
    
    // Phi2.5 Fetch
    NTSC { sFirstAccess(0); }
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle60()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(0); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    DRAW
    
    // Phi1.3 Fetch
    PAL  { sFinalize(0); pAccess(1); }
    NTSC { sSecondAccess(0); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR1 | SPR2)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR0 | SPR1 | SPR2)); }
    
    // Phi2.5 Fetch
    NTSC { sThirdAccess(0); }
    
    END_VISIBLE_CYCLE
}

template <VICIICycleType type> void
VICII::cycle61()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(1); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw the last visible column
    DRAW
    
    // Phi1.3 Fetch
    PAL  { sSecondAccess(1); }
    NTSC { sFinalize(0); pAccess(1); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR1 | SPR2 | SPR3)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR1 | SPR2)); }
    
    // Phi2.5 Fetch
    NTSC { sFirstAccess(1); }
    
    END_VISIBLE_CYCLE
    isVisibleColumn = false;
}

template <VICIICycleType type> void
VICII::cycle62()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sThirdAccess(1); }

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sFinalize(1); pAccess(2); }
    NTSC { sSecondAccess(1); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR2 | SPR3)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR1 | SPR2 | SPR3)); }
    
    // Phi2.5 Fetch
    NTSC { sThirdAccess(1); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle63()
{
    // Phi2.5 Fetch (previous cycle)
    PAL  { sFirstAccess(2); }
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    PAL  { yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine()); }
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    PAL  { sSecondAccess(2); }
    NTSC { sFinalize(1); pAccess(2); }
    
    // Phi2.4 BA logic
    PAL  { BA_LINE(spriteDmaOnOff & (SPR2 | SPR3 | SPR4)); }
    NTSC { BA_LINE(spriteDmaOnOff & (SPR2 | SPR3)); }
    
    // Phi2.5 Fetch
    NTSC { sFirstAccess(2); }
    
    END_CYCLE
}

template <VICIICycleType type> void
VICII::cycle64()
{
    PAL { assert(false); } // NTSC only
    
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

template <VICIICycleType type> void
VICII::cycle65()
{
    PAL { assert(false); } // NTSC only

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi1.2 Draw sprites (invisible area)
    DRAW_IDLE

    // Phi1.3 Fetch
    sFinalize(2);
    pAccess(3);
    
    // Phi2.4 BA logic
    BA_LINE(spriteDmaOnOff & (SPR3 | SPR4));
    
    END_CYCLE
}

template void VICII::cycle1<PAL_CYCLE>();
template void VICII::cycle2<PAL_CYCLE>();
template void VICII::cycle3<PAL_CYCLE>();
template void VICII::cycle4<PAL_CYCLE>();
template void VICII::cycle5<PAL_CYCLE>();
template void VICII::cycle6<PAL_CYCLE>();
template void VICII::cycle7<PAL_CYCLE>();
template void VICII::cycle8<PAL_CYCLE>();
template void VICII::cycle9<PAL_CYCLE>();
template void VICII::cycle10<PAL_CYCLE>();
template void VICII::cycle11<PAL_CYCLE>();
template void VICII::cycle12<PAL_CYCLE>();
template void VICII::cycle13<PAL_CYCLE>();
template void VICII::cycle14<PAL_CYCLE>();
template void VICII::cycle15<PAL_CYCLE>();
template void VICII::cycle16<PAL_CYCLE>();
template void VICII::cycle17<PAL_CYCLE>();
template void VICII::cycle18<PAL_CYCLE>();
template void VICII::cycle19to54<PAL_CYCLE>();
template void VICII::cycle55<PAL_CYCLE>();
template void VICII::cycle56<PAL_CYCLE>();
template void VICII::cycle57<PAL_CYCLE>();
template void VICII::cycle58<PAL_CYCLE>();
template void VICII::cycle59<PAL_CYCLE>();
template void VICII::cycle60<PAL_CYCLE>();
template void VICII::cycle61<PAL_CYCLE>();
template void VICII::cycle62<PAL_CYCLE>();
template void VICII::cycle63<PAL_CYCLE>();
template void VICII::cycle64<PAL_CYCLE>();
template void VICII::cycle65<PAL_CYCLE>();

template void VICII::cycle1<PAL_DEBUG_CYCLE>();
template void VICII::cycle2<PAL_DEBUG_CYCLE>();
template void VICII::cycle3<PAL_DEBUG_CYCLE>();
template void VICII::cycle4<PAL_DEBUG_CYCLE>();
template void VICII::cycle5<PAL_DEBUG_CYCLE>();
template void VICII::cycle6<PAL_DEBUG_CYCLE>();
template void VICII::cycle7<PAL_DEBUG_CYCLE>();
template void VICII::cycle8<PAL_DEBUG_CYCLE>();
template void VICII::cycle9<PAL_DEBUG_CYCLE>();
template void VICII::cycle10<PAL_DEBUG_CYCLE>();
template void VICII::cycle11<PAL_DEBUG_CYCLE>();
template void VICII::cycle12<PAL_DEBUG_CYCLE>();
template void VICII::cycle13<PAL_DEBUG_CYCLE>();
template void VICII::cycle14<PAL_DEBUG_CYCLE>();
template void VICII::cycle15<PAL_DEBUG_CYCLE>();
template void VICII::cycle16<PAL_DEBUG_CYCLE>();
template void VICII::cycle17<PAL_DEBUG_CYCLE>();
template void VICII::cycle18<PAL_DEBUG_CYCLE>();
template void VICII::cycle19to54<PAL_DEBUG_CYCLE>();
template void VICII::cycle55<PAL_DEBUG_CYCLE>();
template void VICII::cycle56<PAL_DEBUG_CYCLE>();
template void VICII::cycle57<PAL_DEBUG_CYCLE>();
template void VICII::cycle58<PAL_DEBUG_CYCLE>();
template void VICII::cycle59<PAL_DEBUG_CYCLE>();
template void VICII::cycle60<PAL_DEBUG_CYCLE>();
template void VICII::cycle61<PAL_DEBUG_CYCLE>();
template void VICII::cycle62<PAL_DEBUG_CYCLE>();
template void VICII::cycle63<PAL_DEBUG_CYCLE>();
template void VICII::cycle64<PAL_DEBUG_CYCLE>();
template void VICII::cycle65<PAL_DEBUG_CYCLE>();

template void VICII::cycle1<NTSC_CYCLE>();
template void VICII::cycle2<NTSC_CYCLE>();
template void VICII::cycle3<NTSC_CYCLE>();
template void VICII::cycle4<NTSC_CYCLE>();
template void VICII::cycle5<NTSC_CYCLE>();
template void VICII::cycle6<NTSC_CYCLE>();
template void VICII::cycle7<NTSC_CYCLE>();
template void VICII::cycle8<NTSC_CYCLE>();
template void VICII::cycle9<NTSC_CYCLE>();
template void VICII::cycle10<NTSC_CYCLE>();
template void VICII::cycle11<NTSC_CYCLE>();
template void VICII::cycle12<NTSC_CYCLE>();
template void VICII::cycle13<NTSC_CYCLE>();
template void VICII::cycle14<NTSC_CYCLE>();
template void VICII::cycle15<NTSC_CYCLE>();
template void VICII::cycle16<NTSC_CYCLE>();
template void VICII::cycle17<NTSC_CYCLE>();
template void VICII::cycle18<NTSC_CYCLE>();
template void VICII::cycle19to54<NTSC_CYCLE>();
template void VICII::cycle55<NTSC_CYCLE>();
template void VICII::cycle56<NTSC_CYCLE>();
template void VICII::cycle57<NTSC_CYCLE>();
template void VICII::cycle58<NTSC_CYCLE>();
template void VICII::cycle59<NTSC_CYCLE>();
template void VICII::cycle60<NTSC_CYCLE>();
template void VICII::cycle61<NTSC_CYCLE>();
template void VICII::cycle62<NTSC_CYCLE>();
template void VICII::cycle63<NTSC_CYCLE>();
template void VICII::cycle64<NTSC_CYCLE>();
template void VICII::cycle65<NTSC_CYCLE>();

template void VICII::cycle1<NTSC_DEBUG_CYCLE>();
template void VICII::cycle2<NTSC_DEBUG_CYCLE>();
template void VICII::cycle3<NTSC_DEBUG_CYCLE>();
template void VICII::cycle4<NTSC_DEBUG_CYCLE>();
template void VICII::cycle5<NTSC_DEBUG_CYCLE>();
template void VICII::cycle6<NTSC_DEBUG_CYCLE>();
template void VICII::cycle7<NTSC_DEBUG_CYCLE>();
template void VICII::cycle8<NTSC_DEBUG_CYCLE>();
template void VICII::cycle9<NTSC_DEBUG_CYCLE>();
template void VICII::cycle10<NTSC_DEBUG_CYCLE>();
template void VICII::cycle11<NTSC_DEBUG_CYCLE>();
template void VICII::cycle12<NTSC_DEBUG_CYCLE>();
template void VICII::cycle13<NTSC_DEBUG_CYCLE>();
template void VICII::cycle14<NTSC_DEBUG_CYCLE>();
template void VICII::cycle15<NTSC_DEBUG_CYCLE>();
template void VICII::cycle16<NTSC_DEBUG_CYCLE>();
template void VICII::cycle17<NTSC_DEBUG_CYCLE>();
template void VICII::cycle18<NTSC_DEBUG_CYCLE>();
template void VICII::cycle19to54<NTSC_DEBUG_CYCLE>();
template void VICII::cycle55<NTSC_DEBUG_CYCLE>();
template void VICII::cycle56<NTSC_DEBUG_CYCLE>();
template void VICII::cycle57<NTSC_DEBUG_CYCLE>();
template void VICII::cycle58<NTSC_DEBUG_CYCLE>();
template void VICII::cycle59<NTSC_DEBUG_CYCLE>();
template void VICII::cycle60<NTSC_DEBUG_CYCLE>();
template void VICII::cycle61<NTSC_DEBUG_CYCLE>();
template void VICII::cycle62<NTSC_DEBUG_CYCLE>();
template void VICII::cycle63<NTSC_DEBUG_CYCLE>();
template void VICII::cycle64<NTSC_DEBUG_CYCLE>();
template void VICII::cycle65<NTSC_DEBUG_CYCLE>();
