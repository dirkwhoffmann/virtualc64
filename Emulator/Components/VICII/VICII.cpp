// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

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
    subComponents = std::vector<CoreComponent *> { &dmaDebugger };

    initFuncTable(VICII_PAL_6569_R1);
    initFuncTable(VICII_PAL_6569_R3);
    initFuncTable(VICII_PAL_8565);
    initFuncTable(VICII_NTSC_6567_R56A);
    initFuncTable(VICII_NTSC_6567);
    initFuncTable(VICII_NTSC_8562);

    // Assign reference clock to all time delayed variables
    baLine.setClock(&cpu.clock);
    gAccessResult.setClock(&cpu.clock);    
}

void 
VICII::_initialize()
{
    setRevision(config.revision);
}

void
VICII::_didReset(bool hard)
{
    if (hard) {

        clearStats();
        
        // See README of VICE test VICII/spritemcbase
        for (isize i = 0; i < 8; i++) mcbase[i] = is656x ? 0x3F : 0x00;
        
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
VICII::resetEmuTexture(isize nr)
{
    assert(nr == 1 || nr == 2);

    if (nr == 1) { resetTexture(emuTexture1); }
    if (nr == 2) { resetTexture(emuTexture2); }
}

void
VICII::resetDmaTexture(isize nr)
{
    assert(nr == 1 || nr == 2);
    
    u32 *p = nr == 1 ? dmaTexture1 : dmaTexture2;

    for (int i = 0; i < Texture::height * Texture::width; i++) {
        p[i] = 0xFF000000;
    }
}

void
VICII::resetTexture(u32 *p)
{
    // Determine the HBLANK / VBLANK area
    long width = isPAL ? PAL::PIXELS_PER_LINE : NTSC::PIXELS_PER_LINE;
    long height = getLinesPerFrame();
    
    for (int y = 0; y < Texture::height; y++) {
        for (int x = 0; x < Texture::width; x++) {

            int pos = y * Texture::width + x;

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

void 
VICII::updateRevision()
{
    // Replace the VICII revision if requested
    if (config.revision != config.awaiting) setRevision(config.awaiting);
}

void
VICII::setRevision(VICIIRevision revision)
{
    assert_enum(VICIIRevision, revision);

    /* Changing the VICII revision is only allowed in certain emulator states.
     * If the emulator is powered off, the operation is harmless and can be
     * performed immediately. Changing the VICII revision on the fly must be
     * carried out with caution. I.e., it is only permitted to alter the
     * revision at the beginning of a frame, as changing it in the middle would
     * corrupt several internal data structures.
     */
    if (isEmulatorThread()) {

        assert(scanline() == 0);
        assert(rastercycle() == 1);

    } else {

        assert(isPoweredOff() || !emulator.isInitialized());
    }

    recorder.stopRecording();

    config.revision = revision;
    isFirstDMAcycle = isSecondDMAcycle = 0;
    updatePalette();
    resetEmuTextures();
    resetDmaTextures();

    isPAL =
    revision == VICII_PAL_6569_R1 ||
    revision == VICII_PAL_6569_R3 ||
    revision == VICII_PAL_8565;

    is856x =
    revision == VICII_PAL_8565 ||
    revision == VICII_NTSC_8562;

    isNTSC = !isPAL;
    is656x = !is856x;

    vic.updateVicFunctionTable();
    c64.updateClockFrequency();
    
    msgQueue.put(isPAL ? MSG_PAL : MSG_NTSC);
}

void
VICII::_trackOn()
{

}

void
VICII::_trackOff()
{

}

bool
VICII::delayedLightPenIrqs(VICIIRevision rev)
{
    bool result = (rev == VICII_PAL_6569_R1 || rev == VICII_NTSC_6567_R56A);
    assert(result == traits[rev].delayedLpIrqs);
    return result;
}

double
VICII::getFps(VICIIRevision rev)
{
    double result = (double)getFrequency(rev) / (double)getCyclesPerFrame(rev);
    assert(result == traits[rev].fps);
    return result;
}

isize
VICII::getFrequency(VICIIRevision rev)
{
    isize result;

    switch (rev) {
            
        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
        case VICII_NTSC_6567_R56A:
            result = NTSC::CLOCK_FREQUENCY;
            break;

        default:
            result = PAL::CLOCK_FREQUENCY;
            break;
    }

    assert(result == traits[rev].frequency);
    return result;
}

isize
VICII::getCyclesPerLine(VICIIRevision rev)
{
    isize result;

    switch (rev) {
            
        case VICII_NTSC_6567_R56A:
            result = 64;
            break;

        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            result = 65;
            break;

        default:
            result = 63;
            break;
    }

    assert(result == traits[rev].cyclesPerLine);
    return result;
}

isize
VICII::getLinesPerFrame(VICIIRevision rev)
{
    isize result;

    switch (rev) {
            
        case VICII_NTSC_6567_R56A:
            result = 262;
            break;

        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            result = 263;
            break;

        default:
            result = 312;
            break;
    }

    assert(result == traits[rev].linesPerFrame);
    return result;
}

isize
VICII::getCyclesPerFrame(VICIIRevision rev)
{
    isize result = getLinesPerFrame(rev) * getCyclesPerLine(rev);
    assert(result == traits[rev].cyclesPerFrame);
    return result;
}

isize
VICII::numVisibleLines(VICIIRevision rev)
{
    isize result;

    switch (rev) {
            
        case VICII_NTSC_6567_R56A:
            result = 234;
            break;

        case VICII_NTSC_6567:
        case VICII_NTSC_8562:
            result = 235;
            break;

        default:
            result = 284;
            break;
    }

    assert(result == traits[rev].visibleLines);
    return result;
}

bool
VICII::hasGrayCodeBug(VICIIRevision rev)
{
    bool result = (rev == VICII_PAL_8565 || rev == VICII_NTSC_8562);
    assert(result == traits[rev].grayCodeBug);
    return result;
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
VICII::getTexture() const
{
    return emuTexture == emuTexture1 ? emuTexture2 : emuTexture1;
}

u32 *
VICII::getDmaTexture() const
{
    return dmaTexture == dmaTexture1 ? dmaTexture2 : dmaTexture1;
}

void
VICII::updatePalette()
{
    for (isize i = 0; i < 16; i++) {
        rgbaTable[i] = monitor.getColor(i);
    }
}

u32 
VICII::getColor(isize nr, Palette palette) const
{
    return monitor.getColor(nr, palette);
}

u16
VICII::scanline() const
{
    return c64.scanline;
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
    if (match && !lineMatchesIrqLine) {
        
        trace(RASTERIRQ_DEBUG, "Triggering raster interrupt\n");
        triggerIrq(1);
    }
    
    lineMatchesIrqLine = match;
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
    (yCounter & 0x07) == (u32(reg.current.ctrl1) & 0x07) && /* [2] */
    DENwasSetInLine30; /* [3] */
}

void
VICII::updateBA(u8 value)
{
    if (value != baLine.current()) {

        if (value) {

            baLine.write(value);
            cpu.pullDownRdyLine(INTSRC_VIC);

        } else {

            baLine.clear();
            cpu.releaseRdyLine(INTSRC_VIC);
        }
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
            fatalError;
    }
}

u16
VICII::lightpenY() const
{
    return (u16)yCounter;
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

    // ... we are in the last PAL scanline and not in cycle 1
    if (yCounter == 311 && vicCycle != 1) return;
    
    // Latch coordinates
    latchedLPX = (u8)(lightpenX() / 2);
    latchedLPY = (u8)(lightpenY());
    
    // Newer VICII models trigger an interrupt immediately
    if (!delayedLightPenIrqs()) triggerIrq(8);
    
    // Lightpen interrupts can only occur once per frame
    lpIrqHasOccurred = true;
}

void
VICII::checkForLightpenIrqAtStartOfFrame()
{
    // This function is called at the beginning of a frame, only.
    assert(c64.scanline == 0);
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
VICII::spriteDepth(isize nr) const
{
    return
    GET_BIT(reg.delayed.sprPriority, nr) ?
    (u8)(DEPTH_SPRITE_BG | nr) :
    (u8)(DEPTH_SPRITE_FG | nr);
}

u8
VICII::compareSpriteY() const
{
    u8 result = 0;
    
    for (isize i = 0; i < 8; i++) {
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
    for (isize i = 0; i < 8; i++) {
        
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
    
    for (isize i = 0; i < 8; i++) {
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
    for (isize i = 0; i < 8; i++) {
        mc[i] = mcbase[i];
    }
    
    spriteDisplay |= reg.current.sprEnable & compareSpriteY();
    spriteDisplay &= spriteDmaOnOff;
}

void
VICII::loadSpriteShiftRegister(isize nr)
{
    spriteSr[nr].data = LO_LO_HI(spriteSr[nr].chunk3,
                                 spriteSr[nr].chunk2,
                                 spriteSr[nr].chunk1);
}

void
VICII::updateSpriteShiftRegisters()
{
    if (!isSecondDMAcycle) return;
    
    for (isize sprite = 0; sprite < 8; sprite++) {
        
        if (GET_BIT(isSecondDMAcycle, sprite)) {
            loadSpriteShiftRegister(sprite);
        }
    }
}

void 
VICII::beginFrame()
{
    // Setup cycle-specific callbacks
    updateVicFunctionTable();

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

    lpIrqHasOccurred = false;

    // Clear statistics
    clearStats();
}

void
VICII::endFrame()
{
    // Update the VICII revision if requested
    updateRevision();

    // Only proceed if the current frame hasn't been executed in headless mode
    if (c64.getHeadless()) return;

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
VICII::beginScanline()
{
    u16 line = c64.scanline;

    // Check if a new frame begins
    if (line == 0) {

        beginFrame();
        sidBridge.beginFrame();
    }

    // Reset some variables
    verticalFrameFFsetCond = false;

    // Adjust the texture pointers
    emuTexturePtr = emuTexture + line * Texture::width;
    dmaTexturePtr = dmaTexture + line * Texture::width;

    // Determine if we're inside the VBLANK area
    vblank = isVBlankLine(line);

    // Increase the y counter (overflow is handled in cycle 2)
    if (!yCounterOverflow()) yCounter++;
    
    // Check the DEN bit in line 30 (value might change later)
    if (line == 0x30) DENwasSetInLine30 = DENbit();

    // Check if this line is a DMA line (bad line) (value might change later)
    if ((badLine = badLineCondition()) == true) delay |= VICSetDisplayState;
    
    // Reset the pixel buffer offset
    bufferoffset = 0;
}

void 
VICII::endScanline()
{
    // Set vertical flipflop if condition was hit
    if (verticalFrameFFsetCond) setVerticalFrameFF(true);
    
    // Cut out layers if requested
    dmaDebugger.cutLayers();

    // Prepare buffers for the next line
    for (isize i = 0; i < Texture::width; i++) { zBuffer[i] = 0; }
}

ScreenGeometry
VICII::getScreenGeometry(void) const
{
    unsigned rows = GET_BIT(reg.current.ctrl1, 3) ? 25 : 24;
    unsigned cols = GET_BIT(reg.current.ctrl2, 3) ? 40 : 38;

    if (cols == 40) {
        return rows == 25 ? SCREEN_GEOMETRY_25_40 : SCREEN_GEOMETRY_24_40;
    } else {
        return rows == 25 ? SCREEN_GEOMETRY_25_38 : SCREEN_GEOMETRY_24_38;
    }
}

}
