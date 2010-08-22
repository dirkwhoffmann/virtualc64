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

/* Cycle accurate VIC II emulation.
   Mostly based on the extensive VIC II documentation by Christian Bauer. Thanks, Christian! */

#include "C64.h"


// Update value of the display variable according to the dma line condition 
#define update_display if (dmaLine) { displayState = true; }

// Update value of the display variable and the BA line according to the dma line condition 
#define update_display_and_ba if (dmaLine) { displayState = true; pullDownBA(0x100); }


VIC::VIC()
{
	debug(2, "  Creating VIC at address %p...\n", this);

	name = "VIC";
	c64 = NULL;
	cpu = NULL;
	mem = NULL;
	
	// Delete screen buffers
	for (unsigned i = 0; i < sizeof(screenBuffer1) / sizeof(int); i++) {
		screenBuffer1[i] = colors[BLUE];
	}
	for (unsigned i = 0; i < sizeof(screenBuffer2) / sizeof(int); i++) {
		screenBuffer2[i] = colors[BLUE];
	}
	currentScreenBuffer = screenBuffer1;
	pixelBuffer = currentScreenBuffer;
	
	// Initialize colors
	setColorScheme(CCS64);

	// Start with all debug options disabled
	markIRQLines = false;
	markDMALines = false;
}

VIC::~VIC()
{
}

void 
VIC::reset() 
{
	debug(2, "  Resetting VIC...\n");
		
	// Internal registers
	scanline = 0;
	xCounter = 0;
	registerVC = 0;
	registerVCBASE = 0;
	registerRC = 0;
	registerVMLI = 0;
	dmaLine = false;
	dmaLinesEnabled = false;
	displayState = false;
	BAlow = 0;
	mainFrameFF = false;
	verticalFrameFF = false;
	drawVerticalFrame = false;
	drawHorizontalFrame = false;
	
	// Memory
	memset(iomem, 0x00, sizeof(iomem));
	iomem[0x20] = LTBLUE; // Let the border color look correct right from the beginning
	iomem[0x21] = BLUE;   // Let the background color look correct right from the beginning
	iomem[0x11] = 0x10;   // Make screen visible from the beginning	
	bankAddr = 0;
	screenMemoryAddr = 0x0000;
	characterMemoryAddr = 0x0000;
	characterMemoryMappedToROM = false;
	
	// Sprites
	for (int i = 0; i < 8; i++) {
		mc[i] = 0;
		mcbase[i] = 0;
		spriteShiftReg[i][0] = 0;
		spriteShiftReg[i][1] = 0;
		spriteShiftReg[i][3] = 0; 
	}
	spriteOnOff = 0;
	oldSpriteOnOff = 0;
	spriteDmaOnOff = 0;
	expansionFF = 0xff;
	
	// Lightpen
	lightpenIRQhasOccured = false;
	
	// Debugging	
	drawSprites = true;
	for (int i = 0; i < PAL_RASTERLINES; i++) {
		rasterlineDebug[i] = -1;
	}
	spriteSpriteCollisionEnabled = 0xFF;
	spriteBackgroundCollisionEnabled = 0xFF;
}
			
// Loading and saving snapshots
bool
VIC::load(uint8_t **buffer)
{
	debug(2, "  Loading VIC state...\n");
	
	// Internal registers
	scanline = read32(buffer);
	xCounter = read16(buffer);
	registerVC = read16(buffer);
	registerVCBASE = read16(buffer);
	registerRC = read16(buffer);
	registerVMLI = read16(buffer);
	dmaLine = (bool)read8(buffer);
	dmaLinesEnabled = (bool)read8(buffer);
	displayState = (bool)read8(buffer);
	BAlow = (bool)read8(buffer);
	mainFrameFF = (bool)read8(buffer);
	verticalFrameFF = (bool)read8(buffer);
	drawVerticalFrame = (bool)read8(buffer);
	drawHorizontalFrame = (bool)read8(buffer);
	
	// Memory
	for (unsigned i = 0; i < sizeof(iomem); i++)
		iomem[i] = read8(buffer);
	bankAddr = read16(buffer);
	screenMemoryAddr = read16(buffer);
	characterMemoryAddr = read16(buffer);
	characterMemoryMappedToROM = (bool)read8(buffer);
	
	// Sprites
	for (int i = 0; i < 8; i++) {
		mc[i] = read8(buffer);
		mcbase[i] = read8(buffer);
		spriteShiftReg[i][0] = read8(buffer);
		spriteShiftReg[i][1] = read8(buffer);
		spriteShiftReg[i][3] = read8(buffer);
	}
	spriteOnOff = read8(buffer);
	oldSpriteOnOff = read8(buffer);
	spriteDmaOnOff = read8(buffer);
	expansionFF = read8(buffer);
	
	// Lightpen
	lightpenIRQhasOccured = (bool)read8(buffer);
	
	return true;
}

bool
VIC::save(uint8_t **buffer)
{
	debug(2, "  Saving VIC state...\n");

	// Internal registers
	write32(buffer, scanline);
	write16(buffer, xCounter);
	write16(buffer, registerVC);
	write16(buffer, registerVCBASE);
	write16(buffer, registerRC);
	write16(buffer, registerVMLI);
	write8(buffer, (uint8_t)dmaLine);
	write8(buffer, (uint8_t)dmaLinesEnabled);
	write8(buffer, (uint8_t)displayState);
	write8(buffer, (uint8_t)BAlow);
	write8(buffer, (uint8_t)mainFrameFF);
	write8(buffer, (uint8_t)verticalFrameFF);
	write8(buffer, (uint8_t)drawVerticalFrame);
	write8(buffer, (uint8_t)drawHorizontalFrame);
	
	// Memory
	for (unsigned i = 0; i < sizeof(iomem); i++)
		write8(buffer, iomem[i]);
	write16(buffer, bankAddr);
	write16(buffer, screenMemoryAddr);
	write16(buffer, characterMemoryAddr);
	write8(buffer, characterMemoryMappedToROM);
	
	// Sprites
	for (int i = 0; i < 8; i++) {
		write8(buffer, mc[i]);
		write8(buffer, mcbase[i]);
		write8(buffer, spriteShiftReg[i][0]);
		write8(buffer, spriteShiftReg[i][1]);
		write8(buffer, spriteShiftReg[i][3]); 
	}
	write8(buffer, spriteOnOff);
	write8(buffer, oldSpriteOnOff);
	write8(buffer, spriteDmaOnOff);
	write8(buffer, expansionFF);
	
	// Lightpen
	write8(buffer, lightpenIRQhasOccured);

	return true;
}

bool
VIC::loadScreenshot(uint8_t **buffer)
{
	uint16_t width;
	uint16_t height;
	
	debug(2, "  Loading VIC screen buffer...\n");
	
	// Load width and height
	width = read16(buffer);
	height = read16(buffer);
	
	// Skip bytes in buffer
	for (int i = 0; i < width * height; i++) 
		(void)read32(buffer);
	
	return true;
}

bool
VIC::saveScreenshot(uint8_t **buffer)
{
	uint16_t width = 512;  // TODO: Only save viewable area of screenbuffer. Value differs between PAL and NTSC machines
	uint16_t height = 512; // TODO: Only save viewable area of screenbuffer. Value differs between PAL and NTSC machines
	
	debug(2, "  Saving VIC screen buffer...\n");
	
	// Write width and height
	write16(buffer, width);
	write16(buffer, height);
	
	// Write currently unused screenbuffer to file
	if (currentScreenBuffer == screenBuffer1) {
		for (int i = 0; i < width * height; i++) 
			write32(buffer, screenBuffer2[i]);
	} else {
		for (int i = 0; i < width * height; i++) 
			write32(buffer, screenBuffer1[i]);
	}
	
	return true;
}

void 
VIC::dumpState()
{
	debug(1, "VIC\n");
	debug(1, "---\n\n");
	debug(1, "     Bank address : %04X\n", bankAddr, bankAddr);
	debug(1, "    Screen memory : %04X\n", screenMemoryAddr);
	debug(1, " Character memory : %04X (%s)\n", characterMemoryAddr, characterMemoryMappedToROM ? "ROM" : "RAM");
	debug(1, "  Text resolution : %d x %d\n", numberOfRows(), numberOfColumns());
	debug(1, "X/Y raster scroll : %d / %d\n", getVerticalRasterScroll(), getHorizontalRasterScroll());
	debug(1, "     Display mode : ");
	switch (getDisplayMode()) {
		case STANDARD_TEXT: 
			debug(1, "Standard character mode\n");
			break;
		case MULTICOLOR_TEXT:
			debug(1, "Multicolor character mode\n");
			break;
		case STANDARD_BITMAP:
			debug(1, "Standard bitmap mode\n");
			break;
		case MULTICOLOR_BITMAP:
			debug(1, "Multicolor bitmap mode\n");
			break;
		case EXTENDED_BACKGROUND_COLOR:
			debug(1, "Extended background color mode\n");
			break;
		default:
			debug(1, "Invalid\n");
	}
	debug(1, "            (X,Y) : (%d,%d) %s %s\n", xCounter, scanline,  dmaLine ? "(DMA line)" : "", dmaLinesEnabled ? "" : "(DMA lines disabled)");
	debug(1, "               VC : %02X\n", registerVC);
	debug(1, "           VCBASE : %02X\n", registerVCBASE);
	debug(1, "               RC : %02X\n", registerRC);
	debug(1, "             VMLI : %02X\n", registerVMLI);
	debug(1, "          BA line : %s\n", BAlow ? "low" : "high");
	debug(1, "      MainFrameFF : %d\n", mainFrameFF);
	debug(1, "  VerticalFrameFF : %d\n", verticalFrameFF);
	debug(1, "      Draw Vframe : %s\n", drawVerticalFrame ? "yes" : "no");
	debug(1, "      Draw Hframe : %s\n", drawHorizontalFrame ? "yes" : "no");	
	debug(1, "     DisplayState : %s\n", displayState ? "on" : "off");
	debug(1, "         SpriteOn : ");
	for (int i = 0; i < 8; i++) debug(1, "%d ", spriteOnOff & (1 << i) != 0);
	debug(1, "\n        SpriteDma : ");
	for (int i = 0; i < 8; i++) debug(1, "%d ", spriteDmaOnOff & (1 << i) != 0 );
	debug(1, "\n      Y expansion : ");
	for (int i = 0; i < 8; i++) debug(1, "%d ", expansionFF & (1 << i) != 0);
	
	debug(1, "\n        IO memory : ");
	for (unsigned i = 0; i < sizeof(iomem); i += 16) {
		for (unsigned j = 0; j < 16; j ++) {
			debug(1, "%02X ", iomem[i + j]);
		}
		debug(1, "\n                    ");
	}
	debug(1, "\n");
}


// -----------------------------------------------------------------------------------------------
//                                         Configuring
// -----------------------------------------------------------------------------------------------

void 
VIC::setPAL()
{ 
	// Nothing to do so far
}

void
VIC::setNTSC()
{
	// Nothing to do so far
}


// -----------------------------------------------------------------------------------------------
//                                         Drawing
// -----------------------------------------------------------------------------------------------

void 
VIC::gAccess()
{
	uint8_t pattern;
	uint8_t fgcolor;
	uint8_t bgcolor;
	int colorLookup[4];
	uint16_t xCoord = xCounter + getHorizontalRasterScroll();
	
	switch (getDisplayMode()) {
		case STANDARD_TEXT:
			pattern   = displayState ? getCharacterPattern() : getIdleAccessPattern();
			fgcolor   = colorSpace[registerVMLI];
			bgcolor   = getBackgroundColor();
			drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[bgcolor]);
			break;
		case MULTICOLOR_TEXT:
			pattern   = displayState ? getCharacterPattern() : getIdleAccessPattern();
			fgcolor   = colorSpace[registerVMLI];
			if (fgcolor & 0x8) {
				colorLookup[0] = colors[getBackgroundColor()];
				colorLookup[1] = colors[getExtraBackgroundColor(1)];
				colorLookup[2] = colors[getExtraBackgroundColor(2)];
				colorLookup[3] = colors[fgcolor & 0x07];
				drawMultiColorCharacter(xCoord, pattern, colorLookup);
			} else {
				drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[getBackgroundColor()]);
			}
			break;
		case STANDARD_BITMAP:
			pattern = displayState ? getBitmapPattern() : getIdleAccessPattern();
			fgcolor = characterSpace[registerVMLI] >> 4;
			bgcolor = characterSpace[registerVMLI] & 0xf;
			drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[bgcolor]);
			break;
		case MULTICOLOR_BITMAP:
			pattern = displayState ? getBitmapPattern() : getIdleAccessPattern();
			colorLookup[0]  = colors[getBackgroundColor()];
			colorLookup[1]  = colors[characterSpace[registerVMLI] >> 4];
			colorLookup[2]  = colors[characterSpace[registerVMLI] & 0x0F];
			colorLookup[3]  = colors[colorSpace[registerVMLI]];			
			drawMultiColorCharacter(xCoord, pattern, colorLookup);
			break;
		case EXTENDED_BACKGROUND_COLOR:
			pattern = displayState ? getExtendedCharacterPattern() : getIdleAccessPattern();
			fgcolor = colorSpace[registerVMLI]; 
			bgcolor = getExtraBackgroundColor(characterSpace[registerVMLI] >> 6);
			if (fgcolor & 0x8) {
				colorLookup[0] = colors[bgcolor];
				colorLookup[1] = colors[getExtraBackgroundColor(1)];
				colorLookup[2] = colors[getExtraBackgroundColor(2)];
				colorLookup[3] = colors[fgcolor & 0x07];
				drawMultiColorCharacter(xCoord, pattern, colorLookup);
			} else {
				drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[getBackgroundColor()]);
			}
			break;		
		case INVALID_DISPLAY_MODE:
			// do nothing (?)
			break;
	}
	
	// VC and VMLI are increased after each g access 
	if (displayState) {
		registerVC++;
		registerVC &= 0x3ff; // 10 bit overflow
		registerVMLI++;
		registerVMLI &= 0x3f; // 6 bit overflow; 	
	}
}

inline void 
VIC::cAccess()
{
	if (dmaLine) {
		characterSpace[registerVMLI] = mem->ram[bankAddr + screenMemoryAddr + registerVC];
		colorSpace[registerVMLI] = mem->peekColorRam(registerVC) & 0xf;
	}
}

inline void 
VIC::drawSingleColorCharacter(int offset, uint8_t pattern, int fgcolor, int bgcolor)
{
	assert(offset >= 0 && offset+7 < TOTAL_SCREEN_WIDTH);
	if (pattern & 128) setForegroundPixel(offset+0, fgcolor); else setBackgroundPixel(offset+0, bgcolor);
	if (pattern & 64)  setForegroundPixel(offset+1, fgcolor); else setBackgroundPixel(offset+1, bgcolor);
	if (pattern & 32)  setForegroundPixel(offset+2, fgcolor); else setBackgroundPixel(offset+2, bgcolor);
	if (pattern & 16)  setForegroundPixel(offset+3, fgcolor); else setBackgroundPixel(offset+3, bgcolor);
	if (pattern & 8)   setForegroundPixel(offset+4, fgcolor); else setBackgroundPixel(offset+4, bgcolor);
	if (pattern & 4)   setForegroundPixel(offset+5, fgcolor); else setBackgroundPixel(offset+5, bgcolor);
	if (pattern & 2)   setForegroundPixel(offset+6, fgcolor); else setBackgroundPixel(offset+6, bgcolor);
	if (pattern & 1)   setForegroundPixel(offset+7, fgcolor); else setBackgroundPixel(offset+7, bgcolor);
}

inline void 
VIC::drawMultiColorCharacter(int offset, uint8_t pattern, int *colorLookup)
{
	int col;
	uint8_t colBits;
	assert(offset >= 0 && offset+7 < TOTAL_SCREEN_WIDTH);
	colBits = (pattern >> 6) & 0x03;
	col = colorLookup[colBits];
	if (colBits & 0x02) {
		setForegroundPixel(offset, col);
		setForegroundPixel(offset + 1, col);
	} else {
		setBackgroundPixel(offset, col);
		setBackgroundPixel(offset + 1, col);
	}
	offset += 2;
	
	colBits = (pattern >> 4) & 0x03;
	col = colorLookup[colBits];
	if (colBits & 0x02) {
		setForegroundPixel(offset, col);
		setForegroundPixel(offset + 1, col);
	} else {
		setBackgroundPixel(offset, col);
		setBackgroundPixel(offset + 1, col);
	}
	offset += 2;
	
	colBits = (pattern >> 2) & 0x03;
	col = colorLookup[colBits];
	if (colBits & 0x02) {
		setForegroundPixel(offset, col);
		setForegroundPixel(offset + 1, col);
	} else {
		setBackgroundPixel(offset, col);
		setBackgroundPixel(offset + 1, col);
	}
	offset += 2;
	
	colBits = (pattern >> 0) & 0x03;
	col = colorLookup[colBits];
	if (colBits & 0x02) {
		setForegroundPixel(offset, col);
		setForegroundPixel(offset + 1, col);
	} else {
		setBackgroundPixel(offset, col);
		setBackgroundPixel(offset + 1, col);
	}	
}

inline void 
VIC::setForegroundPixel(int offset, int color) 
{
	pixelBuffer[offset] = color;
	zBuffer[offset]     = 0x10; //TODO: deprecated
	pixelSource[offset] = 0x80; //
}

inline void 
VIC::setBackgroundPixel(int offset, int color) 
{
	pixelBuffer[offset] = color;
}

inline void 
VIC::setSpritePixel(int offset, int color, int nr) 
{	
	uint8_t mask = (1 << nr);
	
	if (offset >= 0 && offset < TOTAL_SCREEN_WIDTH) {
		int depth = spriteDepth(nr);
		if (depth < zBuffer[offset]) {
			pixelBuffer[offset] = color;
			zBuffer[offset] = depth;
		}
		
		// Check sprite/sprite collision
		if ((spriteSpriteCollisionEnabled & mask) && (pixelSource[offset] & 0x7F)) {
			iomem[0x1E] |= ((pixelSource[offset] & 0x7F) | mask);
			triggerIRQ(4);
		}
		
		// Check sprite/background collision
		if ((spriteBackgroundCollisionEnabled & mask) && (pixelSource[offset] & 0x80)) {
			iomem[0x1F] |= mask;
			triggerIRQ(2);
		}
		
		if (nr < 7)
			pixelSource[offset] |= mask;
	}
}

void inline
VIC::drawAllSprites()
{	
	if (drawSprites) {
		for (int i = 0; i < 8; i++) {
			if (oldSpriteOnOff & (1 << i)) {
				drawSprite(i);
			}				
		}
	}
}

void
VIC::drawSprite(uint8_t nr)
{
	assert(nr < 8);
	
	int spriteX, offset;
	spriteX = getSpriteX(nr);
	
	if (spriteX < 488) 
		offset = spriteX;
	else
		offset = spriteX - 488; 
	
	if (spriteIsMulticolor(nr)) {
		
		int colorLookup[4] = { 
			0x00, 
			colors[spriteExtraColor1()], 
			colors[spriteColor(nr)],
			colors[spriteExtraColor2()]
		};
		
		for (int i = 0; i < 3; i++) {
			uint8_t pattern = spriteShiftReg[nr][i]; 
			
			uint8_t col;
			if (spriteWidthIsDoubled(nr)) {
				col = (pattern >> 6) & 0x03;
				if (col) {
					setSpritePixel(offset, colorLookup[col], nr);
					setSpritePixel(offset+1, colorLookup[col], nr);
					setSpritePixel(offset+2, colorLookup[col], nr);
					setSpritePixel(offset+3, colorLookup[col], nr);
				}
				col = (pattern >> 4) & 0x03;
				if (col) {
					setSpritePixel(offset+4, colorLookup[col], nr);
					setSpritePixel(offset+5, colorLookup[col], nr);
					setSpritePixel(offset+6, colorLookup[col], nr);
					setSpritePixel(offset+7, colorLookup[col], nr);
				}
				col = (pattern >> 2) & 0x03;
				if (col) {
					setSpritePixel(offset+8, colorLookup[col], nr);
					setSpritePixel(offset+9, colorLookup[col], nr);
					setSpritePixel(offset+10, colorLookup[col], nr);
					setSpritePixel(offset+11, colorLookup[col], nr);
				}
				col = pattern & 0x03;
				if (col) {
					setSpritePixel(offset+12, colorLookup[col], nr);
					setSpritePixel(offset+13, colorLookup[col], nr);
					setSpritePixel(offset+14, colorLookup[col], nr);
					setSpritePixel(offset+15, colorLookup[col], nr);
				}				
				offset += 16;
			} else {
				col = (pattern >> 6) & 0x03;
				if (col) {
					setSpritePixel(offset, colorLookup[col], nr);
					setSpritePixel(offset+1, colorLookup[col], nr);
				}
				col = (pattern >> 4) & 0x03;
				if (col) {
					setSpritePixel(offset+2, colorLookup[col], nr);
					setSpritePixel(offset+3, colorLookup[col], nr);
				}
				col = (pattern >> 2) & 0x03;
				if (col) {
					setSpritePixel(offset+4, colorLookup[col], nr);
					setSpritePixel(offset+5, colorLookup[col], nr);
				}
				col = pattern & 0x03;
				if (col) {
					setSpritePixel(offset+6, colorLookup[col], nr);
					setSpritePixel(offset+7, colorLookup[col], nr);
				}				
				offset += 8;
			}
		}
	} else {
		int fgcolor = colors[spriteColor(nr)]; 
		for (int i = 0; i < 3; i++) {
			uint8_t pattern = spriteShiftReg[nr][i]; 
			
			if (spriteWidthIsDoubled(nr)) {
				if (pattern & 128) {
					setSpritePixel(offset, fgcolor, nr);
					setSpritePixel(offset+1, fgcolor, nr);
				}
				if (pattern & 64) {
					setSpritePixel(offset+2, fgcolor, nr);
					setSpritePixel(offset+3, fgcolor, nr);
				}
				if (pattern & 32) {
					setSpritePixel(offset+4, fgcolor, nr);
					setSpritePixel(offset+5, fgcolor, nr);
				}
				if (pattern & 16) {
					setSpritePixel(offset+6, fgcolor, nr);
					setSpritePixel(offset+7, fgcolor, nr);
				}
				if (pattern & 8) {
					setSpritePixel(offset+8, fgcolor, nr);
					setSpritePixel(offset+9, fgcolor, nr);
				}
				if (pattern & 4) {
					setSpritePixel(offset+10, fgcolor, nr);
					setSpritePixel(offset+11, fgcolor, nr);
				}
				if (pattern & 2) {
					setSpritePixel(offset+12, fgcolor, nr);
					setSpritePixel(offset+13, fgcolor, nr);
				}
				if (pattern & 1) {
					setSpritePixel(offset+14, fgcolor, nr);
					setSpritePixel(offset+15, fgcolor, nr);
				}
				offset += 16;
			} else {
				if (pattern & 128) {
					setSpritePixel(offset, fgcolor, nr);
				}
				if (pattern & 64) {
					setSpritePixel(offset+1, fgcolor, nr);
				}
				if (pattern & 32) {
					setSpritePixel(offset+2, fgcolor, nr);
				}
				if (pattern & 16) {
					setSpritePixel(offset+3, fgcolor, nr);
				}
				if (pattern & 8) {
					setSpritePixel(offset+4, fgcolor, nr);
				}
				if (pattern & 4) {
					setSpritePixel(offset+5, fgcolor, nr);
				}
				if (pattern & 2) {
					setSpritePixel(offset+6, fgcolor, nr);
				}
				if (pattern & 1) {
					setSpritePixel(offset+7, fgcolor, nr);
				}
				offset += 8;
			}
		}
	}
}

void inline
VIC::drawHorizontalBorder()
{
	int bcolor = colors[getBorderColor()];
	
	for (int i = 0; i < xStart(); i++) {
		pixelBuffer[i] = bcolor;
	}
	for (int i = xEnd()+1; i < TOTAL_SCREEN_WIDTH; i++) {
		pixelBuffer[i] = bcolor;
	}
}

void inline
VIC::drawVerticalBorder()
{
	int bcolor = colors[getBorderColor()];
	
	for (int i = 0; i < TOTAL_SCREEN_WIDTH; i++) {
		pixelBuffer[i] = bcolor;
	}
}

// -----------------------------------------------------------------------------------------------
//                                       Getter and setter
// -----------------------------------------------------------------------------------------------

uint16_t 
VIC::getMemoryBankAddr()
{
	return bankAddr;
}

void 
VIC::setMemoryBankAddr(uint16_t addr)
{
	assert(addr % 0x4000 == 0);
	
	bankAddr = addr;
	
	// changing the memory bank also affects the start address of the screen and character memory
	setScreenMemoryAddr((iomem[0x18] & 0xF0) << 6);
	setCharacterMemoryAddr((iomem[0x18] & 0x0E) << 10);
}

uint16_t
VIC::getScreenMemoryAddr()
{
	return screenMemoryAddr;
}

void
VIC::setScreenMemoryAddr(uint16_t addr)
{
	assert(addr <= 0x3C00);
	assert(addr % 0x400 == 0);
	screenMemoryAddr = addr;
}

uint16_t 
VIC::getCharacterMemoryAddr()
{
	return characterMemoryAddr % 0x4000;
}

void 
VIC::setCharacterMemoryAddr(uint16_t addr)
{
	assert(addr <= 0x3800);
	assert(addr % 0x800 == 0);
	
	if (bankAddr == 0x0000 || bankAddr == 0x8000) {
		if (addr == 0x1000) {
			characterMemoryMappedToROM = true;
			characterMemoryAddr = 0xD000;
			return;
		}
		if (addr == 0x1800) {
			characterMemoryMappedToROM = true;
			characterMemoryAddr = 0xD800;
			return;
		}
	}
	characterMemoryMappedToROM = false;
	characterMemoryAddr = bankAddr + addr;
}

uint8_t 
VIC::peek(uint16_t addr)
{
	uint8_t result;
	
	assert(addr <= VIC_END_ADDR - VIC_START_ADDR);
	
	switch(addr) {
		case 0x11: // SCREEN CONTROL REGISTER #1
			result = (iomem[addr] & 0x7f) + (scanline > 0xff ? 128 : 0);
			return result;		
		case 0x12: // VIC_RASTER_READ_WRITE
			result = scanline & 0xff;
			return result;
		case 0x13:
			debug(2, "Reading lightpen X position: %d\n", iomem[addr]);
			return iomem[addr];			
		case 0x14:
			debug(2, "Reading lightpen Y position: %d\n", iomem[addr]);
			return iomem[addr];			
		case 0x19:
			result = iomem[addr] | 0x70; // Bits 4 to 6 are not used and always contain "1"
			return result;
		case 0x1A:
			result = iomem[addr] | 0xF0; // Bits 4 to 7 are not used and always contain "1"
			return result;
		case 0x1E: // Sprite-to-sprite collision
			result = iomem[addr];
			iomem[addr] = 0x00;  // Clear on read
			return result;
		case 0x1F: // Sprite-to-background collision
			result = iomem[addr];
			iomem[addr] = 0x00;  // Clear on read
			return result;
	}
	
	if (addr >= 0x20 && addr <= 0x2E) {
		// Color registers: Bits 4 to 7 are not used and always contain "1"
		return iomem[addr] | 0xF0;
	}
	
	if (addr >= 0x2F && addr <= 0x3F) {
		// Unusable register area
		return 0xFF; 
	}
	
	// Default action
	return iomem[addr];
}

void
VIC::poke(uint16_t addr, uint8_t value)
{
	assert(addr <= VIC_END_ADDR - VIC_START_ADDR);
	
	switch(addr) {		
		case 0x11: // CONTROL_REGISTER_1
			if ((iomem[addr] & 0x80) != (value & 0x80)) {
				// Value changed: Check if we need to trigger an interrupt immediately
				iomem[addr] = value;
				if (scanline == rasterInterruptLine())
					triggerIRQ(1);
			} else {
				iomem[addr] = value;
			}
			
			// Bit 4 is the DEN bit and controls whether DMA lines are enabled or disabled. 
			// Note that it is only inspected in rasterline 0x30
			if (scanline == 0x30 && (value & 0x10))
				dmaLinesEnabled = true;
			
			// Bit 0 - 3 determine the vertical scroll offset. By changing these bits, the DMA line condition 
			// can appear or disappear in the middle of a rasterline.
			checkDmaLineCondition();
			return;
			
		case 0x12: // RASTER_COUNTER
			if (iomem[addr] != value) {
				// Value changed: Check if we need to trigger an interrupt immediately
				iomem[addr] = value;
				if (scanline == rasterInterruptLine())
					triggerIRQ(1);
			} else {
				iomem[addr] = value;
			}
			return;
			
		case 0x16:
			iomem[addr] = value | (128 + 64); // The upper two bits are unused and always return 1 when read
			return;
			
		case 0x17:
			iomem[addr] = value;
			expansionFF |= ~value;
			return;
			
		case 0x18: // MEMORY_SETUP_REGISTER
			iomem[addr] = value | 0x01; // Bit 0 is unused and always 1 when read
			setScreenMemoryAddr((value & 0xF0) << 6);
			setCharacterMemoryAddr((value & 0x0E) << 10);
			return;
			
		case 0x19: // IRQ flags
			// A bit is cleared when a "1" is written
			iomem[addr] &= (~value & 0x0f);
			cpu->clearIRQLineVIC();
			if (iomem[addr] & iomem[0x1a])
				iomem[addr] |= 0x80;
			return;
			
		case 0x1a: // IRQ mask
			iomem[addr] = value & 0x0f;
			if (iomem[addr] & iomem[0x19]) {
				iomem[0x19] |= 0x80; // set uppermost bit (is directly connected to the IRQ line)
				cpu->setIRQLineVIC(); 
			} else {
				iomem[0x19] &= 0x7f; // clear uppermost bit
				cpu->clearIRQLineVIC(); 
			}
			return;		
			
		case 0x1E:
		case 0x1F:
			// Writing has no effect
			return;
	}
	
	// Default action
	iomem[addr] = value;
}


// -----------------------------------------------------------------------------------------------
//                                         Properties
// -----------------------------------------------------------------------------------------------

void 
VIC::setScreenGeometry(ScreenGeometry mode)
{
	setNumberOfRows((mode == COL_40_ROW_25 || mode == COL_38_ROW_25) ? 25 : 24);
	setNumberOfColumns((mode == COL_40_ROW_25 || mode == COL_40_ROW_24) ? 40 : 38);
}

VIC::ScreenGeometry 
VIC::getScreenGeometry()
{
	if (numberOfColumns() == 40) {
		if (numberOfRows() == 25)
			return COL_40_ROW_25;
		else
			return COL_40_ROW_24;
	} else {
		if (numberOfRows() == 25)
			return COL_38_ROW_25;
		else
			return COL_38_ROW_24;
	}
}


// -----------------------------------------------------------------------------------------------
//                                DMA lines, BA signal and IRQs
// -----------------------------------------------------------------------------------------------

void 
VIC::pullDownBA(uint16_t source)
{ 
	BAlow |= source;
	cpu->setRDY(BAlow == 0); 
}

void 
VIC::releaseBA(uint16_t source)
{ 
	BAlow &= ~source;
	cpu->setRDY(BAlow == 0); 
}

void 
VIC::triggerIRQ(uint8_t source)
{
	iomem[0x19] |= source;
	if (iomem[0x1A] & source) {
		// Interrupt is enabled
		iomem[0x19] |= 128;
		cpu->setIRQLineVIC();
		// debug("Interrupting at rasterline %x %d\n", scanline, scanline);
	}
}

void
VIC::simulateLightPenInterrupt()
{
	int x, x_max, y;
	
	if (!lightpenIRQhasOccured) {

		// lightpen interrupts can only occur once per frame
		lightpenIRQhasOccured = true;

		// determine current coordinates
		x_max = 8 * c64->getCyclesPerRasterline() - 4;
		if (xCounter >= 16)
			x = xCounter - 12;
		else 
			x = x_max - xCounter;
		y = scanline;
				
		// latch coordinates 
		iomem[0x13] = x / 2; // value equals the current x coordinate divided by 2
		iomem[0x14] = y + 1; // value is based on sprite coordinate system (hence, + 1)

		// Simulate interrupt
		triggerIRQ(0x08);
	}
}

// -----------------------------------------------------------------------------------------------
//                                              Sprites
// -----------------------------------------------------------------------------------------------

void 
VIC::updateSpriteDmaOnOff()
{
	// determine which sprites are displayes in the next rasterline
	for (int i = 0; i < 8; i++) {
		if (spriteIsEnabled(i)) {
			uint8_t y = getSpriteY(i);
			if (y == (scanline & 0xff)) {
				spriteDmaOnOff |= (1 << i);
				mcbase[i] = 0;
				if (spriteHeightIsDoubled(i))
					expansionFF &= ~(1 << i);
			}
		}
	}
}


// -----------------------------------------------------------------------------------------------
//                                    Execution functions
// -----------------------------------------------------------------------------------------------

void 
VIC::beginFrame()
{
	lightpenIRQhasOccured = false; // only one event per frame is permitted
	registerVCBASE = 0;
}

void 
VIC::endFrame()
{
	// Frame complete. Notify listener...
	// c64->putMessage(MSG_DRAW, 0, currentScreenBuffer);
	
	// Switch frame buffer
	currentScreenBuffer = (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1;	
	pixelBuffer = currentScreenBuffer;
}

void 
VIC::beginRasterline(uint16_t line)
{
	scanline = line;
		
	// Clear z buffer. The buffer is initialized with a high, positive value (meaning the pixel is far away)
	memset(zBuffer, 0x7f, sizeof(zBuffer));

	// Clear pixel source
	memset(pixelSource, 0x00, sizeof(pixelSource));		
}

void 
VIC::endRasterline()
{
	// Copy pixel buffer of old line to screen buffer
	// memcpy(currentScreenBuffer + (scanline * TOTAL_SCREEN_WIDTH), pixelBuffer, sizeof(pixelBuffer));
	pixelBuffer += TOTAL_SCREEN_WIDTH;
}

void 
VIC::cycle1()
{
	// Check, if we are currently processing a DMA line. The result is stored in variable dmaLine.
	// Be aware that the value of variable dmaLine can change in the middle of a rasterline as a side effect 
	// of modifying the y scroll offset.
	checkDmaLineCondition();

	// Trigger rasterline interrupt if applicable
	// Note: In line 0, the interrupt is triggered in cycle 2
	if (scanline == rasterInterruptLine() && scanline != 0)
		triggerIRQ(1);
			
	// Determine the value of the DEN bit (in rasterline 0x30 only)
	if (scanline == 0x30)
		dmaLinesEnabled = isVisible();
			
	// Get sprite data address and sprite data of sprite 3
	releaseBusForSprite(2);
	readSpritePtr(3);
	readSpriteData(3);
	countX();
	update_display;
}

void
VIC::cycle2()
{
	// Trigger rasterline interrupt if applicable
	if (scanline == 0 && scanline == rasterInterruptLine())
		triggerIRQ(1);
			
	readSpriteData(3);
	readSpriteData(3);
	requestBusForSprite(5);
	countX();
	update_display;
}

void 
VIC::cycle3()
{
	releaseBusForSprite(3);
	readSpritePtr(4);
	readSpriteData(4);
	countX();
	update_display;
}

void 
VIC::cycle4()
{
	readSpriteData(4);
	readSpriteData(4);
	requestBusForSprite(6);
	countX();
	update_display;
}

void
VIC::cycle5()
{
	releaseBusForSprite(4);
	readSpritePtr(5);
	readSpriteData(5);
	countX();
	update_display;
}

void 
VIC::cycle6()
{
	readSpriteData(5);
	readSpriteData(5);
	requestBusForSprite(7);
	countX();
	update_display;
}

void 
VIC::cycle7()
{
	releaseBusForSprite(5);
	readSpritePtr(6);
	readSpriteData(6);
	countX();
	update_display;
}

void 
VIC::cycle8()
{
	readSpriteData(6);
	readSpriteData(6);
	countX();
	update_display;
}

void 
VIC::cycle9()
{
	releaseBusForSprite(6);
	readSpritePtr(7);
	readSpriteData(7);
	countX();
	update_display;
}

void 
VIC::cycle10()
{
	readSpriteData(7);
	readSpriteData(7);
	countX();
	update_display;
}

void
VIC::cycle11()
{
	releaseBusForSprite(7);
	countX();
	update_display;
}

void
VIC::cycle12()
{
	if (dmaLine) {
		pullDownBA(0x100);
	}
	releaseBusForSprite(7);

	// Reset the X coordinate to 0
	xCounter = 0;
	update_display_and_ba;
}

void
VIC::cycle13()
{
	countX();
	update_display_and_ba;
}

void
VIC::cycle14()
{
	/* In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
	 (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
	 Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt. */
	registerVC = registerVCBASE;
	registerVMLI = 0;
	if (dmaLine) 
		registerRC = 0;
	countX();
	update_display_and_ba;
}

void
VIC::cycle15()
{
	/* In der ersten Phase von Zyklus 15 wird geprüft, ob das
	 Expansions-Flipflop gesetzt ist. Wenn ja, wird MCBASE um 2 erhöht. */
	// Note: Done in cycle 16

	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle16()
{
	if (isCSEL()) mainFrameFF = false;		
			
	/* 8. In der ersten Phase von Zyklus 16 wird geprüft, ob das
	 Expansions-Flipflop gesetzt ist. Wenn ja, wird MCBASE um 1 erhöht.
	 Dann wird geprüft, ob MCBASE auf 63 steht und bei positivem Vergleich
	 der DMA und die Darstellung für das jeweilige Sprite abgeschaltet. */
			
	for (int i = 0; i < 8; i++) {
		uint8_t mask = (1 << i);
		if (expansionFF & mask) {
			mcbase[i] += 3;
			mcbase[i] &= 0x3F; // 6 bit counter
		}
		if (mcbase[i] == 63) {			
			// spriteOnOff &= ~mask;
			spriteDmaOnOff &= ~mask;
		}
	}		
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle17()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle18()
{
	if (!isCSEL()) mainFrameFF = false;
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle19()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle20()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle21()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle22()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle23()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle24()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle25()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle26()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle27()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle28()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle29()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle30()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle31()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle32()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle33()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle34()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle35()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle36()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle37()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle38()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle39()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle40()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle41()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle42()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle43()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle44()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle45()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle46()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle47()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle48()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle49()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle50()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle51()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle52()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle53()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle54()
{
	gAccess();
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle55()
{
	if (!isCSEL()) mainFrameFF = true;
	
	gAccess();
	
	/* In der ersten Phase von Zyklus 55 wird das Expansions-Flipflop
	 invertiert, wenn das MxYE-Bit gesetzt ist. */
	expansionFF ^= iomem[0x17];
			
	releaseBA(0x100);

	/* In den ersten Phasen von Zyklus 55 und 56 wird für jedes Sprite geprüft,
	 ob das entsprechende MxE-Bit in Register $d015 gesetzt und die
	 Y-Koordinate des Sprites (ungerade Register $d001-$d00f) gleich den
	 unteren 8 Bits von RASTER ist. Ist dies der Fall und der DMA für das
	 Sprite noch ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht
	 und, wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
	 */
	updateSpriteDmaOnOff();
	requestBusForSprite(0);
	countX();
	update_display;
}

void
VIC::cycle56()
{
	updateSpriteDmaOnOff();
	requestBusForSprite(0);
	countX();
	update_display;
}

void
VIC::cycle57()
{
	if (isCSEL()) mainFrameFF = true;
	drawHorizontalFrame = mainFrameFF;
	requestBusForSprite(1);
	countX();
	update_display;
}

void
VIC::cycle58()
{
	/* Der Übergang vom Display- in den Idle-Zustand erfolgt in Zyklus 58 einer Zeile, 
	 wenn der RC den Wert 7 hat und kein Bad-Line-Zustand vorliegt.
	 In der ersten Phase von Zyklus 58 wird geprüft, ob RC=7 ist. Wenn ja,
	 geht die Videologik in den Idle-Zustand und VCBASE wird mit VC geladen
	 (VC->VCBASE). Ist die Videologik danach im Display-Zustand (liegt ein
	 Bad-Line-Zustand vor, ist dies immer der Fall), wird RC erhöht. */
	if (displayState && registerRC == 7 && !dmaLine) {
		displayState = false;	
		registerVCBASE = registerVC;	
	}
	if (displayState) {
		registerRC++;
		registerRC &= 7;  // 3 bit overflow
	}
			
	/* In der ersten Phase von Zyklus 58 wird für jedes Sprite MC mit MCBASE
	 geladen (MCBASE->MC) und geprüft, ob der DMA für das Sprite angeschaltet
	 und die Y-Koordinate des Sprites gleich den unteren 8 Bits von RASTER
	 ist. Ist dies der Fall, wird die Darstellung des Sprites angeschaltet. */
	oldSpriteOnOff = spriteOnOff; // remember last value
	for (int i = 0; i < 8; i++) {
		mc[i] = mcbase[i];
		uint8_t mask = (1 << i);
		if (spriteDmaOnOff & mask) {
			uint8_t y = getSpriteY(i);
			if (y == (scanline & 0xff)) 
				spriteOnOff |= mask;
		}
	}
		
	/* Draw rasterline into pixel buffer */
	drawAllSprites();
			
	// switch off sprites if dma is off
	for (int i = 0; i < 8; i++) {
		uint8_t mask = (1 << i);
		if ((spriteOnOff & mask) && !(spriteDmaOnOff & mask))
			spriteOnOff &= ~mask;
	}
			
	readSpritePtr(0);
	readSpriteData(0);
	countX();
}

void
VIC::cycle59()
{
	readSpriteData(0);
	readSpriteData(0);
	requestBusForSprite(2);
	countX();
	update_display;
}

void
VIC::cycle60()
{
	releaseBusForSprite(0);
	readSpritePtr(1);
	readSpriteData(1);
	countX();
	update_display;
}

void
VIC::cycle61()
{
	readSpriteData(1);
	readSpriteData(1);
	requestBusForSprite(3);
	countX();
	update_display;
}

void
VIC::cycle62()
{
	releaseBusForSprite(1);
	readSpritePtr(2);
	readSpriteData(2);
	countX();
	update_display;
}

void
VIC::cycle63()
{			
	// update border flipflops
	if (scanline == 51 && isRSEL() && isVisible()) 
		drawVerticalFrame = verticalFrameFF = false;
	else if (scanline == 55 && !isRSEL() && isVisible()) 
		drawVerticalFrame = verticalFrameFF = false;
	else if (scanline == 247 && !isRSEL()) 
		drawVerticalFrame = verticalFrameFF = true;
	else if (scanline == 251 && isRSEL()) 
		drawVerticalFrame = verticalFrameFF = true;
			
	// draw border
	drawBorder();
			
	// illegal display modes cause a black line to appear
	if (getDisplayMode() > EXTENDED_BACKGROUND_COLOR) markLine(xStart(), xEnd(), colors[BLACK]);

	// draw debug markers
	if (markIRQLines && scanline == rasterInterruptLine()) 
		markLine(0, TOTAL_SCREEN_WIDTH, colors[WHITE]);
	if (markDMALines && dmaLine)	
		markLine(0, TOTAL_SCREEN_WIDTH, colors[RED]);
	if (rasterlineDebug[scanline] >= 0) {
		markLine(0, TOTAL_SCREEN_WIDTH, colors[rasterlineDebug[scanline] % 16]);
		rasterlineDebug[scanline] = -1;
	}		

	readSpriteData(2);
	readSpriteData(2);
	requestBusForSprite(4);
	countX();
	update_display;
}

void
VIC::cycle64()
{
	// NTSC only
	countX();
}

void
VIC::cycle65()
{
	// NTSC only
	countX();
}


// -----------------------------------------------------------------------------------------------
//                                              Debugging
// -----------------------------------------------------------------------------------------------

void inline
VIC::markLine(int start, int end, int color)
{
	for (int i = start; i <= end; i++) {
		pixelBuffer[i] = color;
	}	
}

