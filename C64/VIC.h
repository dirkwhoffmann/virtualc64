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

// Last review: 25.7.06

#ifndef _VIC_INC
#define _VIC_INC

#include "VirtualComponent.h"

// Forward declarations
class C64Memory;

#define RED(x)   ((x & 0xff000000) >> 24)
#define GREEN(x) ((x & 0x00ff0000) >> 16)
#define BLUE(x)  ((x & 0x0000ff00) >> 8)
#define ALPHA(x) (x & 0x000000ff)

//! The virtual Video Controller
/*! VIC is the video controller chip of the Commodore 64.
	The VIC chip occupied the memory mapped I/O space from address 0xD000 to 0xD02E.
*/
class VIC : public VirtualComponent {

public:
	//! Color scheme
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
		GRAYSCALE       = 0x0B,
		CUSTOM_PALETTE  = 0xFF,
	};
	
	//! Display mode
	/*! The VIC chip supports five distinct display modes. The currently set display mode is 
		determined by Bit 5 and Bit 6 of control register 1 and Bit 4 of control register 2.
	*/
	enum DisplayMode {
		STANDARD_TEXT             = 0x00,
		MULTICOLOR_TEXT           = 0x10,
		STANDARD_BITMAP           = 0x20,
		MULTICOLOR_BITMAP         = 0x30,
		EXTENDED_BACKGROUND_COLOR = 0x40,
		INVALID_DISPLAY_MODE      = 0x01,
	};

	//! Screen geometry
	/*! The VIC chip supports four different screen geometries. */
	enum ScreenGeometry {
		COL_40_ROW_25 = 0x01,
		COL_38_ROW_25 = 0x02,
		COL_40_ROW_24 = 0x03,
		COL_38_ROW_24 = 0x04,
	};

	//! Integer value for standard character mode as returned by \a getDisplayMode
	static const int STANDARD_CHARACTER_MODE = 0x00;
	//! Integer value for multi-color character mode as returned by \a getDisplayMode
	static const int MULTICOLOR_CHARACTER_MODE = 0x10;
	//! Integer value for standard bitmap mode as returned by \a getDisplayMode
	static const int STANDARD_BITMAP_MODE = 0x20;
	//! Integer value for multi-color bitmap mode as returned by \a getDisplayMode
	static const int MULTICOLOR_BITMAP_MODE = 0x30;
	//! Integer value for extended background color mode as returned by \a getDisplayMode
	static const int EXTENDED_BACKGROUND_COLOR_MODE = 0x40;

	static const int BLACK   = 0x00;
	static const int WHITE   = 0x01;
	static const int RED     = 0x02;
	static const int CYAN    = 0x03;
	static const int PURPLE  = 0x04;
	static const int GREEN   = 0x05;
	static const int BLUE    = 0x06;
	static const int YELLOW  = 0x07;
	static const int LTBROWN = 0x08;
	static const int BROWN   = 0x09;
	static const int LTRED   = 0x0A;
	static const int GREY1   = 0x0B;
	static const int GREY2   = 0x0C;
	static const int LTGREEN = 0x0D;
	static const int LTBLUE  = 0x0E;
	static const int GREY3   = 0x0F;
	
	//! Start address of the VIC I/O space
	static const uint16_t VIC_START_ADDR = 0xD000;
	//! End address of the VIC I/O space
	static const uint16_t VIC_END_ADDR = 0xD3FF;

	//! Address offset of the read raster / write raster register
	//static const uint16_t VIC_RASTER_READ_WRITE = 0x12;
	
	//! Number of drawn rasterlines on an NTSC screen
	static const uint16_t NTSC_RASTERLINES = 263;

	//! Number of drawn rasterlines on a PAL format
	static const uint16_t PAL_RASTERLINES = 312;
	
	//! Refresh rate of the NTSC screen format in Hz
	static const uint16_t NTSC_REFRESH_RATE = 60;

	//! Refresh rate of the PAL screen format in Hz
	static const uint16_t PAL_REFRESH_RATE = 50;

	//! CPU cycles per rasterline on a NTSC machine
	static const uint16_t NTSC_CYCLES_PER_RASTERLINE = 65;

	//! CPU cycles per rasterline on a PAL machine
	static const uint16_t PAL_CYCLES_PER_RASTERLINE = 63;
	
	//! Widht of the drawable screen area in pixels
	static const uint16_t SCREEN_WIDTH = 320;

	//! Height of the drawable screen area in pixels
	static const uint16_t SCREEN_HEIGHT = 200;

	//! First column coordinate that belongs to the drawable screen area
	static const uint16_t BORDER_WIDTH = 24;
	
	//! First rasterline that belongs to the drawable screen area
	static const uint16_t BORDER_HEIGHT = 51;

	//! First rasterline that can be seen
	static const uint16_t FIRST_VIEABLE_LINE = 16;

	//! Last rasterline that can be seen
	static const uint16_t LAST_VIEABLE_LINE = 287;

	//! Total number of pixels in one screen buffer line
	// TODO this should be 403 for PAL and 418 for NTSC currently this is 368
	static const uint16_t TOTAL_SCREEN_WIDTH = BORDER_WIDTH + SCREEN_WIDTH + BORDER_WIDTH;

	//! Total number of lines in the screen buffer
	// TODO this should be 284 for PAL and 235 for NTSC, note that these are visible rasterlines
	static const uint16_t TOTAL_SCREEN_HEIGHT = NTSC_RASTERLINES;
		
private:
	//! VIC I/O Memory
	/*! If a value is poked to the VIC address space, it is stored here. */
	uint8_t iomem[1 + VIC_END_ADDR - VIC_START_ADDR];

	//! All 16 color codes in an array
	uint32_t colors[16];

	//! Temporary space for display characters
	/*! Every 8th rasterline, the VIC chips performs a DMA access and fills the array with the characters to display */
	uint8_t characterSpace[40];

	//! Temporary space for display colors
	/*! Every 8th rasterline, the VIC chips performs a DMA access and fills the array with the characters to display */
	uint8_t colorSpace[40];
	
	//! Screen area that is currently not covered by the border 
	inline int xStart() { return numberOfColumns() == 40 ? 24 : 31; }
	inline int xEnd() { return numberOfColumns() == 40 ? 343 : 334; }
	inline int yStart() { return numberOfRows() == 25 ? 51 : 55; }
	inline int yEnd() { return numberOfRows() == 25 ? 250 : 246; }
	
		
	//! Reference to the connected CPU. 
	/*! Use \a setCPU to set the value during initialization. \a cpu is "write once".
		The VIC chip needs to know about the CPU for sending interrupt requests.
		
		\warning The variable is "write once".
		\todo The interrupt mechanism needs to be implemented.
	*/
	CPU *cpu;

	//! Reference to the connected virtual memory
	/*! Use \a setMemory to set the value during initialization. 
		The VIC chip needs to know about the virtual memory for getting the characters to display.

		\warning The variable is "write once".
		*/	
	C64Memory *mem;
	
	//! Determines whether sprites are drawn or not
	/*! During normal emulation, the value is always true. For debugging purposes, the value can be set to false.
		In this case, sprites are no longer drawn.
	*/
	bool drawSprites;

	//! Enable sprite-sprite collision
	/*! If set to true, the virtual VIC chips checks for sprite-sprite collision as the original C64 does.
		For debugging purposes and cheating, collision detection can be disabled by setting the variabel to false.
		Collision detection can be enabled or disabled for each sprite seperately.
	*/
	bool spriteSpriteCollisionEnabled[8];
	
	//! Enable sprite-background collision
	/*! If set to true, the virtual VIC chips checks for sprite-background collision as the original C64 does.
		For debugging purposes and cheating, collision detection can be disabled by setting the variabel to false.
		Collision detection can be enabled or disabled for each sprite seperately.
	*/
	bool spriteBackgroundCollisionEnabled[8];
	
	//! Determines whether IRQ lines will be made visible.
	/*! Each rasterline that will potentially trigger a raster IRQ is highlighted. This feature is useful for
		debugging purposes as it visualizes how the screen is divided into multiple parts. 
	*/
	bool markIRQLines;

	//! Determines whether DMA lines will be made visible.
	/*! Each rasterline in which the vic will read additional data from the memory and stun the CPU is made visible.
		Note that partial DMA lines may not appear.
	 */	
	bool markDMALines;
	
	//! Determines, if DMA lines (bad lines) can occurr within the current frame
	/*! The value of this flag is determined in rasterline 30, by checking Bit 4 of the VIC control register */
	bool dmaLinesEnabled;
		
	//! First screen buffer
	/*! The VIC chip writes it output into this buffer. The contents of the array is later copied into to
		texture RAM of your graphic card by the drawRect method in the OpenGL related code. */
	int screenBuffer1[512 * 512]; // [TOTAL_SCREEN_WIDTH * TOTAL_SCREEN_HEIGHT];

	//! Second screen buffer
	/*! The VIC chip uses double buffering. Once a frame is drawn, the VIC chip writes the next frame to the second buffer */
	int screenBuffer2[512 * 512]; // [TOTAL_SCREEN_WIDTH * TOTAL_SCREEN_HEIGHT];
		
	//! Currently used screen buffer
	/*! The variable points either to screenBuffer1 or screenBuffer2 */
	int *currentScreenBuffer;
	
	//! Pixel buffer
	/*! The pixel buffer is used for drawing a single line on the screen. When a sreen line is drawn, the pixels
		are first written in the pixel buffer. When the whole line is drawn, it is copied into the screen buffer.
	*/
	int pixelBuffer[TOTAL_SCREEN_WIDTH];
	// int *pixelBuffer;
	
	//! Pointer into the screen buffer
	/*! The variable points to the first pixel of the current rasterline in the screen buffer. */
	// int *pixelBuffer;
	
	//! Z buffer
	/*! The z Buffer is used for drawing a single line on the screen. A pixel is only written to the screen buffer,
		if it is closer to the view point. The depth of the closest pixel is kept in the z buffer. The lower the value
		of the z buffer, the closer it is to the viewer.
		The z buffer is cleared before a new rasterline is drawn.
	*/
	// uint8_t zBuffer[TOTAL_SCREEN_WIDTH];
	int zBuffer[TOTAL_SCREEN_WIDTH];

	//! Indicates the source of a drawn pixel
	/*! Whenever a foreground pixel or sprite pixel is drawn, a distinct bit in the pixelSource array is set.
		The information is utilized to detect sprite-sprite and sprite-background collisions. 
	*/
	// uint8_t pixelSource[TOTAL_SCREEN_WIDTH];
	int pixelSource[TOTAL_SCREEN_WIDTH];
	
	//! Number of the frame currently drawn
	/*! After a frame is completet, the frame number is incremented by one */
	uint64_t frame;
	
	//! Number of the next screen line to be drawn
	/*! Right now, drawing to the border is not supported. Therefore, the value is always in the range
		from 0 to SCREEN_HEIGHT-1. */
	uint32_t scanline;
	
	//! Number of the last scanline to draw
	/*! When the rasterline reaches this value, it is reset to 0. 
		The value varies depending on the emulated C64 model. The American NTSC version draws 263 rasterlines whereas the
		European PAL version draws 312 rasterlines per frame. */
	uint32_t lastScanline;
	
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
	uint16_t screenMemoryAddr;
	
	//! Physical start address of the screen memory.
	/*! The physical memory address is stored only to improve efficiency. */
	uint8_t *screenMemory;

	//! Start address of the sprite pointer memory
	/*! The next 8 bytes starting at this address contain the sprite pointers of all 8 sprites.
		The sprite pointers are needed to compute the start address of the sprite bitmaps in memory. */
	uint8_t *spriteMemory;
	
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
	
	//! Read sprite pointer
	//inline void readSpritePtr(int nr) 
	//	{ read_byte(matrix_base | 0x03f8 | num) << 6; }

	//! Read sprite pointer
	/*! Determines the start adress of sprite data and stores the value into spritePtr */
	inline void readSpritePtr(int sprite)
		{ spritePtr[sprite] = bankAddr + ((uint16_t)spriteMemory[sprite] << 6); }
	
	//! Read sprite data into shift register
	//inline void readSpriteData(int sprite, int byte) 
	//	{ if (spriteDmaOnOff & (1 << sprite)) { spriteShiftReg[sprite][byte] = mem->ram[getSpriteData(sprite)+mc[sprite]; }}
	
	inline void readSpriteData(int sprite) 
		{ if (spriteDmaOnOff & (1 << sprite)) { spriteShiftReg[sprite][mc[sprite]%3] = mem->ram[spritePtr[sprite]+mc[sprite]]; mc[sprite]++; }}

	//! Expansion flipflop
	/*! Used to handle Y sprite stretching. One bit for each sprite */
	uint8_t expansionFF;
	
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
	uint16_t characterMemoryAddr;
	
	//! Physical start address of the character memory.
	/*! The variable can point into the RAM or ROM of the virtual machine.
	    The physical memory address is stored only to improve efficiency. */
	uint8_t *characterMemory;
		
	//! Draw a single character line (8 pixels) in single-color mode
	/*! \param offset X coordinate of the first pixel to draw
		\param pattern Bitmap of the character row to draw
		\param fgcolor Foreground color in RGBA format
		\param bgcolor Background color in RGBA format
	*/		
	void drawSingleColorCharacter(int offset, uint8_t pattern, int fgcolor, int bgcolor);
		
	//! Draw a single character line (8 pixels) in multi-color mode
	/*! \param offset X coordiate of the first pixel to draw
		\param pattern Bitmap of the character row to draw
		\param colorLookup Four element array containing the different colors in RGBA format
	*/
	void drawMultiColorCharacter(int offset, uint8_t pattern, int *colorLookup);
	
	//! Draw a single foreground pixel
	/*! \param offset X coordinate of the pixel to draw
		\param color Pixel color in RGBA format
	*/
	void setForegroundPixel(int offset, int color);

	//! Draw a single foreground pixel
	/*! \param offset X coordinate of the pixel to draw
		\param color Pixel color in RGBA format
	*/
	void setBackgroundPixel(int offset, int color);

	//! Clear a pixel
	/*! \param offset X coordinate of the pixel to clear
		\param color Pixel color in RGBA format
		*/
	// inline void VIC::clearPixel(int offset, int color);
	
	//! Draw a single foreground pixel
	/*! \param offset X coordinate of the pixel to draw
		\param color Pixel color in RGBA format
		\param nr Number of sprite (0 to 7)
		\note The function may trigger an interrupt, if a sprite/sprite or sprite/background collision is detected
	*/
	void setSpritePixel(int offset, int color, int nr);
	
	//! Helper function for the execute method
	/*! Will draw the bitmap of the specified sprite into the screen buffer. */
	void drawSprite(uint8_t nr);
	
	//! Return true if the screen contents is visible, aslo known as DEN bit
	/*! If the screen is off, the whole area will be covered by the border color.
	 The technical documentation calls this the DEN (display enable?) bit. */
	inline bool isVisible() { return iomem[0x11] & 0x10; }
			
	//! internal VIC register, 10 bit video counter
	uint16_t registerVC;
	//! internal VIC-II register, 10 bit video counter base
	uint16_t registerVCBASE; 
	//! internal VIC-II register, 3 bit row counter
	uint8_t registerRC;
	//! internal VIC-II register, 6 bit video matrix line index
	uint8_t registerVMLI; 
	
	//! the current cycle of the current raster line.
	/*! To stay consistent with the documentation the cycles start with 1 and go up to 63(PAL) or 65(NTSC). */
	uint8_t cycle;
	
	//! This dertermines if a dma (bad line)  condidion is present
	bool dmaLine;
	
	//! Display State
	/*! The VIC is either in idle or display state */
	bool displayState;
	
	//! main frame Flipflop
	bool mainFrameFF;
	
	//! vertiacl frame Flipflop
	bool verticalFrameFF;
	
	//! 
	bool drawVerticalFrame;
	
	//!
	bool drawHorizontalFrame;
	
	//! the internal x counter of the sequencer
	uint16_t xCounter;
	
	//! Returns the state of the CSEL register
	inline bool isCSEL() { return iomem[0x16] & 8; }
	
	//! Returns the state of the RSEL register
	inline bool isRSEL() { return iomem[0x11] & 8; }
	
	//! Sprite on off
	/*! Determines if a sprite needs to be drawn in the current rasterline. Each bit represents a single sprite. */
	 uint8_t spriteOnOff, oldSpriteOnOff; 
	
	//! Sprite DMA on off
	/*! Determines  if sprite dma access is enabled or disabled. Each bit represents a single sprite. */
	uint8_t spriteDmaOnOff;
	
	
	//! cycles per rasterline
	uint16_t cyclesPerRasterline;
	
	//! rasterlines per frame
	uint16_t rasterlinesPerFrame;
	
	//! implementation of the VIC register behaviour
	void updateRegisters0();
	
	//! implementation of the VIC register behaviour
	void updateRegisters1();
	
	//! the g-access of the VIC, this is the cycle based counterpart to drawInnerScanline
	void gAccess();
	
	//! if enabled, marks the current scanline on certain conditions, like interrupts or something else
	void markLine(int start, int end, int color);
	
	//! draws the frame to the pixelbuffer of the current scanline
	void drawFrame();
	
	//! draws the sprites to the pixelbuffer of the current scanline
	/*! returns the number of addidional dead cycles required by the vic */
	int drawSpritesM();

public:
	
	//! Sets the VIC-II emulation to PAL
	void setPAL();
	
	//! Sets the VIC-II emulation to PAL
	void setNTSC();	
	
	//! Returns true if the specified address lies in the VIC I/O range
	static inline bool isVicAddr(uint16_t addr)	{ return (VIC_START_ADDR <= addr && addr <= VIC_END_ADDR); }
	
	//! Constructor
	VIC();

	//! Destructor
	~VIC();

	//! Reset the VIC chip to its initial state
	void reset();

	//! Get current scanline
	inline uint16_t getScanline() { return scanline; }
	
	//! Set scanline
	/*! Should only be invoked for debugging purposes */
	void setScanline(uint16_t line) { scanline = line; }

	//! Get current frame
	inline uint64_t getFrame() { return frame; }
	
	//! Set frame
	/*! Should only be invoked for debugging purposes */
	void setFrame(uint64_t f) { frame = f; }
	
	//! Load internal state from a file
	/*! The function is only used for loading a snapshot file. 
		\param file File handle of the snaphshot file. */
	bool load(FILE *file);
	
	//! Save internal state from a file
	/*! The function is only used for saving a snapshot file. 
		\param file File handle of the snaphshot file. */
	bool save(FILE *file);
	
	//! Bind the VIC chip to the specified CPU.
	/*! The binding is irreversible and the function "call once". */
	void setCPU(CPU *c) { assert(cpu == NULL); cpu = c; }

	//! Bind the VIC chip to the specified virtual memory.
	/*! The binding is irreversible and the function "call once". */
	void setMemory(C64Memory *m) { assert(mem == NULL); mem = m; }

	//! Get color
	/*! Returns the rgb color code of the specified color scheme */
	void getColor(ColorScheme scheme, int nr, uint8_t *r, uint8_t *g, uint8_t *b);
	
	//! Set color
	/*! Changes the RGB value of one of the 16 colors */
	void setColor(int nr, uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xff);
	
	//! Set color scheme
	/*! Changes the RGB values of all 16 colors */
	void setColorScheme(ColorScheme scheme);
	
	//! Toggles the values of variable \a drawSprites 
	/*! \see drawSprites */
	void toggleDrawSprites() { drawSprites = !drawSprites; }
	
	//! Enable or disable sprite-sprite collision detection
	inline bool getSpriteSpriteCollision(uint8_t nr) { return spriteSpriteCollisionEnabled[nr]; }
	inline void setSpriteSpriteCollision(uint8_t nr, bool b) { assert(nr < 8); spriteSpriteCollisionEnabled[nr] = b; }
	inline void toggleSpriteSpriteCollisionFlag(uint8_t nr) { setSpriteSpriteCollision(nr, !getSpriteSpriteCollision(nr)); }
	
	//! Enable or disable sprite-background collision detection
	bool getSpriteBackgroundCollision(uint8_t nr) { return spriteBackgroundCollisionEnabled[nr]; }
	void setSpriteBackgroundCollision(uint8_t nr, bool b) { assert(nr < 8); spriteBackgroundCollisionEnabled[nr] = b; }
	inline void toggleSpriteBackgroundCollisionFlag(uint8_t nr) { setSpriteBackgroundCollision(nr, !getSpriteBackgroundCollision(nr)); }

	//! Toggles the values of variable \a markIRQLines 
	/*! \see markIRQLines */
	void toggleMarkIRQLines() { markIRQLines = !markIRQLines; }
	
	//! Toggles the value of variable \a markDMALines 
	/*! \see markDMALines */
	void toggleMarkDMALines() { markDMALines = !markDMALines; }

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

	//! Returns true, if the specified rasterline is a DMA line
	/*! Every eigths row, the VIC chip performs a DMA access and fetches data from screen memory and color memory
		The first DMA access occurrs within lines 0x30 to 0xf7 and  */
	inline bool isDMALine() { return scanline >= 0x30 && scanline <= 0xf7 && (scanline & 7) == getVerticalRasterScroll(); }	
	//  inline bool isDMALine(uint16_t scanline) { return (scanline >= 0x30 && scanline <= 0xf7) ? (scanline % 8) == getVerticalRasterScroll() : false; }
		
	//! Returns the vertical raster scroll offset (0 to 7)
	/*! The vertical raster offset is usally used by games for smoothly scrolling the screen */
	inline int getVerticalRasterScroll() { return iomem[0x11] & 7; }

	//! Set vertical raster scroll offset (0 to 7)
	inline void setVerticalRasterScroll(int offset) { iomem[0x11] = (iomem[0x11] & 0xF8) | (offset & 0x07); }
		
	//! Returns the horizontal raster scroll offset (0 to 7)
	/*! The vertical raster offset is usally used by games for smoothly scrolling the screen */
	inline int getHorizontalRasterScroll() { return iomem[0x16] & 7; }
	
	//! Set horizontan raster scroll offset (0 to 7)
	inline void setHorizontalRasterScroll(int offset) { iomem[0x16] = (iomem[0x16] & 0xF8) | (offset & 0x07); }

	//! Returns the number of rows to be drawn (24 or 25)
	inline int numberOfRows() { return (iomem[0x11] & 8) ? 25 : 24; }

	//! Set the number of rows to be drawn (24 or 25)
	inline void setNumberOfRows(int rows) 
	{ assert(rows == 24 || rows == 25); if (rows == 25) iomem[0x11] |= 0x8; else iomem[0x11] &= (0xff - 0x8); }
	
	
	//! Get the current screen geometry
	ScreenGeometry VIC::getScreenGeometry(void);
	
	//! Set the screen geometry 
	void setScreenGeometry(ScreenGeometry mode);
	
	//! Returns the row number for a given rasterline
	inline uint8_t getRowNumberForRasterline(uint16_t line) { return (line - BORDER_HEIGHT + 3 - getVerticalRasterScroll()) / 8; }
	// >> 3?
	
	//! Returns the character row number for a given rasterline
	inline uint8_t getRowOffsetForRasterline(uint16_t line) { return (line - BORDER_HEIGHT + 3 - getVerticalRasterScroll()) % 8; }
	// & 7? 
	
	//! Return the number of columns to be drawn (38 or 40)
	inline int numberOfColumns() { return (iomem[0x16] & 8) ? 40 : 38; }

	//! Set the number of columns to be drawn (38 or 40)
	inline void setNumberOfColumns(int columns) 
	{ assert(columns == 38 || columns == 40); if (columns == 40) iomem[0x16] |= 0x8; else iomem[0x16] &= (0xff - 0x8); }
	
	//! Return true in text mode, false in bitmap mode
	//inline bool isBitmapMode() { return iomem[0x11] & 32; }

	//! Return true in multicolor mode, false otherwise
	//inline bool isMulticolorMode() { return iomem[0x16] & 16; }

	//! Return true in extended background mode
	//inline bool isExtendedBackgroundColorMode() { return iomem[0x11] & 64; }

	//! Returns the currently set display mode
	/*! The display mode is determined by Bit 5 and Bit 6 of control register 1 and Bit 4 of control register 2.
		To enable a fast handling, we put the bits together into a single integer value. */
	inline DisplayMode getDisplayMode() 
	{ return (DisplayMode)((iomem[0x11] & 0x60) | (iomem[0x16] & 0x10)); }

	//! Set display mode
	inline void setDisplayMode(DisplayMode m) 
	{ iomem[0x11] = (iomem[0x11] & (0xff - 0x60)) | (m & 0x60); iomem[0x16] = (iomem[0x16] & (0xff-0x10)) | (m & 0x10); }

	//! Return true, if the provided bit pattern represents a valid display mode
//	inline bool isValidDisplayMode(DisplayMode m) 
//	{ return (m == STANDARD_TEXT) || (m == MULTICOLOR_TEXT) || (m == STANDARD_BITMAP) || (m == MULTICOLOR_BITMAP) || (m == EXTENDED_BACKGROUND_COLOR); }
	
	 //! Returns the start address of the bitmap data of a sprite
	/*! \param nr Number of sprite (0 to 7) */
	//inline uint16_t getSpriteData(uint8_t nr) { assert(nr < 8); return bankAddr + ((uint16_t)spriteMemory[nr] << 6); }

	//! Returns the X position of a sprite
	/* \param nr Number of sprite (0 to 7) */
	inline uint16_t getSpriteX(uint8_t nr) { return iomem[2*nr] + (iomem[0x10] & (1 << nr) ? 256 : 0); }
	inline void setSpriteX(uint8_t nr, uint16_t x) { poke(2*nr, x & 0xFF); if (x > 0xFF) poke(0x10, peek(0x10) | (1 << nr)); else poke(0x10, peek(0x10) & ~(1 << nr)); }
	
	//! Returns the Y position of a sprite
	/* \param nr Number of sprite (0 to 7) */
	inline uint8_t getSpriteY(uint8_t nr) { return iomem[1+2*nr]; }
	inline void setSpriteY(uint8_t nr, int y) { poke(1+2*nr, y); }
	
	//! Returns true, if sprite is enabled
	/*! Only enabled sprites will be drawn to the screen
		\param nr Number of sprite (0 to 7) */
	// TODO: CHANGE NAME TO sprite_X_VisibilityFlag
	inline bool spriteIsEnabled(uint8_t nr) { return iomem[0x15] & (1 << nr); }		
	inline void setSpriteEnabled(uint8_t nr, bool b) { if (b) poke(0x15, peek(0x15) | (1 << nr)); else poke(0x15, peek(0x15) & (0xFF - (1 << nr))); }
    inline void toggleSpriteEnabled(uint8_t nr) { setSpriteEnabled(nr, !spriteIsEnabled(nr)); }
	
	//! Return the raster that is supposed to trigger an interrupt
	inline uint16_t rasterInterruptLine() { return ((iomem[0x11] & 128) << 1) + iomem[0x12]; }
	inline void setRasterInterruptLine(uint16_t line) { iomem[0x12] = line & 0xFF; if (line > 0xFF) iomem[0x10] |= 0x80; else iomem[0x10] &= 0x7F; }
	
	//! Returns true, iff an interrupt will be triggered when a certain rasterline is reached.
	/*! The VIC chip compares the current rasterline value with register D012 and bit 7 of register D011 */
	inline bool rasterInterruptEnabled() { return iomem[0x1A] & 1; }
	inline void setRasterInterruptEnable(bool b) { if (b) iomem[0x1A] |= 0x01; else iomem[0x1A] &= 0xFE; }
	inline void toggleRasterInterruptFlag() { setRasterInterruptEnable(!rasterInterruptEnabled()); }

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

	//! Returns true, iff sprite is drawn behind the scenary. 
	/*! Otherwise, it will be drawn in front. 
		\param nr Number of sprite (0 to 7) */
	inline bool spriteIsDrawnInBackground(uint8_t nr) { return iomem[0x1B] & (1 << nr); }
	inline void setSpriteInBackground(uint8_t nr, bool b) 
		{ if (b) poke(0x1B, peek(0x1B) | (1 << nr)); else poke(0x1B, peek(0x1B) & ~(1 << nr)); }
	inline void spriteToggleBackgroundPriorityFlag(uint8_t nr)
		{ setSpriteInBackground(nr, !spriteIsDrawnInBackground(nr)); }
	
	//! Returns true, iff sprite is a multicolor sprite
	/* \param nr Number of sprite (0 to 7) */
	inline bool spriteIsMulticolor(uint8_t nr) { return iomem[0x1C] & (1 << nr); }
	inline void setSpriteMulticolor(uint8_t nr, bool b) { if (b) poke(0x1C, peek(0x1C) | (1 << nr)); else poke(0x1C, peek(0x1C) & ~(1 << nr)); }
	inline void toggleMulticolorFlag(uint8_t nr)
		{ setSpriteMulticolor(nr, !spriteIsMulticolor(nr)); }
		
	//! Returns true, if the sprite's height is doubled
	/* \param nr Number of sprite (0 to 7) */
	inline bool spriteHeightIsDoubled(uint8_t nr) { return iomem[0x17] & (1 << nr); }	
	inline void setSpriteStretchY(uint8_t nr, bool b) 
		{ if (b) poke(0x17, peek(0x17) | (1 << nr)); else poke(0x17, peek(0x17) & ~(1 << nr)); }
	inline void spriteToggleStretchYFlag(uint8_t nr) 
		{ setSpriteStretchY(nr, !spriteHeightIsDoubled(nr)); }

	//! Returns true, if the sprite's width is doubled
	/* \param nr Number of sprite (0 to 7) */
	inline bool spriteWidthIsDoubled(uint8_t nr) 
		{ return iomem[0x1D] & (1 << nr); }
	inline void setSpriteStretchX(uint8_t nr, bool b) 
		{ if (b) poke(0x1D, peek(0x1D) | (1 << nr)); else poke(0x1D, peek(0x1D) & (0xFF - (1 << nr))); }
	inline void spriteToggleStretchXFlag(uint8_t nr) 
		{ setSpriteStretchX(nr, !spriteWidthIsDoubled(nr)); }

	//! Returns true, iff sprite collides with another sprite
	/* \param nr Number of sprite (0 to 7) */
	inline bool spriteCollidesWithSprite(uint8_t nr) { return iomem[0x1E] & (1 << nr); }

	//! Returns true, iff sprite collides with background
	/* \param nr Number of sprite (0 to 7) */
	inline bool spriteCollidesWithBackground(uint8_t nr) { return iomem[0x1F] & (1 << nr); }

	//! Returns color code of the border color 
	inline uint8_t getBorderColor() { return iomem[0x20] & 0x0F; }

	//! Returns color code of the background color 
	inline uint8_t getBackgroundColor() { return iomem[0x21] & 0x0F; }

	//! Returns color code of one of the extra background colors
	inline uint8_t getExtraBackgroundColor(int offset) { 
		assert(offset >= 0 && offset < 4);
		return iomem[0x21 + offset] & 0x0F; 
	}

	//! returns the character pattern for the current cycle
	inline uint8_t getCharacterPattern() 
		{ return characterMemory[(characterSpace[registerVMLI] << 3) | registerRC]; }
	
	//! returns the extended character pattern for the current cycle
	inline uint8_t getExtendedCharacterPattern()
		{ return characterMemory[((characterSpace[registerVMLI] & 0x3f) << 3 )  | registerRC]; }
	
	//! returns the bitmap pattern for the current cycle
	inline uint8_t getBitmapPattern() 
		{ return characterMemory[(registerVC << 3) | registerRC];	}
	
	//! This method returns the pattern for a idle access. 
	/*! This is iportant for the Hyperscreen and FLD effects (maybe others as well). */
	uint8_t getIdleAccessPattern();
	
	//! Returns color code of multicolor sprites (extra color 1)
	inline uint8_t spriteExtraColor1() { return iomem[0x25] & 0x0F; }

	//! Returns color code of multicolor sprites (extra color 2)
	inline uint8_t spriteExtraColor2() { return iomem[0x26] & 0x0F; }

	//! Returns color code of sprite
	/*! \param nr Number of sprite (0 to 7) */	
	inline uint8_t spriteColor(uint8_t nr) { return iomem[0x27 + nr] & 0x0F; }
	void setSpriteColor(uint8_t nr, uint8_t color) { assert(nr < 8); iomem[0x27 + nr] = color; }
	
	//! Return depth of sprite
	/*! The value is written to the z buffer to resolve overlapping pixels */
	inline uint8_t spriteDepth(uint8_t nr) { return spriteIsDrawnInBackground(nr) ? 0xf0 + nr : nr; }
		
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
	
	//! Trigger a VIC interrupt
	/*! \param source Interrupt source */
	void triggerIRQ(uint8_t source);

	//! Set rasterline
	/*! Called within the execute method. Updates internal values. The function also triggers a rasterline interrupt,
		if the current line matches the interrupt target. */
	void setRasterline(int line);
	
	//! Prepare for new frame
	/*! This function is called prior to cycle 1 of rasterline 0 */
	void beginFrame();
	
	//! Prepare for new rasterline
	/*! This function is called prior to cycle 1 at the beginning of each rasterline */
	void beginRasterline(uint16_t rasterline);
	
	//! Execute one VIC cycle
	/*! Cycle based emulation routine, for better emulation. Additionally, all things executeOneLine did are
		done as well. 
		\param line Scanline (starting with 0)
		\param cycle Cycle to perform (starting with 1)
	*/
	void executeOneCycle(uint16_t cycle);

	void dumpState();	
};

#endif

