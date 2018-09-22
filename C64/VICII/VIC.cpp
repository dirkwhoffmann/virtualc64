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

/* Cycle accurate VIC II emulation.
   Mostly based on the extensive VIC II documentation by Christian Bauer ([C.B.])
   Many thanks, Christian! 
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

VIC::VIC()
{
	setDescription("VIC");
	debug(3, "  Creating VIC at address %p...\n", this);
    
	markIRQLines = false;
	markDMALines = false;
    emulateGrayDotBug = true;
    
    // Register sub components
    /*
    VirtualComponent *subcomponents[] = { &pixelEngine, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));
     */
    
    // Register snapshot items
    SnapshotItem items[] = {

        // Configuration items
        { &chipModel,                   sizeof(chipModel),                      KEEP_ON_RESET },
        { &glueLogic,                   sizeof(glueLogic),                      KEEP_ON_RESET },
        { &emulateGrayDotBug,           sizeof(emulateGrayDotBug),              KEEP_ON_RESET },

        // Internal state
        { &reg,                         sizeof(reg),                            CLEAR_ON_RESET },
        { &rasterIrqLine,               sizeof(rasterIrqLine),                  CLEAR_ON_RESET },
        { &latchedLightPenX,            sizeof(latchedLightPenX),               CLEAR_ON_RESET },
        { &latchedLightPenY,            sizeof(latchedLightPenY),               CLEAR_ON_RESET },
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
        { &spriteSr,                   sizeof(spriteSr),                        CLEAR_ON_RESET },
        { &spriteSpriteCollision,       sizeof(spriteSpriteCollision),          CLEAR_ON_RESET },
        { &spriteBackgroundColllision,  sizeof(spriteBackgroundColllision),     CLEAR_ON_RESET },

        { &flipflops,                   sizeof(flipflops),                      CLEAR_ON_RESET },
        { &verticalFrameFFsetCond,      sizeof(verticalFrameFFsetCond),         CLEAR_ON_RESET },
        { &leftComparisonVal,           sizeof(leftComparisonVal),              CLEAR_ON_RESET },
        { &rightComparisonVal,          sizeof(rightComparisonVal),             CLEAR_ON_RESET },
        { &upperComparisonVal,          sizeof(upperComparisonVal),             CLEAR_ON_RESET },
        { &lowerComparisonVal,          sizeof(lowerComparisonVal),             CLEAR_ON_RESET },

        { &visibleColumnCnt,            sizeof(visibleColumnCnt),               CLEAR_ON_RESET },
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
        { &lightpenIRQhasOccured,       sizeof(lightpenIRQhasOccured),          CLEAR_ON_RESET },

        { &dataBusPhi1,                 sizeof(dataBusPhi1),                    CLEAR_ON_RESET },
        { &dataBusPhi2,                 sizeof(dataBusPhi2),                    CLEAR_ON_RESET },
        // { &dataBus,                     sizeof(dataBus),                        CLEAR_ON_RESET },
        { &addrBus,                     sizeof(addrBus),                        CLEAR_ON_RESET },
        { &bankAddr,                    sizeof(bankAddr),                       CLEAR_ON_RESET },
      
        { &delay,                       sizeof(delay),                          CLEAR_ON_RESET },
        { &bufferoffset,                sizeof(bufferoffset),                   CLEAR_ON_RESET },

        { NULL,                         0,                                      0 }};

    registerSnapshotItems(items, sizeof(items));
}

VIC::~VIC()
{
}

void
VIC::setC64(C64 *c64)
{
    VirtualComponent::setC64(c64);

    // Assign reference clock to all time delayed variables
    baLine.setClock(&c64->cpu.cycle);
    gAccessResult.setClock(&c64->cpu.cycle);
}

void 
VIC::reset()
{
    VirtualComponent::reset();
    
    yCounter = PAL_HEIGHT;
    
    // Reset timed delay variables
    baLine.reset(0);
    gAccessResult.reset(0);
    
    expansionFF = 0xFF;
    
    // Preset some video parameters to show a blank blue sreen on power up
    memSelect = 0x10;
    memset(&c64->mem.ram[0x400], 32, 40*25);
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
VIC::ping()
{
    VirtualComponent::ping();
    c64->putMessage(isPAL() ? MSG_PAL : MSG_NTSC);
}

void 
VIC::dumpState()
{
    uint8_t ctrl1 = reg.current.ctrl1;
    uint8_t ctrl2 = reg.current.ctrl2; 
    int yscroll = ctrl1 & 0x07;
    int xscroll = ctrl2 & 0x07;
    DisplayMode mode = (DisplayMode)((ctrl1 & 0x60) | (ctrl2 & 0x10));
    
	msg("VIC\n");
	msg("---\n\n");
    msg("       Chip model : %d\n", chipModel);
    msg("              PAL : %s\n", isPAL() ? "yes" : "no");
    msg("             NTSC : %s\n", isNTSC() ? "yes" : "no");
    msg("       Glue logic : %d\n", glueLogic);
    msg("     Gray dot bug : %s\n", hasGrayDotBug() ? "yes" : "no");
    msg("   is656x, is856x : %d %d\n", is656x(), is856x());
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

size_t
VIC::stateSize()
{
    size_t result = VirtualComponent::stateSize();

    result += baLine.stateSize();
    result += gAccessResult.stateSize();

    return result;
}

void
VIC::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    VirtualComponent::loadFromBuffer(buffer);

    baLine.loadFromBuffer(buffer);
    gAccessResult.loadFromBuffer(buffer);
    
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
VIC::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    VirtualComponent::saveToBuffer(buffer);
    
    baLine.saveToBuffer(buffer);
    gAccessResult.saveToBuffer(buffer);
 
    if (*buffer - old != stateSize()) {
        assert(false);
    }
}

void
VIC::setChipModel(VICChipModel model)
{
    debug(2, "VIC::setChipModel(%d)\n", model);
    
    if (!isVICChhipModel(model)) {
        warn("Unknown VICII chip model (%d). Assuming a MOS8565.\n", model);
        model = PAL_8565;
    }
    
    c64->suspend();
    
    chipModel = model;
    updatePalette();
    resetScreenBuffers();
    c64->updateVicFunctionTable();
    
    switch(chipModel) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            c64->setClockFrequency(PAL_CLOCK_FREQUENCY);
            c64->putMessage(MSG_PAL);
            break;
            
        case NTSC_6567:
        case NTSC_6567_R56A:
        case NTSC_8562:
            c64->setClockFrequency(NTSC_CLOCK_FREQUENCY);
            c64->putMessage(MSG_NTSC);
            break;
            
        default:
            assert(false);
    }
    
    c64->resume();
}

void
VIC::setVideoPalette(VICPalette type)
{
    if (!isVICPalette(type)) {
        warn("Unknown palette type (%d). Assuming color palette.\n", type);
        type = COLOR_PALETTE;
    }
    
    palette = type;
    updatePalette();
}

void
VIC::setGlueLogic(GlueLogic type)
{
    debug(2, "VIC::setGlueLogic(%d)\n", type);
    
    if (!isGlueLogic(type)) {
        warn("Unknown glue logic type (%d). Assuming discrete logic.\n", type);
        type = GLUE_DISCRETE;
    }
    
    c64->suspend();
    glueLogic = type;
    c64->resume();
}

unsigned
VIC::getClockFrequency()
{
    switch (chipModel) {
            
        case NTSC_6567:
        case NTSC_8562:
        case NTSC_6567_R56A:
            return NTSC_CLOCK_FREQUENCY;
            
        default:
            return PAL_CLOCK_FREQUENCY;
    }
}

unsigned
VIC::getCyclesPerRasterline()
{
    switch (chipModel) {
            
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
VIC::isLastCycleInRasterline(unsigned cycle)
{
    return cycle >= getCyclesPerRasterline();
}

unsigned
VIC::getRasterlinesPerFrame()
{
    switch (chipModel) {
            
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
VIC::isVBlankLine(unsigned rasterline)
{
    switch (chipModel) {
            
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
VIC::screenBuffer() {
    if (currentScreenBuffer == screenBuffer1) {
        return screenBuffer2;
    } else {
        return screenBuffer1;
    }
}

void
VIC::resetScreenBuffers()
{
    for (unsigned line = 0; line < PAL_RASTERLINES; line++) {
        for (unsigned i = 0; i < NTSC_PIXELS; i++) {
            screenBuffer1[line * NTSC_PIXELS + i] =
            screenBuffer2[line * NTSC_PIXELS + i] =
            (line % 2) ? rgbaTable[8] : rgbaTable[9];
        }
    }
}

uint16_t
VIC::rasterline()
{
    return c64->rasterLine;
}

uint8_t
VIC::rastercycle()
{
    return c64->rasterCycle;
}


//
// Frame flipflops
//

void
VIC::checkVerticalFrameFF()
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
VIC::checkFrameFlipflopsLeft(uint16_t comparisonValue)
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
VIC::checkFrameFlipflopsRight(uint16_t comparisonValue)
{
    /* "1. If the X coordinate reaches the right comparison value, the main
     *     border flip flop is set." [C.B.]
     */
    if (comparisonValue == rightComparisonVal) {
        setMainFrameFF(true);
    }
}

void
VIC::setVerticalFrameFF(bool value)
{
    if (value != flipflops.delayed.vertical) {
        flipflops.current.vertical = value;
        delay |= VICUpdateFlipflops;
    }
}

void
VIC::setMainFrameFF(bool value)
{
    if (value != flipflops.delayed.main) {
        flipflops.current.main = value;
        delay |= VICUpdateFlipflops;
    }
}

bool
VIC::badLineCondition() {
    
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
VIC::updateBA(uint8_t value)
{
    if (value != baLine.current()) {
       
        if (value) {
            baLine.write(value);
        } else {
            baLine.clear();
        }
        
        c64->cpu.setRDY(value == 0);
    }
}

void 
VIC::triggerIrq(uint8_t source)
{
    assert(source == 1 || source == 2 || source == 4 || source == 8);
    
    irr |= source;
    delay |= VICUpdateIrqLine;
}

uint16_t
VIC::lightpenX()
{
    uint8_t cycle = c64->rasterCycle; 
    
    switch (chipModel) {
            
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

uint16_t
VIC::lightpenY()
{
    return yCounter;
}

void
VIC::setLP(bool value)
{
    // A negative transition on LP triggers a lightpen event.
    if (FALLING_EDGE(lpLine, value)) {
        delay |= VICLpTransition;
    }
    
    lpLine = value;
}

void
VIC::checkForLightpenIrq()
{
    uint8_t vicCycle = c64->rasterCycle;
    // debug("Negative LP transition at rastercycle %d\n", vicCycle);

    // An interrupt is suppressed if ...
    
    // ... a previous interrupt has occured in the current frame.
    if (lightpenIRQhasOccured)
        return;

    // ... we are in the last PAL rasterline and not in cycle 1.
    if (yCounter == PAL_HEIGHT - 1 && vicCycle != 1)
        return;
    
    // Latch coordinates
    latchedLightPenX = lightpenX() / 2;
    latchedLightPenY = lightpenY();
    debug("Frame %lld Rasterline %d cycle = %d\n", c64->frame, rasterline(), c64->rasterCycle);
    debug("Lightpen x / y = %d %d\n", lightpenX() / 2, lightpenY());
    
    // Newer VIC models trigger an interrupt immediately
    if (!delayedLightPenIrqs()) triggerIrq(8);
    
    // Lightpen interrupts can only occur once per frame
    lightpenIRQhasOccured = true;
}

void
VIC::checkForLightpenIrqAtStartOfFrame()
{
    // This function is called at the beginning of a frame, only.
    assert(c64->rasterLine == 0);
    assert(c64->rasterCycle == 1);
    assert(lightpenIRQhasOccured == false);
 
    // Do we latch a new coordinate here? 
    if (!delayedLightPenIrqs()) {
        latchedLightPenX = lightpenX() / 2;
        latchedLightPenY = lightpenY();
    }
    
    // Trigger interrupt
    triggerIrq(8);

    // Lightpen interrupts can only occur once per frame
    lightpenIRQhasOccured = true;
}


//
// Sprites
//

uint8_t
VIC::spriteDepth(uint8_t nr)
{
    return
    GET_BIT(reg.delayed.sprPriority, nr) ?
    (SPRITE_LAYER_BG_DEPTH | nr) :
    (SPRITE_LAYER_FG_DEPTH | nr);
}

uint8_t
VIC::compareSpriteY()
{
    uint8_t result = 0;
    
    for (unsigned i = 0; i < 8; i++) {
        result |= (reg.current.sprY[i] == (yCounter & 0xFF)) << i;
    }
    
    return result;
}

void
VIC::turnSpriteDmaOff()
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
VIC::turnSpriteDmaOn()
{
    /* "In the first phases of cycle 55 and 56, the VIC checks for every sprite
     *  if the corresponding MxE bit in register $d015 is set and the Y
     *  coordinate of the sprite (odd registers $d001-$d00f) match the lower 8
     *  bits of RASTER. If this is the case and the DMA for the sprite is still
     *  off, the DMA is switched on, MCBASE is cleared, and if the MxYE bit is
     *  set the expansion flip flip is reset." [C.B.]
     */
    uint8_t risingEdges = ~spriteDmaOnOff & (reg.current.sprEnable & compareSpriteY());
    
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(risingEdges,i))
            mcbase[i] = 0;
    }
    spriteDmaOnOff |= risingEdges;
    expansionFF |= risingEdges;
}

void
VIC::turnSpritesOnOrOff()
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
VIC::updateSpriteShiftRegisters() {
    if (isSecondDMAcycle) {
        for (unsigned sprite = 0; sprite < 8; sprite++) {
            if (GET_BIT(isSecondDMAcycle, sprite)) {
                loadShiftRegister(sprite);
            }
        }
    }
}

void 
VIC::beginFrame()
{
    // isVisibleColumn = false;
	lightpenIRQhasOccured = false;

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
    
    // Trigger lightpen interrupt if lp line is down
    if (!lpLine)
        checkForLightpenIrqAtStartOfFrame();
}

void
VIC::endFrame()
{
    // Switch active screen buffer
    bool first = (currentScreenBuffer == screenBuffer1);
    currentScreenBuffer = first ? screenBuffer2 : screenBuffer1;
    pixelBuffer = currentScreenBuffer;
}

void 
VIC::beginRasterline(uint16_t line)
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
    badLine = badLineCondition();
    displayState |= badLine;
    
    // Prepare sprite pixel shift register
    for (unsigned i = 0; i < 8; i++) {
        spriteSr[i].remaining_bits = -1;
        spriteSr[i].colBits = 0;
    }
    
    // We adjust the position of the first pixel in the pixel buffer to make
    // sure that the screen always appears centered.
    if (c64->vic.isPAL()) {
        bufferoffset = PAL_LEFT_BORDER_WIDTH - 32;
    } else {
        bufferoffset = NTSC_LEFT_BORDER_WIDTH - 32;
    }
}

void 
VIC::endRasterline()
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
        
        // Advance pixelBuffer
        uint16_t nextline = c64->rasterLine - PAL_UPPER_VBLANK + 1;
        if (nextline < PAL_RASTERLINES) {
            pixelBuffer = currentScreenBuffer + (nextline * NTSC_PIXELS);
        }
    }
}








