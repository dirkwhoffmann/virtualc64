// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _VICII_H
#define _VICII_H

#include "C64Component.h"
#include "TimeDelayed.h"

class VICII : public C64Component {

    friend class C64Memory;
    
    // Current configuration
    VICConfig config;
    
    // Result of the latest inspection
    VICIIInfo info;
    SpriteInfo spriteInfo[8];
    

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
    
    // Raster interrupt line ($D012)
    u8 rasterIrqLine;
    
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
    
    /* Y raster counter (3): The rasterline counter is usually incremented in
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
    
    /* Video matrix (6): Every 8th rasterline, the VICII chips performs a
     * c-access and fills this array with character information.
     */
    u8 videoMatrix[40];
    
    /* Color line (7): Every 8th rasterline, the VICII chips performs a
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
        u8 latchedCharacter;
        
        /* Latched color info. Whenever the shift register is loaded, the
         * current color value (which was once read during a gAccess) is
         * latched. This value is used until the shift register loads again.
         */
        u8 latchedColor;
        
        /* Color bits. Every second pixel (as synchronized with mcFlop), the
         * multi-color bits are remembered.
         */
        u8 colorbits;
        
        /* Remaining bits to be pumped out. Makes sure no more than 8 pixels
         * are outputted.
         */
        int remainingBits;
        
    } sr;
    
    /* Sprite data sequencer (11): The VICII chip has a 24 bit (3 byte) shift
     * register for each sprite. It stores the sprite for one rasterline. If a
     * sprite is a display candidate in the current rasterline, its shift
     * register is activated when the raster X coordinate matches the sprites
     * X coordinate. The comparison is done in method drawSprite(). Once a
     * shift register is activated, it remains activated until the beginning of
     * the next rasterline. However, after an activated shift register has
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
     * frame flipflop needs to be set in the current rasterline.
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
    // Housekeeping information
    //
    
    /* Indicates wether we are in a visible display column or not. The visible
     * columns comprise canvas columns and border columns. The first visible
     * column is drawn in cycle 14 (first left border column) and the last in
     * cycle 61 (fourth right border column).
     */
    bool isVisibleColumn;
    
    /* Set to true in cycle 1, cycle 63 (65) iff yCounter matches D012. This
     * variable is needed to determine if a rasterline interrupt should be
     * triggered in cycle 1 or 2.
     * DEPRECATED: Will be replaced by rasterlineMatchesIrqLine
     */
    bool yCounterEqualsIrqRasterline;
    
    // True if the current rasterline belongs to the VBLANK area
    bool vblank;
    
    // Indicates if the current rasterline is a DMA line (bad line)
    bool badLine;
    
    /* True, if DMA lines can occurr within the current frame. Bad lines can
     * occur only if the DEN bit was set during an arbitary cycle in rasterline
     * 30. The DEN bit is located in control register 1 (0x11).
     */
    bool DENwasSetInRasterline30;
    
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
        
    // Determines if a sprite needs to be drawn in the current rasterline
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
    
private:
    
    /* Memory source lookup table. If VICII is not running in Ultimax mode, it
     * has access to RAM and the character Rom. In ultimax mode, VICII has
     * access to ROMH and some portions of RAM.
     */
    MemoryType memSrc[16];
    
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
    TimeDelayed <u16,4> baLine = TimeDelayed <u16,4> (3);
    
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
    TimeDelayed <u32,3> gAccessResult = TimeDelayed <u32,3> (2);
    
    
    //
    // Color management
    //
    
    /* The brightness, contrast, and saturation parameters used for computing
     * the color palette. Valid values: 0.0 - 100.0
     */
    double brightness = 50.0;
    double contrast = 100.0;
    double saturation = 50.0;

 
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
    
    // Buffer storing background noise (random black and white pixels)
    u32 *noise;

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
    int *emuTexture1 = new int[TEX_HEIGHT * TEX_WIDTH];
    int *emuTexture2 = new int[TEX_HEIGHT * TEX_WIDTH];
    int *dmaTexture1 = new int[TEX_HEIGHT * TEX_WIDTH];
    int *dmaTexture2 = new int[TEX_HEIGHT * TEX_WIDTH];
    /*
     */
     
    /* Pointer to the current working texture. This variable points either to
     * the first or the second texture buffer. After a frame has been finished,
     * the pointer is redirected to the other buffer.
     */
    int *emuTexture;
    int *dmaTexture;

    /* Pointer to the beginning of the current rasterline inside the current
     * working textures. These pointers are used by all rendering methods to
     * write pixels. It always points to the beginning of a rasterline, either
     * the first or the second texture buffer. They are reset at the beginning
     * of each frame and incremented at the beginning of each rasterline.
     */
    int *emuTexturePtr;
    int *dmaTexturePtr;

    /* VICII utilizes a depth buffer to determine pixel priority. The render
     * routines only write a color value, if it is closer to the view point.
     * The depth of the closest pixel is kept in this buffer. The lower the
     * value, the closer it is to the viewer.
     */
    u8 zBuffer[TEX_WIDTH];
    
    /* Indicates the source of a drawn pixel. Whenever a foreground pixel or
     * sprite pixel is drawn, a distinct bit in the pixelSource array is set.
     * The information is needed to detect sprite-sprite and sprite-background
     * collisions.
     *
     *     Bit      8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0
     *     --------------------------------------------------
     *     Format: FG | S7 | S6 | S5 | S4 | S3 | S2 | S1 | S0
     */
    u16 pixelSource[TEX_WIDTH];
    
    /* Offset into to pixelBuffer. This variable points to the first pixel of
     * the currently drawn 8 pixel chunk.
     */
    short bufferoffset;
    
    /* Color storage filled by loadColors()
     *
     *     [0] : color for '0'  pixels in single color mode
     *                  or '00' pixels in multicolor mode
     *     [1] : color for '1'  pixels in single color mode
     *                  or '01' pixels in multicolor mode
     *     [2] : color for '10' pixels in multicolor mode
     *     [3] : color for '11' pixels in multicolor mode
     */
    u8 col[4];
    
    
    //
    // DMA debugger
    //
    
    // Indicates if memory accesses of a certain type should be visualized
    // bool visualize[ACCESS_CNT];

    // Lookup table for DMA debugging colors
    u32 debugColor[ACCESS_CNT][4];

    // Opacity of DMA pixels
    // double opacity = 0.5;

    // Currently selected display mode
    // DmaDisplayMode displayMode = MODULATE_FG_LAYER;

    
    //
    // Initializing
    //
    
public:
	
    VICII(C64 &ref);
    
private:
    
    void _initialize() override;
    void _reset() override;

    void resetEmuTexture(int nr);
    void resetEmuTextures() { resetEmuTexture(1); resetEmuTexture(2); }
    void resetDmaTexture(int nr);
    void resetDmaTextures() { resetDmaTexture(1); resetDmaTexture(2); }

    
    //
    // Configuring
    //
    
public:
    
    VICConfig getConfig() { return config; }
    
    long getConfigItem(ConfigOption option);
    bool setConfigItem(ConfigOption option, long value) override;
    
    VICRevision getRevision() { return config.revision; }    
    void setRevision(VICRevision revision);
    
    void setDmaDebugColor(MemAccessType type, GpuColor color);
    void setDmaDebugColor(MemAccessType type, RgbColor color);
    
private:
    
    void _dumpConfig() override;


    //
    // Analyzing
    //
    
public:
    
    VICIIInfo getInfo() { return HardwareComponent::getInfo(info); }
    SpriteInfo getSpriteInfo(int nr);

private:
    
    void _inspect() override;
    void _dump() override;

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
        worker
        
        & config.revision
        & config.glueLogic
        & config.grayDotBug
        
        & memSrc;
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & reg.current
        & reg.delayed
        & rasterIrqLine
        & latchedLPX
        & latchedLPY
        & memSelect
        & irr
        & imr
        & refreshCounter
        & xCounter
        & yCounter
        & vc
        & vcBase
        & rc
        & videoMatrix
        & colorLine
        & vmli
        & sr.data
        & sr.canLoad
        & sr.mcFlop
        & sr.latchedCharacter
        & sr.latchedColor
        & sr.colorbits
        & sr.remainingBits
        & spriteSr
        & spriteSrActive
        & spriteSpriteCollision
        & spriteBackgroundColllision
        & flipflops.current.vertical
        & flipflops.current.main
        & flipflops.delayed.vertical
        & flipflops.delayed.main
        & verticalFrameFFsetCond
        & leftComparisonVal
        & rightComparisonVal
        & upperComparisonVal
        & lowerComparisonVal
        & isVisibleColumn
        & yCounterEqualsIrqRasterline
        & vblank
        & badLine
        & DENwasSetInRasterline30
        & displayState
        & mc
        & mcbase
        & spritePtr
        & isFirstDMAcycle
        & isSecondDMAcycle
        & spriteDisplay
        & spriteDisplayDelayed
        & spriteDmaOnOff
        & expansionFF
        & cleared_bits_in_d017
        & lpLine
        & lpIrqHasOccurred
        & ultimax
        & dataBusPhi1
        & dataBusPhi2
        & addrBus
        & baLine
        & bankAddr
        & gAccessResult
        & delay
        & bufferoffset;
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }

    
    //
    // Accessing
    //
    
public:
    
    // Returns true if a PAL chip is plugged in
    static bool isPAL(VICRevision revision);
    bool isPAL() { return isPAL(config.revision); }
    
    // Returns true if a NTSC chip is plugged in
    static bool isNTSC(VICRevision revision);
    bool isNTSC() { return isNTSC(config.revision); }

    // Returns true if a newer MOS 856x chip is plugged in
    static bool is856x(VICRevision revision);
    bool is856x() { return is856x(config.revision); }
    
    // Returns true if an older MOS 656x chip is plugged in
    static bool is656x(VICRevision revision);
    bool is656x() { return is656x(config.revision); }

    // Returns true if light pen interrupts are triggered with a delay
    static bool delayedLightPenIrqs(VICRevision revision);
    bool delayedLightPenIrqs() { return delayedLightPenIrqs(config.revision); }

    // Returns the clock frequencay of the selected VICII model
    static unsigned getFrequency(VICRevision revision);
    unsigned getFrequency() { return getFrequency(config.revision); }
    
    // Returns the number of CPU cycles performed per rasterline
    static unsigned getCyclesPerLine(VICRevision revision);
    unsigned getCyclesPerLine() { return getCyclesPerLine(config.revision); }
    
    // Returns true if the end of the rasterline has been reached
    bool isLastCycleInRasterline(unsigned cycle);
    
    // Returns the number of rasterlines drawn per frame
    long getRasterlinesPerFrame();

    // Returns the number of visible rasterlines in a single frame
    long numVisibleRasterlines();

    // Returns true if rasterline belongs to the VBLANK area
    bool isVBlankLine(unsigned rasterline);
    
    // Returns the number of CPU cycles executed in one frame
    long getCyclesPerFrame() {
        return getRasterlinesPerFrame() * getCyclesPerLine(); }
    
    /* Returns the number of frames drawn per second. The result is returned as
     * a floating point value, because Commodore did not manage to match the
     * expected values exactly (50 Hz for PAL and 60 Hz for NTSC). E.g., a PAL
     * C64 outputs 50.125 Hz.
     */
    double getFramesPerSecond() {
        return (double)getFrequency() / (double)getCyclesPerFrame();
    }
    
    // Returns the time interval between two frames in nanoseconds
    u64 getFrameDelay() {
        return u64(1000000000.0 / getFramesPerSecond());
    }
    
    
    //
    // Accessing the screen buffer and display properties
    //
    
    // Returns the currently stable textures
    void *stableEmuTexture();
    void *stableDmaTexture();
    
    // Returns a pointer to randon noise
    u32 *getNoise();
    
    // Returns a C64 color in 32 bit big endian RGBA format
    u32 getColor(unsigned nr);
    u32 getColor(unsigned nr, Palette palette);
    
    // Gets or sets a monitor parameter
    double getBrightness() { return brightness; }
    void setBrightness(double value);
    double getContrast() { return contrast; }
    void setContrast(double value);
    double getSaturation() { return saturation; }
    void setSaturation(double value);
    
private:
    
    /* Updates the RGBA values for all sixteen C64 colors. The base palette is
     * determined by the selected VICII model.
     */
    void updatePalette();

    
    //
    // Accessing memory (VIC_memory.cpp)
    //

public:
    
    // Peeks a value from a VICII register without side effects
    u8 spypeek(u16 addr);
    
    // Returns the ultimax flag
    u8 getUltimax() { return ultimax; }

    // Sets the ultimax flag
    void setUltimax(bool value);
    
    // Returns the latest value of the VICII's data bus during phi1
    u8 getDataBusPhi1() { return dataBusPhi1; }

    // Returns the latest value of the VICII's data bus during phi2
    u8 getDataBusPhi2() { return dataBusPhi2; }

    /* Schedules the VICII bank to to switched. This method is called if the
     * bank switch is triggered by a change of CIA2::PA or CIA2::DDRA.
     */
    void switchBank(u16 addr);

private:

    /* Updates the VICII bank address. The new address is computed from the
     * provided bank number.
     */
    void updateBankAddr(u8 bank) { assert(bank < 4); bankAddr = bank << 14; }

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
    bool isCharRomAddr(u16 addr);

    // Performs a DRAM refresh (r-access)
    template <VICIIMode type> void rAccess();
    
    // Performs an idle access (i-access)
    template <VICIIMode type> void iAccess();
    
    // Performs a character access (c-access)
    template <VICIIMode type> void cAccess();
    
    // Performs a graphics access (g-access)
    template <VICIIMode type> void gAccess();

    // Computes the g-access fetch address for different VICII models
    u16 gAccessAddr85x();
    u16 gAccessAddr65x();

    /* Computes the g-access fetch address. The fetch address is influences by
     * both the BMM and ECM bit.
     */
    u16 gAccessAddr(bool bmm, bool ecm);
    
    // Performs a sprite pointer access (p-access)
    template <VICIIMode type> void pAccess(unsigned sprite);
    
    // Performs one of the three sprite data accesses
    template <VICIIMode type> void sFirstAccess(unsigned sprite);
    template <VICIIMode type> void sSecondAccess(unsigned sprite);
    template <VICIIMode type> void sThirdAccess(unsigned sprite);
    
    /* Finalizes the sprite data access. This method is invoked one cycle after
     * the second and third sprite DMA has occurred.
     */
    void sFinalize(unsigned sprite);
    

    //
    // Handling the x and y counters
    //
    
    /* Returns the current rasterline. This value is not always identical to
     * the yCounter, because the yCounter is incremented with a little delay.
     */
    u16 rasterline();

    // Returns the current rasterline cycle
    u8 rastercycle();

    /* Indicates if yCounter needs to be reset in this rasterline. PAL models
     * reset the yCounter in cycle 2 in the first rasterline wheras NTSC models
     * reset the yCounter in cycle 2 in the middle of the lower border area.
     */
    bool yCounterOverflow() { return rasterline() == (isPAL() ? 0 : 238); }


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
    u16 leftComparisonValue() { return isCSEL() ? 24 : 31; }
    u16 rightComparisonValue() { return isCSEL() ? 344 : 335; }
    u16 upperComparisonValue() { return isRSEL() ? 51 : 55; }
    u16 lowerComparisonValue() { return isRSEL() ? 251 : 247; }
    
  
	//
	// Accessing registers
	//
	
public:
		
    // Returns the current value of the DEN (Display ENabled) bit
    bool DENbit() { return GET_BIT(reg.current.ctrl1, 4); }
    
    // Returns the number of the next interrupt rasterline
    u16 rasterInterruptLine() {
        return ((reg.current.ctrl1 & 0x80) << 1) | rasterIrqLine;
    }
    
    // Returns the masked CB13 bit
    u8 CB13() { return memSelect & 0x08; }

    // Returns the masked CB13/CB12/CB11 bits
    u8 CB13CB12CB11() { return memSelect & 0x0E; }

    // Returns the masked VM13/VM12/VM11/VM10 bits
    u8 VM13VM12VM11VM10() { return memSelect & 0xF0; }

	// Returns the state of the CSEL bit
	bool isCSEL() { return GET_BIT(reg.current.ctrl2, 3); }
    
	// Returns the state of the RSEL bit
    bool isRSEL() { return GET_BIT(reg.current.ctrl1, 3); }


    //
    // Handling DMA lines and the display state
    //

private:
    
    // Returns true if the bad line condition holds
    bool badLineCondition();
    
    
    //
    // Interacting with the CPU
    //
    
private:
    
    // Sets the value of the BA line which is connected to the CPU's RDY pin.
    void updateBA(u8 value);
    
    /* Indicates if a c-access can occur. A c-access can only be performed if
     * the BA line is down for more than 2 cycles.
     */
    bool BApulledDownForAtLeastThreeCycles() { return baLine.delayed(); }
    
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
    u16 lightpenX();
    u16 lightpenY();
    
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
    u8 spriteDepth(u8 nr);
    
    // Compares the Y coordinates of all sprites with the yCounter
    u8 compareSpriteY();
    
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
    void loadShiftRegister(unsigned nr) {
        spriteSr[nr].data = LO_LO_HI(spriteSr[nr].chunk3,
                                     spriteSr[nr].chunk2,
                                     spriteSr[nr].chunk1);
    }
    
    /* Updates the sprite shift registers. Checks if a sprite has completed
     * it's last DMA fetch and calls loadShiftRegister() accordingly.
     */
    void updateSpriteShiftRegisters();
    
    /* Toggles expansion flipflop for vertically stretched sprites. In cycle 56,
     * register D017 is read and the flipflop gets inverted for all sprites with
     * vertical stretching enabled. When the flipflop goes down, advanceMCBase()
     * will have no effect in the next rasterline. This causes each sprite line
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
	
	/* Prepares VICII for drawing a new rasterline. This function is called
     * prior to the first cycle of each rasterline.
     */
	void beginRasterline(u16 rasterline);

	/* Finishes up a rasterline. This function is called after the last cycle
     * of each rasterline.
     */
	void endRasterline();
	
	/* Finishes up a frame. This function is called after the last cycle of
     * each frame.
     */
	void endFrame();
    
    /* Processes all time delayed actions. This function is called at the
     * beginning of each VICII cycle.
     */
    void processDelayedActions();
    
	// Emulates a specific rasterline cycle
    template <VICIIMode type> void cycle1();
    template <VICIIMode type> void cycle2();
    template <VICIIMode type> void cycle3();
    template <VICIIMode type> void cycle4();
    template <VICIIMode type> void cycle5();
    template <VICIIMode type> void cycle6();
    template <VICIIMode type> void cycle7();
    template <VICIIMode type> void cycle8();
    template <VICIIMode type> void cycle9();
    template <VICIIMode type> void cycle10();
    template <VICIIMode type> void cycle11();
    template <VICIIMode type> void cycle12();
    template <VICIIMode type> void cycle13();
    template <VICIIMode type> void cycle14();
    template <VICIIMode type> void cycle15();
    template <VICIIMode type> void cycle16();
    template <VICIIMode type> void cycle17();
    template <VICIIMode type> void cycle18();
    template <VICIIMode type> void cycle19to54();
    template <VICIIMode type> void cycle55();
    template <VICIIMode type> void cycle56();
    template <VICIIMode type> void cycle57();
    template <VICIIMode type> void cycle58();
    template <VICIIMode type> void cycle59();
    template <VICIIMode type> void cycle60();
    template <VICIIMode type> void cycle61();
    template <VICIIMode type> void cycle62();
    template <VICIIMode type> void cycle63();
    template <VICIIMode type> void cycle64();
    template <VICIIMode type> void cycle65();

    // DEPRECATED
    void cycle1pal();   void cycle1ntsc();
    void cycle2pal();   void cycle2ntsc();
    void cycle3pal();   void cycle3ntsc();
    void cycle4pal();   void cycle4ntsc();
    void cycle5pal();   void cycle5ntsc();
    void cycle6pal();   void cycle6ntsc();
    void cycle7pal();   void cycle7ntsc();
    void cycle8pal();   void cycle8ntsc();
    void cycle9pal();   void cycle9ntsc();
    void cycle10pal();  void cycle10ntsc();
    void cycle11pal();  void cycle11ntsc();
    void cycle12();
    void cycle13();
    void cycle14();
    void cycle15();
    void cycle16();
    void cycle17();
    void cycle18();
    void cycle19to54();
    void cycle55pal();  void cycle55ntsc();
    void cycle56();
    void cycle57pal();  void cycle57ntsc();
    void cycle58pal();  void cycle58ntsc();
    void cycle59pal();  void cycle59ntsc();
    void cycle60pal();  void cycle60ntsc();
    void cycle61pal();  void cycle61ntsc();
    void cycle62pal();  void cycle62ntsc();
    void cycle63pal();  void cycle63ntsc();
    void cycle64ntsc();
    void cycle65ntsc();
	
    #define DRAW_SPRITES if (spriteDisplay || isSecondDMAcycle) drawSprites();
    #define DRAW_SPRITES59 if (spriteDisplayDelayed || spriteDisplay || isSecondDMAcycle) drawSprites();

    #define DRAW if (!vblank) draw(); DRAW_SPRITES;
    #define DRAW17 if (!vblank) draw17(); DRAW_SPRITES;
    #define DRAW55 if (!vblank) draw55(); DRAW_SPRITES;
    #define DRAW59 if (!vblank) draw(); DRAW_SPRITES59;
    #define DRAW_IDLE DRAW_SPRITES;
        
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
    
    /* Draws 8 pixels. This is the main entry point to the VICII code and
     * invoked in each drawing cycle. An exception are cycle 17 and cycle 55
     * which are handled seperately for speedup reasons.
     */
    void draw();
    
    // Special draw routine for cycle 17
    void draw17();
    
    // Special draw routine for cycle 55
    void draw55();
        
    
    //
    // Internal drawing routines (called by draw(), draw17(), and drae55())
    //
    
    // Draws 8 border pixels. Invoked inside draw().
    void drawBorder();
    
    // Draws the border pixels in cycle 17 (see draw17())
    void drawBorder17();
    
    // Draws the border pixels in cycle 55 (see draw55())
    void drawBorder55();
    
    // Draws 8 canvas pixels (see draw())
    void drawCanvas();
    
    /* Draws a single canvas pixel
     *
     *         pixel : pixel number (0 ... 7)
     *          mode : display mode for this pixel
     *          d016 : current value of register D016
     *  loadShiftReg : forces the shift register to be reloaded
     *  updateColors : forces the four selectable colors to be reloaded
     */
    void drawCanvasPixel(u8 pixel,
                         u8 mode,
                         u8 d016,
                         bool loadShiftReg,
                         bool updateColors);
    
    // Draws 8 sprite pixels (see draw())
    void drawSprites();
    
    /* Draws a single sprite pixel for all sprites
     *
     *         pixel : pixel number (0 ... 7)
     *    enableBits : the spriteDisplay bits
     *    freezeBits : forces the sprites shift register to freeze temporarily
     */
    void drawSpritePixel(unsigned pixel,
                         u8 enableBits,
                         u8 freezeBits);
    
    
    //
    // Mid level drawing (semantic pixel rendering)
    //
    
    // Determines pixel colors accordig to the provided display mode
    void loadColors(u8 mode);
    
    
    //
    // Low level drawing (pixel buffer access)
    //
    
    // Writes a single color value into the screenbuffer
    #define COLORIZE(index,color) \
        assert(index < TEX_WIDTH); \
        emuTexturePtr[index] = rgbaTable[color];
    
    /* Sets a single frame pixel. The upper bit in pixelSource is cleared to
     * prevent sprite/foreground collision detection in border area.
     */
    #define SET_FRAME_PIXEL(pixel,color) { \
        int index = bufferoffset + pixel; \
        COLORIZE(index, color); \
        zBuffer[index] = BORDER_LAYER_DEPTH; \
        pixelSource[index] &= (~0x100); }
    
    // Sets a single foreground pixel
    #define SET_FOREGROUND_PIXEL(pixel,color) { \
        int index = bufferoffset + pixel; \
        COLORIZE(index,color) \
        zBuffer[index] = FOREGROUND_LAYER_DEPTH; \
        pixelSource[index] = 0x100; }

    // Sets a single background pixel
    #define SET_BACKGROUND_PIXEL(pixel,color) { \
        int index = bufferoffset + pixel; \
        COLORIZE(index,color) \
        zBuffer[index] = BACKGROUD_LAYER_DEPTH; \
        pixelSource[index] = 0x00; }
    
    // Draw a single sprite pixel
    void setSpritePixel(unsigned sprite, unsigned pixel, u8 color);
        
    
	//
	// Debugging
	//

public: 

    // Returns the current screen geometry
    ScreenGeometry getScreenGeometry(void);

    /* Draw a horizontal colored line into the screen buffer. This method is
     * utilized for debugging purposes, only.
     */
    // void markLine(u8 color, unsigned start = 0, unsigned end = TEX_WIDTH - 1);
    
    /* Cuts out certain graphics layers
     */
    void cutLayers();
    
    // Initializes the DMA debugger textures
    void clearDmaDebuggerTexture();
    
    // Visualizes a memory access by drawing into the DMA debuger texture
    void visualizeDma(u8 offset, u8 data, MemAccessType type);
    
    // Superimposes the debug output onto the current rasterline
    void computeOverlay();
};

#endif

