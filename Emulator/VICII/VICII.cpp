// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"
#include "C64.h"
#include "IO.h"

#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

VICII::VICII(C64 &ref) : SubComponent(ref), dmaDebugger(ref)
{    
    subComponents = std::vector<C64Component *> { &dmaDebugger };

    // Assign reference clock to all time delayed variables
    baLine.setClock(&cpu.cycle);
    gAccessResult.setClock(&cpu.cycle);
    
    // Create random background noise pattern
    const usize noiseSize = 16 * 512 * 512;
    noise = new u32[noiseSize];
    for (usize i = 0; i < noiseSize; i++) {
        noise[i] = rand() % 2 ? 0xFF000000 : 0xFFFFFFFF;
    }
}

void 
VICII::_reset(bool hard)
{
    RESET_SNAPSHOT_ITEMS(hard)
     
    if (hard) {

        clearStats();
        
        // See README of VICE test VICII/spritemcbase
        for (isize i = 0; i < 8; i++) mcbase[i] = is656x() ? 0x3F : 0x00;
        
        // Reset counters
        yCounter = (u32)getLinesPerFrame();
        
        // Reset the memory source lookup table
        setUltimax(false);
        
        // Reset the sprite logic
        expansionFF = 0xFF;
        
        // Reset the frame flipflops
        leftComparisonVal = leftComparisonValue();
        rightComparisonVal = rightComparisonValue();
        upperComparisonVal = upperComparisonValue();
        lowerComparisonVal = lowerComparisonValue();
        
        // Reset the screen buffer pointers
        emuTexture = emuTexture1;
        dmaTexture = dmaTexture1;
    }
}

void
VICII::resetEmuTexture(int nr)
{
    if (nr == 1) { resetTexture(emuTexture1); return; }
    if (nr == 2) { resetTexture(emuTexture2); return; }
    
    assert(false);
}

void
VICII::resetDmaTexture(int nr)
{
    assert(nr == 1 || nr == 2);
    u32 *p = nr == 1 ? dmaTexture1 : dmaTexture2;

    for (int i = 0; i < TEX_HEIGHT * TEX_WIDTH; i++) {
        p[i] = 0xFF000000;
    }
}

void
VICII::resetTexture(u32 *p)
{
    // Determine the HBLANK / VBLANK area
    long width = isPAL() ? PAL_PIXELS : NTSC_PIXELS;
    long height = getLinesPerFrame();
    
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {

            int pos = y * TEX_WIDTH + x;

            if (y < height && x < width) {
                
                // Draw black pixels inside the used area
                p[pos] = 0xFF000000;

            } else {

                // Draw a checkerboard pattern outside the used area
                p[pos] = (y / 4) % 2 == (x / 8) % 2 ? 0xFF222222 : 0xFF444444;
            }
        }
    }
}

VICIIConfig
VICII::getDefaultConfig()
{
    VICIIConfig defaults;
    
    defaults.revision = VICII_PAL_8565;
    defaults.speed = VICII_NATIVE;
    defaults.powerSave = true;
    defaults.grayDotBug = true;
    defaults.glueLogic = GLUE_LOGIC_DISCRETE;

    defaults.palette = PALETTE_COLOR;
    defaults.brightness = 50;
    defaults.contrast = 100;
    defaults.saturation = 50;
    
    defaults.hideSprites = false;
    
    defaults.checkSSCollisions = true;
    defaults.checkSBCollisions = true;
    
    return defaults;
}

void
VICII::resetConfig()
{
    VICIIConfig defaults = getDefaultConfig();
    
    setConfigItem(OPT_VIC_REVISION, defaults.revision);
    setConfigItem(OPT_VIC_SPEED, defaults.speed);
    setConfigItem(OPT_VIC_POWER_SAVE, defaults.powerSave);
    setConfigItem(OPT_GRAY_DOT_BUG, defaults.grayDotBug);
    setConfigItem(OPT_GLUE_LOGIC, defaults.glueLogic);

    setConfigItem(OPT_PALETTE, defaults.palette);
    setConfigItem(OPT_BRIGHTNESS, defaults.brightness);
    setConfigItem(OPT_CONTRAST, defaults.contrast);
    setConfigItem(OPT_SATURATION, defaults.saturation);

    setConfigItem(OPT_HIDE_SPRITES, defaults.hideSprites);
    
    setConfigItem(OPT_SB_COLLISIONS, defaults.checkSSCollisions);
    setConfigItem(OPT_SS_COLLISIONS, defaults.checkSBCollisions);
}

i64
VICII::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_VIC_REVISION:      return config.revision;
        case OPT_VIC_SPEED:         return config.speed;
        case OPT_VIC_POWER_SAVE:    return config.powerSave;
        case OPT_PALETTE:           return config.palette;
        case OPT_BRIGHTNESS:        return config.brightness;
        case OPT_CONTRAST:          return config.contrast;
        case OPT_SATURATION:        return config.saturation;
        case OPT_GRAY_DOT_BUG:      return config.grayDotBug;
        case OPT_GLUE_LOGIC:        return config.glueLogic;
        case OPT_HIDE_SPRITES:      return config.hideSprites;
        case OPT_SS_COLLISIONS:     return config.checkSSCollisions;
        case OPT_SB_COLLISIONS:     return config.checkSBCollisions;

        default:
            assert(false);
            return 0;
    }
}

void
VICII::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_VIC_REVISION:
            
            if (!VICIIRevisionEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, VICIIRevisionEnum::keyList());
            }
            
            suspend();
            setRevision((VICIIRevision)value);
            resume();
            return;

        case OPT_VIC_SPEED:
            
            if (!VICIISpeedEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, VICIISpeedEnum::keyList());
            }
            
            suspend();
            // setSpeed((VICIISpeed)value);
            config.speed = value;
            resume();
            return;

        case OPT_VIC_POWER_SAVE:
            
            config.powerSave = value;
            return;
            
        case OPT_PALETTE:
            
            if (!PaletteEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, PaletteEnum::keyList());
            }
            
            suspend();
            config.palette = (Palette)value;
            updatePalette();
            resume();
            return;
            
        case OPT_BRIGHTNESS:
            
            if (config.brightness < 0 || config.brightness > 100) {
                throw VC64Error(ERROR_OPT_INV_ARG, "Expected 0...100");
            }

            config.brightness = value;
            updatePalette();
            return;
            
        case OPT_CONTRAST:

            if (config.contrast < 0 || config.contrast > 100) {
                throw VC64Error(ERROR_OPT_INV_ARG, "Expected 0...100");
            }

            config.contrast = value;
            updatePalette();
            return;

        case OPT_SATURATION:
        
            if (config.saturation < 0 || config.saturation > 100) {
                throw VC64Error(ERROR_OPT_INV_ARG, "Expected 0...100");
            }

            config.saturation = value;
            updatePalette();
            return;

        case OPT_GRAY_DOT_BUG:
            
            config.grayDotBug = value;
            return;
            
        case OPT_HIDE_SPRITES:
            
            config.hideSprites = value;
            return;
            
        case OPT_SS_COLLISIONS:
            
            config.checkSSCollisions = value;
            return;

        case OPT_SB_COLLISIONS:
            
            config.checkSBCollisions = value;
            return;

        case OPT_GLUE_LOGIC:
            
            if (!GlueLogicEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INV_ARG, GlueLogicEnum::keyList());
            }
            
            config.glueLogic = (GlueLogic)value;
            return;
            
        default:
            return;
    }
}

void
VICII::setRevision(VICIIRevision revision)
{
    assert_enum(VICIIRevision, revision);
    assert(!isRunning());
    
    debug(VIC_DEBUG, "setRevision(%s)\n", VICIIRevisionEnum::key(revision));

    /* If the VICII revision is changed while the emulator is powered on, some
     * precautions must be taken. First, we interrupt a running screen capture.
     * After that, we move the emulator to a safe spot by finishing the current
     * frame.
     */
    if (isPoweredOn()) {
        recorder.stopRecording();
        c64.finishFrame();
    }
    
    config.revision = revision;
    isFirstDMAcycle = isSecondDMAcycle = 0;
    updatePalette();
    resetEmuTextures();
    resetDmaTextures();
    vic.updateVicFunctionTable();
    
    c64.putMessage(isPAL() ? MSG_PAL : MSG_NTSC);
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
        info.displayMode = reg.current.mode;
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
        
        info.irqRasterline = rasterIrqLine;
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
VICII::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::Config) {

        os << tab("Chip model");
        os << VICIIRevisionEnum::key(config.revision) << std::endl;
        os << tab("Speed");
        os << VICIISpeedEnum::key(config.speed) << std::endl;
        os << tab("Power save mode");
        os << bol(config.powerSave, "during warp", "never") << std::endl;
        os << tab("Gray dot bug");
        os << bol(config.grayDotBug) << std::endl;
        os << tab("PAL");
        os << bol(isPAL()) << std::endl;
        os << tab("NTSC");
        os << bol (isNTSC()) << std::endl;
        os << tab("is656x");
        os << bol(is656x()) << std::endl;
        os << tab("is856x");
        os << bol(is856x()) << std::endl;
        os << tab("Glue logic");
        os << GlueLogicEnum::key(config.glueLogic) << std::endl;
        os << tab("Check SS collisions");
        os << bol(config.checkSSCollisions) << std::endl;
        os << tab("Check SB collisions");
        os << bol(config.checkSBCollisions) << std::endl;
    }

    if (category & dump::Registers) {
        
        string addr[8] = {
            "$D000 - $D007", "$D008 - $D00F", "$D010 - $D017", "$D018 - $D01F",
            "$D020 - $D027", "$D028 - $D02F", "$D030 - $D037", "$D038 - $D03F" };
        
        for (isize i = 0; i < 6; i++) {
            os << tab(addr[i]);
            for (isize j = 0; j < 8; j++) os << hex(spypeek(8 * i + j)) << " ";
            os << std::endl;
        }
    }
    
    if (category & dump::State) {
        
        /*
        u8 ctrl1 = reg.current.ctrl1;
        u8 ctrl2 = reg.current.ctrl2;
        int yscroll = ctrl1 & 0x07;
        int xscroll = ctrl2 & 0x07;
        u16 screenMem = VM13VM12VM11VM10() << 6;
        u16 charMem = (CB13CB12CB11() << 10) % 0x4000;
        */
        
        os << tab("Bank address");
        os << hex(bankAddr) << std::endl;
        os << tab("Screen memory");
        os << hex(u16(VM13VM12VM11VM10() << 6)) << std::endl;
        os << tab("Character memory");
        os << hex(u16((CB13CB12CB11() << 10) % 0x4000)) << std::endl;
        os << tab("X scroll");
        os << dec(reg.current.ctrl2 & 0x07) << std::endl;
        os << tab("Y scroll");
        os << dec(reg.current.ctrl1 & 0x07) << std::endl;
        os << tab("Control reg 1");
        os << hex(reg.current.ctrl1) << std::endl;
        os << tab("Control reg 2");
        os << hex(reg.current.ctrl2) << std::endl;
        os << tab("Display mode");
        os << DisplayModeEnum::key(reg.current.mode) << std::endl;
        os << tab("badLine");
        os << bol(badLine) << std::endl;
        os << tab("DENwasSetIn30");
        os << bol(DENwasSetInRasterline30) << std::endl;
        os << tab("VC");
        os << hex(vc) << std::endl;
        os << tab("VCBASE");
        os << hex(vcBase) << std::endl;
        os << tab("RC");
        os << hex(rc) << std::endl;
        os << tab("VMLI");
        os << hex(vmli) << std::endl;
        os << tab("BA line");
        os << bol(baLine.current(), "low", "high") << std::endl;
        os << tab("MainFrameFF");
        os << bol(flipflops.current.main, "set", "cleared") << std::endl;
        os << tab("VerticalFrameFF");
        os << bol(flipflops.current.vertical, "set", "cleared") << std::endl;
        os << tab("DisplayState");
        os << bol(displayState, "on", "off") << std::endl;
        os << tab("SpriteDisplay");
        os << hex(spriteDisplay) << " / " << hex(spriteDisplayDelayed) << std::endl;
        os << tab("SpriteDma");
        os << hex(spriteDmaOnOff) << std::endl;
        os << tab("Y expansion");
        os << hex(expansionFF) << std::endl;
        os << tab("expansionFF");
        os << hex(expansionFF) << std::endl;
    }
}

void
VICII::clearStats()
{
    if (VIC_STATS) {
        
        double canvasTotal = stats.canvasFastPath + stats.canvasSlowPath;
        double spriteTotal = stats.spriteFastPath + stats.spriteSlowPath;
        double exitTotal = stats.quickExitHit + stats.quickExitMiss;
        
        msg("Canvas: Fast path: %ld Slow path: %ld Ratio: %f\n",
            stats.canvasFastPath,
            stats.canvasSlowPath,
            canvasTotal != 0 ? stats.canvasFastPath / canvasTotal : -1);

        msg("Sprites: Fast path: %ld Slow path: %ld Ratio: %f\n",
            stats.spriteFastPath,
            stats.spriteSlowPath,
            spriteTotal != 0 ? stats.spriteFastPath / spriteTotal : -1);

        msg("Exits: Hit: %ld Miss: %ld Ratio: %f\n",
            stats.quickExitHit,
            stats.quickExitMiss,
            exitTotal != 0 ? stats.quickExitHit / exitTotal : -1);

        memset(&stats, 0, sizeof(stats));
    }
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
VICII::isPAL(VICIIRevision rev)
{
    return rev & (VICII_PAL_6569_R1 | VICII_PAL_6569_R3 | VICII_PAL_8565);
}

bool
VICII::isNTSC(VICIIRevision rev)
{
     return rev & (VICII_NTSC_6567 | VICII_NTSC_6567_R56A | VICII_NTSC_8562);
}

bool
VICII::is856x(VICIIRevision rev)
{
     return rev & (VICII_PAL_8565 | VICII_NTSC_8562);
}
 
bool
VICII::is656x(VICIIRevision rev)
{
     return rev & ~(VICII_PAL_8565 | VICII_NTSC_8562);
}

bool
VICII::delayedLightPenIrqs(VICIIRevision rev)
{
     return rev & (VICII_PAL_6569_R1 | VICII_NTSC_6567_R56A);
}

double
VICII::getFps(VICIIRevision rev, VICIISpeed speed)
{
    return (double)getFrequency(rev, speed) / (double)getCyclesPerFrame(rev);
}

i64
VICII::getFrameDelay(VICIIRevision rev, VICIISpeed speed)
{
    return i64(1000000000 / getFps(rev, speed));
}

isize
VICII::getFrequency(VICIIRevision rev, VICIISpeed speed)
{
    switch (rev) {
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
        case VICII_NTSC_6567_R56A:
            return NTSC_CLOCK_FREQUENCY;
            
        default:
            return PAL_CLOCK_FREQUENCY;
    }
}

isize
VICII::getCyclesPerLine(VICIIRevision rev)
{
    switch (rev) {
            
        case VICII_NTSC_6567_R56A:
            return 64;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            return 65;
            
        default:
            return 63;
    }
}

isize
VICII::getLinesPerFrame(VICIIRevision rev)
{
    switch (rev) {
            
        case VICII_NTSC_6567_R56A:
            return 262;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            return 263;
            
        default:
            return 312;
    }
}

isize
VICII::getCyclesPerFrame(VICIIRevision rev)
{
    return getLinesPerFrame(rev) * getCyclesPerLine(rev);
}

isize
VICII::numVisibleLines(VICIIRevision rev)
{
    switch (rev) {
            
        case VICII_NTSC_6567_R56A:
            return 234;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            return 235;
            
        default:
            return 284;
    }
}

bool
VICII::isLastCycleInLine(isize cycle) const
{
    return cycle >= getCyclesPerLine();
}

bool
VICII::isVBlankLine(isize line) const
{
    switch (config.revision) {
            
        case VICII_NTSC_6567_R56A:
            return line < 16 || line >= 16 + 234;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            return line < 16 || line >= 16 + 235;
            
        default:
            return line < 16 || line >= 16 + 284;
    }
}

u32 *
VICII::stableEmuTexture() const
{
    return emuTexture == emuTexture1 ? emuTexture2 : emuTexture1;
}

u32 *
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

void
VICII::checkForRasterIrq()
{
    // Determine the comparison value
    u32 counter = isLastCycleInLine(c64.rasterCycle) ? yCounter + 1 : yCounter;
    
    // Check if the interrupt line matches
    bool match = rasterIrqLine == counter;

    // A positive edge triggers a raster interrupt
    if (match && !rasterlineMatchesIrqLine) {
        
        trace(RASTERIRQ_DEBUG, "Triggering raster interrupt\n");
        triggerIrq(1);
    }
    
    rasterlineMatchesIrqLine = match;
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
            
        case VICII_PAL_6569_R1:
        case VICII_PAL_6569_R3:

            return 4 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);

        case VICII_PAL_8565:
            
            return 2 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);
            
        case VICII_NTSC_6567:
        case VICII_NTSC_6567_R56A:
            
            return 4 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        case VICII_NTSC_8562:
            
            return 2 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        default:
            
            assert(false);
            return 0;
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
    if (value == lpLine) return;
        
    // A negative transition on LP triggers a lightpen event
    if (FALLING_EDGE(lpLine, value)) delay |= VICLpTransition;
    
    lpLine = value;
}

void
VICII::checkForLightpenIrq()
{
    u8 vicCycle = c64.rasterCycle;

    // An interrupt is suppressed if ...
    
    // ... a previous interrupt has occurred in the current frame
    if (lpIrqHasOccurred) return;

    // ... we are in the last PAL rasterline and not in cycle 1
    if (yCounter == 311 && vicCycle != 1) return;
    
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
            
        case VICII_PAL_6569_R1:
        case VICII_PAL_6569_R3:
        case VICII_PAL_8565:
            
            latchedLPX = 209;
            latchedLPY = 0;
            break;
            
        case VICII_NTSC_6567:
        case VICII_NTSC_6567_R56A:
        case VICII_NTSC_8562:
            
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
    (DEPTH_SPRITE_BG | nr) :
    (DEPTH_SPRITE_FG | nr);
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
                loadSpriteShiftRegister(sprite);
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
    
    // Clear statistics
    clearStats();
    
    // Check if this frame should be executed in headless mode
    headless = c64.inWarpMode() && config.powerSave && (c64.frame % 8) != 0;
}

void
VICII::endFrame()
{
    // Only proceed if the current frame hasn't been executed in headless mode
    if (headless) return;
    
    // Run the DMA debugger if enabled
    bool debug = dmaDebugger.config.dmaDebug;
    if (debug) dmaDebugger.computeOverlay(emuTexture, dmaTexture);

    // Switch texture buffers
    if (emuTexture == emuTexture1) {
        
        assert(dmaTexture == dmaTexture1);
        emuTexture = emuTexture2;
        dmaTexture = dmaTexture2;
        if (debug) { resetEmuTexture(2); resetDmaTexture(2); }

    } else {
        
        assert(emuTexture == emuTexture2);
        assert(dmaTexture == dmaTexture2);
        emuTexture = emuTexture1;
        dmaTexture = dmaTexture1;
        if (debug) { resetEmuTexture(1); resetDmaTexture(1); }
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

    // Adjust the texture pointers
    emuTexturePtr = emuTexture + line * TEX_WIDTH;
    dmaTexturePtr = dmaTexture + line * TEX_WIDTH;

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
    if (verticalFrameFFsetCond) setVerticalFrameFF(true);
    
    // Cut out layers if requested
    dmaDebugger.cutLayers();

    // Prepare buffers for the next line
    for (unsigned i = 0; i < TEX_WIDTH; i++) { zBuffer[i] = 0; }
}
