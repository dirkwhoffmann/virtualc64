/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

// TODO:
// Implement VBlanking
// Implement proper background drawing

#ifndef _VIC_INC
#define _VIC_INC

#include "VirtualComponent.h"

// Forward declarations
class C64;
class C64Memory;

#define EXTRACT_RED(x)   ((x & 0xff000000) >> 24)
#define EXTRACT_GREEN(x) ((x & 0x00ff0000) >> 16)
#define EXTRACT_BLUE(x)  ((x & 0x0000ff00) >> 8)
#define EXTRACT_ALPHA(x) (x & 0x000000ff)


//! The virtual Video Controller
/*! VIC is the video controller chip of the Commodore 64.
	The VIC chip occupied the memory mapped I/O space from address 0xD000 to 0xD02E.
*/
class VIC : public VirtualComponent {

	// -----------------------------------------------------------------------------------------------
	//                                     Constant definitions
	// -----------------------------------------------------------------------------------------------
	
public:
    //! Predefined color schemes
	enum ColorScheme {
		CCS64           = 0x00,
		VICE            = 0x01,
		FRODO           = 0x02,
		PC64            = 0x03,
		C64S            = 0x04,
		ALEC64          = 0x05,
		WIN64           = 0x06,
		C64ALIVE_0_9    = 0x07,
		GODOT           = 0x08,
		C64SALLY        = 0x09,
		PEPTO           = 0x0A,
		GRAYSCALE       = 0x0B
	};
	
	//! Display mode
	enum DisplayMode {
		STANDARD_TEXT             = 0x00,
		MULTICOLOR_TEXT           = 0x10,
		STANDARD_BITMAP           = 0x20,
		MULTICOLOR_BITMAP         = 0x30,
		EXTENDED_BACKGROUND_COLOR = 0x40,
        INVALID_TEXT              = 0x50,
        INVALID_STANDARD_BITMAP   = 0x60,
        INVALID_MULTICOLOR_BITMAP = 0x70
	};

	//! Screen geometry
	enum ScreenGeometry {
		COL_40_ROW_25 = 0x01,
		COL_38_ROW_25 = 0x02,
		COL_40_ROW_24 = 0x03,
		COL_38_ROW_24 = 0x04
	};

    //! VIC colors
	enum Color {
		BLACK   = 0x00,
		WHITE   = 0x01,
		RED     = 0x02,
		CYAN    = 0x03,
		PURPLE  = 0x04,
		GREEN   = 0x05,
		BLUE    = 0x06,
		YELLOW  = 0x07,
		LTBROWN = 0x08,
		BROWN   = 0x09,
		LTRED   = 0x0A,
		GREY1   = 0x0B,
		GREY2   = 0x0C,
		LTGREEN = 0x0D,
		LTBLUE  = 0x0E,
		GREY3   = 0x0F
	};
		
	//! Start address of the VIC I/O space
	static const uint16_t VIC_START_ADDR = 0xD000;
	
	//! End address of the VIC I/O space
	static const uint16_t VIC_END_ADDR = 0xD3FF;
	
	//! Widht of inner screen area in pixels
	static const uint16_t SCREEN_WIDTH = 320;
	
	//! Height of inner screen area in pixels
	static const uint16_t SCREEN_HEIGHT = 200;
	
	//! First column coordinate of inner screen area
	static const uint16_t FIRST_X_COORD_OF_INNER_AREA = 24;  
	
	//! First row coordinate of inner screen area
	static const uint16_t FIRST_Y_COORD_OF_INNER_AREA = 51;
	
	
	// NTSC machines

	//! Refresh rate in Hz
	static const uint16_t NTSC_REFRESH_RATE = 60;

	//! CPU cycles per rasterline
	static const uint16_t NTSC_CYCLES_PER_RASTERLINE = 65;

	//! Width of left border
	static const uint16_t NTSC_LEFT_BORDER_WIDTH = 49; // ??? Needs verification
	
	//! Width of right border
	static const uint16_t NTSC_RIGHT_BORDER_WIDTH = 49; // ??? Needs verification

	//! Number of viewable pixels in one rasterline
	static const uint16_t NTSC_VIEWABLE_PIXELS = 418;

	//! Number of invisible lines above upper border (first visible line is ???) // Needs verification
	static const uint16_t NTSC_UPPER_INVISIBLE = 28;  

	//! Upper border height 
	static const uint16_t NTSC_UPPER_BORDER_HEIGHT = 23; // Needs verification
	
	//! Lower border height
	static const uint16_t NTSC_LOWER_BORDER_HEIGHT = 12; // Needs verification

	//! Number of invisible lines below lower border (last visible line is ???) // Needs verification
	static const uint16_t NTSC_LOWER_INVISIBLE = 0;  
	
	//! Number of viewable rasterlines
	static const uint16_t NTSC_VIEWABLE_RASTERLINES = 235;

	//! Total number of rasterlines, including invisible areas
	static const uint16_t NTSC_RASTERLINES = 263;
	
	
	// PAL machines
	
	//! Refresh rate in Hz
	static const uint16_t PAL_REFRESH_RATE = 50;
	
	//! CPU cycles per rasterline
	static const uint16_t PAL_CYCLES_PER_RASTERLINE = 63;
		
	//! Width of left border
	static const uint16_t PAL_LEFT_BORDER_WIDTH = 46;

	//! Width of right border
	static const uint16_t PAL_RIGHT_BORDER_WIDTH = 36;
	
	//! Number of viewable pixels in one rasterline
	static const uint16_t PAL_VIEWABLE_PIXELS = 402;
	
	//! Number of invisible lines above upper border (first visible line is 0x008)
	static const uint16_t PAL_UPPER_INVISIBLE = 8;  

	//! Upper border height 
	static const uint16_t PAL_UPPER_BORDER_HEIGHT = 43;
	
	//! Lower border height
	static const uint16_t PAL_LOWER_BORDER_HEIGHT = 49;
	
	//! Number of invisible lines below lower border (last visible line is 0x12B)
	static const uint16_t PAL_LOWER_INVISIBLE = 12;  

	//! Number of viewable rasterlines 
	static const uint16_t PAL_VIEWABLE_RASTERLINES = 292;

	//! Total number of rasterlines, including invisible areas
	static const uint16_t PAL_RASTERLINES = 312;

	//! Maximum number of viewable rasterlines
	static const uint16_t MAX_VIEWABLE_RASTERLINES = PAL_VIEWABLE_RASTERLINES;

	//! Maximum number of viewable pixels per rasterline
	static const uint16_t MAX_VIEWABLE_PIXELS = NTSC_VIEWABLE_PIXELS;
	
		
	// -----------------------------------------------------------------------------------------------
	//                                      Member variables
	// -----------------------------------------------------------------------------------------------
	
private:
	
	//! Reference to the connected virtual C64
	C64 *c64;
	
	//! Reference to the connected CPU. 
	CPU *cpu;
	
	//! Reference to the connected virtual memory
	C64Memory *mem;
	
	
	// -----------------------------------------------------------------------------------------------
	//                                     Internal registers
	// -----------------------------------------------------------------------------------------------
	
	//! Number of the next screen line to be drawn
	/*! Right now, drawing to the border is not supported. Therefore, the value is always in the range
	 from 0 to SCREEN_HEIGHT-1. */
	uint32_t scanline;
	
	//! Internal x counter of the sequencer
	uint16_t xCounter;
	
	//! Internal VIC register, 10 bit video counter
	uint16_t registerVC;
	
	//! Internal VIC-II register, 10 bit video counter base
	uint16_t registerVCBASE; 
	
	//! Internal VIC-II register, 3 bit row counter
	uint8_t registerRC;
	
	//! Internal VIC-II register, 6 bit video matrix line index
	uint8_t registerVMLI; 
		
    //! DRAM refresh counter
    /*! "In jeder Rasterzeile f�hrt der VIC f�nf Lesezugriffe zum Refresh des
         dynamischen RAM durch. Es wird ein 8-Bit Refreshz�hler (REF) zur Erzeugung
         von 256 DRAM-Zeilenadressen benutzt. Der Z�hler wird in Rasterzeile 0 mit
         $ff gel�scht und nach jedem Refresh-Zugriff um 1 verringert.
         Der VIC greift also in Zeile 0 auf die Adressen $3fff, $3ffe, $3ffd, $3ffc
         und $3ffb zu, in Zeile 1 auf $3ffa, $3ff9, $3ff8, $3ff7 und $3ff6 usw." [C.B.] */
    uint8_t refreshCounter;
    
    //! Address bus
    /*! Whenever VIC performs a memory read, the generated memory address is stored here */
    uint16_t addrBus;

    //! Data bus
    /*! Whenever VIC performs a memory read, the result is stored here */
    uint8_t dataBus;
    
	//! Indicates that we are curretly processing a DMA line (bad line)
	bool badLineCondition;
	
	//! Determines, if DMA lines (bad lines) can occurr within the current frame.
    /*! Bad lines can only occur, if the DEN bit was set during an arbitary cycle in rasterline 30
	    The DEN bit is located in register 0x11 (CONTROL REGISTER 1) */
    bool DENwasSetInRasterline30;

	//! Display State
	/*! The VIC is either in idle or display state */
	bool displayState;

	//! BA line
	/* Remember: Each CPU cycle is split into two phases:
     
     First phase (LOW):   VIC gets access to the bus
     Second phase (HIGH): CPU gets access to the bus

     In rare cases, VIC needs access in the HIGH phase, too. To block the CPU, the BA line is pulled down.
     
     Note: The BA line can be pulled down by multiple sources (wired AND). */
	uint16_t BAlow;
	
    //! Remember at which cycle BA line has been pulled down
    uint64_t BAwentLowAtCycle;
    
    //! cAccesses can only be performed is BA line is down for more than 2 cycles
    bool BApulledDownForAtLeastThreeCycles();
    
    /* "Der VIC benutzt zwei Flipflops, um den Rahmen um das Anzeigefenster
        herum zu erzeugen: Ein Haupt-Rahmenflipflop und ein vertikales
        Rahmenflipflop. [...]

        Die Flipflops werden nach den folgenden Regeln geschaltet:
     
        1. Erreicht die X-Koordinate den rechten Vergleichswert, wird das
           Haupt-Rahmenflipflop gesetzt.
        2. Erreicht die Y-Koordinate den unteren Vergleichswert in Zyklus 63, wird
           das vertikale Rahmenflipflop gesetzt.
        3. Erreicht die Y-Koordinate den oberern Vergleichswert in Zyklus 63 und
           ist das DEN-Bit in Register $d011 gesetzt, wird das vertikale
           Rahmenflipflop gel�scht.
        4. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
           den unteren, wird das vertikale Rahmenflipflop gesetzt.
        5. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
           den oberen und ist das DEN-Bit in Register $d011 gesetzt, wird das
           vertikale Rahmenflipflop gel�scht.
        6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
           vertikale Rahmenflipflop gel�scht, wird das Haupt-Flipflop gel�scht." [C.B.]
     */

	//! Main frame Flipflop
	bool mainFrameFF;
	
	//! Vertiacl frame Flipflop
	bool verticalFrameFF;
	
    //! Comparison values for frame flipflops
    inline uint16_t leftComparisonValue() { return isCSEL() ? 24 : 31; }
    inline uint16_t rightComparisonValue() { return isCSEL() ? 344 : 335; }
    inline uint16_t upperComparisonValue() { return isRSEL() ? 51 : 55; }
    inline uint16_t lowerComparisonValue() { return isRSEL() ? 251 : 247; }
    
	//! Clear main frame flipflop
    /*  "Das vertikale Rahmenflipflop dient zur Unterst�tzung bei der Darstellung
         des oberen/unteren Rahmens. Ist es gesetzt, kann das Haupt-Rahmenflipflop
         nicht gel�scht werden." [C.B.] */
    inline void clearMainFrameFF() { if (!verticalFrameFF) mainFrameFF = false; }
     
    
	// -----------------------------------------------------------------------------------------------
	//                                        Screen parameters
	// -----------------------------------------------------------------------------------------------

private:
	
	// Current border width in pixels
	unsigned leftBorderWidth, rightBorderWidth;
	
	// Current border height in pixels
	unsigned upperBorderHeight, lowerBorderHeight;

	// First and last visible rasterline
	unsigned firstVisibleLine, lastVisibleLine;
		
	// Total width of visible screen (including border)
	unsigned totalScreenWidth;

	// Total height of visible screen (including border)
	unsigned totalScreenHeight;

	// Pixel aspect ratio (X:Y)
	float pixelAspectRatio;

public:

	inline unsigned getFirstVisiblePixel() { return 0; }
	inline unsigned getLastVisiblePixel() { return totalScreenWidth - 1; }	
	inline unsigned getFirstVisibleLine() { return firstVisibleLine; }
	inline unsigned getLastVisibleLine() { return lastVisibleLine; }
	inline unsigned getTotalScreenWidth() {	return totalScreenWidth; }
	inline unsigned getTotalScreenHeight() { return totalScreenHeight; }
	inline float getPixelAspectRatio() { return pixelAspectRatio; }
	
	
	// -----------------------------------------------------------------------------------------------
	//                              I/O memory handling and RAM access
	// -----------------------------------------------------------------------------------------------

private:
	
	//! I/O Memory
	/*! If a value is poked to the VIC address space, it is stored here. */
	uint8_t iomem[64]; 

    //! Start address of the currently selected memory bank
    /*! There are four banks in total since the VIC chip can only "see" 16 KB of memory at one time
        Two bank select bits in the CIA I/O space determine which quarter of the memory we're actually seeing
     
        \verbatim
        +-------+------+-------+----------+-------------------------------------+
        | VALUE | BITS |  BANK | STARTING |  VIC-II CHIP RANGE                  |
        |  OF A |      |       | LOCATION |                                     |
        +-------+------+-------+----------+-------------------------------------+
        |   0   |  00  |   3   |   49152  | ($C000-$FFFF)                       |
        |   1   |  01  |   2   |   32768  | ($8000-$BFFF)                       |
        |   2   |  10  |   1   |   16384  | ($4000-$7FFF)                       |
        |   3   |  11  |   0   |       0  | ($0000-$3FFF) (DEFAULT VALUE)       |
        +-------+------+-------+----------+-------------------------------------+
        \endverbatim 
    */
    uint16_t bankAddr;

    //! General memory access via address and data bus
    inline uint8_t memAccess(uint16_t addr);
    
    // VIC performs four special types of memory accesses (c, g, p and s)
    
    //! During a 'c access', VIC accesses the video matrix
    inline void cAccess();
    
    //! During a 'g access', VIC reads graphics data (character or bitmap patterns)
    inline void gAccess();
    
    //! During a 'p access', VIC reads sprite pointers
    inline void pAccess();
    
    //! During a 's access', VIC reads sprite data
    inline void sAccess();

    //! Perform a DRAM refresh
    inline void rAccess() { (void)memAccess(0x3F00 | refreshCounter--); }
    
    //! Perform a DRAM idle access
    inline void rIdleAccess() { (void)memAccess(0x3FFF); }
    
    //! Display mode during gAccess
    uint8_t gAccessDisplayMode;

    //! Results of the gAccess is stored here
    uint8_t gAccessResult;
    uint8_t gAccessfgColor;
    uint8_t gAccessbgColor;

	//! Temporary space for display characters
	/*! Every 8th rasterline, the VIC chips performs a DMA access and fills the array with the characters to display */
	uint8_t characterSpace[40];
	
	//! Temporary space for display colors
	/*! Every 8th rasterline, the VIC chips performs a DMA access and fills the array with the characters to display */
	uint8_t colorSpace[40];

    //! Currently used color scheme
    ColorScheme colorScheme;
    
	//! All 16 color codes in an array
	uint32_t colors[16];

	//! First screen buffer
	/*! The VIC chip writes it output into this buffer. The contents of the array is later copied into to
	 texture RAM of your graphic card by the drawRect method in the OpenGL related code. */
	int screenBuffer1[512 * 512];
	
	//! Second screen buffer
	/*! The VIC chip uses double buffering. Once a frame is drawn, the VIC chip writes the next frame to the second buffer */
	int screenBuffer2[512 * 512];
	
	//! Currently used screen buffer
	/*! The variable points either to screenBuffer1 or screenBuffer2 */
	int *currentScreenBuffer;
	
	//! Pixel buffer
	/*! The pixel buffer is used for drawing a single line on the screen. When a sreen line is drawn, the pixels
	 are first written in the pixel buffer. When the whole line is drawn, it is copied into the screen buffer.
	 */
	int *pixelBuffer;
	
	//! Z buffer
	/*! The z Buffer is used for drawing a single line on the screen. A pixel is only written to the screen buffer,
	 if it is closer to the view point. The depth of the closest pixel is kept in the z buffer. The lower the value
	 of the z buffer, the closer it is to the viewer.
	 The z buffer is cleared before a new rasterline is drawn.
	 */
	int zBuffer[MAX_VIEWABLE_PIXELS];
	
	//! Indicates the source of a drawn pixel
	/*! Whenever a foreground pixel or sprite pixel is drawn, a distinct bit in the pixelSource array is set.
	 The information is utilized to detect sprite-sprite and sprite-background collisions. 
	 */
	int pixelSource[MAX_VIEWABLE_PIXELS];
	
	
	//! Start address of screen memory
	/*! The screen memory stores the character codes to display
	 The upper four bits of the VIC register 0xD018 determine where the screen memory starts (relative to the bank address):
	 
	 \verbatim
	 +---------+------------+-----------------------------+
	 |         |            |         LOCATION*           |
	 |    A    |    BITS    +---------+-------------------+
	 |         |            | DECIMAL |        HEX        |
	 +---------+------------+---------+-------------------+
	 |     0   |  0000XXXX  |      0  |  $0000            |
	 |    16   |  0001XXXX  |   1024  |  $0400 (DEFAULT)  |
	 |    32   |  0010XXXX  |   2048  |  $0800            |
	 |    48   |  0011XXXX  |   3072  |  $0C00            |
	 |    64   |  0100XXXX  |   4096  |  $1000            |
	 |    80   |  0101XXXX  |   5120  |  $1400            |
	 |    96   |  0110XXXX  |   6144  |  $1800            |
	 |   112   |  0111XXXX  |   7168  |  $1C00            |
	 |   128   |  1000XXXX  |   8192  |  $2000            |
	 |   144   |  1001XXXX  |   9216  |  $2400            |
	 |   160   |  1010XXXX  |  10240  |  $2800            |
	 |   176   |  1011XXXX  |  11264  |  $2C00            |
	 |   192   |  1100XXXX  |  12288  |  $3000            |
	 |   208   |  1101XXXX  |  13312  |  $3400            |
	 |   224   |  1110XXXX  |  14336  |  $3800            |
	 |   240   |  1111XXXX  |  15360  |  $3C00            |
	 +---------+------------+---------+-------------------+	
	 \endverbatim
	 */
    // DEPRECATED
	uint16_t screenMemoryAddr;
		
	//! Start address of character memory
	/*! The character memory stores the bitmaps for each character.
	 The location of character memory is determined by VIC register 0xD018
	 
	 \verbatim
	 +-----+----------+------------------------------------------------------+
	 |VALUE|          |            LOCATION OF CHARACTER MEMORY*             |
	 | of A|   BITS   +-------+----------------------------------------------+
	 |     |          |DECIMAL|         HEX                                  |
	 +-----+----------+-------+----------------------------------------------+
	 |   0 | XXXX000X |     0 | $0000-$07FF                                  |
	 |   2 | XXXX001X |  2048 | $0800-$0FFF                                  |
	 |   4 | XXXX010X |  4096 | $1000-$17FF ROM IMAGE in BANK 0 & 2 (default)|
	 |   6 | XXXX011X |  6144 | $1800-$1FFF ROM IMAGE in BANK 0 & 2          |
	 |   8 | XXXX100X |  8192 | $2000-$27FF                                  |
	 |  10 | XXXX101X | 10240 | $2800-$2FFF                                  |
	 |  12 | XXXX110X | 12288 | $3000-$37FF                                  |
	 |  14 | XXXX111X | 14336 | $3800-$3FFF                                  |
	 +-----+----------+-------+----------------------------------------------+
	 \endverbatim
	 */
    // DEPRECATED
	uint16_t characterMemoryAddr;
		
	//! True, iff character data is read from ROM space
    // DEPRECATED
	bool characterMemoryMappedToROM;

    // -----------------------------------------------------------------------------------------------
    //                                      Sequencers
    // -----------------------------------------------------------------------------------------------

    //! Graphic sequencer raw data (not yet converted to pixels)
    uint8_t gs_data;
    
    //! Graphic sequencer display mode (conversion method)
    DisplayMode gs_mode;
    
    //! Graphic sequencer foreground color to be used in data->pixel conversion)
    uint8_t gs_fg_color;
    
    //! Graphic sequencer foreground color to be used in data->pixel conversion)
    uint8_t gs_bg_color;

    //! Graphic sequencer colors for multi color modes
    uint8_t gs_multicol0, gs_multicol1, gs_multicol2, gs_multicol3;

    //! Graphic sequencer load delay
    uint8_t gs_delay;

    //! Load graphic sequencer with data and determine conversion parameters
    void loadGraphicSequencer(uint8_t data, uint8_t load_delay);
    
    //! Synthesize pixels in border area
    void runGraphicSequencerAtBorder(int cycle);

    //! Synthesize pixels in main screen area
    void runGraphicSequencer();


	// -----------------------------------------------------------------------------------------------
	//                                         Sprites
	// -----------------------------------------------------------------------------------------------

	//! MC register
	/*! MOB data counter (6 bit counter). One register for each sprite */
	uint8_t mc[8];
	
	//! MCBASE register
	/*! MOB data counter (6 bit counter). One register for each sprite */
	uint8_t mcbase[8];
	
	//! Sprite data shift registers
	/*! The VIC chip has a 24 bit (3 byte) shift register for each sprite. It stores the sprite data for each rasterline */
	uint8_t spriteShiftReg[8][3];
	
	//! Sprite pointer
	/*! Determines where the sprite data comes from */
	uint16_t spritePtr[8];
	
	//! Sprite on off
	/*! Determines if a sprite needs to be drawn in the current rasterline. Each bit represents a single sprite. */
	uint8_t spriteOnOff;
	
	//! Previous value of spriteOnOff
	uint8_t oldSpriteOnOff; 
	
	//! Sprite DMA on off
	/*! Determines  if sprite dma access is enabled or disabled. Each bit represents a single sprite. */
	uint8_t spriteDmaOnOff;
	
	//! Expansion flipflop
	/*! Used to handle Y sprite stretching. One bit for each sprite */
	uint8_t expansionFF;
    uint8_t expansionFF_in_015;
	
				
	// -----------------------------------------------------------------------------------------------
	//                                             Lightpen
	// -----------------------------------------------------------------------------------------------
	
	//! Lightpen triggered?
	/*! This variable ndicates whether a lightpen interrupt has occurred within the current frame.
	    The variable is needed, because a lightpen interrupt can only occur once in a frame. It is set to false
	    at the beginning of each frame. */
	bool lightpenIRQhasOccured;
	
	
	// -----------------------------------------------------------------------------------------------
	//                                             Debugging
	// -----------------------------------------------------------------------------------------------
	
	//! Determines whether sprites are drawn or not
	/*! During normal emulation, the value is always true. For debugging purposes, the value can be set to false.
	 In this case, sprites are no longer drawn.
	 */
	bool drawSprites;
	
	//! Enable sprite-sprite collision
	/*! If set to true, the virtual VIC chips checks for sprite-sprite collision as the original C64 does.
	    For debugging purposes and cheating, collision detection can be disabled by setting the variabel to false.
	    Collision detection can be enabled or disabled for each sprite seperately. Each bit is dedicated to a single sprite. 
	*/
	uint8_t spriteSpriteCollisionEnabled;
	
	//! Enable sprite-background collision
	/*! If set to true, the virtual VIC chips checks for sprite-background collision as the original C64 does.
	    For debugging purposes and cheating, collision detection can be disabled by setting the variabel to false.
	    Collision detection can be enabled or disabled for each sprite seperately. Each bit is dedicated to a single sprite. 
	*/
	uint8_t spriteBackgroundCollisionEnabled;
	
	//! Determines whether IRQ lines will be made visible.
	/*! Each rasterline that will potentially trigger a raster IRQ is highlighted. This feature is useful for
	    debugging purposes as it visualizes how the screen is divided into multiple parts. */
	bool markIRQLines;
	
	//! Determines whether DMA lines will be made visible.
	/*! Each rasterline in which the vic will read additional data from the memory and stun the CPU is made visible.
	    Note that partial DMA lines may not appear. */	
	bool markDMALines;

	//! mark rasterline for debugging
	/*! If set to a positive value, the specific rasterline is highlighted. The feature is intended for 
	    debugging purposes, only */
	int rasterlineDebug[MAX_VIEWABLE_RASTERLINES];

	
	// -----------------------------------------------------------------------------------------------
	//                                             Methods
	// -----------------------------------------------------------------------------------------------

public:
	
	//! Constructor
	VIC();
	
	//! Destructor
	~VIC();
	
	//! Get screen buffer
	inline void *screenBuffer() { return (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1; }

	//! Reset the VIC chip to its initial state
	void reset();
	
	//! Load state
	void loadFromBuffer(uint8_t **buffer);
	
	//! Save state
	void saveToBuffer(uint8_t **buffer);	
	
	//! Dump internal state to console
	void dumpState();	
	
	// -----------------------------------------------------------------------------------------------
	//                                         Configuring
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Configure the VIC chip for PAL video output
	void setPAL();
	
	//! Configure the VIC chip for NTSC video output
	void setNTSC();	

    //! Get color scheme
	ColorScheme getColorScheme() { return colorScheme; }

	//! Set color scheme
	void setColorScheme(ColorScheme scheme);
	
    //! Get color
	uint32_t getColor(int nr) { return colors[nr]; }
    
    
	// -----------------------------------------------------------------------------------------------
	//                                         Drawing
	// -----------------------------------------------------------------------------------------------

private:	
	
    //! Increase the x coordinate by 8 (sptrite coordinate system)
    inline void countX() { xCounter += 8; }

    //! Draw single pixel into pixel buffer
    /*! \param offset X coordinate of the pixel to draw
        \param color Pixel color in RGBA format
        \param z buffer depth (0 = background)
     */
    void setPixel(unsigned offset, int color, int depth, int source);

    //! Draw a single foreground pixel
    /*! \param offset X coordinate of the pixel to draw
        \param color Pixel color in RGBA format
     */
    void setFramePixel(unsigned offset, int color);
    
	//! Draw a single foreground pixel
	/*! \param offset X coordinate of the pixel to draw
        \param color Pixel color in RGBA format
	 */
	void setForegroundPixel(unsigned offset, int color);
	
	//! Draw a single foreground pixel
	/*! \param offset X coordinate of the pixel to draw
        \param color Pixel color in RGBA format
	 */
	void setBackgroundPixel(unsigned offset, int color);
    
    //! Draw a single character line (8 pixels) in single-color mode
    /*! \param offset X coordinate of the first pixel to draw */
    void drawSingleColorCharacter(unsigned offset);
    
    //! Draw a single character line (8 pixels) in multi-color mode
    /*! \param offset X coordiate of the first pixel to draw
     */
    void drawMultiColorCharacter(unsigned offset);

    //! Draw a single color character in invalid text mode
    /*! \param offset X coordiate of the first pixel to draw
     */
    void drawInvalidSingleColorCharacter(unsigned offset);

    //! Draw a multi color character in invalid text mode
    /*! \param offset X coordiate of the first pixel to draw
     */
    void drawInvalidMultiColorCharacter(unsigned offset);

    
    
	//! Draw a single foreground pixel
	/*! \param offset X coordinate of the pixel to draw
	    \param color Pixel color in RGBA format
	    \param nr Number of sprite (0 to 7)
        \note The function may trigger an interrupt, if a sprite/sprite or sprite/background collision is detected
	 */
	void setSpritePixel(unsigned offset, int color, int nr);
		
	//! Draws all sprites into the pixelbuffer
	/*! A sprite is only drawn if it's enabled and if sprite drawing is not switched off for debugging */
	void drawAllSprites();

	//! Draw single sprite into pixel buffer
	/*! Helper function for drawSprites */
	void drawSprite(uint8_t nr);
			
	
	// -----------------------------------------------------------------------------------------------
	//                                       Getter and setter
	// -----------------------------------------------------------------------------------------------

public:
	
	//! Returns true if the specified address lies in the VIC I/O range
	static inline bool isVicAddr(uint16_t addr)	{ return (VIC_START_ADDR <= addr && addr <= VIC_END_ADDR); }

	//! Bind the VIC chip to the virtual C64.
	void setC64(C64 *c) { assert(c64 == NULL); c64 = c; }
	
	//! Bind the VIC chip to the specified CPU.
	void setCPU(CPU *c) { assert(cpu == NULL); cpu = c; }
	
	//! Bind the VIC chip to the specified virtual memory.
	void setMemory(C64Memory *m) { assert(mem == NULL); mem = m; }
	
	//! Get current scanline
	inline uint16_t getScanline() { return scanline; }
			
	//! Set rasterline
	inline void setScanline(uint16_t line) { scanline = line; }

	//! Get memory bank start address
	uint16_t getMemoryBankAddr();
	
	//! Set memory bank start address
	void setMemoryBankAddr(uint16_t addr);
			
	//! Get screen memory address
	uint16_t getScreenMemoryAddr();
	
	//! Set screen memory address
	void setScreenMemoryAddr(uint16_t addr);
		
	//! Get character memory start address
	uint16_t getCharacterMemoryAddr();
	
	//! Set character memory start address
	void setCharacterMemoryAddr(uint16_t addr);
		
	//! Peek fallthrough
	/*! The fallthrough mechanism works as follows:
	 If the memory is asked to peek a value, it first checks whether the RAM, ROM, or I/O space is visible.
	 If an address in the I/O space is specified, the memory is unable to handle the request itself and
	 passes it to the corresponding I/O chip.
	 */
	uint8_t peek(uint16_t addr);
    
	//! Poke fallthrough
	/*! The fallthrough mechanism works as follows:
	 If the memory is asked to poke a value, it first checks whether the RAM, ROM, or I/O space is visible.
	 If an address in the I/O space is specified, the memory is unable to handle the request itself and
	 passes it to the corresponding I/O chip. 
	 */	
	void poke(uint16_t addr, uint8_t value);
	
	
	// -----------------------------------------------------------------------------------------------
	//                                         Properties
	// -----------------------------------------------------------------------------------------------
	
public:
	
	//! Return left bound of inner screen area. 
	//* The returned value is the leftmost coordinate inside the inner screen area */
	//inline int xStart() { return numberOfColumns() == 40 ? 24 : 31; }
	inline int xStart() { return numberOfColumns() == 40 ? leftBorderWidth : leftBorderWidth + 7; }

	//! Return right bound of inner screen area
	//* The returned value is the leftmost coordinate inside the right border */
	// inline int xEnd() { return numberOfColumns() == 40 ? 343 : 334; }
	inline int xEnd() { return numberOfColumns() == 40 ? leftBorderWidth + SCREEN_WIDTH : leftBorderWidth + SCREEN_WIDTH - 7; }
	
	//! Return upper bound of inner screen area
	// inline int yStart() { return numberOfRows() == 25 ? 51 : 55; }
	inline int yStart() { return numberOfRows() == 25 ? 51 : 55; }
	
	//! Return lower bound of inner screen area
	// inline int yEnd() { return numberOfRows() == 25 ? 250 : 246; }
	inline int yEnd() { return numberOfRows() == 25 ? 250 : 246; }

    //! Returns the DEN bit (DIsplay Enabled)
    inline bool DENbit() { return iomem[0x11] & 0x10; }

    //! Returns the BMM bit (Bit Map Mode)
    inline bool BMMbit() { return iomem[0x11] & 0x20; }

    //! Returns the ECM bit (Extended Character Mode)
    inline bool ECMbit() { return iomem[0x11] & 0x40; }

    //! Returns masked CB13 bit (controls memory access)
    inline uint8_t CB13() { return iomem[0x18] & 0x08; }

    //! Returns masked CB13/CB12/CB11 bits (controls memory access)
    inline uint8_t CB13CB12CB11() { return iomem[0x18] & 0x0E; }

    //! Returns masked VM13/VM12/VM11/VM10 bits (controls memory access)
    inline uint8_t VM13VM12VM11VM10() { return iomem[0x18] & 0xF0; }

	//! Returns the state of the CSEL bit
	inline bool isCSEL() { return iomem[0x16] & 0x08; }
	
	//! Returns the state of the RSEL bit
	inline bool isRSEL() { return iomem[0x11] & 0x08; }
    
	//! Returns the currently set display mode
	/*! The display mode is determined by bits 5 and 6 of control register 1 and bit 4 of control register 2. */
	inline DisplayMode getDisplayMode() 
	{ return (DisplayMode)((iomem[0x11] & 0x60) | (iomem[0x16] & 0x10)); }
	
	//! Set display mode
	inline void setDisplayMode(DisplayMode m) 
	{ iomem[0x11] = (iomem[0x11] & (0xff - 0x60)) | (m & 0x60); iomem[0x16] = (iomem[0x16] & (0xff-0x10)) | (m & 0x10); }
	
	//! Get the current screen geometry
	ScreenGeometry getScreenGeometry(void);
	
	//! Set the screen geometry 
	void setScreenGeometry(ScreenGeometry mode);
	
	//! Returns the number of rows to be drawn (24 or 25)
	inline int numberOfRows() { return (iomem[0x11] & 8) ? 25 : 24; }
	
	//! Set the number of rows to be drawn (24 or 25)
	inline void setNumberOfRows(int rows) 
	{ assert(rows == 24 || rows == 25); if (rows == 25) iomem[0x11] |= 0x8; else iomem[0x11] &= (0xff - 0x8); }
	
	//! Return the number of columns to be drawn (38 or 40)
	inline int numberOfColumns() { return (iomem[0x16] & 8) ? 40 : 38; }

	//! Set the number of columns to be drawn (38 or 40)
	inline void setNumberOfColumns(int columns) 
	{ assert(columns == 38 || columns == 40); if (columns == 40) iomem[0x16] |= 0x8; else iomem[0x16] &= (0xff - 0x8); }
		
	//! Returns the vertical raster scroll offset (0 to 7)
	/*! The vertical raster offset is usally used by games for smoothly scrolling the screen */
	inline uint8_t getVerticalRasterScroll() { return iomem[0x11] & 7; }
	
	//! Set vertical raster scroll offset (0 to 7)
	inline void setVerticalRasterScroll(uint8_t offset) { iomem[0x11] = (iomem[0x11] & 0xF8) | (offset & 0x07); }
	
	//! Returns the horizontal raster scroll offset (0 to 7)
	/*! The vertical raster offset is usally used by games for smoothly scrolling the screen */
	inline uint8_t getHorizontalRasterScroll() { return iomem[0x16] & 7; }
	
	//! Set horizontan raster scroll offset (0 to 7)
	inline void setHorizontalRasterScroll(uint8_t offset) { iomem[0x16] = (iomem[0x16] & 0xF8) | (offset & 0x07); }
		
	//! Returns the row number for a given rasterline
	inline uint8_t getRowNumberForRasterline(uint16_t line) { return (line - FIRST_Y_COORD_OF_INNER_AREA + 3 - getVerticalRasterScroll()) / 8; }
	
	//! Returns the character row number for a given rasterline
	inline uint8_t getRowOffsetForRasterline(uint16_t line) { return (line - FIRST_Y_COORD_OF_INNER_AREA + 3 - getVerticalRasterScroll()) % 8; }
	
	//! Return border color
	inline uint8_t getBorderColor() { return iomem[0x20] & 0x0F; }
	
	//! Returns background color
	inline uint8_t getBackgroundColor() { return iomem[0x21] & 0x0F; }
	
	//! Returns extra background color (for multicolor modes)
	inline uint8_t getExtraBackgroundColor(int offset) { return iomem[0x21 + offset] & 0x0F; }
	
	
	
	// -----------------------------------------------------------------------------------------------
	//                                DMA lines, BA signal and IRQs
	// -----------------------------------------------------------------------------------------------

private:
    
	//! Returns true, if the specified rasterline is a DMA line
	/*! Every eigths row, the VIC chip performs a DMA access and fetches data from screen memory and color memory
	 The first DMA access occurrs within lines 0x30 to 0xf7 and  */
	// inline bool isDMALine() { return scanline >= 0x30 && scanline <= 0xf7 && (scanline & 7) == getVerticalRasterScroll(); }

    /*! Update bad line condition
        From Christina Bauers VIC II documentation:

        "Ein Bad-Line-Zustand liegt in einem beliebigen Taktzyklus vor, wenn an der
         negativen Flanke von �0 zu Beginn des 
         [1] Zyklus RASTER >= $30 und RASTER <= $f7 und
         [2] die unteren drei Bits von RASTER mit YSCROLL �bereinstimmen 
         [3] und in einem beliebigen Zyklus von Rasterzeile $30 das DEN-Bit gesetzt war." */
    
     inline void updateBadLineCondition() {
         badLineCondition =
            scanline >= 0x30 && scanline <= 0xf7 /* [1] */ &&
            (scanline & 0x07) == getVerticalRasterScroll() /* [2] */ &&
            DENwasSetInRasterline30 /* [3] */;
         if (badLineCondition)
             displayState = true;
     }
    
	//! checkDmaLineCondition
//	inline void checkDmaLineCondition() { if ((dmaLine = (DENwasSetInRasterline30 && isDMALine()))) displayState = true; }
	
	//! Set BA line to low
	/*! Note: The BA pin is directly connected to the RDY line of the CPU */
	void pullDownBA(uint16_t source);
	
	//! Set BA line to high
	/*! Note: The BA pin is directly connected to the RDY line of the CPU */
	void releaseBA(uint16_t source);

	//! Request memory bus for a specific sprite
	inline void requestBusForSprite(uint8_t spriteNr) {
        if (spriteDmaOnOff & (1 << spriteNr))
            pullDownBA(1 << spriteNr);
    }
	
	//! Release memory bus for a specific sprite
	inline void releaseBusForSprite(uint8_t spriteNr) { releaseBA(1 << spriteNr); }
	
	//! Trigger a VIC interrupt
	/*! VIC interrupts can be triggered from multiple sources. Each one is associated with a specific bit */
	void triggerIRQ(uint8_t source);
		
public: 
	
	//! Return next interrupt rasterline
	inline uint16_t rasterInterruptLine() { return ((iomem[0x11] & 128) << 1) + iomem[0x12]; }

	//! Set interrupt rasterline 
	inline void setRasterInterruptLine(uint16_t line) { iomem[0x12] = line & 0xFF; if (line > 0xFF) iomem[0x11] |= 0x80; else iomem[0x11] &= 0x7F; }
	
	//! Returns true, iff rasterline interrupts are enabled
	inline bool rasterInterruptEnabled() { return iomem[0x1A] & 1; }

	//! Enable or disable rasterline interrupts
	inline void setRasterInterruptEnable(bool b) { if (b) iomem[0x1A] |= 0x01; else iomem[0x1A] &= 0xFE; }
	
	//! Enable or disable rasterline interrupts
	inline void toggleRasterInterruptFlag() { setRasterInterruptEnable(!rasterInterruptEnabled()); }
	
	//! Simulate a light pen event
	/*! Although we do not support hardware lightpens, we need to take care of it because lightpen interrupts 
	 can be triggered by software. It is used by some games to determine the current X position within 
	 the current rasterline. */
	void simulateLightPenInterrupt();

	
	// -----------------------------------------------------------------------------------------------
	//                                              Sprites
	// -----------------------------------------------------------------------------------------------

private:
	
	//! Update sprite DMA bits
	void updateSpriteDmaOnOff();
	
	//! Read sprite pointer
	/*! Determines the start adress of sprite data and stores the value into spritePtr */
	inline void readSpritePtr(int sprite) { 
		spritePtr[sprite] = bankAddr + (mem->ram[bankAddr + screenMemoryAddr + 0x03F8 + sprite] << 6); 
	}
	
	//! Read sprite data 
	/*! Read next byte of sprite data into shift register. */
	inline void readSpriteData(int sprite) { 
		if (spriteDmaOnOff & (1 << sprite)) { 
			spriteShiftReg[sprite][mc[sprite]%3] = mem->ram[spritePtr[sprite]+mc[sprite]]; mc[sprite]++; 
		}
	}
	
	//! Get sprite depth
	/*! The value is written to the z buffer to resolve overlapping pixels */
	inline uint8_t spriteDepth(uint8_t nr) { return spriteIsDrawnInBackground(nr) ? 0x30 | nr : 0x10 | nr; }
	
public: 
	
	//! Returns color code of multicolor sprites (extra color 1)
	inline uint8_t spriteExtraColor1() { return iomem[0x25] & 0x0F; }
	
	//! Returns color code of multicolor sprites (extra color 2)
	inline uint8_t spriteExtraColor2() { return iomem[0x26] & 0x0F; }
	
	//! Get sprite color 
	inline uint8_t spriteColor(uint8_t nr) { return iomem[0x27 + nr] & 0x0F; }

	//! Set sprite color
	inline void setSpriteColor(uint8_t nr, uint8_t color) { assert(nr < 8); iomem[0x27 + nr] = color; }
		
	//! Get X coordinate of sprite 
	inline uint16_t getSpriteX(uint8_t nr) { return iomem[2*nr] + (iomem[0x10] & (1 << nr) ? 256 : 0); }

	//! Set X coordinate if sprite
	inline void setSpriteX(uint8_t nr, int x) { if (x < 512) { poke(2*nr, x & 0xFF); if (x > 0xFF) poke(0x10, peek(0x10) | (1 << nr)); else poke(0x10, peek(0x10) & ~(1 << nr));} }
	
	//! Get Y coordinate of sprite
	inline uint8_t getSpriteY(uint8_t nr) { return iomem[1+2*nr]; }

	//! Set Y coordinate of sprite
	inline void setSpriteY(uint8_t nr, int y) { if (y < 256) { poke(1+2*nr, y);} }
	
	//! Returns true, if sprite is enabled (drawn on the screen)
	inline bool spriteIsEnabled(uint8_t nr) { return iomem[0x15] & (1 << nr); }		

	//! Enable or disable sprite
	inline void setSpriteEnabled(uint8_t nr, bool b) { if (b) poke(0x15, peek(0x15) | (1 << nr)); else poke(0x15, peek(0x15) & (0xFF - (1 << nr))); }

	//! Enable or disable sprite
    inline void toggleSpriteEnabled(uint8_t nr) { setSpriteEnabled(nr, !spriteIsEnabled(nr)); }
	
	//! Returns true, iff an interrupt will be triggered when a sprite/background collision occurs
	inline bool spriteBackgroundInterruptEnabled() { return iomem[0x1A] & 2; }

	//! Returns true, iff an interrupt will be triggered when a sprite/sprite collision occurs
	inline bool spriteSpriteInterruptEnabled() { return iomem[0x1A] & 4; }

	//! Returns true, iff a rasterline interrupt has occurred
	inline bool rasterInterruptOccurred() { return iomem[0x19] & 1; }

	//! Returns true, iff a sprite/background interrupt has occurred
	inline bool spriteBackgroundInterruptOccurred() { return iomem[0x19] & 2; }

	//! Returns true, iff a sprite/sprite interrupt has occurred
	inline bool spriteSpriteInterruptOccurred() { return iomem[0x19] & 2; }

	//! Returns true, iff sprites are drawn behind the scenary
	inline bool spriteIsDrawnInBackground(uint8_t nr) { return iomem[0x1B] & (1 << nr); }

	//! Determine whether a sprite is drawn before or behind the scenary
	inline void setSpriteInBackground(uint8_t nr, bool b) 
		{ if (b) poke(0x1B, peek(0x1B) | (1 << nr)); else poke(0x1B, peek(0x1B) & ~(1 << nr)); }

	//! Determine whether a sprite is drawn before or behind the scenary
	inline void spriteToggleBackgroundPriorityFlag(uint8_t nr)
		{ setSpriteInBackground(nr, !spriteIsDrawnInBackground(nr)); }
	
	//! Returns true, iff sprite is a multicolor sprite
	inline bool spriteIsMulticolor(uint8_t nr) { return iomem[0x1C] & (1 << nr); }

	//! Set single color or multi color mode for sprite
	inline void setSpriteMulticolor(uint8_t nr, bool b) { if (b) poke(0x1C, peek(0x1C) | (1 << nr)); else poke(0x1C, peek(0x1C) & ~(1 << nr)); }

	//! Switch between single color or multi color mode
	inline void toggleMulticolorFlag(uint8_t nr) { setSpriteMulticolor(nr, !spriteIsMulticolor(nr)); }
		
	//! Returns true, if the sprite is vertically stretched
	inline bool spriteHeightIsDoubled(uint8_t nr) { return iomem[0x17] & (1 << nr); }	

	//! Stretch or shrink sprite vertically
	inline void setSpriteStretchY(uint8_t nr, bool b) { if (b) poke(0x17, peek(0x17) | (1 << nr)); else poke(0x17, peek(0x17) & ~(1 << nr)); }

	//! Stretch or shrink sprite vertically
	inline void spriteToggleStretchYFlag(uint8_t nr) { setSpriteStretchY(nr, !spriteHeightIsDoubled(nr)); }

	//! Returns true, if the sprite is horizontally stretched 
	inline bool spriteWidthIsDoubled(uint8_t nr) { return iomem[0x1D] & (1 << nr); }

	//! Stretch or shrink sprite horizontally
	inline void setSpriteStretchX(uint8_t nr, bool b) { if (b) poke(0x1D, peek(0x1D) | (1 << nr)); else poke(0x1D, peek(0x1D) & (0xFF - (1 << nr))); }

	//! Stretch or shrink sprite horizontally
	inline void spriteToggleStretchXFlag(uint8_t nr) { setSpriteStretchX(nr, !spriteWidthIsDoubled(nr)); }

	//! Returns true, iff sprite collides with another sprite
	inline bool spriteCollidesWithSprite(uint8_t nr) { return iomem[0x1E] & (1 << nr); }

	//! Returns true, iff sprite collides with background
	inline bool spriteCollidesWithBackground(uint8_t nr) { return iomem[0x1F] & (1 << nr); }

	
	
	// -----------------------------------------------------------------------------------------------
	//                                    Execution functions
	// -----------------------------------------------------------------------------------------------

public:
	
	//! Prepare for new frame
	/*! This function is called prior to cycle 1 of rasterline 0 */
	void beginFrame();
	
	//! Prepare for new rasterline
	/*! This function is called prior to cycle 1 at the beginning of each rasterline */
	void beginRasterline(uint16_t rasterline);

	//! Finish rasterline
	/*! This function is called after the last cycle of each rasterline. */
	void endRasterline();
	
	//! Finish frame
	/*! This function is called after the last cycle of the last rasterline */
	void endFrame();
		
	//! VIC execution functions
	void cycle1();  void cycle2();  void cycle3();  void cycle4();
    void cycle5();  void cycle6();  void cycle7();  void cycle8();
    void cycle9();  void cycle10(); void cycle11(); void cycle12();
    void cycle13(); void cycle14(); void cycle15(); void cycle16();
    void cycle17(); void cycle18();

    void cycle19to54();

    void cycle55(); void cycle56(); void cycle57(); void cycle58();
    void cycle59(); void cycle60(); void cycle61(); void cycle62();
    void cycle63(); void cycle64(); void cycle65();
	
	
	// -----------------------------------------------------------------------------------------------
	//                                              Debugging
	// -----------------------------------------------------------------------------------------------

private:
	
	//! Colorize line
	void markLine(int start, unsigned length, int color);

public: 
	
	//! Return true iff IRQ lines are colorized
	bool showIrqLines() { return markIRQLines; }

	//! Show or hide IRQ lines
	void setShowIrqLines(bool show) { markIRQLines = show; }

	//! Return true iff DMA lines are colorized
	bool showDmaLines() { return markDMALines; }
	
	//! Show or hide DMA lines
	void setShowDmaLines(bool show) { markDMALines = show; }

	//! Return true iff sprites are hidden
	bool hideSprites() { return !drawSprites; }

	//! Hide or show sprites
	void setHideSprites(bool hide) { drawSprites = !hide; }
	
	//! Return true iff sprite-sprite collision detection is enabled
	bool getSpriteSpriteCollision(uint8_t nr) { return spriteSpriteCollisionEnabled & (1 << nr); }

	//! Enable or disable sprite-sprite collision detection
	void setSpriteSpriteCollision(uint8_t nr, bool b) { if (b) SET_BIT(spriteSpriteCollisionEnabled, nr); else CLR_BIT(spriteSpriteCollisionEnabled, nr); }

	//! Enable or disable sprite-sprite collision detection
	void toggleSpriteSpriteCollisionFlag(uint8_t nr) { setSpriteSpriteCollision(nr, !getSpriteSpriteCollision(nr)); }
	
	//! Return true iff sprite-background collision detection is enabled
	bool getSpriteBackgroundCollision(uint8_t nr) { return spriteBackgroundCollisionEnabled & (1 << nr); }

	//! Enable or disable sprite-background collision detection
	void setSpriteBackgroundCollision(uint8_t nr, bool b) { if (b) SET_BIT(spriteBackgroundCollisionEnabled, nr); else CLR_BIT(spriteBackgroundCollisionEnabled, nr); }

	//! Enable or disable sprite-background collision detection
	void toggleSpriteBackgroundCollisionFlag(uint8_t nr) { setSpriteBackgroundCollision(nr, !getSpriteBackgroundCollision(nr)); }
};

#endif

