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
    config.grayDotBug = true;
    config.palette = PALETTE_COLOR;
    config.cutLayers = 0xFF;
    config.cutOpacity = 0xFF;
    config.dmaOpacity = 0x80;
    config.dmaDebug = false;
    config.dmaChannel[MEMACCESS_R] = true;
    config.dmaChannel[MEMACCESS_I] = true;
    config.dmaChannel[MEMACCESS_C] = true;
    config.dmaChannel[MEMACCESS_G] = true;
    config.dmaChannel[MEMACCESS_P] = true;
    config.dmaChannel[MEMACCESS_S] = true;

    // Assign default DMA debugging colors
    setDmaDebugColor(MEMACCESS_R, RgbColor(1.0, 0.0, 0.0));
    setDmaDebugColor(MEMACCESS_I, RgbColor(1.0, 0.8, 0.0));
    setDmaDebugColor(MEMACCESS_C, RgbColor(1.0, 1.0, 0.0));
    setDmaDebugColor(MEMACCESS_G, RgbColor(0.0, 1.0, 1.0));
    setDmaDebugColor(MEMACCESS_P, RgbColor(0.0, 1.0, 0.0));
    setDmaDebugColor(MEMACCESS_S, RgbColor(0.0, 0.5, 1.0));
    
    // Assign reference clock to all time delayed variables
    baLine.setClock(&cpu.cycle);
    gAccessResult.setClock(&cpu.cycle);
    
    // Create random background noise pattern
    const usize noiseSize = 2 * 512 * 512;
    noise = new u32[noiseSize];
    for (usize i = 0; i < noiseSize; i++) {
        noise[i] = rand() % 2 ? 0xFF000000 : 0xFFFFFFFF;
    }
}

void
VICII::_initialize()
{
    setRevision(VICREV_PAL_8565);
    
    config.hideSprites = false;
    config.checkSBCollisions = true;
    config.checkSSCollisions = true;
}

void 
VICII::_reset()
{
    RESET_SNAPSHOT_ITEMS
    
    // Reset counters
    yCounter = (u32)getRasterlinesPerFrame();
        
    // Reset the memory source lookup table
    setUltimax(false);

    // Reset the sprite logic
    expansionFF = 0xFF;
    
    // Preset some video parameters to show a blank blue sreen on power up
    memSelect = 0x10;
    reg.delayed.ctrl1 = 0x10;
    reg.current.ctrl1 = 0x10;
    reg.delayed.colors[COLREG_BORDER] = VICII_LIGHT_BLUE;
    reg.current.colors[COLREG_BORDER] = VICII_LIGHT_BLUE;
    reg.delayed.colors[COLREG_BG0] = VICII_BLUE;
    reg.current.colors[COLREG_BG0] = VICII_BLUE;
    
    // Reset the drame flipflops
    leftComparisonVal = leftComparisonValue();
    rightComparisonVal = rightComparisonValue();
    upperComparisonVal = upperComparisonValue();
    lowerComparisonVal = lowerComparisonValue();
        
    // Reset the screen buffer pointers
    emuTexture = emuTexturePtr = emuTexture1;
    dmaTexture = dmaTexturePtr = dmaTexture1;
}

void
VICII::resetEmuTexture(int nr)
{
    assert(nr == 1 || nr == 2);
    int *p = nr == 1 ? emuTexture1 : emuTexture2;

    // Determine the HBLANK / VBLANK area
    long width = isPAL() ? PAL_PIXELS : NTSC_PIXELS;
    long height = getRasterlinesPerFrame();
    
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {

            int pos = y * TEX_WIDTH + x;

            if (y < height && x < width) {
                
                // Draw a checkerboard pattern inside the used texture area
                p[pos] = (y / 4) % 2 == (x / 8) % 2 ? 0xFF222222 : 0xFF444444;

            } else {
                
                // Draw black pixels outside the used texture area
                p[pos] = 0xFF000000;
            }
        }
    }
}

void
VICII::resetDmaTexture(int nr)
{
    assert(nr == 1 || nr == 2);
    int *p = nr == 1 ? dmaTexture1 : dmaTexture2;

    for (int i = 0; i < TEX_HEIGHT * TEX_WIDTH; i++) {
        p[i] = 0xFF000000;
    }
}

long
VICII::getConfigItem(Option option)
{
    switch (option) {
            
        case OPT_VIC_REVISION:     return config.revision;
        case OPT_PALETTE:          return config.palette;
        case OPT_GRAY_DOT_BUG:     return config.grayDotBug;
        case OPT_GLUE_LOGIC:       return config.glueLogic;
        case OPT_DMA_DEBUG:        return config.dmaDebug;
        case OPT_DMA_CHANNEL_R:    return config.dmaChannel[MEMACCESS_R];
        case OPT_DMA_CHANNEL_I:    return config.dmaChannel[MEMACCESS_I];
        case OPT_DMA_CHANNEL_C:    return config.dmaChannel[MEMACCESS_C];
        case OPT_DMA_CHANNEL_G:    return config.dmaChannel[MEMACCESS_G];
        case OPT_DMA_CHANNEL_P:    return config.dmaChannel[MEMACCESS_P];
        case OPT_DMA_CHANNEL_S:    return config.dmaChannel[MEMACCESS_S];
        case OPT_DMA_COLOR_R:      return config.dmaColor[MEMACCESS_R];
        case OPT_DMA_COLOR_I:      return config.dmaColor[MEMACCESS_I];
        case OPT_DMA_COLOR_C:      return config.dmaColor[MEMACCESS_C];
        case OPT_DMA_COLOR_G:      return config.dmaColor[MEMACCESS_G];
        case OPT_DMA_COLOR_P:      return config.dmaColor[MEMACCESS_P];
        case OPT_DMA_COLOR_S:      return config.dmaColor[MEMACCESS_S];
        case OPT_DMA_DISPLAY_MODE: return config.dmaDisplayMode;
        case OPT_DMA_OPACITY:      return config.dmaOpacity;
        case OPT_HIDE_SPRITES:     return config.hideSprites;
        case OPT_CUT_LAYERS:       return config.cutLayers;
        case OPT_CUT_OPACITY:      return config.cutOpacity;
        case OPT_SS_COLLISIONS:    return config.checkSSCollisions;
        case OPT_SB_COLLISIONS:    return config.checkSBCollisions;

        default:
            assert(false);
            return 0;
    }
}

bool
VICII::setConfigItem(Option option, long value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
            
            if (!VICRevisionEnum::verify(value)) return false;
            if (config.revision == value) return false;
            
            suspend();
            config.revision = (VICRevision)value;
            setRevision(config.revision);
            resume();
            return true;
            
        case OPT_PALETTE:
            
            if (!PaletteEnum::verify(value)) return false;
            if (config.palette == value) return false;
            
            suspend();
            config.palette = (Palette)value;
            updatePalette();
            resume();
            return true;
            
        case OPT_GRAY_DOT_BUG:
            
            config.grayDotBug = value;
            return true;
            
        case OPT_HIDE_SPRITES:
            
            config.hideSprites = value;
            return true;

        case OPT_DMA_DEBUG:
            
            if (config.dmaDebug == value) {
                return false;
            }
            suspend();
            config.dmaDebug = value;
            resetDmaTextures();
            c64.updateVicFunctionTable();
            resume();
            return true;
            
        case OPT_DMA_CHANNEL_R:
            
            config.dmaChannel[MEMACCESS_R] = value;
            return true;
            
        case OPT_DMA_CHANNEL_I:
            
            config.dmaChannel[MEMACCESS_I] = value;
            return true;
            
        case OPT_DMA_CHANNEL_C:
            
            config.dmaChannel[MEMACCESS_C] = value;
            return true;
            
        case OPT_DMA_CHANNEL_G:
            
            config.dmaChannel[MEMACCESS_G] = value;
            return true;
            
        case OPT_DMA_CHANNEL_P:
            
            config.dmaChannel[MEMACCESS_P] = value;
            return true;
            
        case OPT_DMA_CHANNEL_S:
            
            config.dmaChannel[MEMACCESS_S] = value;
            return true;

        case OPT_DMA_COLOR_R:
            
            setDmaDebugColor(MEMACCESS_R, GpuColor((u32)value));
            config.dmaColor[MEMACCESS_R] = (u32)value;
            return true;
            
        case OPT_DMA_COLOR_I:
            
            config.dmaColor[MEMACCESS_I] = (u32)value;
            return true;

        case OPT_DMA_COLOR_C:
            
            config.dmaColor[MEMACCESS_C] = (u32)value;
            return true;

        case OPT_DMA_COLOR_G:
            
            config.dmaColor[MEMACCESS_G] = (u32)value;
            return true;

        case OPT_DMA_COLOR_P:
            
            config.dmaColor[MEMACCESS_P] = (u32)value;
            return true;

        case OPT_DMA_COLOR_S:
            
            config.dmaColor[MEMACCESS_S] = (u32)value;
            return true;
            
        case OPT_DMA_OPACITY:
            
            config.dmaOpacity = value;
            return false; // 'false' to avoid a MSG_CONFIG being sent
            
        case OPT_DMA_DISPLAY_MODE:
            
            config.dmaDisplayMode = (DmaDisplayMode)value;
            return true;

        case OPT_CUT_LAYERS:
            
            config.cutLayers = value;
            return true;
            
        case OPT_CUT_OPACITY:
            
            config.cutOpacity = value;
            return false; // False to avoid MSG_CONFIG being sent to the GUI
            
        case OPT_SS_COLLISIONS:
            
            config.checkSSCollisions = value;
            return true;

        case OPT_SB_COLLISIONS:
            
            config.checkSBCollisions = value;
            return true;

        case OPT_GLUE_LOGIC:
            
            if (!GlueLogicEnum::verify(value)) return false;
            if (config.glueLogic == value) return false;
            
            config.glueLogic = (GlueLogic)value;
            return true;
            
        default:
            return false;
    }
}

void
VICII::setRevision(VICRevision revision)
{
    assert_enum(VICRevision, revision);
    
    debug(VIC_DEBUG, "setRevision(%ld)\n", (long)revision);
    
    config.revision = revision;
    
    updatePalette();
    resetEmuTextures();
    resetDmaTextures();
    c64.updateVicFunctionTable();
    
    c64.putMessage(isPAL() ? MSG_PAL : MSG_NTSC);
}

void
VICII::setDmaDebugColor(MemAccess type, GpuColor color)
{
    assert_enum(MemAccess, type);
    
    config.dmaColor[type] = color.rawValue;
        
    // Update the color lookup table
    debugColor[type][0] = color.shade(0.3).rawValue;
    debugColor[type][1] = color.shade(0.1).rawValue;
    debugColor[type][2] = color.tint(0.1).rawValue;
    debugColor[type][3] = color.tint(0.3).rawValue;
}

void
VICII::setDmaDebugColor(MemAccess type, RgbColor color)
{
    setDmaDebugColor(type, GpuColor(color));
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
VICII::_dumpConfig() const
{
    msg("    Chip model : %lld (%s)\n", config.revision, VICRevisionEnum::key(config.revision));
    msg("  Gray dot bug : %s\n", config.grayDotBug ? "yes" : "no");
    msg("           PAL : %s\n", isPAL() ? "yes" : "no");
    msg("          NTSC : %s\n", isNTSC() ? "yes" : "no");
    msg("is656x, is856x : %d %d\n", is656x(), is856x());
    msg("    Glue logic : %lld (%s)\n", config.glueLogic, GlueLogicEnum::key(config.glueLogic));
}

void 
VICII::_dump() const
{
    u8 ctrl1 = reg.current.ctrl1;
    u8 ctrl2 = reg.current.ctrl2; 
    int yscroll = ctrl1 & 0x07;
    int xscroll = ctrl2 & 0x07;
    DisplayMode mode = (DisplayMode)((ctrl1 & 0x60) | (ctrl2 & 0x10));
    
	msg("     Bank address : %04X\n", bankAddr);
    msg("    Screen memory : %04X\n", VM13VM12VM11VM10() << 6);
	msg(" Character memory : %04X\n", (CB13CB12CB11() << 10) % 0x4000);
	msg("X/Y raster scroll : %d / %d\n", xscroll, yscroll);
    msg("    Control reg 1 : %02X\n", reg.current.ctrl1);
    msg("    Control reg 2 : %02X\n", reg.current.ctrl2);
	msg("     Display mode : %s\n", DisplayModeEnum::key(mode));
    msg("          badLine : %s\n", badLine ? "yes" : "no");
    msg("    DENwasSetIn30 : %s\n", DENwasSetInRasterline30 ? "yes" : "no");
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
	for (usize i = 0; i < 8; i++)
		msg("%d ", (spriteDmaOnOff & (1 << i)) != 0 );
	msg(")\n");
	msg("      Y expansion : %02X ( ", expansionFF);
	for (usize i = 0; i < 8; i++) 
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

void
VICII::_run()
{
}

bool
VICII::isPAL(VICRevision revision)
{
    return revision & (VICREV_PAL_6569_R1 | VICREV_PAL_6569_R3 | VICREV_PAL_8565);
}

bool
VICII::isNTSC(VICRevision revision)
{
     return revision & (VICREV_NTSC_6567 | VICREV_NTSC_6567_R56A | VICREV_NTSC_8562);
}

bool
VICII::is856x(VICRevision revision)
{
     return revision & (VICREV_PAL_8565 | VICREV_NTSC_8562);
}
 
bool
VICII::is656x(VICRevision revision)
{
     return revision & ~(VICREV_PAL_8565 | VICREV_NTSC_8562);
}

bool
VICII::delayedLightPenIrqs(VICRevision revision)
{
     return revision & (VICREV_PAL_6569_R1 | VICREV_NTSC_6567_R56A);
}

unsigned
VICII::getFrequency(VICRevision revision)
{
    switch (revision) {
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_8562:
        case VICREV_NTSC_6567_R56A:
            return NTSC_CLOCK_FREQUENCY;
            
        default:
            return PAL_CLOCK_FREQUENCY;
    }
}

unsigned
VICII::getCyclesPerLine(VICRevision revision)
{
    switch (revision) {
            
        case VICREV_NTSC_6567_R56A:
            return 64;
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_8562:
            return 65;
            
        default:
            return 63;
    }
}

bool
VICII::isLastCycleInRasterline(unsigned cycle) const
{
    return cycle >= getCyclesPerLine();
}

long
VICII::getRasterlinesPerFrame() const
{
    switch (config.revision) {
            
        case VICREV_NTSC_6567_R56A:
            return 262;
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_8562:
            return 263;
            
        default:
            return 312;
    }
}

long
VICII::numVisibleRasterlines() const
{
    switch (config.revision) {
            
        case VICREV_NTSC_6567_R56A:
            return 234;
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_8562:
            return 235;
            
        default:
            return 284;
    }
}

bool
VICII::isVBlankLine(unsigned rasterline) const
{
    switch (config.revision) {
            
        case VICREV_NTSC_6567_R56A:
            return rasterline < 16 || rasterline >= 16 + 234;
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_8562:
            return rasterline < 16 || rasterline >= 16 + 235;
            
        default:
            return rasterline < 16 || rasterline >= 16 + 284;
    }
}

void *
VICII::stableEmuTexture() const
{
    return emuTexture == emuTexture1 ? emuTexture2 : emuTexture1;
}

void *
VICII::stableDmaTexture() const
{
    return dmaTexture == dmaTexture1 ? dmaTexture2 : dmaTexture1;
}

u32 *
VICII::getNoise() const
{
    int offset = rand() % (512 * 512);
    return noise + offset;
}

u16
VICII::rasterline() const
{
    return c64.rasterLine;
}

u8
VICII::rastercycle() const
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
VICII::badLineCondition() const
{    
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
VICII::lightpenX() const
{
    u8 cycle = c64.rasterCycle;
    
    switch (config.revision) {
            
        case VICREV_PAL_6569_R1:
        case VICREV_PAL_6569_R3:

            return 4 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);

        case VICREV_PAL_8565:
            
            return 2 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_6567_R56A:
            
            return 4 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        case VICREV_NTSC_8562:
            
            return 2 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        default:
            assert(false);
    }
}

u16
VICII::lightpenY() const
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
    if (yCounter == 311 && vicCycle != 1)
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
            
        case VICREV_PAL_6569_R1:
        case VICREV_PAL_6569_R3:
        case VICREV_PAL_8565:
            
            latchedLPX = 209;
            latchedLPY = 0;
            break;
            
        case VICREV_NTSC_6567:
        case VICREV_NTSC_6567_R56A:
        case VICREV_NTSC_8562:
            
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
VICII::spriteDepth(u8 nr) const
{
    return
    GET_BIT(reg.delayed.sprPriority, nr) ?
    (SPRITE_LAYER_BG_DEPTH | nr) :
    (SPRITE_LAYER_FG_DEPTH | nr);
}

u8
VICII::compareSpriteY() const
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
    // Run the DMA debugger (if enabled)
    if (config.dmaDebug) {
        computeOverlay();
    }

    // Switch texture buffers
    if (emuTexture == emuTexture1) {
        
        assert(dmaTexture == dmaTexture1);
        emuTexture = emuTexturePtr = emuTexture2;
        dmaTexture = dmaTexturePtr = dmaTexture2;
        if (config.dmaDebug) { resetEmuTexture(2); resetDmaTexture(2); }

    } else {
        
        assert(emuTexture == emuTexture2);
        assert(dmaTexture == dmaTexture2);
        emuTexture = emuTexturePtr = emuTexture1;
        dmaTexture = dmaTexturePtr = dmaTexture1;
        if (config.dmaDebug) { resetEmuTexture(1); resetDmaTexture(1); }
    }
}

void
VICII::processDelayedActions()
{
    if (delay & VICUpdateIrqLine) {
        if (irr & imr) {
            cpu.pullDownIrqLine(INTSRC_VIC);
        } else {
            cpu.releaseIrqLine(INTSRC_VIC);
        }
    }
    if (delay & VICUpdateFlipflops) {
        flipflops.delayed = flipflops.current;
    }
    if (delay & VICSetDisplayState) {
        displayState |= badLine;
    }
    if (delay & VICUpdateRegisters) {
        reg.delayed = reg.current;
    }

    // Less frequent actions
    if (delay & (VICLpTransition | VICUpdateBankAddr | VICClrSprSprCollReg | VICClrSprBgCollReg)) {
        
        if (delay & VICLpTransition) {
            checkForLightpenIrq();
        }
        if (delay & VICUpdateBankAddr) {
            updateBankAddr();
            // bankAddr = (~cia2.getPA() & 0x03) << 14;
        }
        if (delay & VICClrSprSprCollReg) {
            spriteSpriteCollision = 0;
        }
        if (delay & VICClrSprBgCollReg) {
            spriteBackgroundColllision = 0;
        }
    }
    
    delay = (delay << 1) & VICClearanceMask;
}

void 
VICII::beginRasterline(u16 line)
{
    verticalFrameFFsetCond = false;

    // Determine if we're inside the VBLANK area
    vblank = isVBlankLine(line);
 
    // Increase the y counter (overflow is handled in cycle 2)
    if (!yCounterOverflow()) yCounter++;
    
    // Check the DEN bit in rasterline 30 (value might change later)
    if (line == 0x30) DENwasSetInRasterline30 = DENbit();

    // Check if this line is a DMA line (bad line) (value might change later)
    if ((badLine = badLineCondition())) delay |= VICSetDisplayState;
    
    // Reset the pixel buffer offset
    bufferoffset = 0;
}

void 
VICII::endRasterline()
{
    // Set vertical flipflop if condition was hit
    // Do we need to do this here? It is handled in cycle 1 as well.
    if (verticalFrameFFsetCond) {
        setVerticalFrameFF(true);
    }
    
    // Cut out layers if requested
    if (config.cutLayers) cutLayers();

    // Prepare buffers ready for the next line
    for (unsigned i = 0; i < TEX_WIDTH; i++) { zBuffer[i] = pixelSource[i] = 0; }
        
    // Advance texture pointers
    emuTexturePtr = emuTexture + (c64.rasterLine * TEX_WIDTH);
    dmaTexturePtr = dmaTexture + (c64.rasterLine * TEX_WIDTH);
}
