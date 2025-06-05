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

#pragma once

#include "VICIITypes.h"
#include "SubComponent.h"
#include "Colors.h"
#include "Constants.h"
#include "DmaDebugger.h"
#include "MemoryTypes.h"
#include "MonitorTypes.h"
#include "TimeDelayed.h"

namespace vc64 {

struct VICIIRegisters : Serializable
{
    // Registers
    u16 sprX[8];     // D000, D002, ..., D00E, upper bits from D010
    u8  sprY[8];     // D001, D003, ..., D00F
    u8  ctrl1;       // D011
    u8  sprEnable;   // D015
    u8  ctrl2;       // D016
    u8  sprExpandY;  // D017
    u8  sprPriority; // D01B
    u8  sprMC;       // D01C
    u8  sprExpandX;  // D01D
    u8  colors[15];  // D020 - D02E

    // Derived values
    u8 xscroll;
    DisplayMode mode;


    template <class W>
    void serialize(W& worker)
    {
        worker

        << sprX
        << sprY
        << ctrl1
        << sprEnable
        << ctrl2
        << sprExpandY
        << sprPriority
        << sprMC
        << sprExpandX
        << colors
        << xscroll
        << mode;

    } SERIALIZERS(serialize);
};

struct SpriteSR : Serializable
{
    // Shift register data (24 bit)
    u32 data;

    // The shift register data is read in three chunks
    u8 chunk1, chunk2, chunk3;

    /* Multi-color synchronization flipflop
     * Whenever the shift register is loaded, the synchronization flipflop
     * is also set. It is toggled with each pixel and used to synchronize
     * the synthesis of multi-color pixels.
     */
    bool mcFlop;

    // X expansion synchronization flipflop
    bool expFlop;

    /* Color bits of the currently processed pixel
     * In single-color mode, these bits are updated every cycle. In
     * multi-color mode, these bits are updated every second cycle
     * (synchronized with mcFlop).
     */
    u8 colBits;

    template <class W>
    void serialize(W& worker)
    {
        worker

        << data
        << chunk1
        << chunk2
        << chunk3
        << mcFlop
        << expFlop
        << colBits;

    } SERIALIZERS(serialize);
};

class VICII final : public SubComponent, public Inspectable<VICIIInfo, VICIIStats> {

    friend class Memory;
    friend class DmaDebugger;
    friend class VideoPort;
    friend class Reu;
    
    // REMOVE ASAP
    friend class Heatmap;
    
    Descriptions descriptions = {{

        .type           = Class::VICII,
        .name           = "VIC",
        .description    = "Video Interface Controller",
        .shell          = "vic"
    }};

    static constexpr VICIITraits traits[6] = {
        {
            // VICII_PAL_6569_R1
            .pal            = true,
            .frequency      = PAL::CLOCK_FREQUENCY,
            .fps            = PAL::CLOCK_FREQUENCY / (312.0 * 63.0),
            .linesPerFrame  = 312,
            .cyclesPerLine  = 63,
            .cyclesPerFrame = 312 * 63,
            .visibleLines   = 284,
            .grayCodeBug    = false,
            .delayedLpIrqs  = true
        },
        {
            // VICII_PAL_6569_R3
            .pal            = true,
            .frequency      = PAL::CLOCK_FREQUENCY,
            .fps            = PAL::CLOCK_FREQUENCY / (312.0 * 63.0),
            .linesPerFrame  = 312,
            .cyclesPerLine  = 63,
            .cyclesPerFrame = 312 * 63,
            .visibleLines   = 284,
            .grayCodeBug    = false,
            .delayedLpIrqs  = false
        },
        {
            // VICII_PAL_8565
            .pal            = true,
            .frequency      = PAL::CLOCK_FREQUENCY,
            .fps            = PAL::CLOCK_FREQUENCY / (312.0 * 63.0),
            .linesPerFrame  = 312,
            .cyclesPerLine  = 63,
            .cyclesPerFrame = 312 * 63,
            .visibleLines   = 284,
            .grayCodeBug    = true,
            .delayedLpIrqs  = false
        },
        {
            // VICII_NTSC_6567_R56A
            .pal            = false,
            .frequency      = NTSC::CLOCK_FREQUENCY,
            .fps            = NTSC::CLOCK_FREQUENCY / (262.0 * 64.0),
            .linesPerFrame  = 262,
            .cyclesPerLine  = 64,
            .cyclesPerFrame = 262 * 64,
            .visibleLines   = 234,
            .grayCodeBug    = false,
            .delayedLpIrqs  = true
        },
        {
            // VICII_NTSC_6567
            .pal            = false,
            .frequency      = NTSC::CLOCK_FREQUENCY,
            .fps            = NTSC::CLOCK_FREQUENCY / (263.0 * 65.0),
            .linesPerFrame  = 263,
            .cyclesPerLine  = 65,
            .cyclesPerFrame = 263 * 65,
            .visibleLines   = 235,
            .grayCodeBug    = false,
            .delayedLpIrqs  = false
        },
        {
            // VICII_NTSC_8562
            .pal            = false,
            .frequency      = NTSC::CLOCK_FREQUENCY,
            .fps            = NTSC::CLOCK_FREQUENCY / (263.0 * 65.0),
            .linesPerFrame  = 263,
            .cyclesPerLine  = 65,
            .cyclesPerFrame = 263 * 65,
            .visibleLines   = 235,
            .grayCodeBug    = true,
            .delayedLpIrqs  = false
        }
    };

    Options options = {

        Opt::VICII_REVISION,
        Opt::VICII_POWER_SAVE,
        Opt::VICII_GRAY_DOT_BUG,
        Opt::GLUE_LOGIC,
        Opt::VICII_HIDE_SPRITES,
        Opt::VICII_SS_COLLISIONS,
        Opt::VICII_SB_COLLISIONS
    };

    // Current configuration
    VICIIConfig config = { };

    // Result of the latest inspection
    mutable SpriteInfo spriteInfo[8] = { };

    // Chip properties (derived from config.revision)
    bool isPAL;
    bool isNTSC;
    bool is856x;
    bool is656x;

public:
    
    // Subcomponents
    DmaDebugger dmaDebugger;

    /* The VICII function table. Each entry in this table is a pointer to a
     * VICII method executed in a certain scanline cycle:
     *
     *   functable[model][flags][cycle]
     *
     *   model: VICII revision
     *
     *   flags: One or more of the following:
     *
     *     CYCLE_PAL       : Emulates a PAL cycle, NTSC otherwise
     *     CYCLE_DMA       : Runs the DMA debugger code for the specific cycle
     *     CYLCE_HEADLESS  : Skips all pixel-drawing related code
     *
     *   cycle: 1 .. 65 (cycle 0 is a stub and never called)
     *
     *   CYCLE_DMA and CYCLE_HEADLESS must be be set simultaneously as this
     *   combination does not make sense.
     */
    typedef void (VICII::*ViciiFunc)(void);
    ViciiFunc functable[6][8][66] = {};

    // Function pointers currently in use
    ViciiFunc vicfunc[66] = {};


    //
    // I/O space (CPU accessible)
    //
    
private:
    
    /* Piped I/O register state. When an I/O register is written to, the
     * corresponding value in variable current is changed and a flag is set in
     * variable delay. Function processDelayedActions() reads the flag and, if
     * set to true, updates the delayed values.
     */
    struct {
        VICIIRegisters current;
        VICIIRegisters delayed;
    } reg;
    
    // Raster interrupt line ($D011:8 + $D012)
    u16 rasterIrqLine;
    
    // Latched lightpen coordinates ($D013 and $D014)
    u8 latchedLPX;
    u8 latchedLPY;
    
    // Memory address register ($D018)
    u8 memSelect;
    
    // Interrupt Request and Mask Register ($D019 and $D01A)
    u8 irr;
    u8 imr;

    
    //
    // Chip internals
    //
    
    // IRQ <---------------------------------+
    //             (1)                       |
    //             +---------------+ +-----------------+
    //             |Refresh counter| | Interrupt logic |<----------------------+
    //             +---------------+ +-----------------+                       |
    //         +-+    |               ^                                        |
    //   A     |M|    v     (2),(3)   |       (4),(5)                          |
    //   d     |e|   +-+    +--------------+  +-------+                        |
    //   d     |m|   |A|    |Raster counter|->| VC/RC |                        |
    //   r     |o|   |d| +->|      X/Y     |  +-------+                        |
    //   . <==>|r|   |d| |  +--------------+      |                            |
    //  +      |y|   |r| |     | | |              | (6),(7)                    |
    //   d     | |   |.|<--------+----------------+ +------------------------+ |
    //   a     |i|   |g|===========================>|40×12 bit video matrix-/| |
    //   t     |n|<=>|e| |     |   |                |       color line       | |
    //   a     |t|   |n| |     |   |                +------------------------+ |
    //         |e|   |e| |     |   | (8)                        ||             |
    //         |r|   |r| |     |   | +----------------+         ||             |
    //  BA  <--|f|   |a|============>|8×24 bit sprite |         ||             |
    //         |a|   |t|<----+ |   | |  data buffers  |         ||             |
    //  AEC <--|c|   |o| |   | v   | +----------------+         ||             |
    //         |e|   |r| | +-----+ |         ||                 ||             |
    //         +-+   +-+ | |MC0-7| |  (10)   \/          (11)   \/             |
    //                   | +-----+ |  +--------------+   +--------------+      |
    //                   |     (9) |  | Sprite data  |   |Graphics data |      |
    //         +---------------+   |  |  sequencer   |   |  sequencer   |      |
    //  RAS <--|               |   |  +--------------+   +--------------+      |
    //  CAS <--|Clock generator|   |              |         |                  |
    //  ø0  <--|               |   |              v         v                  |
    //         +---------------+   |       +-----------------------+           |
    //                 ^           |       |          MUX          |           |
    //                 |           |       | Sprite priorities and |-----------+
    //  øIN -----------+           |       |  collision detection  |
    //                             |       +-----------------------+ (12)
    //    VC: Video Matrix Counter |                   |
    //        (14)                 |            (13)   v
    //    RC: Row Counter          |            +-------------+
    //        (15)                 +----------->| Border unit |
    //    MC: MOB Data Counter     |            +-------------+
    //        (16)                 |                   |
    //                             v                   v
    //                     +----------------+  +----------------+
    //                     |Sync generation |  |Color generation|<------- øCOLOR
    //                     +----------------+  +----------------+
    //                                    |      |
    //                                    v      v
    //                                  Video output
    //                                (S/LUM and COLOR)              [C.B.]

    
    /* Refresh counter (1): "The VICII does five read accesses in every raster
     * line for the refresh of the dynamic RAM. An 8 bit refresh counter (REF)
     * is used to generate 256 DRAM row addresses. The counter is reset to $ff
     * in raster line 0 and decremented by 1 after each refresh access." [C.B.]
     * See also: rAccess()
     */
    u8 refreshCounter;
    
    /* Raster counter X (2): Defines the sprite coordinate system.
     */
    u16 xCounter;
    
    /* Y raster counter (3): The scanline counter is usually incremented in
     * cycle 1. The only exception is the overflow condition which is handled
     * in cycle 2.
     */
    u32 yCounter;
    
    /* Video counter (14): A 10 bit counter that can be loaded with the value
     * from vcBase.
     */
    u16 vc;
    
    /* Video counter base: A 10 bit data register that can be loaded with the
     * value from vc.
     */
    u16 vcBase;
    
    /* Row counter (15): A 3 bit counter with reset input.
     */
    u8 rc;
    
    /* Video matrix (6): Every 8th scanline, the VICII chips performs a
     * c-access and fills this array with character information.
     */
    u8 videoMatrix[40];
    
    /* Color line (7): Every 8th scanline, the VICII chips performs a
     * c-access and fills the array with color information.
     */
    u8 colorLine[40];
    
    /* Video matrix line index: "Besides this, there is a 6 bit counter with
     * reset input that keeps track of the position within the internal 40×12
     * bit video matrix/color line where read character pointers are stored
     * resp. read again. I will call this 'VMLI' (video matrix line index)
     * here." [C.B.]
     */
    u8 vmli;
    

    /* Graphics data sequencer (10): An 8 bit shift register to synthesize
     * canvas pixels.
     */
    struct {
        
        // Shift register data
        u8 data;
        
        /* Indicates whether the shift register can load data. If true, the
         * register is loaded when the current x scroll offset matches the
         * current pixel number.
         */
        bool canLoad;
        
        /* Multi-color synchronization flipflop. Whenever the shift register is
         * loaded, the synchronization flipflop is also set. It is toggled with
         * each pixel and used to synchronize the synthesis of multi-color
         * pixels.
         */
        bool mcFlop;
        
        /* Latched character info. Whenever the shift register is loaded, the
         * current character value (which was once read during a gAccess) is
         * latched. This value is used until the shift register loads again.
         */
        u8 latchedChr;
        
        /* Latched color info. Whenever the shift register is loaded, the
         * current color value (which was once read during a gAccess) is
         * latched. This value is used until the shift register loads again.
         */
        u8 latchedCol;
        
        /* Color bits. Every second pixel (as synchronized with mcFlop), the
         * multi-color bits are remembered.
         */
        u8 colorbits;

    } sr;
    
    /* Sprite data sequencer (11): The VICII chip has a 24 bit (3 byte) shift
     * register for each sprite. It stores the sprite for one scanline. If a
     * sprite is a display candidate in the current scanline, its shift
     * register is activated when the raster X coordinate matches the sprites
     * X coordinate. The comparison is done in method drawSprite(). Once a
     * shift register is activated, it remains activated until the beginning of
     * the next scanline. However, after an activated shift register has
     * dumped out its 24 pixels, it can't draw anything else than transparent
     * pixels (which is the same as not to draw anything). An exception is
     * during DMA cycles. When a shift register is activated during such a
     * cycle, it freezes a short period of time in which it repeats the
     * previous drawn pixel.
     */
    SpriteSR spriteSr[8];

    /* Indicates for each sprite if the shift register is active. Once the
     * shift register is started, it runs as long it contains at least one '1'
     * bit (data != 0).
     */
    u8 spriteSrActive;
    
    // Sprite-sprite collision register (12)
    u8 spriteSpriteCollision;

    // Sprite-background collision register (12)
    u8 spriteBackgroundColllision;

    
    //
    // Border flipflops
    //
    
    /* Piped frame flipflops state (13): When a flipflop toggles, the value in
     * variable 'current' is changed and a flag is set in variable 'delay'.
     * Function processDelayedActions() reads the flag and if set to true,
     * updates the delayed values with the current ones.
     */
    struct {
        FrameFlipflops current;
        FrameFlipflops delayed;
    } flipflops;
    
    /* Vertical frame flipflop set condition. Indicates whether the vertical
     * frame flipflop needs to be set in the current scanline.
     */
    bool verticalFrameFFsetCond;
    
    /* First coordinate where the main frame flipflop is checked. Either 24 or
     * 31, dependend on the CSEL bit.
     */
    u16 leftComparisonVal;
    
    /* Second coordinate where the main frame flipflop is checked. Either 344
     * or 335, dependend on the CSEL bit.
     */
    u16 rightComparisonVal;
    
    /* First coordinate where the vertical frame flipflop is checked. Either 51
     * or 55, dependend on the RSEL bit.
     */
    u16 upperComparisonVal;
    
    /* Second coordinate where the vertical frame flipflop is checked. Either
     * 251 or 247, dependend on the RSEL bit.
     */
    u16 lowerComparisonVal;
    
    
    //
    // Raster interrupt logic
    //
    
    /* Indicates whether the current raster line matches the IRQ line. A
     * positive edge on this value triggers a raster interrupt.
     */
    bool lineMatchesIrqLine;
    
    
    //
    // Housekeeping information
    //
    
    /* Indicates wether we are in a visible display column or not. The visible
     * columns comprise canvas columns and border columns. The first visible
     * column is drawn in cycle 14 (first left border column) and the last in
     * cycle 61 (fourth right border column).
     */
    bool isVisibleColumn;

    // True if the current scanline belongs to the VBLANK area
    bool vblank;
    
    // Indicates if the current scanline is a DMA line (bad line)
    bool badLine;
    
    /* True, if DMA lines can occurr within the current frame. Bad lines can
     * occur only if the DEN bit was set during an arbitary cycle in scanline
     * 30. The DEN bit is located in control register 1 (0x11).
     */
    bool DENwasSetInLine30;
    
    /* Current display State
     *
     * "The text/bitmap display logic in the VICII is in one of two states at
     *  any time: The idle state and the display state.
     *
     *  - In display state, c- and g-accesses take place, the addresses and
     *    interpretation of the data depend on the selected display mode.
     *
     *  - In idle state, only g-accesses occur. The VICII is either in idle or
     *    display state" [C.B.]
     */
    bool displayState;


    //
    // Sprites
    //

private:

    // MOB data counter (16)
    u8 mc[8];

    // MCBASE register
    u8 mcbase[8];

    // Sprite pointer fetched during a pAccess
    u16 spritePtr[8];

    // Flags the first DMA access for each sprite
    u8 isFirstDMAcycle;
    
    // Flags the second or third DMA access for each sprite
    u8 isSecondDMAcycle;

    // Determines if a sprite needs to be drawn in the current scanline
    u8 spriteDisplay;

    // Value of spriteDisplay, delayed by one cycle
    u8 spriteDisplayDelayed;

    // Sprite DMA on off register
    u8 spriteDmaOnOff;
    
    // Expansion flipflop (used to handle Y sprite stretching)
    u8 expansionFF;

    /* Remembers which bits the CPU has cleared in the expansion Y register
     * (D017). This value is set in pokeIO and cycle 15 and read in cycle 16
     */
    u8 cleared_bits_in_d017;
    
    // Collision bits
    u8 collision[8];
    
    
    //
    // Lightpen
    //

    /* Current value of the LP pin. A negative transition on this pin triggers
     * a lightpen interrupt.
     */
    bool lpLine;
    
    /* Indicates whether the lightpen has triggered. This variable indicates
     * whether a lightpen interrupt has occurred within the current frame. The
     * variable is needed, because a lightpen interrupt can only occur once per
     * frame.
     */
    bool lpIrqHasOccurred;

    
    //
    // CPU control and memory access
    //
    
public:

    /* Current value of the BA line. Remember: Each CPU cycle is split into two
     * phases:
     *
     *     - phi1 (First phase, LOW): VICII gets access to the bus
     *     - phi2 (Second phase, HIGH): CPU gets access to the bus
     *
     * In rare cases, VICII needs access in the HIGH phase, too. To block the
     * CPU, the BA line is pulled down. Note that BA can be pulled down by
     * multiple sources (wired AND) and this variable indicates which sources
     * are holding the line low.
     */
    TimeDelayed <u16,3> baLine = TimeDelayed <u16,3> ();

private:
    
    /* Memory source lookup table. If VICII is not running in Ultimax mode, it
     * has access to RAM and the character Rom. In ultimax mode, VICII has
     * access to ROMH and some portions of RAM.
     */
    MemType memSrc[16];
    
    /* Indicates whether VICII is running in ultimax mode. Ultimax mode can be
     * enabled by external cartridges by pulling the game line low and keeping
     * the exrom line high. In ultimax mode, VICII has access to ROMH and some
     * portions of RAM.
     */
    bool ultimax;
    
    /* Value on the data bus during the latest phi1 access. Only VICII performs
     * a memory access during phi1.
     */
    u8 dataBusPhi1;
    
    /* Value on the data bus during the latest phi2 access. VICII or the CPU
     * can perform a memory access during phi2. If none of them does, 0xFF will
     * be on the bus.
     */
    u8 dataBusPhi2;
    
    /* Address bus. Whenever VICII performs a memory read, the generated memory
     * address is stored in this variable.
     */
    u16 addrBus;

    /* Start address of the currently selected memory bank. There are four
     * banks in total since the VICII chip can only 'see' 16 KB of memory at
     * the same time. Two bank select bits in the CIA I/O space determine which
     * quarter of memory is currently seen.
     *
     * +-------+------+-------+----------+-------------------------+
     * | VALUE | BITS |  BANK | STARTING |  VIC-II CHIP RANGE      |
     * |  OF A |      |       | LOCATION |                         |
     * +-------+------+-------+----------+-------------------------+
     * |   0   |  00  |   3   |   49152  | ($C000-$FFFF)           |
     * |   1   |  01  |   2   |   32768  | ($8000-$BFFF)           |
     * |   2   |  10  |   1   |   16384  | ($4000-$7FFF)           |
     * |   3   |  11  |   0   |       0  | ($0000-$3FFF) (DEFAULT) |
     * +-------+------+-------+----------+-------------------------+
     */
    u16 bankAddr;
    
    // Result of the lastest g-access
    TimeDelayed <u32,2> gAccessResult = TimeDelayed <u32,2> ();
    

    //
    // Pipeline
    //
    
private:
    
    /* Event pipeline. If a time delayed event needs to be performed, a flag is
     * set inside this variable and executed at the beginning of the next cycle.
     * See processDelayedActions()
     */
    u64 delay;

    
    //
    // Screen buffers and colors
    //
    
private:
    
    // C64 colors in RGBA format (updated in updatePalette())
    u32 rgbaTable[16];

    /* Texture buffers. VICII outputs the generated texture into these buffers.
     * At any time, one buffer is the working buffer and the other one is the
     * stable buffer. While VICII always writes into the working buffer, the
     * GUI accesses the stable buffer at a constant frame rate and copies it
     * into the texture RAM of the graphics card.
     *
     * The emuTexture buffers contain the emulator texture. It is the texture
     * that is usually drawn by the GUI. The dmaTexture buffers contain the
     * texture generated by the DMA debugger. If DMA debugging is enabled, this
     * texture is superimposed on the emulator texture.
     */
    u32 *emuTexture1 = new u32[Texture::height * Texture::width];
    u32 *emuTexture2 = new u32[Texture::height * Texture::width];
    u32 *dmaTexture1 = new u32[Texture::height * Texture::width];
    u32 *dmaTexture2 = new u32[Texture::height * Texture::width];

    /* Pointer to the current working texture. This variable points either to
     * the first or the second texture buffer. After a frame has been finished,
     * the pointer is redirected to the other buffer.
     */
    u32 *emuTexture;
    u32 *dmaTexture;

    /* Pointer to the beginning of the current scanline inside the current
     * working textures. These pointers are used by all rendering methods to
     * write pixels. It always points to the beginning of a scanline, either
     * the first or the second texture buffer. They are reset at the beginning
     * of each frame and incremented at the beginning of each scanline.
     */
    u32 *emuTexturePtr;
    u32 *dmaTexturePtr;

    /* VICII utilizes a depth buffer to determine pixel priority. The render
     * routines only write a color value, if it is closer to the view point.
     * The depth of the closest pixel is kept in this buffer. The lower the
     * value, the closer it is to the viewer.
     * The depth values have been chosen in a way that preserves the source
     * of the drawn pixel (border pixel, sprite pixel, etc.).
     */
    u8 zBuffer[Texture::width];

    /* Offset into to pixelBuffer. This variable points to the first pixel of
     * the currently drawn 8 pixel chunk.
     */
    short bufferoffset;

    
    //
    // Debugging
    //
    
    // Lookup table for DMA debugging colors
    u32 debugColor[6][4];

public:
    
    // Filename used by dumpTexture()
    string dumpTexturePath = "texture";
    
    // Pixel area used by dumpTexture()
    isize x1 = 104;
    isize y1 = 16;
    isize x2 = 488;
    isize y2 = 290;

    
    //
    // Methods
    //
    
public:

    VICII(C64 &ref);

    VICII& operator= (const VICII& other) {

        CLONE(dmaDebugger)

        CLONE(reg)
        CLONE(rasterIrqLine)
        CLONE(latchedLPX)
        CLONE(latchedLPY)
        CLONE(memSelect)
        CLONE(irr)
        CLONE(imr)
        CLONE(refreshCounter)
        CLONE(xCounter)
        CLONE(yCounter)
        CLONE(vc)
        CLONE(vcBase)
        CLONE(rc)
        CLONE_ARRAY(videoMatrix)
        CLONE_ARRAY(colorLine)
        CLONE(vmli)
        CLONE(sr)
        CLONE_ARRAY(spriteSr)
        CLONE(spriteSrActive)
        CLONE(spriteSpriteCollision)
        CLONE(spriteBackgroundColllision)
        CLONE(flipflops)
        CLONE(verticalFrameFFsetCond)
        CLONE(leftComparisonVal)
        CLONE(rightComparisonVal)
        CLONE(upperComparisonVal)
        CLONE(lowerComparisonVal)
        CLONE(lineMatchesIrqLine)
        CLONE(isVisibleColumn)
        CLONE(vblank)
        CLONE(badLine)
        CLONE(DENwasSetInLine30)
        CLONE(displayState)
        CLONE_ARRAY(mc)
        CLONE_ARRAY(mcbase)
        CLONE_ARRAY(spritePtr)
        CLONE(isFirstDMAcycle)
        CLONE(isSecondDMAcycle)
        CLONE(spriteDisplay)
        CLONE(spriteDisplayDelayed)
        CLONE(spriteDmaOnOff)
        CLONE(expansionFF)
        CLONE(cleared_bits_in_d017)
        CLONE_ARRAY(collision)
        CLONE(lpLine)
        CLONE(lpIrqHasOccurred)
        CLONE_ARRAY(memSrc)
        CLONE(ultimax)
        CLONE(dataBusPhi1)
        CLONE(dataBusPhi2)
        CLONE(addrBus)
        CLONE(baLine)
        CLONE(bankAddr)
        CLONE(gAccessResult)
        CLONE(delay)

        CLONE_ARRAY(rgbaTable)

        CLONE(bufferoffset)

        CLONE_ARRAY(memSrc)
        CLONE(isPAL)
        CLONE(isNTSC)
        CLONE(is856x)
        CLONE(is656x)

        CLONE(config)

        updateVicFunctionTable();
        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isSoftResetter(worker)) return;

        worker

        << reg.current
        << reg.delayed
        << rasterIrqLine
        << latchedLPX
        << latchedLPY
        << memSelect
        << irr
        << imr
        << refreshCounter
        << xCounter
        << yCounter
        << vc
        << vcBase
        << rc
        << videoMatrix
        << colorLine
        << vmli
        << sr.data
        << sr.canLoad
        << sr.mcFlop
        << sr.latchedChr
        << sr.latchedCol
        << sr.colorbits
        << spriteSr
        << spriteSrActive
        << spriteSpriteCollision
        << spriteBackgroundColllision
        << flipflops.current.vertical
        << flipflops.current.main
        << flipflops.delayed.vertical
        << flipflops.delayed.main
        << verticalFrameFFsetCond
        << leftComparisonVal
        << rightComparisonVal
        << upperComparisonVal
        << lowerComparisonVal
        << lineMatchesIrqLine
        << isVisibleColumn
        << vblank
        << badLine
        << DENwasSetInLine30
        << displayState
        << mc
        << mcbase
        << spritePtr
        << isFirstDMAcycle
        << isSecondDMAcycle
        << spriteDisplay
        << spriteDisplayDelayed
        << spriteDmaOnOff
        << expansionFF
        << cleared_bits_in_d017
        << collision
        << lpLine
        << lpIrqHasOccurred
        << memSrc
        << ultimax
        << dataBusPhi1
        << dataBusPhi2
        << addrBus
        << baLine
        << bankAddr
        << gAccessResult
        << delay
        << bufferoffset;

        if (isResetter(worker)) return;

        worker

        << memSrc
        << isPAL
        << isNTSC
        << is856x
        << is656x

        << config.revision
        << config.awaiting
        << config.powerSave
        << config.grayDotBug
        << config.glueLogic;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream &os) const override;
    void _initialize() override;
    void _didReset(bool hard) override;
    void _trackOn() override;
    void _trackOff() override;


    //
    // Methods from Inspectable
    //

private:

    void cacheInfo(VICIIInfo &result) const override;
    void cacheStats(VICIIStats &result) const override;
    void clearStats() override;

public:

    SpriteInfo getSpriteInfo(isize nr);


    //
    // Methods from Configurable
    //

public:

    const VICIIConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;

    bool dmaDebug() const { return dmaDebugger.config.dmaDebug; }

private:
    
    void updateRevision();
    void setRevision(VICIIRev revision);


    //
    // Initializing
    //

private:

    void resetEmuTexture(isize nr);
    void resetEmuTextures() { resetEmuTexture(1); resetEmuTexture(2); }
    void resetDmaTexture(isize nr);
    void resetDmaTextures() { resetDmaTexture(1); resetDmaTexture(2); }
    void resetTexture(u32 *p);

    void initFuncTable(VICIIRev revision);
    void initFuncTable(VICIIRev revision, u16 flags);
    ViciiFunc getViciiFunc(u16 flags, isize cycle);
    template <u16 flags> ViciiFunc getViciiFunc(isize cycle);

public:

    void updateVicFunctionTable();


    //
    // Deriving chip properties
    //
    
public:
    
    // Returns properties about the currently selected VICII revision
    const VICIITraits &getTraits() const { return traits[(long)config.revision]; }

    // Returns true if a PAL or an NTSC chip is plugged in
    bool pal() const { return isPAL; }
    bool ntsc() const { return isNTSC; }

    // Returns true if light pen interrupts are triggered with a delay
    static bool delayedLightPenIrqs(VICIIRev rev);
    bool delayedLightPenIrqs() { return delayedLightPenIrqs(config.revision); }

    // Returns the display refresh rate
    static double getFps(VICIIRev rev);
    double getFps() const { return getFps(config.revision); }

    // Returns the clock frequency
    static isize getFrequency(VICIIRev rev);
    isize getFrequency() const { return getFrequency(config.revision); }
    
    // Returns the number of CPU cycles performed per scanline
    static isize getCyclesPerLine(VICIIRev rev);
    isize getCyclesPerLine() const { return getCyclesPerLine(config.revision); }

    // Returns the number of scanline drawn per frame
    static isize getLinesPerFrame(VICIIRev rev);
    isize getLinesPerFrame() const { return getLinesPerFrame(config.revision); }

    // Returns the number of CPU cycles executed in one frame
    static isize getCyclesPerFrame(VICIIRev rev);
    isize getCyclesPerFrame() const { return getCyclesPerFrame(config.revision); }

    // Returns the number of visible scanlines in a single frame
    static isize numVisibleLines(VICIIRev rev);
    long numVisibleLines() const { return numVisibleLines(config.revision); }
    
    // Indicates if VICII is affected by the gray-dot bug
    static bool hasGrayCodeBug(VICIIRev rev);
    bool hasGrayCodeBug() const { return hasGrayCodeBug(config.revision); }

    // Returns true if the end of the scanline has been reached
    bool isLastCycleInLine(isize cycle) const;

    // Returns true if scanline belongs to the VBLANK area
    bool isVBlankLine(isize line) const;


    //
    // Accessing the screen buffer and display properties
    //

    // Returns a C64 color in 32 bit big endian RGBA format
    u32 getColor(isize nr) const { return rgbaTable[nr]; }
    u32 getColor(isize nr, Palette palette) const;

    // Updates the RGBA values for all 16 C64 colors
    void updatePalette();

private:

    // Returns pointers to the stable textures
    u32 *getTexture() const;
    u32 *getDmaTexture() const;


    //
    // Accessing memory (VIC_memory.cpp)
    //

public:
    
    // Peeks a value from a VICII register without side effects
    u8 spypeek(u16 addr) const;
    
    // Returns the ultimax flag
    u8 getUltimax() { return ultimax; }

    // Sets the ultimax flag
    void setUltimax(bool value);
    
    // Returns the latest value of the VICII's data bus during phi1
    u8 getDataBusPhi1() const { return dataBusPhi1; }

    // Returns the latest value of the VICII's data bus during phi2
    u8 getDataBusPhi2() const { return dataBusPhi2; }

    /* Schedules the VICII bank to to switched. This method is called if the
     * bank switch is triggered by a change of CIA2::PA or CIA2::DDRA.
     */
    void switchBank(u16 addr);

private:

    /* Updates the VICII bank address. The new address is computed from the
     * provided bank number.
     */
    void updateBankAddr(u8 bank) { bankAddr = (u16)(bank << 14); }

    /* Updates the VICII bank address. The new address is computed from the
     * bits in CIA2::PA.
     */
    void updateBankAddr();
    
    // Reads a value from a VICII register
    u8 peek(u16 addr);
    
    // Writes a value into a VICII register
    void poke(u16 addr, u8 value);
    
    // Simulates a memory access via the address and data bus
    u8 memAccess(u16 addr);

    // Same as memAccess without side effects
    u8 memSpyAccess(u16 addr);

    // Returns true if memAccess will read from Character ROM
    bool isCharRomAddr(u16 addr) const;

    // Performs a DRAM refresh (r-access)
    template <u16 flags> void rAccess();
    
    // Performs an idle access (i-access)
    template <u16 flags> void iAccess();
    
    // Performs a character access (c-access)
    template <u16 flags> void cAccess();
    
    // Performs a graphics access (g-access)
    template <u16 flags> void gAccess();

    // Computes the g-access fetch address for different VICII models
    u16 gAccessAddr85x();
    u16 gAccessAddr65x();

    /* Computes the g-access fetch address. The fetch address is influences by
     * both the BMM and ECM bit.
     */
    u16 gAccessAddr(bool bmm, bool ecm);
    
    // Performs a sprite pointer access (p-access)
    template <u16 flags> void pAccess(isize sprite);
    
    // Performs one of the three sprite data accesses
    template <u16 flags, isize sprite> void sAccess1();
    template <u16 flags, isize sprite> void sAccess2();
    template <u16 flags, isize sprite> void sAccess3();
    
    /* Finalizes the sprite data access. This method is invoked one cycle after
     * the second and third sprite DMA has occurred.
     */
    void sFinalize(isize sprite);
    

    //
    // Handling the x and y counters
    //
    
    /* Returns the current scanline. This value is not always identical to
     * the yCounter, because the yCounter is incremented with a little delay.
     */
    u16 scanline() const;

    // Returns the current scanline cycle
    u8 rastercycle() const;

    /* Indicates if yCounter needs to be reset in this scanline. PAL models
     * reset the yCounter in cycle 2 in the first scanline wheras NTSC models
     * reset the yCounter in cycle 2 in the middle of the lower border area.
     */
    bool yCounterOverflow() const { return scanline() == (isPAL ? 0 : 238); }

    /* Matches the yCounter with the raster interrupt line and stores the
     * result. If a positive edge is detected, a raster interrupt is triggered.
     */
    void checkForRasterIrq();

    
    //
    // Handling the border flip flops
    //
    
    /* "Der VIC benutzt zwei Flipflops, um den Rahmen um das Anzeigefenster
     *  herum zu erzeugen: Ein Haupt-Rahmenflipflop und ein vertikales
     *  Rahmenflipflop. [...]
     *
     *  The flip flops are switched according to the following rules:
     *
     *  1. If the X coordinate reaches the right comparison value, the main
     *     border flip flop is set.
     *  2. If the Y coordinate reaches the bottom comparison value in cycle 63,
     *     the vertical border flip flop is set.
     *  3. If the Y coordinate reaches the top comparison value in cycle 63 and
     *     the DEN bit in register $d011 is set, the vertical border flip flop
     *     is reset.
     *  4. If the X coordinate reaches the left comparison value and the Y
     *     coordinate reaches the bottom one, the vertical border flip flop is
     *     set.
     *  5. If the X coordinate reaches the left comparison value and the Y
     *     coordinate reaches the top one and the DEN bit in register $d011 is
     *     set, the vertical border flip flop is reset.
     *  6. If the X coordinate reaches the left comparison value and the
     *     vertical border flip flop is not set, the main flip flop is reset."
     * [C.B.]
     */
    
    // Takes care of the vertical frame flipflop value (invoked in each cycle)
    void checkVerticalFrameFF();
    
    // Checks the frame fliplops at left border
    void checkFrameFlipflopsLeft(u16 comparisonValue);
    
    // Checks the frame fliplops at right border
    void checkFrameFlipflopsRight(u16 comparisonValue);
    
    // Sets the vertical frame flipflop with a delay of one cycle
    void setVerticalFrameFF(bool value);
    
    // Sets the main frame flipflop with a delay of one cycle
    void setMainFrameFF(bool value);
    
    // Returns a comparison value for the border flipflops
    u16 leftComparisonValue() const { return isCSEL() ? 24 : 31; }
    u16 rightComparisonValue() const { return isCSEL() ? 344 : 335; }
    u16 upperComparisonValue() const { return isRSEL() ? 51 : 55; }
    u16 lowerComparisonValue() const { return isRSEL() ? 251 : 247; }
    

    //
    // Accessing registers
    //

public:

    // Returns the current value of the DEN (Display ENabled) bit
    bool DENbit() const { return GET_BIT(reg.current.ctrl1, 4); }

    // Returns the masked CB13 bit
    u8 CB13() const { return memSelect & 0x08; }

    // Returns the masked CB13/CB12/CB11 bits
    u8 CB13CB12CB11() const { return memSelect & 0x0E; }

    // Returns the masked VM13/VM12/VM11/VM10 bits
    u8 VM13VM12VM11VM10() const { return memSelect & 0xF0; }

    // Returns the state of the CSEL bit
    bool isCSEL() const { return GET_BIT(reg.current.ctrl2, 3); }
    
    // Returns the state of the RSEL bit
    bool isRSEL() const { return GET_BIT(reg.current.ctrl1, 3); }


    //
    // Handling DMA lines and the display state
    //

private:
    
    // Returns true if the bad line condition holds
    bool badLineCondition() const;
    
    
    //
    // Interacting with the CPU
    //
    
private:
    
    // Sets the value of the BA line which is connected to the CPU's RDY pin.
    void updateBA(u8 value);
    
    /* Indicates if a c-access can occur. A c-access can only be performed if
     * the BA line is down for more than 2 cycles.
     */
    bool BApulledDownForAtLeastThreeCycles() const { return baLine.delayed(); }
    
    // Triggers a VICII interrupt
    void triggerIrq(u8 source);

    
    //
    // Handling lightpen events
    //
    
public:
    
    // Sets the value of the LP pin
    void setLP(bool value);
    
private:

    // Returns the coordinate of a light pen event
    u16 lightpenX() const;
    u16 lightpenY() const;
    
    /* Trigger a lightpen interrupt if conditions are met. This function is
     * called on each negative transition of the LP pin. It latches the x and
     * y coordinates and immediately triggers an interrupt if a newer VICII
     * model is emulated. Older models trigger the interrupt later, at the
     * beginning of a new frame.
     */
    void checkForLightpenIrq();

    /* Retriggers a lightpen interrupt if conditions are met. This function is
     * called at the beginning of each frame. If the lp line is still low at
     * this point of time, a lightpen interrupt is retriggered. Note that older
     * VICII models trigger interrupts only at this point in time.
     */
    void checkForLightpenIrqAtStartOfFrame();
    
    
    //
    // Sprites
    //

private:

    // Gets the depth of a sprite (will be written into the z buffer)
    u8 spriteDepth(isize nr) const;
    
    // Compares the Y coordinates of all sprites with the yCounter
    u8 compareSpriteY() const;
    
    /* Turns off sprite dma if conditions are met. In cycle 16, the mcbase
     * pointer is advanced three bytes for all dma enabled sprites. Advancing
     * three bytes means that mcbase will then point to the next sprite line.
     * When mcbase reached 63, all 21 sprite lines have been drawn and sprite
     * dma is switched off. The whole operation is skipped when the y expansion
     * flipflop is 0. This never happens for normal sprites (there is no
     * skipping then), but happens every other cycle for vertically expanded
     * sprites. Thus, mcbase advances for those sprites at half speed which
     * actually causes the expansion.
     */
    void turnSpriteDmaOff();

    /* Turns on sprite dma accesses if conditions are met. This function is
     * called in cycle 55 and cycle 56.
     */
    void turnSpriteDmaOn();

    /* Turns sprite display on or off. This function is called in cycle 58.
     */
    void turnSpritesOnOrOff();
    
    /* Loads a sprite shift register. The shift register is loaded with the
     * three data bytes fetched in the previous sAccesses.
     */
    void loadSpriteShiftRegister(isize nr);
    
    /* Updates the sprite shift registers. Checks if a sprite has completed
     * it's last DMA fetch and calls loadSpriteShiftRegister() accordingly.
     */
    void updateSpriteShiftRegisters();
    
    /* Toggles expansion flipflop for vertically stretched sprites. In cycle 56,
     * register D017 is read and the flipflop gets inverted for all sprites with
     * vertical stretching enabled. When the flipflop goes down, advanceMCBase()
     * will have no effect in the next scanline. This causes each sprite line
     * to be drawn twice.
     */
    void toggleExpansionFlipflop() { expansionFF ^= reg.current.sprExpandY; }
    
    
    //
    // Running the device (VICII.cpp and VIC_cycles_xxx.cpp)
    //

public:

    /* Prepares VICII for drawing a new frame. This function is called prior to
     * the first cycle of each frame.
     */
    void beginFrame();

    /* Prepares VICII for drawing a new scanline. This function is called
     * prior to the first cycle of each scanline.
     */
    void beginScanline();

    /* Finishes up a scanline. This function is called after the last cycle
     * of each scanline.
     */
    void endScanline();

    /* Finishes up a frame. This function is called after the last cycle of
     * each frame.
     */
    void endFrame();

    /* Processes all time delayed actions. This function is called at the
     * beginning of each VICII cycle.
     */
    void processDelayedActions();
    
    // Emulates a specific scanline cycle
    template <u16 flags> void cycle1();
    template <u16 flags> void cycle2();
    template <u16 flags> void cycle3();
    template <u16 flags> void cycle4();
    template <u16 flags> void cycle5();
    template <u16 flags> void cycle6();
    template <u16 flags> void cycle7();
    template <u16 flags> void cycle8();
    template <u16 flags> void cycle9();
    template <u16 flags> void cycle10();
    template <u16 flags> void cycle11();
    template <u16 flags> void cycle12();
    template <u16 flags> void cycle13();
    template <u16 flags> void cycle14();
    template <u16 flags> void cycle15();
    template <u16 flags> void cycle16();
    template <u16 flags> void cycle17();
    template <u16 flags> void cycle18();
    template <u16 flags> void cycle19to54();
    template <u16 flags> void cycle55();
    template <u16 flags> void cycle56();
    template <u16 flags> void cycle57();
    template <u16 flags> void cycle58();
    template <u16 flags> void cycle59();
    template <u16 flags> void cycle60();
    template <u16 flags> void cycle61();
    template <u16 flags> void cycle62();
    template <u16 flags> void cycle63();
    template <u16 flags> void cycle64();
    template <u16 flags> void cycle65();

#define DRAW_SPRITES_DMA1 \
assert(isFirstDMAcycle); assert(!isSecondDMAcycle); \
if constexpr (!(flags & HEADLESS_CYCLE)) { drawSpritesSlowPath(); }

#define DRAW_SPRITES_DMA2 \
assert(!isFirstDMAcycle); assert(isSecondDMAcycle); \
if constexpr (!(flags & HEADLESS_CYCLE)) { drawSpritesSlowPath(); }

#define DRAW_SPRITES \
assert(!isFirstDMAcycle && !isSecondDMAcycle); \
if (spriteDisplay && !(flags & HEADLESS_CYCLE)) { drawSprites(); }

#define DRAW_SPRITES59 \
if ((spriteDisplayDelayed || spriteDisplay || isSecondDMAcycle) && !(flags & HEADLESS_CYCLE)) \
{ drawSpritesSlowPath(); }
    
#define DRAW   if (!vblank && !(flags & HEADLESS_CYCLE)) { drawCanvas(); drawBorder(); };
#define DRAW17 if (!vblank && !(flags & HEADLESS_CYCLE)) { drawCanvas(); drawBorder17(); };
#define DRAW55 if (!vblank && !(flags & HEADLESS_CYCLE)) { drawCanvas(); drawBorder55(); };
#define DRAW59 if (!vblank && !(flags & HEADLESS_CYCLE)) { drawCanvas(); drawBorder(); };

#define END_CYCLE \
dataBusPhi2 = 0xFF; \
xCounter += 8; \
bufferoffset += 8; \
if (unlikely(delay)) { processDelayedActions(); }

#define END_VISIBLE_CYCLE \
END_CYCLE
    
#define BA_LINE(x) updateBA(x);
    
    
    //
    // Drawing routines (VIC_draw.cpp)
    //
    
private:

    // Draws 8 border pixels. Invoked inside draw().
    void drawBorder();
    
    // Draws the border pixels in cycle 17
    void drawBorder17();
    
    // Draws the border pixels in cycle 55
    void drawBorder55();
    
    // Draws 8 canvas pixels
    void drawCanvas();
    void drawCanvasFastPath();
    void drawCanvasSlowPath();

    // Draws a single canvas pixel
    void drawCanvasPixel(u8 pixel, u8 mode, u8 d016);
    
    // Reloads the sequencer shift register with the gAccess result
    void loadShiftRegister();
    
    //
    // Drawing routines (VIC_sprites.cpp)
    //
    
private:
    
    // Draws 8 sprite pixels (see draw())
    void drawSprites();
    void drawSpritesFastPath();
    void drawSpritesSlowPath();
    
    /* Draws all sprite pixels for a single sprite. This function is used when
     * the fast path is taken.
     */
    template <bool multicolor> void drawSpriteNr(isize nr, bool enable, bool active);

    /* Draws a single sprite pixel for all sprites. This function is used when
     * the slow path is taken.
     *
     *         pixel : pixel number (0 ... 7)
     *    enableBits : the spriteDisplay bits
     *    freezeBits : forces the sprites shift register to freeze temporarily
     */
    void drawSpritePixel(isize pixel, u8 enableBits, u8 freezeBits);

    // Performs collision detection
    void checkCollisions();
    
    
    //
    // Low level drawing (pixel buffer access)
    //
    
    // Writes a single color value into the screenbuffer
#define COLORIZE(index,color) \
emuTexturePtr[index] = rgbaTable[color];
    
    // Sets a single frame pixel
#define SET_FRAME_PIXEL(pixel,color) { \
isize index = bufferoffset + pixel; \
COLORIZE(index, color); \
zBuffer[index] = DEPTH_BORDER; }
    
    // Sets a single foreground pixel
#define SET_FG_PIXEL(pixel,color) { \
isize index = bufferoffset + pixel; \
COLORIZE(index,color) \
zBuffer[index] = DEPTH_FG; }

    // Sets a single background pixel
#define SET_BG_PIXEL(pixel,color) { \
isize index = bufferoffset + pixel; \
COLORIZE(index,color) \
zBuffer[index] = DEPTH_BG; }
    
    // Sets a single sprite pixel
#define SET_SPRITE_PIXEL(sprite,pixel,color) { \
isize index = bufferoffset + pixel; \
if (u8 depth = spriteDepth(sprite); depth <= zBuffer[index]) { \
if (isVisibleColumn) COLORIZE(index, color); \
zBuffer[index] = depth | (zBuffer[index] & 0x10); \
} }

    
    //
    // Debugging
    //

public: 

    // Returns the current screen geometry
    ScreenGeometry getScreenGeometry(void) const;
    
    // Returns the coordinates of a certain sprite
    u16 getSpriteX(int nr) const { return reg.current.sprX[nr]; }
    u8 getSpriteY(int nr) const { return reg.current.sprY[nr]; }
};

}
