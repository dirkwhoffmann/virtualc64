// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"

#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

VICII::VICII(C64 &ref) : C64Component(ref)
{
	setDescription("VICII");
    
	markIRQLines = false;
	markDMALines = false;
    config.grayDotBug = true;
    palette = COLOR_PALETTE;
    
    // Register snapshot items
    SnapshotItem items[] = {

        // Configuration items
        { &config.revision,             sizeof(config.revision),                KEEP_ON_RESET },
        { &config.glueLogic,            sizeof(config.glueLogic),               KEEP_ON_RESET },
        { &config.grayDotBug,           sizeof(config.grayDotBug),              KEEP_ON_RESET },

        // Internal state
        { &reg,                         sizeof(reg),                            CLEAR_ON_RESET },
        { &rasterIrqLine,               sizeof(rasterIrqLine),                  CLEAR_ON_RESET },
        { &latchedLPX,            sizeof(latchedLPX),               CLEAR_ON_RESET },
        { &latchedLPY,            sizeof(latchedLPY),               CLEAR_ON_RESET },
        { &memSelect,                   sizeof(memSelect),                      CLEAR_ON_RESET },
        { &irr,                         sizeof(irr),                            CLEAR_ON_RESET },
        { &imr,                         sizeof(imr),                            CLEAR_ON_RESET },

        { &refreshCounter,              sizeof(refreshCounter),                 CLEAR_ON_RESET },
        { &xCounter,                    sizeof(xCounter),                       CLEAR_ON_RESET },
        { &yCounter,                    sizeof(yCounter),                       CLEAR_ON_RESET },
        { &vc,                          sizeof(vc),                             CLEAR_ON_RESET },
        { &vcBase,                      sizeof(vcBase),                         CLEAR_ON_RESET },
        { &rc,                          sizeof(rc),                             CLEAR_ON_RESET },
        { &videoMatrix,                 sizeof(videoMatrix),                    CLEAR_ON_RESET },
        { &colorLine,                   sizeof(colorLine),                      CLEAR_ON_RESET },
        { &vmli,                        sizeof(vmli),                           CLEAR_ON_RESET },
        { &sr,                          sizeof(sr),                             CLEAR_ON_RESET },
        { &spriteSr,                    sizeof(spriteSr),                       CLEAR_ON_RESET },
        { &spriteSrActive,              sizeof(spriteSrActive),                 CLEAR_ON_RESET },
        { &spriteSpriteCollision,       sizeof(spriteSpriteCollision),          CLEAR_ON_RESET },
        { &spriteBackgroundColllision,  sizeof(spriteBackgroundColllision),     CLEAR_ON_RESET },

        { &flipflops,                   sizeof(flipflops),                      CLEAR_ON_RESET },
        { &verticalFrameFFsetCond,      sizeof(verticalFrameFFsetCond),         CLEAR_ON_RESET },
        { &leftComparisonVal,           sizeof(leftComparisonVal),              CLEAR_ON_RESET },
        { &rightComparisonVal,          sizeof(rightComparisonVal),             CLEAR_ON_RESET },
        { &upperComparisonVal,          sizeof(upperComparisonVal),             CLEAR_ON_RESET },
        { &lowerComparisonVal,          sizeof(lowerComparisonVal),             CLEAR_ON_RESET },

        { &isVisibleColumn,             sizeof(isVisibleColumn),                CLEAR_ON_RESET },
        { &yCounterEqualsIrqRasterline, sizeof(yCounterEqualsIrqRasterline),    CLEAR_ON_RESET },
        { &vblank,                      sizeof(vblank),                         CLEAR_ON_RESET },
        { &badLine,                     sizeof(badLine),                        CLEAR_ON_RESET },
        { &DENwasSetInRasterline30,     sizeof(DENwasSetInRasterline30),        CLEAR_ON_RESET },
        { &displayState,                sizeof(displayState),                   CLEAR_ON_RESET },
        
        { &mc,                          sizeof(mc),                             CLEAR_ON_RESET },
        { &mcbase,                      sizeof(mcbase),                         CLEAR_ON_RESET },
        { spritePtr,                    sizeof(spritePtr),                      CLEAR_ON_RESET },
        { &isFirstDMAcycle,             sizeof(isFirstDMAcycle),                CLEAR_ON_RESET },
        { &isSecondDMAcycle,            sizeof(isSecondDMAcycle),               CLEAR_ON_RESET },
        { &spriteDisplay,               sizeof(spriteDisplay),                  CLEAR_ON_RESET },
        { &spriteDisplayDelayed,        sizeof(spriteDisplayDelayed),           CLEAR_ON_RESET },
        { &spriteDmaOnOff,              sizeof(spriteDmaOnOff),                 CLEAR_ON_RESET },
        { &expansionFF,                 sizeof(expansionFF),                    CLEAR_ON_RESET },
        { &cleared_bits_in_d017,        sizeof(cleared_bits_in_d017),           CLEAR_ON_RESET },

        { &lpLine,                      sizeof(lpLine),                         CLEAR_ON_RESET },
        { &lpIrqHasOccurred,       sizeof(lpIrqHasOccurred),          CLEAR_ON_RESET },

        { &memSrc,                      sizeof(memSrc),                         KEEP_ON_RESET },
        { &ultimax,                     sizeof(ultimax),                        CLEAR_ON_RESET },
        { &dataBusPhi1,                 sizeof(dataBusPhi1),                    CLEAR_ON_RESET },
        { &dataBusPhi2,                 sizeof(dataBusPhi2),                    CLEAR_ON_RESET },
        { &addrBus,                     sizeof(addrBus),                        CLEAR_ON_RESET },
        { &bankAddr,                    sizeof(bankAddr),                       CLEAR_ON_RESET },
      
        { &delay,                       sizeof(delay),                          CLEAR_ON_RESET },
        { &bufferoffset,                sizeof(bufferoffset),                   CLEAR_ON_RESET },

        { NULL,                         0,                                      0 }};

    registerSnapshotItems(items, sizeof(items));
    
    // Assign reference clock to all time delayed variables
    baLine.setClock(&cpu.cycle);
    gAccessResult.setClock(&cpu.cycle);
    
    // Create random background noise pattern
    const size_t noiseSize = 2 * 512 * 512;
    noise = new u32[noiseSize];
    for (int i = 0; i < noiseSize; i++) {
        noise[i] = rand() % 2 ? 0xFF000000 : 0xFFFFFFFF;
    }
}

void
VICII::_initialize()
{
    setRevision(PAL_8565);
}

void 
VICII::_reset()
{
    // Clear snapshot items marked with 'CLEAR_ON_RESET'
     if (snapshotItems != NULL)
         for (unsigned i = 0; snapshotItems[i].data != NULL; i++)
             if (snapshotItems[i].flags & CLEAR_ON_RESET)
                 memset(snapshotItems[i].data, 0, snapshotItems[i].size);

    yCounter = PAL_HEIGHT;
    
    // Reset timed delay variables
    baLine.clear();
    gAccessResult.clear();
    
    // Reset the memory source lookup table
    setUltimax(false);

    // Reset sprite logic
    expansionFF = 0xFF;
    
    // Preset some video parameters to show a blank blue sreen on power up
    memSelect = 0x10;
    reg.delayed.ctrl1 = 0x10;
    reg.current.ctrl1 = 0x10;
    reg.delayed.colors[COLREG_BORDER] = VICII_LIGHT_BLUE;
    reg.current.colors[COLREG_BORDER] = VICII_LIGHT_BLUE;
    reg.delayed.colors[COLREG_BG0] = VICII_BLUE;
    reg.current.colors[COLREG_BG0] = VICII_BLUE;
    
    // Frame flipflops
    leftComparisonVal = leftComparisonValue();
    rightComparisonVal = rightComparisonValue();
    upperComparisonVal = upperComparisonValue();
    lowerComparisonVal = lowerComparisonValue();
    
	// Disable cheating by default
	hideSprites = false;
	spriteSpriteCollisionEnabled = 0xFF;
	spriteBackgroundCollisionEnabled = 0xFF;
    
    // Screen buffer
    currentScreenBuffer = screenBuffer1;
    pixelBuffer = currentScreenBuffer;
}

void
VICII::_inspect()
{
    synchronized {
        
        u8 ctrl1 = reg.current.ctrl1;
        u8 ctrl2 = reg.current.ctrl2;
        
        info.rasterLine = c64.rasterLine;
        info.rasterCycle = c64.rasterCycle;
        info.yCounter = yCounter;
        info.xCounter = xCounter;
        info.vc = vc;
        info.vcBase = vcBase;
        info.rc = rc;
        info.vmli = vmli;
        
        info.ctrl1 = ctrl1;
        info.ctrl2 = ctrl2;
        info.dy = ctrl1 & 0x07;
        info.dx = ctrl2 & 0x07;
        info.denBit = DENbit();
        info.badLine = badLine;
        info.displayState = displayState;
        info.vblank = vblank;
        info.screenGeometry = getScreenGeometry();
        info.frameFF = flipflops.current;
        info.displayMode = (DisplayMode)((ctrl1 & 0x60) | (ctrl2 & 0x10));
        info.borderColor = reg.current.colors[COLREG_BORDER];
        info.bgColor0 = reg.current.colors[COLREG_BG0];
        info.bgColor1 = reg.current.colors[COLREG_BG1];
        info.bgColor2 = reg.current.colors[COLREG_BG2];
        info.bgColor3 = reg.current.colors[COLREG_BG3];
        
        info.memSelect = memSelect;
        info.ultimax = ultimax;
        info.memoryBankAddr = bankAddr;
        info.screenMemoryAddr = VM13VM12VM11VM10() << 6;
        info.charMemoryAddr = (CB13CB12CB11() << 10) % 0x4000;
        
        info.irqRasterline = rasterInterruptLine();
        info.imr = imr;
        info.irr = irr;
        
        info.latchedLPX = latchedLPX;
        info.latchedLPY = latchedLPY;
        info.lpLine = lpLine;
        info.lpIrqHasOccurred = lpIrqHasOccurred;
        
        for (int i = 0; i < 8; i++) {
            
            spriteInfo[i].enabled = GET_BIT(reg.current.sprEnable, i);
            spriteInfo[i].x = reg.current.sprX[i];
            spriteInfo[i].y = reg.current.sprY[i];
            spriteInfo[i].color = reg.current.colors[COLREG_SPR0 + i];
            spriteInfo[i].extraColor1 = reg.current.colors[COLREG_SPR_EX1];
            spriteInfo[i].extraColor2 = reg.current.colors[COLREG_SPR_EX2];
            spriteInfo[i].multicolor = GET_BIT(reg.current.sprMC, i);
            spriteInfo[i].expandX = GET_BIT(reg.current.sprExpandX, i);
            spriteInfo[i].expandY = GET_BIT(reg.current.sprExpandY, i);
            spriteInfo[i].priority = GET_BIT(reg.current.sprPriority, i);
            spriteInfo[i].ssCollision = GET_BIT(spriteSpriteCollision, i);
            spriteInfo[i].sbCollision = GET_BIT(spriteBackgroundColllision, i);
        }
    }
}

void
VICII::_ping()
{
    c64.putMessage(isPAL() ? MSG_PAL : MSG_NTSC);
}

void
VICII::_dumpConfig()
{
    msg("    Chip model : %d (%s)\n", config.revision, vicRevisionName(config.revision));
    msg("  Gray dot bug : %s\n", config.grayDotBug ? "yes" : "no");
    msg("           PAL : %s\n", isPAL() ? "yes" : "no");
    msg("          NTSC : %s\n", isNTSC() ? "yes" : "no");
    msg("is656x, is856x : %d %d\n", is656x(), is856x());
    msg("    Glue logic : %d (%s)\n", config.glueLogic, glueLogicName(config.glueLogic));
}

void 
VICII::_dump()
{
    u8 ctrl1 = reg.current.ctrl1;
    u8 ctrl2 = reg.current.ctrl2; 
    int yscroll = ctrl1 & 0x07;
    int xscroll = ctrl2 & 0x07;
    DisplayMode mode = (DisplayMode)((ctrl1 & 0x60) | (ctrl2 & 0x10));
    
	msg("     Bank address : %04X\n", bankAddr, bankAddr);
    msg("    Screen memory : %04X\n", VM13VM12VM11VM10() << 6);
	msg(" Character memory : %04X\n", (CB13CB12CB11() << 10) % 0x4000);
	msg("X/Y raster scroll : %d / %d\n", xscroll, yscroll);
    msg("    Control reg 1 : %02X\n", reg.current.ctrl1);
    msg("    Control reg 2 : %02X\n", reg.current.ctrl2);
	msg("     Display mode : ");
	switch (mode) {
		case STANDARD_TEXT: 
			msg("Standard character mode\n");
			break;
		case MULTICOLOR_TEXT:
			msg("Multicolor character mode\n");
			break;
		case STANDARD_BITMAP:
			msg("Standard bitmap mode\n");
			break;
		case MULTICOLOR_BITMAP:
			msg("Multicolor bitmap mode\n");
			break;
		case EXTENDED_BACKGROUND_COLOR:
			msg("Extended background color mode\n");
			break;
		default:
			msg("Invalid\n");
	}
	msg("            (X,Y) : (%d,%d) %s %s\n", xCounter, yCounter,  badLine ? "(DMA line)" : "", DENwasSetInRasterline30 ? "" : "(DMA lines disabled, no DEN bit in rasterline 30)");
	msg("               VC : %02X\n", vc);
	msg("           VCBASE : %02X\n", vcBase);
	msg("               RC : %02X\n", rc);
	msg("             VMLI : %02X\n", vmli);
	msg("          BA line : %s\n", baLine.current() ? "low" : "high");
    msg("      MainFrameFF : %d\n", flipflops.current.main);
    msg("  VerticalFrameFF : %d\n", flipflops.current.vertical);
	msg("     DisplayState : %s\n", displayState ? "on" : "off");
    msg("    SpriteDisplay : %02X (%02X)\n", spriteDisplay, spriteDisplayDelayed);
	msg("        SpriteDma : %02X ( ", spriteDmaOnOff);
	for (int i = 0; i < 8; i++) 
		msg("%d ", (spriteDmaOnOff & (1 << i)) != 0 );
	msg(")\n");
	msg("      Y expansion : %02X ( ", expansionFF);
	for (int i = 0; i < 8; i++) 
		msg("%d ", (expansionFF & (1 << i)) != 0);
	msg(")\n");
}

SpriteInfo
VICII::getSpriteInfo(int nr)
{
    SpriteInfo result;
    synchronized { result = spriteInfo[nr]; }
    return result;
}

size_t
VICII::oldStateSize()
{
    return HardwareComponent::oldStateSize()
    + baLine.stateSize()
    + gAccessResult.stateSize();
}

void
VICII::oldDidLoadFromBuffer(u8 **buffer)
{
    baLine.loadFromBuffer(buffer);
    gAccessResult.loadFromBuffer(buffer);
}

void
VICII::oldDidSaveToBuffer(u8 **buffer)
{
    baLine.saveToBuffer(buffer);
    gAccessResult.saveToBuffer(buffer);
}

void
VICII::setRevision(VICRevision revision)
{
    debug(VIC_DEBUG, "setRevision(%d)\n", revision);
    debug("setRevision(%d)\n", revision);
    
    assert(isVICRevision(revision));
    config.revision = revision;
    
    updatePalette();
    resetScreenBuffers();
    c64.updateVicFunctionTable();
    
    switch(revision) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            c64.setClockFrequency(PAL_CLOCK_FREQUENCY);
            c64.putMessage(MSG_PAL);
            break;
            
        case NTSC_6567:
        case NTSC_6567_R56A:
        case NTSC_8562:
            c64.setClockFrequency(NTSC_CLOCK_FREQUENCY);
            c64.putMessage(MSG_NTSC);
            break;
            
        default:
            assert(false);
    }
}

void
VICII::setGlueLogic(GlueLogic type)
{
    debug(VIC_DEBUG, "setGlueLogic(%d)\n", type);
    
    assert(isGlueLogic(type));
    config.glueLogic = type;
}

void
VICII::setVideoPalette(Palette type)
{
    if (!isPalette(type)) {
        warn("Unknown palette type (%d). Assuming color palette.\n", type);
        type = COLOR_PALETTE;
    }
    
    palette = type;
    updatePalette();
}

unsigned
VICII::getClockFrequency()
{
    switch (config.revision) {
            
        case NTSC_6567:
        case NTSC_8562:
        case NTSC_6567_R56A:
            return NTSC_CLOCK_FREQUENCY;
            
        default:
            return PAL_CLOCK_FREQUENCY;
    }
}

unsigned
VICII::getCyclesPerRasterline()
{
    switch (config.revision) {
            
        case NTSC_6567_R56A:
            return 64;
            
        case NTSC_6567:
        case NTSC_8562:
            return 65;
            
        default:
            return 63;
    }
}

bool
VICII::isLastCycleInRasterline(unsigned cycle)
{
    return cycle >= getCyclesPerRasterline();
}

unsigned
VICII::getRasterlinesPerFrame()
{
    switch (config.revision) {
            
        case NTSC_6567_R56A:
            return 262;
            
        case NTSC_6567:
        case NTSC_8562:
            return 263;
            
        default:
            return 312;
    }
}

bool
VICII::isVBlankLine(unsigned rasterline)
{
    switch (config.revision) {
            
        case NTSC_6567_R56A:
            return rasterline < 16 || rasterline >= 16 + 234;
            
        case NTSC_6567:
        case NTSC_8562:
            return rasterline < 16 || rasterline >= 16 + 235;
            
        default:
            return rasterline < 16 || rasterline >= 16 + 284;
    }
}

void *
VICII::screenBuffer() {
    if (currentScreenBuffer == screenBuffer1) {
        return screenBuffer2;
    } else {
        return screenBuffer1;
    }
}

void
VICII::resetScreenBuffers()
{
    for (unsigned line = 0; line < PAL_RASTERLINES; line++) {
        for (unsigned i = 0; i < NTSC_PIXELS; i++) {
            screenBuffer1[line * NTSC_PIXELS + i] =
            screenBuffer2[line * NTSC_PIXELS + i] =
            (line % 2) ? rgbaTable[8] : rgbaTable[9];
        }
    }
}

u32 *
VICII::getNoise()
{
    int offset = rand() % (512 * 512);
    return noise + offset;
}

u16
VICII::rasterline()
{
    return c64.rasterLine;
}

u8
VICII::rastercycle()
{
    return c64.rasterCycle;
}


//
// Frame flipflops
//

void
VICII::checkVerticalFrameFF()
{

    // Check for upper border
    if (yCounter == upperComparisonVal) {
        
        if (DENbit()) {
            
            // Clear immediately
            setVerticalFrameFF(false);
        }
        
    } else if (yCounter == lowerComparisonVal) {
        
            // Set later, in cycle 1
            verticalFrameFFsetCond = true;
    }
}

void
VICII::checkFrameFlipflopsLeft(u16 comparisonValue)
{
    /* "6. If the X coordinate reaches the left comparison value and the
     *     vertical border flip flop is not set, the main flip flop is reset."
     */
    if (comparisonValue == leftComparisonVal) {
        
        // Note that the main frame flipflop can not be cleared when the
        // vertical border flipflop is set.
        if (!flipflops.current.vertical && !verticalFrameFFsetCond) {
            setMainFrameFF(false);
        }        
    }
}

void
VICII::checkFrameFlipflopsRight(u16 comparisonValue)
{
    /* "1. If the X coordinate reaches the right comparison value, the main
     *     border flip flop is set." [C.B.]
     */
    if (comparisonValue == rightComparisonVal) {
        setMainFrameFF(true);
    }
}

void
VICII::setVerticalFrameFF(bool value)
{
    if (value != flipflops.delayed.vertical) {
        flipflops.current.vertical = value;
        delay |= VICUpdateFlipflops;
    }
}

void
VICII::setMainFrameFF(bool value)
{
    if (value != flipflops.delayed.main) {
        flipflops.current.main = value;
        delay |= VICUpdateFlipflops;
    }
}

bool
VICII::badLineCondition() {
    
    /* A Bad Line Condition is given at any arbitrary clock cycle, if at the
     * negative edge of ø0 at the beginning of the cycle
     * [1] RASTER >= $30 and RASTER <= $f7 and
     * [2] the lower three bits of RASTER are equal to YSCROLL and
     * [3] if the DEN bit was set during an arbitrary cycle of
     *     raster line $30." [C.B.]
     */
    return
    (yCounter >= 0x30 && yCounter <= 0xf7) && /* [1] */
    (yCounter & 0x07) == (reg.current.ctrl1 & 0x07) && /* [2] */
    DENwasSetInRasterline30; /* [3] */
}

void
VICII::updateBA(u8 value)
{
    if (value != baLine.current()) {
       
        if (value) {
            baLine.write(value);
        } else {
            baLine.clear();
        }
        
        cpu.setRDY(value == 0);
    }
}

void 
VICII::triggerIrq(u8 source)
{
    assert(source == 1 || source == 2 || source == 4 || source == 8);
    
    irr |= source;
    delay |= VICUpdateIrqLine;
}

u16
VICII::lightpenX()
{
    u8 cycle = c64.rasterCycle;
    
    switch (config.revision) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:

            return 4 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);

        case PAL_8565:
            
            return 2 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);
            
        case NTSC_6567:
        case NTSC_6567_R56A:
            
            return 4 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        case NTSC_8562:
            
            return 2 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        default:
            assert(false);
    }
}

u16
VICII::lightpenY()
{
    return yCounter;
}

void
VICII::setLP(bool value)
{
    // A negative transition on LP triggers a lightpen event.
    if (FALLING_EDGE(lpLine, value)) {
        delay |= VICLpTransition;
    }
    
    lpLine = value;
}

void
VICII::checkForLightpenIrq()
{
    u8 vicCycle = c64.rasterCycle;

    // An interrupt is suppressed if ...
    
    // ... a previous interrupt has occurred in the current frame.
    if (lpIrqHasOccurred)
        return;

    // ... we are in the last PAL rasterline and not in cycle 1.
    if (yCounter == PAL_HEIGHT - 1 && vicCycle != 1)
        return;
    
    // Latch coordinates
    latchedLPX = lightpenX() / 2;
    latchedLPY = lightpenY();
    
    // Newer VICII models trigger an interrupt immediately
    if (!delayedLightPenIrqs()) triggerIrq(8);
    
    // Lightpen interrupts can only occur once per frame
    lpIrqHasOccurred = true;
}

void
VICII::checkForLightpenIrqAtStartOfFrame()
{
    // This function is called at the beginning of a frame, only.
    assert(c64.rasterLine == 0);
    assert(c64.rasterCycle == 2);
 
    // Latch coordinate (values according to VICE 3.1)
    switch (config.revision) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            
            latchedLPX = 209;
            latchedLPY = 0;
            break;
            
        case NTSC_6567:
        case NTSC_6567_R56A:
        case NTSC_8562:
            
            latchedLPX = 213;
            latchedLPY = 0;
            break;
    }
    
    // Trigger interrupt
    triggerIrq(8);

    // Lightpen interrupts can only occur once per frame
    lpIrqHasOccurred = true;
}


//
// Sprites
//

u8
VICII::spriteDepth(u8 nr)
{
    return
    GET_BIT(reg.delayed.sprPriority, nr) ?
    (SPRITE_LAYER_BG_DEPTH | nr) :
    (SPRITE_LAYER_FG_DEPTH | nr);
}

u8
VICII::compareSpriteY()
{
    u8 result = 0;
    
    for (unsigned i = 0; i < 8; i++) {
        result |= (reg.current.sprY[i] == (yCounter & 0xFF)) << i;
    }
    
    return result;
}

void
VICII::turnSpriteDmaOff()
{
    /* "7. In the first phase of cycle 16, [1] it is checked if the expansion
     *     flip flop is set. If so, [2] MCBASE load from MC (MC->MCBASE), [3]
     *     unless the CPU cleared the Y expansion bit in $d017 in the second
     *     phase of cycle 15, in which case [4] MCBASE is set to
     *
     *         X = (101010 & (MCBASE & MC)) | (010101 & (MCBASE | MC)).
     *
     *     After the MCBASE update, [5] the VIC checks if MCBASE is equal to 63
     *     and [6] turns off the DMA of the sprite if it is." [VIC Addendum]
     */
    for (unsigned i = 0; i < 8; i++) {
        
        if (GET_BIT(expansionFF,i)) { /* [1] */
            if (GET_BIT(cleared_bits_in_d017,i)) { /* [3] */
                mcbase[i] =
                (0b101010 & (mcbase[i] & mc[i])) |
                (0b010101 & (mcbase[i] | mc[i])); /* [4] */
            } else {
                mcbase[i] = mc[i]; /* [2] */
            }
            if (mcbase[i] == 63) { /* [5] */
                CLR_BIT(spriteDmaOnOff,i); /* [6] */
            }
        }
    }
}

void
VICII::turnSpriteDmaOn()
{
    /* "In the first phases of cycle 55 and 56, the VIC checks for every sprite
     *  if the corresponding MxE bit in register $d015 is set and the Y
     *  coordinate of the sprite (odd registers $d001-$d00f) match the lower 8
     *  bits of RASTER. If this is the case and the DMA for the sprite is still
     *  off, the DMA is switched on, MCBASE is cleared, and if the MxYE bit is
     *  set the expansion flip flip is reset." [C.B.]
     */
    u8 risingEdges = ~spriteDmaOnOff & (reg.current.sprEnable & compareSpriteY());
    
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(risingEdges,i))
            mcbase[i] = 0;
    }
    spriteDmaOnOff |= risingEdges;
    expansionFF |= risingEdges;
}

void
VICII::turnSpritesOnOrOff()
{
    /* "In the first phase of cycle 58, the MC of every sprite is loaded from
     *  its belonging MCBASE (MCBASE->MC) and it is checked [1] if the DMA for
     *  the sprite is turned on and [2] the Y coordinate of the sprite matches
     *  the lower 8 bits of RASTER. If this is the case, the display of the
     *  sprite is turned on."
     */
    for (unsigned i = 0; i < 8; i++) {
        mc[i] = mcbase[i];
    }
    
    spriteDisplay |= reg.current.sprEnable & compareSpriteY();
    spriteDisplay &= spriteDmaOnOff;
}

void
VICII::updateSpriteShiftRegisters() {
    if (isSecondDMAcycle) {
        for (unsigned sprite = 0; sprite < 8; sprite++) {
            if (GET_BIT(isSecondDMAcycle, sprite)) {
                loadShiftRegister(sprite);
            }
        }
    }
}

void 
VICII::beginFrame()
{
	lpIrqHasOccurred = false;

    /* "The VIC does five read accesses in every raster line for the refresh of
     *  the dynamic RAM. An 8 bit refresh counter (REF) is used to generate 256
     *  DRAM row addresses. The counter is reset to $ff in raster line 0 and
     *  decremented by 1 after each refresh access." [C.B.]
     */
    refreshCounter = 0xFF;

    /* "Once somewhere outside of the range of raster lines $30-$f7 (i.e.
     *  outside of the Bad Line range), VCBASE is reset to zero. This is
     *  presumably done in raster line 0, the exact moment cannot be determined
     *  and is irrelevant." [C.B.]
     */
    vcBase = 0;
}

void
VICII::endFrame()
{
    // Switch active screen buffer
    bool first = (currentScreenBuffer == screenBuffer1);
    currentScreenBuffer = first ? screenBuffer2 : screenBuffer1;
    pixelBuffer = currentScreenBuffer;
}

void 
VICII::beginRasterline(u16 line)
{
    verticalFrameFFsetCond = false;

    // Determine if we're inside the VBLANK area (nothing is drawn there).
    vblank = isVBlankLine(line);
 
    // Increase yCounter. The overflow case is handled in cycle 2.
    if (!yCounterOverflow()) yCounter++;
    
    // Check the DEN bit in rasterline 30.
    // Note: The value might change later if control register 1 is written to.
    if (line == 0x30) DENwasSetInRasterline30 = DENbit();

    // Check if this line is a DMA line (bad line)
    // Note: The value might change later if control register 1 is written to.
    if ((badLine = badLineCondition())) {
        delay |= VICSetDisplayState;
    }
    
    // We adjust the position of the first pixel in the pixel buffer to make
    // sure that the screen always appears centered.
    if (vic.isPAL()) {
        bufferoffset = PAL_LEFT_BORDER_WIDTH - 32;
    } else {
        bufferoffset = NTSC_LEFT_BORDER_WIDTH - 32;
    }
}

void 
VICII::endRasterline()
{
    // Set vertical flipflop if condition was hit
    // Do we need to do this here? It is handled in cycle 1 as well.
    if (verticalFrameFFsetCond) {
        setVerticalFrameFF(true);
    }
    
    // Draw debug markers
    if (markIRQLines && yCounter == rasterInterruptLine())
        markLine(VICII_WHITE);
    if (markDMALines && badLine)
        markLine(VICII_RED);
    
    if (!vblank) {
        
        // Make the border look nice (evetually, we should get rid of this)
        expandBorders();

        //
        // Experimental code for RF modulator effect
        //
        /*
        i8 oldR = (pixelBuffer[0] >> 0) & 0xFF;
        i8 oldG = (pixelBuffer[0] >> 8) & 0xFF;
        i8 oldB = (pixelBuffer[0] >> 16) & 0xFF;

        for (unsigned i = 1; i < NTSC_PIXELS; i++) {

            i8 newR = (pixelBuffer[i] >> 0) & 0xFF;
            i8 newG = (pixelBuffer[i] >> 8) & 0xFF;
            i8 newB = (pixelBuffer[i] >> 16) & 0xFF;

            int diffR = newR - oldR;
            int diffG = newG - oldG;
            int diffB = newB - oldB;

            if (diffR > 0) diffR /= 2;
            if (diffG > 0) diffG /= 2;
            if (diffB > 0) diffB /= 2;

            oldR = oldR + diffR;
            oldG = oldG + diffG;
            oldB = oldB + diffB;

            pixelBuffer[i] = (oldR << 0) | (oldG << 8) | (oldB << 16);
        }
        */

        // Advance pixelBuffer
        u16 nextline = c64.rasterLine - PAL_UPPER_VBLANK + 1;
        if (nextline < PAL_RASTERLINES) {
            pixelBuffer = currentScreenBuffer + (nextline * NTSC_PIXELS);
        }
    }
}








