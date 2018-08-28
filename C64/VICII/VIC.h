/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef _VIC_INC
#define _VIC_INC

#include "VirtualComponent.h"
#include "C64_types.h"
#include "TimeDelayed.h"

// Sprite bit masks
#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

// Depth of different drawing layers
#define BORDER_LAYER_DEPTH 0x10         // In front of everything
#define SPRITE_LAYER_FG_DEPTH 0x20      // Behind border
#define FOREGROUND_LAYER_DEPTH 0x30     // Behind sprite 1 layer
#define SPRITE_LAYER_BG_DEPTH 0x40      // Behind foreground
#define BACKGROUD_LAYER_DEPTH 0x50      // Behind sprite 2 layer
#define BEIND_BACKGROUND_DEPTH 0x60     // Behind background

// Event flags
#define VICUpdateIrqLine    (1ULL << 0) // Sets or releases the IRQ line
#define VICLpTransition     (1ULL << 1) // Triggers a lightpen event
#define VICUpdateFlipflops  (1ULL << 2) // Updates the flipflop value pipeline
#define VICUpdateRegisters  (1ULL << 3) // Updates the register value pipeline
#define VICUpdateBankAddr   (1ULL << 4) // Updates the bank address
#define VICSetDisplayState  (1ULL << 5) // Flagged when control reg 1 changes

#define VICClearanceMask ~((1ULL << 6) | VICUpdateIrqLine | VICLpTransition | VICUpdateFlipflops | VICUpdateRegisters | VICUpdateBankAddr | VICSetDisplayState);



// Forward declarations
class C64Memory;


/*! @brief    Virtual Video Controller (VICII)
 *  @details  VICII is the video controller chip of the Commodore 64.
 *            It occupies the memory mapped I/O space from address 0xD000 to 0xD02E. 
 */
class VIC : public VirtualComponent {

    friend C64Memory;
    
    //
    // Configuration options
    //
    
private:
    
    //! @brief    Selected chip model
    VICChipModel chipModel;

    //! @brief    Glue logic type
    GlueLogic glueLogic;

public:
    
    /*! @brief    Indicates if the gray dot bug should be emulated
     *  @note     The gray mode bug only affects the newer VICII models 856x
     */
    bool emulateGrayDotBug;
    
    
    //
    // I/O space (CPU accessible)
    //
    
private:
    
    /*! @brief    Piped I/O register state.
     *  @details  When an I/O register is written to, the corresponding value
     *            in variable current is changed and a flag is set in variable
     *            delay. Function processDelayedActions() reads the flag and if
     *            set to true, updates the delayed values with the current ones.
     *  @see      processDelayedActions()
     */
    struct {
        VICIIRegisters current;
        VICIIRegisters delayed;
    } reg;
    
    //! @brief    Raster interrupt line ($D012)
    uint8_t rasterIrqLine;
    
    //! @brief    Latched lightpen X coordinate ($D013)
    uint8_t latchedLightPenX;
    
    //! @brief    Latched lightpen Y coordinate ($D014)
    uint8_t latchedLightPenY;
    
    //! @brief    Memory address register ($D018)
    uint8_t memSelect;
    
    //! @brief    Interrupt Request Register ($D019)
    uint8_t irr;
    
    //! @brief    Interrupt Mask Register ($D01A)
    uint8_t imr;

    
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
    //                             |            (13)   v
    //    RC: Row Counter          |            +-------------+
    //                             +----------->| Border unit |
    //    MC: MOB Data Counter     |            +-------------+
    //                             |                   |
    //                             v                   v
    //                     +----------------+  +----------------+
    //                     |Sync generation |  |Color generation|<------- øCOLOR
    //                     +----------------+  +----------------+
    //                                    |      |
    //                                    v      v
    //                                  Video output
    //                                (S/LUM and COLOR)              [C.B.]

    
    /*! @brief    Refresh counter (1)
     *  @details  "The VIC does five read accesses in every raster line for the
     *             refresh of the dynamic RAM. An 8 bit refresh counter (REF)
     *             is used to generate 256 DRAM row addresses. The counter is
     *             reset to $ff in raster line 0 and decremented by 1 after each
     *             refresh access." [C.B.]
     *  @seealso   rAccess()
     */
    uint8_t refreshCounter;
    
    /*! @brief    Raster counter X (2)
     *  @details  Defines the sprite coordinate system.
     */
    uint16_t xCounter;
    
    /*! @brief    Y raster counter (3)
     *  @details  The rasterline counter is usually incremented in cycle 1. The
     *            only exception is the overflow condition which is handled in
     *            cycle 2.
     */
    uint32_t yCounter;
    
    /*! @brief    Video counter
     *  @details  A 10 bit counter that can be loaded with the value from
     *            vcBase.
     */
    uint16_t vc;
    
    /*! @brief    Video counter base
     *  @details  A 10 bit data register with reset input that can be loaded
     *            with the value from vc.
     */
    uint16_t vcBase;
    
    /*! @brief    Row counter
     *  @details  A 3 bit counter with reset input.
     */
    uint8_t rc;
    
    /*! @brief    Video matrix (6)
     *  @details  Every 8th rasterline, the VIC chips performs a c-access and
     *            fills this array with character information.
     */
    uint8_t videoMatrix[40];
    
    /*! @brief    Color line (7)
     *  @details  Every 8th rasterline, the VIC chips performs a c-access and
     *            fills the array with color information.
     */
    uint8_t colorLine[40];
    
    /*! @brief    Video matrix line index
     *  @details  "Besides this, there is a 6 bit counter with reset input that
     *             keeps track of the position within the internal 40×12 bit
     *             video matrix/color line where read character pointers are
     *             stored resp. read again. I will call this 'VMLI' (video
     *             matrix line index) here.
     */
    uint8_t vmli;
    
 
    /*! @brief    Graphics data sequencer (10)
     *  @details  An 8 bit shift register to synthesize canvas pixels.
     */
    struct {
        
        //! @brief    Shift register data
        uint8_t data;
        
        /*! @brief    Indicates whether the shift register can load data
         *  @details  If true, the register is loaded when the current x scroll
         *            offset matches the current pixel number.
         */
        bool canLoad;
        
        /*! @brief    Multi-color synchronization flipflop
         *  @details  Whenever the shift register is loaded, the synchronization
         *            flipflop is also set. It is toggled with each pixel and
         *            used to synchronize the synthesis of multi-color pixels.
         */
        bool mcFlop;
        
        /*! @brief    Latched character info
         *  @details  Whenever the shift register is loaded, the current
         *            character value (which was once read during a gAccess) is
         *            latched. This value is used until the shift register loads
         *            again.
         */
        uint8_t latchedCharacter;
        
        /*! @brief    Latched color info
         *  @details  Whenever the shift register is loaded, the current color
         *            value (which was once read during a gAccess) is latched.
         *            This value is used until the shift register loads again.
         */
        uint8_t latchedColor;
        
        /*! @brief    Color bits
         *  @details  Every second pixel (as synchronized with mcFlop), the
         *            multi-color bits are remembered.
         */
        uint8_t colorbits;
        
        /*! @brief    Remaining bits to be pumped out
         *  @details  Makes sure no more than 8 pixels are outputted.
         */
        int remainingBits;
        
    } sr;
    

    /*! @brief    Sprite data sequencer (11)
     *  @details  The VIC chip has a 24 bit (3 byte) shift register for each
     *            sprite. It stores the sprite for one rasterline. If a sprite
     *            is a display candidate in the current rasterline, its shift
     *            register is activated when the raster X coordinate matches
     *            the sprites X coordinate. The comparison is done in method
     *            drawSprite(). Once a shift register is activated, it remains
     *            activated until the beginning of the next rasterline. However,
     *            after an activated shift register has dumped out its 24 pixels,
     *            it can't draw anything else than transparent pixels (which is
     *            the same as not to draw anything). An exception is during DMA
     *            cycles. When a shift register is activated during such a cycle,
     *            it freezes a short period of time in which it repeats the
     *            previous drawn pixel.
     */
    struct {
        
        //! @brief    Shift register data (24 bit)
        uint32_t data;
        
        //! @brief    The shift register data is read in three chunks
        uint8_t chunk1, chunk2, chunk3;
        
        /*! @brief    Remaining bits to be pumped out
         *  @details  At the beginning of each rasterline, this value is
         *            initialized with -1 and set to 26 when the horizontal
         *            trigger condition is met (sprite X trigger coord reaches
         *            xCounter). When all bits are drawn, this value reaches 0.
         */
        int remaining_bits;
        
        /*! @brief    Multi-color synchronization flipflop
         *  @details  Whenever the shift register is loaded, the synchronization
         *            flipflop is also set. It is toggled with each pixel and
         *            used to synchronize the synthesis of multi-color pixels.
         */
        bool mcFlop;
        
        //! @brief    x expansion synchronization flipflop
        bool expFlop;
        
        /*! @brief    Color bits of the currently processed pixel
         *  @details  In single-color mode, these bits are updated every cycle
         *            In multi-color mode, these bits are updated every second
         *            cycle (synchronized with mcFlop).
         */
        uint8_t colBits;
        
        //! @brief    Sprite color
        uint8_t spriteColor;
        
    } spriteSr[8];
    
    //! @brief    Sprite-sprite collision register (12)
    uint8_t  spriteSpriteCollision;
    
    //! @brief    Sprite-background collision register (12)
    uint8_t  spriteBackgroundColllision;
    
    
    //
    // Border flipflops
    //
    
    /*! @brief    Piped frame flipflops state (13)
     *  @details  When a flipflop toggles, the corresponding value
     *            in variable current is changed and a flag is set in variable
     *            delay. Function processDelayedActions() reads the flag and if
     *            set to true, updates the delayed values with the current ones.
     *  @see      processDelayedActions()
     */
    struct {
        FrameFlipflops current;
        FrameFlipflops delayed;
    } flipflops;
    
    /*! @brief    Vertical frame flipflop set condition
     *  @details  Indicates whether the vertical frame flipflop needs to be set
     *            in the current rasterline.
     */
    bool verticalFrameFFsetCond;
    
    /*! @brief    First coordinate where the main frame flipflop is checked.
     *  @details  Either 24 or 31, dependend on the CSEL bit.
     */
    uint16_t leftComparisonVal;
    
    /*! @brief    Second coordinate where the main frame flipflop is checked.
     *  @details  Either 344 or 335, dependend on the CSEL bit.
     */
    uint16_t rightComparisonVal;
    
    /*! @brief    First coordinate where the vertical frame flipflop is checked.
     *  @details  Either 51 or 55, dependend on the RSEL bit.
     */
    uint16_t upperComparisonVal;
    
    /*! @brief    Second coordinate where the vertical frame flipflop is checked.
     *  @details  Either 251 or 247, dependend on the RSEL bit.
     */
    uint16_t lowerComparisonVal;
    
    
    //
    // Housekeeping information
    //
    
    /*! @brief    Indicates wether we are in a visible display column or not
     *  @details  The visible columns comprise canvas columns and border
     *            columns. The first visible column is drawn in cycle 14 (first
     *            left border column) and the last in cycle ?? (fourth right
     *            border column).
     */
    bool visibleColumn;
    
    /*! @brief    Set to true in cycle 1, cycle 63 (65) iff yCounter matches D012
     *  @details  Variable is needed to determine if a rasterline should be
     *            issued in cycle 1 or 2.
     *  @deprecated Will be replaced by rasterlineMatchesIrqLine
     */
    bool yCounterEqualsIrqRasterline;
    
    //! @brief    True if the current rasterline belongs to the VBLANK area.
    bool vblank;
    
    //! @brief    Indicates if the current rasterline is a DMA line (bad line).
    bool badLine;
    
    /*! @brief    True, if DMA lines can occurr within the current frame.
     *  @details  Bad lines can occur only if the DEN bit was set during an
     *            arbitary cycle in rasterline 30. The DEN bit is located in
     *            control register 1 (0x11).
     */
    bool DENwasSetInRasterline30;
    
    /*! @brief    Display State
     *  @details  "The text/bitmap display logic in the VIC is in one of two
     *             states at any time: The idle state and the display state.
     *
     *              - In display state, c- and g-accesses take place, the
     *                addresses and interpretation of the data depend on the
     *                selected display mode.
     *
     *              - In idle state, only g-accesses occur. The VIC is either in
     *                idle or display state" [C.B.]
     */
    bool displayState;

  
	//
	// Sprites
	//

private:

	/*! @brief    MOB data counter.
	 *  @details  A 6 bit counter, one for each sprite.
     */
	uint8_t mc[8];
	
	/*! @brief    MCBASE register.
	 *  @details  A 6 bit counter, one register for each sprite.
     */
	uint8_t mcbase[8];
		
	/*! @brief    Sprite pointer fetched during a pAccess.
	 *  @details  Determines where the sprite data comes from.
     */
	uint16_t spritePtr[8];

    /*! @brief    Flags the first DMA access for each sprite.
     *  @details  Bit n corresponds to sprite n.
     */
    uint8_t isFirstDMAcycle;
    
    /*! @brief    Flags the second or third DMA access for each sprite.
     *  @details  Bit n corresponds to sprite n.
     */
    uint8_t isSecondDMAcycle;
    
	/*! @brief    Sprite on off register
	 *  @details  Determines if a sprite needs to be drawn in the current rasterline. 
     *            Each bit represents a single sprite.
     *  @deprecated Use TimeDelayed register instead
     */
    TimeDelayed<uint8_t> spriteOnOff = TimeDelayed<uint8_t>(3);
    
	/*! @brief    Sprite DMA on off register
	 *  @details  Determines  if sprite dma access is enabled or disabled. 
     *            Each bit represents a single sprite. 
     */
	uint8_t spriteDmaOnOff;
    
	/*! @brief    Expansion flipflop
	 *  @details  Used to handle Y sprite stretching, one bit for each sprite 
     */
	uint8_t expansionFF;

    /*! @brief    Remembers which bits the CPU has cleared in the expansion Y register (D017)
     *  @details  This value is set in pokeIO and cycle 15 and read in cycle 16 
     */
    uint8_t cleared_bits_in_d017;
    
    
	//
	// Lightpen
	//
	
    /*! @brief    Current value of the LP pin
     *  @details  A negative transition on this pin triggers a lightpen
     *            interrupt.
     */
    bool lpLine;
    
	/*! @brief    Indicates whether the lightpen has triggered
	 *  @details  This variable indicates whether a lightpen interrupt has
     *            occurred within the current frame. The variable is needed,
     *            because a lightpen interrupt can only occur once per frame.
     */
	bool lightpenIRQhasOccured;
	
    
    //
    // CPU control and memory access
    //
    
private:
    
    /*! @brief    Current value of the BA line
     *  @details  Remember: Each CPU cycle is split into two phases:
     *            First phase (LOW):   VIC gets access to the bus
     *            Second phase (HIGH): CPU gets access to the bus
     *            In rare cases, VIC needs access in the HIGH phase, too. To
     *            block the CPU, the BA line is pulled down.
     *  @note     BA can be pulled down by multiple sources (wired AND) and
     *            this variable indicates which sources are holding the line
     *            low.
     */
    TimeDelayed<uint16_t>baLine = TimeDelayed<uint16_t>(3);
    
    /*! @brief    Address bus
     *  @details  Whenever VIC performs a memory read, the generated memory
     *            address is stored in this variable.
     */
    uint16_t addrBus;
    
    /*! @brief    Data bus
     *  @details  Whenever VIC performs a memory read, the result is stored
     *            in this variable.
     */
    uint8_t dataBus;
    
    
    /*! @brief    Start address of the currently selected memory bank
     *  @details  There are four banks in total since the VIC chip can only
     *            'see' 16 KB of memory at the same time. Two bank select bits
     *            in the CIA I/O space determine which quarter of memory is
     *            currently seen.
     */
    /*
     *            +-------+------+-------+----------+-------------------------+
     *            | VALUE | BITS |  BANK | STARTING |  VIC-II CHIP RANGE      |
     *            |  OF A |      |       | LOCATION |                         |
     *            +-------+------+-------+----------+-------------------------+
     *            |   0   |  00  |   3   |   49152  | ($C000-$FFFF)           |
     *            |   1   |  01  |   2   |   32768  | ($8000-$BFFF)           |
     *            |   2   |  10  |   1   |   16384  | ($4000-$7FFF)           |
     *            |   3   |  11  |   0   |       0  | ($0000-$3FFF) (DEFAULT) |
     *            +-------+------+-------+----------+-------------------------+
     */
    uint16_t bankAddr;
    
    //! @brief    Result of the lastest g-access
    TimeDelayed<uint32_t>gAccessResult = TimeDelayed<uint32_t>(2);
    
    
    //
    // Color management (TODO: MOVE TO PIXEL ENGINE)
    //
    
    //! @brief    User adjustable brightness value used in palette computation
    /*! @details  Value may range from 0.0 to 100.0
     */
    double brightness = 50.0;

    //! @brief    User adjustable contrast value used in palette computation
    /*! @details  Value may range from 0.0 to 100.0
     */
    double contrast = 100.0;

    //! @brief    User adjustable saturation value used in palette computation
    /*! @details  Value may range from 0.0 to 100.0
     */
    double saturation = 50.0;

    
	//
	// Debugging and cheating
	//
	
public:
    
	/*! @brief    Determines whether sprites are drawn or not
	 *  @details  During normal emulation, the value is always false. For
     *            debugging purposes, the value can be set to true. In this
     *            case, sprites are no longer drawn.
	 */
	bool hideSprites;
	
	/*! @brief    Enables sprite-sprite collision
	 *  @details  If set to true, the virtual VIC chips checks for sprite-sprite
     *            collision as the original C64 does. For debugging purposes and
     *            cheating, collision detection can be disabled by setting the
     *            variabel to false. Collision detection can be enabled or
     *            disabled for each sprite seperately. Each bit is dedicated to
     *            a single sprite.
     */
	uint8_t spriteSpriteCollisionEnabled;
	
	/*! @brief    Enable sprite-background collision
	 *  @details  If set to true, the virtual VIC chips checks for sprite-
     *            background collision as the original C64 does. For debugging
     *            purposes and cheating, collision detection can be disabled by
     *            setting the variabel to false. Collision detection can be
     *            enabled or disabled for each sprite seperately. Each bit is
     *            dedicated to a single sprite.
     */
	uint8_t spriteBackgroundCollisionEnabled;
	
	/*! @brief    Determines whether IRQ lines will be made visible.
	 *  @details  Each rasterline that will potentially trigger a raster IRQ is
     *            highlighted. This feature is useful for debugging purposes as
     *            it visualizes how the screen is divided into multiple parts.
     */
	bool markIRQLines;
	
	/*! @brief    Determines whether DMA lines will be made visible.
	 *  @details  Each rasterline in which the vic will read additional data
     *            from the memory and stun the CPU is made visible. Note that
     *            partial DMA lines may not appear. 
     */
	bool markDMALines;

    
private:
    
    /*! @brief    Event pipeline
     *  @details  If a time delayed event needs to be performed, a flag is set
     *            inside this variable and executed at the beginning of the next
     *            cycle.
     *  @see      processDelayedActions()
     */
    uint64_t delay;
    
    
	//
	// Screen buffers and colors
	//
    
private:
    
    /*! @brief    Currently used RGBA values for all sixteen C64 colors
     *  @see      updatePalette()
     */
    uint32_t rgbaTable[16];
    
    /*! @brief    First screen buffer
     *  @details  The VIC chip writes its output into this buffer. The contents
     *            of the array is later copied into to texture RAM of your
     *            graphic card by the drawRect method in the GPU related code.
     */
    int *screenBuffer1 = new int[PAL_RASTERLINES * NTSC_PIXELS];
    
    /*! @brief    Second screen buffer
     *  @details  The VIC chip uses double buffering. Once a frame is drawn, the
     *            VIC chip writes the next frame to the second buffer.
     */
    int *screenBuffer2 = new int [PAL_RASTERLINES * NTSC_PIXELS];
    
    /*! @brief    Target screen buffer for all rendering methods
     *  @details  The variable points either to screenBuffer1 or screenBuffer2
     */
    int *currentScreenBuffer;
    
    /*! @brief    Pointer to the beginning of the current rasterline
     *  @details  This pointer is used by all rendering methods to write pixels.
     *            It always points to the beginning of a rasterline, either in
     *            screenBuffer1 or screenBuffer2. It is reset at the beginning
     *            of each frame and incremented at the beginning of each
     *            rasterline.
     */
    int *pixelBuffer;
    
    /*! @brief    Synthesized pixel colors
     *  @details  The colors for the eight pixels of a single VICII cycle are
     *            stored temporaraily in this array. At the end of the cycle,
     *            they are translated into RGBA color values and copied into
     *            the screen buffer.
     */
    uint8_t colBuffer[8];
    
    /*! @brief    Z buffer
     *  @details  Depth buffering is used to determine pixel priority. In the
     *            various render routines, a color value is only retained, if it
     *            is closer to the view point. The depth of the closest pixel is
     *            kept in the z buffer. The lower the value, the closer it is to
     *            the viewer.
     */
    uint8_t zBuffer[8];
    
    /*! @brief    Indicates the source of a drawn pixel
     *  @details  Whenever a foreground pixel or sprite pixel is drawn, a
     *            distinct bit in the pixelSource array is set. The information
     *            is needed to detect sprite-sprite and sprite-background
     *            collisions.
     */
    uint8_t pixelSource[8];
    
    /*! @brief    Offset into pixelBuffer
     *  @details  Variable points to the first pixel of the currently drawn 8
     *            pixel chunk.
     */
    short bufferoffset;
    
    /*! @brief    This is where loadColors() stores all retrieved colors
     *  @details  [0] : color for '0'  pixels in single color mode
     *                         or '00' pixels in multicolor mode
     *            [1] : color for '1'  pixels in single color mode
     *                         or '01' pixels in multicolor mode
     *            [2] : color for '10' pixels in multicolor mode
     *            [3] : color for '11' pixels in multicolor mode
     */
    uint8_t col[4];
    
    //! @brief    Sprite colors
    uint64_t sprExtraCol1;
    uint64_t sprExtraCol2;
    uint64_t sprCol[8];

    
public:
	
	//! @brief    Constructor
	VIC();
	
	//! @brief    Destructor
	~VIC();
	
	//! @brief    Methods from VirtualComponent
	void reset();
    void setC64(C64 *c64);
    void ping();
	void dumpState();
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    

    //
    //! @functiongroup Accessing chip model related properties
    //
    
    //! @brief    Returns the currently plugged in chip model.
    VICChipModel getChipModel() { return chipModel; }
    
    //! @brief    Sets the chip model.
    void setChipModel(VICChipModel model);

    //! @brief    Returns the emulated glue logic type.
    GlueLogic getGlueLogic() { return glueLogic; }

    //! @brief    Sets the glue logic type.
    void setGlueLogic(GlueLogic type);

    //! @brief    Returns true if a PAL chip is plugged in.
    bool isPAL() { return chipModel & (PAL_6569_R1 | PAL_6569_R3 | PAL_8565); }
    
    //! @brief    Returns true if a NTSC chip is plugged in.
    bool isNTSC() { return chipModel & (NTSC_6567 | NTSC_6567_R56A | NTSC_8562); }

    //! @brief    Returns true if a newer MOS 856x chip is plugged in.
    bool is856x() { return chipModel & (PAL_8565 | NTSC_8562); }
    
    //! @brief    Returns true if an older MOS 656x chip is plugged in.
    bool is656x() { return chipModel & ~(PAL_8565 | NTSC_8562); }

    //! @brief    Returns true if the emulated chip model has the gray dot bug.
    bool hasGrayDotBug() { return is856x(); }

    //! @brief    Returns true if light pen interrupts are triggered with a delay.
    bool delayedLightPenIrqs() { return chipModel & (PAL_6569_R1 | NTSC_6567_R56A); }

    //! @brief    Returns the clock frequencay of the selected VICII model.
    unsigned getClockFrequency();
    
    //! @brief    Returns the number of CPU cycles performed per rasterline.
    unsigned getCyclesPerRasterline();
    
    //! @brief    Returns true if the end of the rasterline has been reached.
    bool isLastCycleInRasterline(unsigned cycle);
    
    //! @brief    Returns the number of rasterlines drawn per frame.
    unsigned getRasterlinesPerFrame();
    
    //! @brief    Returns true if rasterline belongs to the VBLANK area.
    bool isVBlankLine(unsigned rasterline);
    
    //! @brief    Returns the number of CPU cycles executed in one frame.
    unsigned getCyclesPerFrame() {
        return getRasterlinesPerFrame() * getCyclesPerRasterline(); }
    
    /*! @brief    Returns the number of frames drawn per second.
     *  @note     The result is returned as a floating point value, because
     *            Commodore did not manage to match the expected values exactly
     *            (50 Hz for PAL and 60 Hz for NTSC). E.g., a PAL C64 outputs
     *            50.125 Hz.
     */
    double getFramesPerSecond() {
        return (double)getClockFrequency() / (double)getCyclesPerFrame();
    }
    
    //! @brief    Returns the time interval between two frames in nanoseconds.
    uint64_t getFrameDelay() {
        return 1000000000.0 / (uint64_t)getFramesPerSecond();
    }
    
    
    //
    //! @functiongroup Accessing the screen buffer and display properties
    //
    
    //! @brief    Returns the currently stabel screen buffer.
    void *screenBuffer();

    //! @brief    Initializes both screenBuffers
    /*! @details  This function is needed for debugging, only. It write some
     *            recognizable pattern into both buffers.
     */
    void resetScreenBuffers();

    /*! @brief    Returns one of the sixteen C64 colors in RGBA format.
     *  @seealso  updateColors
     */
    uint32_t getColor(unsigned nr);
    
    //! @brief    Returns the brightness monitor parameter
    double getBrightness() { return brightness; }
    
    //! @brief    Sets the brightness monitor parameter
    void setBrightness(double value);
    
    //! @brief    Returns the contrast monitor parameter
    double getContrast() { return contrast; }
    
    //! @brief    Sets the contrast monitor parameter
    void setContrast(double value);
    
    //! @brief    Returns the saturation monitor parameter
    double getSaturation() { return saturation; }
    
    //! @brief    Sets the saturation monitor parameter
    void setSaturation(double value);
    
private:
    
    /*! @brief    Updates the RGBA values for all sixteen C64 colors.
     *! @details  The base palette is determined by the selected VICII model.
     */
    void updatePalette();
    
    
    //
    //! @functiongroup Accessing memory (VIC_memory.cpp)
    //

public:
    
    //! @brief    Peeks a value from a VIC register without side effects.
    uint8_t spypeek(uint16_t addr);
    
    //! @brief    Returns the current value of the VICII's data bus.
    uint8_t getDataBus() { return dataBus; }
    
    //! @brief    Updates the bank address in the next cycle.
    void updateBankAddr();
    
private:

    //! @brief    Peeks a value from a VIC register.
	uint8_t peek(uint16_t addr);
    
    //! @brief    Pokes a value into a VIC register.
	void poke(uint16_t addr, uint8_t value);
    
    //! @brief    Simulates a memory access via the address and data bus.
    uint8_t memAccess(uint16_t addr);
    
    //! @brief    Simulates an idle memory access.
    uint8_t memIdleAccess();
    
    //! @brief    Performs a DRAM refresh
    void rAccess() { (void)memAccess(0x3F00 | refreshCounter--); }
    
    //! @brief    Performs a DRAM idle access
    void rIdleAccess() { (void)memIdleAccess(); }
    
    /*! @brief    Performs a character access (c-access).
     *  @details  During a c-access, the video matrix is read.
     */
    void cAccess();
    
    /*! @brief    Performs a graphics access (g-access).
     *  @details  During a g-access, graphics data (character or bitmap patterns)
     *            is reads. The result of the g-access is stored in variables
     *            prefixed with 'g_', i.e., g_data, g_character, g_color, and
     *            g_mode.
     */
    void gAccess();
    
    //! @brief    Performs a sprite pointer access (p-access).
    void pAccess(unsigned sprite);
    
    //! @brief    Performs the first sprite data access.
    void sFirstAccess(unsigned sprite);
    
    //! @brief    Performs the second sprite data access.
    void sSecondAccess(unsigned sprite);
    
    //! @brief    Performs the third sprite data access.
    void sThirdAccess(unsigned sprite);
    
    /*! @brief    Finalizes the sprite data access
     *  @details  This method is invoked one cycle after the second and third
     *            sprite DMA has occured.
     */
    void sFinalize(unsigned sprite);
    

    //
    //! @functiongroup Handling the x and y counters
    //
    
    /*! @brief    Returns the current rasterline
     *  @note     This value is not always identical to the yCounter, because
     *            the yCounter is incremented with a little delay.
     */
    uint16_t rasterline();
        
    /*! @brief    Indicates if yCounter needs to be reset in this rasterline.
     *  @details  PAL models reset the yCounter in cycle 2 in the first
     *            rasterline wheras NTSC models reset the yCounter in cycle 2
     *            in the middle of the lower border area.
     */
    bool yCounterOverflow() { return rasterline() == (isPAL() ? 0 : 238); }

    //
    //! @functiongroup Handling the border flip flops
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
    
    /*! @brief    Takes care of the vertical frame flipflop value.
     *  @details  Invoked in each VIC II cycle
     */
    void checkVerticalFrameFF();
    
    //! @brief    Checks frame fliplops at left border
    void checkFrameFlipflopsLeft(uint16_t comparisonValue);
    
    //! @brief    Checks frame fliplops at right border
    void checkFrameFlipflopsRight(uint16_t comparisonValue);
    
    //! @brief    Sets the vertical frame flipflop with a delay of one cycle.
    void setVerticalFrameFF(bool value);
    
    //! @brief    Sets the main frame flipflop with a delay of one cycle.
    void setMainFrameFF(bool value);
    
    //! @brief    Returns where the frame flipflop is checked for the left border.
    uint16_t leftComparisonValue() { return isCSEL() ? 24 : 31; }
    
    //! @brief    Returns where the frame flipflop is checked for the right border.
    uint16_t rightComparisonValue() { return isCSEL() ? 344 : 335; }
    
    //! @brief    Returns where the frame flipflop is checked for the upper border.
    uint16_t upperComparisonValue() { return isRSEL() ? 51 : 55; }
    
    //! @brief    Returns where the frame flipflop is checked for the lower border.
    uint16_t lowerComparisonValue() { return isRSEL() ? 251 : 247; }
    
  
    
    
	//
	//! @functiongroup Querying the VICII registers
	//
	
public:
		
    /*! @brief    Returns the current value of the DEN (Display ENabled) bit.
     */
    bool DENbit() { return GET_BIT(reg.current.ctrl1, 4); }

    /*! @brief    Returns the value of the BMM (Bit Map Mode) bit.
     *  @details  The value is returned as it is seen during a g-access.
     */
    bool BMMbit() {
        return is856x() ?
        GET_BIT(reg.delayed.ctrl1, 5) :
        GET_BIT(reg.delayed.ctrl1, 5) | GET_BIT(reg.current.ctrl1, 5);
    }
    
    //! @brief    Returns the number of the next interrupt rasterline.
    uint16_t rasterInterruptLine() {
        return ((reg.current.ctrl1 & 0x80) << 1) | rasterIrqLine;
    }
    
    //! @brief    Returns the current value of the ECM bit.
    bool ECMbit() {
        return is856x() ?
        GET_BIT(reg.delayed.ctrl1, 6) :
        GET_BIT(reg.current.ctrl1, 6);
    }

    //! @brief    Returns the masked CB13 bit.
    uint8_t CB13() { return memSelect & 0x08; }

    //! @brief    Returns the masked CB13/CB12/CB11 bits.
    uint8_t CB13CB12CB11() { return memSelect & 0x0E; }

    //! @brief    Returns the masked VM13/VM12/VM11/VM10 bits.
    uint8_t VM13VM12VM11VM10() { return memSelect & 0xF0; }

	//! @brief    Returns the state of the CSEL bit.
	bool isCSEL() { return GET_BIT(reg.current.ctrl2, 3); }
    
	//! @brief    Returns the state of the RSEL bit.
    bool isRSEL() { return GET_BIT(reg.current.ctrl1, 3); }


    //
    //! @functiongroup Handling DMA lines and the display state
    //

private:
    
    //! @brief    Returns true if the bad line condition holds.
    bool badLineCondition();
    
    
    //
    //! @functiongroup Interacting with the C64's CPU
    //
    
private:
    
    /*! @brief   Sets the value of the BA line
     * @details  The BA line is connected to the CPU's RDY pin.
     */
    void updateBA(uint8_t value);
    
    /*! @brief    Indicates if a c-access can occur.
     *  @details  A c-access can only be performed if the BA line is down for
     *            more than 2 cycles.
     */
    bool BApulledDownForAtLeastThreeCycles() { return baLine.delayed(); }
    
	/*! @brief    Triggers a VIC interrupt
     *  @param    source is the interrupt source
     *            1 : Rasterline interrupt
     *            2 : Collision of a sprite with background pixels
     *            4 : Collision between two sprites.
     *            8 : Lightpen interrupt
     */
	void triggerIrq(uint8_t source);
	
    
    //
    //! @functiongroup Handling lightpen events
    //
    
public:
    
    /*! @brief    Sets the value of the LP pin
     *  @details  The LP pin is connected to bit 4 of control port A.
     *  @seealso  checkForLightpenIrq()
     */
    void setLP(bool value);
    
private:

    //! @brief    Returns the X coordinate of a light pen event.
    /*! @details  The coordinate depends on the current rasterline cycle and
     *            differes slightly between the supported VICII chip models.
     */
    uint16_t lightpenX();
    
    //! @brief    Returns the Y coordinate of a light pen event.
    uint16_t lightpenY();
    
    /*! @brief    Trigger lightpen interrupt if conditions are met.
     *  @details  This function is called on each negative transition of the
     *            LP pin. It latches the x and y coordinates and immediately
     *            triggers an interrupt if a newer VICII model is emulated.
     *            Older models trigger the interrupt later, at the beginning of
     *            a new frame.
     *  @seealso  checkForLightpenIrqAtStartOfFrame()
     */
    void checkForLightpenIrq();

    /*! @brief    Retriggers a lightpen interrupt if conditions are met.
     *  @details  This function is called at the beginning of each frame.
     *            If the lp line is still low at this point of time, a lightpen
     *            interrupt is retriggered. Note that older VICII models trigger
     *            interrupts only at this point in time.
     */
    void checkForLightpenIrqAtStartOfFrame();
    
    
	//
	// Sprites
	//

private:

    /*! @brief    Gets the depth of a sprite.
     *  @return   depth value that can be written into the z buffer.
     */
    uint8_t spriteDepth(uint8_t nr);
    
    /*! @brief    Compares the Y coordinates of all sprites with the yCounter
     *  @return   A bit pattern storing the result for each sprite.
     */
    uint8_t compareSpriteY();
    
    /*! @brief    Turns off sprite dma if conditions are met.
     *  @details  In cycle 16, the mcbase pointer is advanced three bytes for
     *            all dma enabled sprites. Advancing three bytes means that
     *            mcbase will then point to the next sprite line. When mcbase
     *            reached 63, all 21 sprite lines have been drawn and sprite dma
     *            is switched off. The whole operation is skipped when the y
     *            expansion flipflop is 0. This never happens for normal sprites
     *            (there is no skipping then), but happens every other cycle for
     *            vertically expanded sprites. Thus, mcbase advances for those
     *            sprites at half speed which actually causes the expansion.
     */
    void turnSpriteDmaOff();

    /*! @brief    Turns on sprite dma accesses if conditions are met.
     *  @details  This function is called in cycle 55 and cycle 56.
     */
    void turnSpriteDmaOn();

    /*! @brief    Turns sprite display on or off.
     *  @details  This function is called in cycle 58.
     */
    void turnSpritesOnOrOff();
    
    /*! @brief    Loads the sprite shift register.
     *  @details  The shift register is loaded with the three data bytes fetched
     *            in the previous sAccesses.
     */
    void loadShiftRegister(unsigned nr) {
        spriteSr[nr].data = LO_LO_HI(spriteSr[nr].chunk3,
                                      spriteSr[nr].chunk2,
                                      spriteSr[nr].chunk1);
    }
    
    /*! @brief    Toggles expansion flipflop for vertically stretched sprites.
     *  @details  In cycle 56, register D017 is read and the flipflop gets
     *            inverted for all sprites with vertical stretching enabled.
     *            When the flipflop goes down, advanceMCBase() will have no
     *            effect in the next rasterline. This causes each sprite line
     *            to be drawn twice.
     */
    void toggleExpansionFlipflop() { expansionFF ^= reg.current.sprExpandY; }
    
    
	//
    //!  @functiongroup Running the device (VIC.cpp and VIC_cycles_xxx.cpp)
	//

public:
	
	/*! @brief    Prepares VICII for drawing a new frame.
	 *  @details  This function is called prior to the first cycle of each frame.
     */
	void beginFrame();
	
	/*! @brief    Prepares VICII for drawing a new rasterline.
	 *  @details  This function is called prior to the first cycle of each rasterline.
     */
	void beginRasterline(uint16_t rasterline);

	/*! @brief    Finishes up a rasterline.
	 *  @details  This function is called after the last cycle of each rasterline.
     */
	void endRasterline();
	
	/*! @brief    Finishes up a frame.
	 *  @details  This function is called after the last cycle of each frame.
     */
	void endFrame();
    
    //! @brief    Processes all time delayed actions.
    /*! @details  This function is called at the beginning of each VIC cycle.
     */
    void processDelayedActions();
    
	/*! @brief    Executes a specific rasterline cycle
     *  @note     The cycle specific actions differ depending on the selected
     *            chip model.
     */
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
	
    #define DRAW if (!vblank) { draw(); }
    #define DRAW17 if (!vblank) { draw17(); }
    #define DRAW55 if (!vblank) { draw55(); }

    #define C_ACCESS if (badLine) cAccess();
    
    #define END_CYCLE \
    xCounter += 8; \
    if (unlikely(delay != 0)) { processDelayedActions(); }
    
    // #define BA_LINE(x) if ((x) != baLine.current()) { updateBA(x); }
    #define BA_LINE(x) updateBA(x);
    
    //
    // Drawing routines (VIC_draw.cpp)
    // 
    
private:
    
    /*! @brief    Synthesize 8 pixels according the the current drawing context.
     *  @details  This is the main entry point and is invoked in each VIC
     *            drawing cycle, except cycle 17 and cycle 55 which are handles
     *            seperately for speedup purposes. To get the correct output,
     *            preparePixelEngine() must be called one cycle before.
     */
    void draw();
    
    //! @brief    Special draw routine for cycle 17
    void draw17();
    
    //! @brief    Special draw routine for cycle 55
    void draw55();
    
    /*! @brief    Draw routine for cycles outside the visible screen region.
     *  @details  The sprite sequencer needs to be run outside the visible area,
     *            although no pixels will be drawn (drawing is omitted by having
     *            visibleColumn set to false.
     */
    void drawOutsideBorder();
    
    
    //
    // Internal drawing routines (called by the external ones)
    //
    
    /*! @brief    Draws a part of the border
     *  @details  Invoked inside draw()
     */
    void drawBorder();
    
    /*! @brief    Draws a part of the border
     *  @details  Invoked inside draw17()
     */
    void drawBorder17();
    
    /*! @brief    Draws a part of the border
     *  @details  Invoked inside draw55()
     */
    void drawBorder55();
    
    /*! @brief    Draws 8 canvas pixels
     *  @details  Invoked inside draw()
     */
    void drawCanvas();
    
    /*! @brief    Draws a single canvas pixel
     *  @param    pixel is the pixel number and must be in the range 0 to 7
     *  @param    loadShiftReg is set to true if the shift register needs to be
     *            reloaded
     *  @param    updateColors is set to true if the four selectable colors
     *            might have changed.
     */
    void drawCanvasPixel(uint8_t pixel,
                         uint8_t mode,
                         uint8_t d016,
                         bool loadShiftReg,
                         bool updateColors);
    
    /*! @brief    Draws 8 sprite pixels
     *  @details  Invoked inside draw()
     */
    void drawSprites();
    
    /*! @brief    Draws a single sprite pixel for a single sprite
     *  @param    spritenr Sprite number (0 to 7)
     *  @param    pixel    Pixel number (0 to 7)
     *  @param    freeze   If set to true, the sprites shift register will
     *                     freeze temporarily
     *  @param    halt     If set to true, the sprites shift shift register will
     *                     be deactivated
     *  @param    load     If set to true, the sprites shift shift register will
     *                     grab new data bits
     */
    void drawSpritePixel(unsigned spritenr,
                         unsigned pixel,
                         bool freeze,
                         bool halt,
                         bool load);
    
    /*! @brief    Draws all sprites into the pixelbuffer
     *  @details  A sprite is only drawn if it's enabled and if sprite drawing
     *            is not switched off for debugging
     */
    void drawAllSprites();
    
    /*! @brief    Draw single sprite into pixel buffer
     *  @details  Helper function for drawSprites
     */
    void drawSprite(uint8_t nr);
    
    
    //
    // Mid level drawing (semantic pixel rendering)
    //
    
    //! @brief    Determines pixel colors accordig to the provided display mode
    void loadColors(uint8_t mode);
    
    /*! @brief    Draws single sprite pixel in single-color mode
     *  @details  Uses the drawing colors that are setup by updateSpriteColors
     */
    void setSingleColorSpritePixel(unsigned spritenr, unsigned pixel, uint8_t bit);
    
    /*! @brief    Draws single sprite pixel in multi-color mode
     *  @details  Uses the drawing colors that are setup by updateSpriteColors
     */
    void setMultiColorSpritePixel(unsigned spritenr, unsigned pixel, uint8_t two_bits);
    
    /*! @brief    Draws a single sprite pixel
     *  @details  This function is invoked by setSingleColorSpritePixel() and
     *            setMultiColorSpritePixel(). It takes care of collison and invokes
     *            setSpritePixel(4) to actually render the pixel.
     */
    void drawSpritePixel(unsigned pixel, uint8_t color, int nr);
    
    
    //
    // Low level drawing (pixel buffer access)
    //
    
    //! @brief    Writes a single color value into the screenbuffer
    #ifdef WRITE_THROUGH
        #define COLORIZE(pixel,color) \
            assert(bufferoffset + pixel < NTSC_PIXELS); \
            pixelBuffer[bufferoffset + pixel] = rgbaTable[color];
    #else
        #define COLORIZE(pixel,color) \
            colBuffer[pixel] = color;
    #endif
    
    /*! @brief    Sets a single frame pixel
     *! @note     The upper bit in pixelSource is cleared to prevent
     *            sprite/foreground collision detection in border area.
     */
    #define SET_FRAME_PIXEL(pixel,color) { \
        COLORIZE(pixel,color) \
        zBuffer[pixel] = BORDER_LAYER_DEPTH; \
        pixelSource[pixel] &= (~0x80); }
    
    //! @brief    Sets a single foreground pixel
    #define SET_FOREGROUND_PIXEL(pixel,color) { \
        COLORIZE(pixel,color) \
        zBuffer[pixel] = FOREGROUND_LAYER_DEPTH; \
        pixelSource[pixel] = 0x80; }

    //! @brief    Sets a single background pixel
    #define SET_BACKGROUND_PIXEL(pixel,color) { \
        COLORIZE(pixel,color) \
        zBuffer[pixel] = BACKGROUD_LAYER_DEPTH; \
        pixelSource[pixel] = 0x00; }
    
    //! @brief    Draw a single sprite pixel
    void setSpritePixel(unsigned pixel, uint8_t color, int depth, int source);
    
    /*! @brief    Copies eight synthesized pixels into to the pixel buffer.
     *  @details  Each pixel is first translated to the corresponding RGBA value
     *            and then copied over.
     */
    void copyPixels();
    
    /*! @brief    Extend border to the left and right to look nice.
     *  @details  This functions replicates the color of the leftmost and
     *            rightmost pixel
     */
    void expandBorders();
    
    /*! @brief    Draw a horizontal colored line into the screen buffer
     *  @details  This method is utilized for debugging purposes, only.
     */
    void markLine(uint8_t color, unsigned start = 0, unsigned end = NTSC_PIXELS);

    
	//
	// The following functions are used by the GUI debugger, only
	//

public: 

    //
    //! @functiongroup Querying information (VIC_debug.cpp)
    //

    //! @brief    Gathers debug information.
    VICInfo getInfo();
    
    //! @brief    Gathers debug information about a certain sprite.
    SpriteInfo getSpriteInfo(unsigned i);

    
    //
    //! @functiongroup Thread-safe manipulation of the VICII state (VIC_debug.cpp)
    //

    //! @brief    Sets the memory bank start address
    void setMemoryBankAddr(uint16_t addr);
    
    //! @brief    Sets the screen memory address.
    void setScreenMemoryAddr(uint16_t addr);
    
    //! @brief    Sets the character memory address.
    void setCharacterMemoryAddr(uint16_t addr);
    
    //! @brief    Sets the display mode.
    void setDisplayMode(DisplayMode m);
    
    //! @brief    Sets the number of canvas rows.
    void setNumberOfRows(unsigned rs);
    
    //! @brief    Sets the number of canvas columns.
    void setNumberOfColumns(unsigned cs);
    
    //! @brief    Returns the current screen geometry.
    ScreenGeometry getScreenGeometry(void);
    
    //! @brief    Sets the current screen geometry.
    void setScreenGeometry(ScreenGeometry mode);
    
    //! @brief    Sets the vertical raster scroll offset.
    void setVerticalRasterScroll(uint8_t offset);
    
    //! @brief    Sets the horizontan raster scroll offset.
    void setHorizontalRasterScroll(uint8_t offset);
    
    //! @brief    Set interrupt rasterline
    void setRasterInterruptLine(uint16_t line);
    
    //! @brief    Enable or disable rasterline interrupts
    void setRasterInterruptEnable(bool b);
    
    //! @brief    Enable or disable rasterline interrupts
    void toggleRasterInterruptFlag();
    
    
    //! @brief    Sets the color of a sprite.
    void setSpriteColor(unsigned nr, uint8_t color);
    
    //! @brief    Set the X coordinate of a sprite.
    void setSpriteX(unsigned nr, uint16_t x);
    
    //! @brief    Sets the Y coordinate of sprite.
    void setSpriteY(unsigned nr, uint8_t y);
    
    //! @brief    Enables or disables a sprite.
    void setSpriteEnabled(uint8_t nr, bool b);
    
    //! @brief    Enables or disables a sprite.
    void toggleSpriteEnabled(uint8_t nr);
    
    //! @brief    Enables or disables IRQs on sprite/background collision
    void setIrqOnSpriteBackgroundCollision(bool b);
    
    //! @brief    Enables or disables IRQs on sprite/background collision
    void toggleIrqOnSpriteBackgroundCollision();
    
    //! @brief    Enables or disables IRQs on sprite/sprite collision
    void setIrqOnSpriteSpriteCollision(bool b);
    
    //! @brief    Enables or disables IRQs on sprite/sprite collision
    void toggleIrqOnSpriteSpriteCollision();
    
    //! @brief    Determines whether a sprite is drawn before or behind the scenary.
    void setSpritePriority(unsigned nr, bool b);
    
    //! @brief    Determines whether a sprite is drawn before or behind the scenary.
    void toggleSpritePriority(unsigned nr);
    
    //! @brief    Sets single color or multi color mode for sprite.
    void setSpriteMulticolor(unsigned nr, bool b);
    
    //! @brief    Switches between single color or multi color mode.
    void toggleMulticolorFlag(unsigned nr);
    
    //! @brief    Stretches or shrinks a sprite vertically.
    void setSpriteStretchY(unsigned nr, bool b);
    
    //! @brief    Stretches or shrinks a sprite vertically.
    void spriteToggleStretchYFlag(unsigned nr);
    
    //! @brief    Stretches or shrinks sprite horizontally.
    void setSpriteStretchX(unsigned nr, bool b);
    
    //! @brief    Stretches or shrinks sprite horizontally.
    void spriteToggleStretchXFlag(unsigned nr);

    
    //
    //! @functiongroup Debugging and cheating (VIC_debug.cpp)
    //

	//! @brief    Shows or hides IRQ lines.
    void setShowIrqLines(bool show);
	
	//! @brief    Shows or hides DMA lines.
    void setShowDmaLines(bool show);

	//! @brief    Hides or shows sprites.
    void setHideSprites(bool hide);
	
	//! @brief    Enables or disables sprite-sprite collision detection.
    void setSpriteSpriteCollisionFlag(bool b);

	//! @brief    Toggles sprite-sprite collision detection.
    void toggleSpriteSpriteCollisionFlag();
	
	//! @brief    Enables or disable sprite-background collision detection.
    void setSpriteBackgroundCollisionFlag(bool b);

	//! @brief    Toggles sprite-background collision detection.
    void toggleSpriteBackgroundCollisionFlag();
};

#endif

