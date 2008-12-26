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

VIC::VIC()
{
	debug("  Creating VIC at address %p...\n", this);

	cpu = NULL;
	mem = NULL;
	
	setColorScheme(CCS64);

	// Delete screen buffers
	for (int i = 0; i < sizeof(screenBuffer1) / sizeof(int); i++) {
		screenBuffer1[i] = colors[BLUE];
	}
	for (int i = 0; i < sizeof(screenBuffer2) / sizeof(int); i++) {
		screenBuffer2[i] = colors[BLUE];
	}
	currentScreenBuffer = screenBuffer1;
}

VIC::~VIC()
{
}

void 
VIC::reset() 
{
	debug("  Resetting VIC...\n");
	bankAddr = 0;
	scanline = 0;
	BAlow = 0;
	drawSprites	= true;
	markIRQLines = false;
	markDMALines = false;
	displayState = false;
	drawVerticalFrame = true;
	drawHorizontalFrame = true;
	spriteOnOff = 0;
	spriteDmaOnOff = 0;
	screenMemory = mem->getRam();
	screenMemoryAddr = 0x0000;
	spriteMemory = screenMemory;
	characterMemory = mem->getRam();
	characterMemoryAddr = 0x0000;
	memset(iomem, 0x00, sizeof(iomem));
	for (int i = 0; i < 8; i++) {
		spriteSpriteCollisionEnabled[i] = true;
		spriteBackgroundCollisionEnabled[i] = true;
		mc[i] = 0;
		mcbase[i] = 0;
		spriteShiftReg[i][0] = spriteShiftReg[i][1] = spriteShiftReg[i][3] = 0; 
	}
	expansionFF = 0xff;
	for (int i = 0; i < PAL_RASTERLINES; i++) {
		rasterlineDebug[i] = -1;
	}
	
	// Let the color look correct right from the beginning
	iomem[0x20] = 14; // Light blue
	iomem[0x21] = 6;  // Blue

	// make screen visible from the beginning
	iomem[0x11] |= 0x10;
}
			
// Loading and saving snapshots
bool
VIC::load(FILE *file)
{
	debug("  Loading VIC state...\n");
	scanline = read32(file);
	setMemoryBankAddr(read16(file));
	setScreenMemoryAddr(read16(file));
	setCharacterMemoryAddr(read16(file));
	
	for (int i = 0; i < sizeof(iomem); i++) {
		iomem[i] = read8(file);
	}
	for (int i = 0; i < sizeof(characterSpace); i++) {
		characterSpace[i] = read8(file);
	}
	for (int i = 0; i < sizeof(colorSpace); i++) {
		colorSpace[i] = read8(file);
	}
	// To be removed
	for (int i = 0; i < 8*512; i++) {
		(void)read8(file);
	}		
	return true;
}

bool
VIC::save(FILE *file)
{
	debug("  Saving VIC state...\n");
	write32(file, scanline);
	write16(file, getMemoryBankAddr());
	write16(file, getScreenMemoryAddr());
	write16(file, getCharacterMemoryAddr());
	
	for (int i = 0; i < sizeof(iomem); i++) {
		write8(file, iomem[i]);
	}
	for (int i = 0; i < sizeof(characterSpace); i++) {
		write8(file, characterSpace[i]);
	}
	for (int i = 0; i < sizeof(colorSpace); i++) {
		write8(file, colorSpace[i]);
	}
	// To be removed
	for (int i = 0; i < 8*512; i++) {
		write8(file, 0);
	}	
	return true;
}

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
			debug("Reading lightpen X position: %d\n", iomem[addr]);
			return iomem[addr];			
		case 0x14:
			debug("Reading lightpen Y position: %d\n", iomem[addr]);
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
	screenMemory = &mem->ram[bankAddr + addr];	
	spriteMemory = screenMemory + 0x03F8;
}

uint16_t 
VIC::getCharacterMemoryAddr()
{
	return characterMemoryAddr;
}

void 
VIC::setCharacterMemoryAddr(uint16_t addr)
{
	assert(addr <= 0x3800);
	assert(addr % 0x800 == 0);

	characterMemoryAddr = addr;
	if (bankAddr == 0x0000 || bankAddr == 0x8000) {
		if (addr == 0x1000) {
			characterMemory = &mem->rom[0xD000];
			return;
		}
		if (addr == 0x1800) {
			characterMemory = &mem->rom[0xD800];
			return;
		}
	}
	characterMemory = &mem->ram[bankAddr + addr];
}

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
	assert(nr < 8);	
	if (offset >= 0 && offset < TOTAL_SCREEN_WIDTH) {
		int depth = spriteDepth(nr);
		if (depth < zBuffer[offset]) {
			pixelBuffer[offset] = color;
			zBuffer[offset] = depth;
		}
	
		// Check sprite/sprite collision
		if (spriteSpriteCollisionEnabled[nr] && (pixelSource[offset] & 0x7F)) {
			iomem[0x1E] |= ((pixelSource[offset] & 0x7F) | (1 << nr));
			triggerIRQ(4);
		}
		
		// Check sprite/background collision
		if (spriteBackgroundCollisionEnabled[nr] && (pixelSource[offset] & 0x80)) {
			iomem[0x1F] |= (1 << nr);
			triggerIRQ(2);
		}
		
		if (nr < 7)
			pixelSource[offset] |= (1 << nr);
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
	if (!lightpenIRQhasOccured) {

		// lightpen interrupts can only occur once per frame
		lightpenIRQhasOccured = true;

		// determine current coordinates
		int x = xCounter; // (cycle >= 13) ? (cycle-13) * 8 : (0x19c + cycle*8);
		int y = scanline;
		
		// latch coordinates 
		iomem[0x13] = x / 2; // value equals the current x coordinate divided by 2
		iomem[0x14] = y + 1; // value is based on sprite coordinate system (hence, + 1)

		// Simulate interrupt
		triggerIRQ(0x08);
	}
}

/* 3.7.1. Idle-Zustand/Display-Zustand
 the idle access always reads at $3fff or $39ff when the ECM bit is set.
 here the doc conflicts: the ECM bit is either at $d016 (chap 3.7.1) or $d011 (3.2)
 for now i'm ging with $d011
 wow... this actually seems to work! noticable in the "rbi 2 baseball" intro (return 0 to see difference)
 TODO: check if one of the addresses is mapped into the rom? */
inline uint8_t VIC::getIdleAccessPattern() { return mem->ram[bankAddr + (iomem[0x11] & 0x40) ? 0x39ff : 0x3fff]; }

// takes care of some special line markings for debugging use 
void inline
VIC::markLine(int start, int end, int color)
{
	for (int i = start; i <= end; i++) {
		pixelBuffer[i] = color;
	}	
}

// Border drawing

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


void inline 
VIC::drawBorder()
{
	if (drawVerticalFrame) {
		drawVerticalBorder();
	} else if (drawHorizontalFrame) {
		drawHorizontalBorder();
	}
}

// Sprite drawing
int inline
VIC::drawSpritesM()
{	
	int deadCycles = 0;
	if (drawSprites) {
		for (int i = 0; i < 8; i++) {
			if (oldSpriteOnOff & (1 << i)) {
				drawSprite(i);
			}				
		}
	}
	return deadCycles;
}


/* this method executes the "g-access" of a cycle.
the g access also occours inside the vertical (upper & lower) frame area, but usually is covered the frame */
void 
VIC::gAccess()
{
	DisplayMode displayMode = getDisplayMode();
	uint8_t pattern;
	uint8_t fgcolor;
	uint8_t bgcolor;
	int colorLookup[4];
	uint16_t xCoord = xCounter + getHorizontalRasterScroll();
	switch (displayMode) {
		case STANDARD_CHARACTER_MODE:
			pattern   = displayState ? getCharacterPattern() : getIdleAccessPattern();
			fgcolor   = colorSpace[registerVMLI];
			bgcolor   = getBackgroundColor();
			drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[bgcolor]);
			break;
		case MULTICOLOR_CHARACTER_MODE:
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
		case STANDARD_BITMAP_MODE:
			pattern = displayState ? getBitmapPattern() : getIdleAccessPattern();
			fgcolor = characterSpace[registerVMLI] >> 4;
			bgcolor = characterSpace[registerVMLI] & 0xf;
			drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[bgcolor]);
			break;
		case MULTICOLOR_BITMAP_MODE:
			pattern = displayState ? getBitmapPattern() : getIdleAccessPattern();
			colorLookup[0]  = colors[getBackgroundColor()];
			colorLookup[1]  = colors[characterSpace[registerVMLI] >> 4];
			colorLookup[2]  = colors[characterSpace[registerVMLI] & 0x0F];
			colorLookup[3]  = colors[colorSpace[registerVMLI]];			
			drawMultiColorCharacter(xCoord, pattern, colorLookup);
			break;
		case EXTENDED_BACKGROUND_COLOR_MODE:
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
	}
	
	/* Nach jedem g-Zugriff im Display-Zustand werden VC und VMLI erhöht. */
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
		characterSpace[registerVMLI] = screenMemory[registerVC]; 
		colorSpace[registerVMLI] = mem->peekColorRam(registerVC) & 0xf;
	}
}

//uint8_t debug0 = 0;
//int debug1 = 0;

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
	getListener()->drawAction(currentScreenBuffer);
	
	// Switch frame buffer
	currentScreenBuffer = (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1;	
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
	memcpy(currentScreenBuffer + (scanline * TOTAL_SCREEN_WIDTH), pixelBuffer, sizeof(pixelBuffer));
}

void 
VIC::cycle1()
{
	checkDmaLineCondition();
	// Trigger rasterline interrupt if applicable
	// Note: In line 0, the interrupt is triggered in cycle 2
	if (scanline != 0 && scanline == rasterInterruptLine())
		triggerIRQ(1);
			
	// Determine the value of the DEN bit (in rasterline 0x30 only)
	if (scanline == 0x30)
		dmaLinesEnabled = isVisible();
			
	// Get sprite data address and sprite data of sprite 3
	readSpritePtr(3);
	readSpriteData(3);
	countX();
}

void
VIC::cycle2()
{
	checkDmaLineCondition();
	// Trigger rasterline interrupt if applicable
	if (scanline == 0 && scanline == rasterInterruptLine())
		triggerIRQ(1);
			
	readSpriteData(3);
	readSpriteData(3);
	requestBusForSprite(5);
	releaseBusForSprite(2);
	countX();
}

void 
VIC::cycle3()
{
	checkDmaLineCondition();
	readSpritePtr(4);
	readSpriteData(4);
	countX();
}

void 
VIC::cycle4()
{
	checkDmaLineCondition();
	readSpriteData(4);
	readSpriteData(4);
	requestBusForSprite(6);
	releaseBusForSprite(3);
	countX();
}

void
VIC::cycle5()
{
	checkDmaLineCondition();
	readSpritePtr(5);
	readSpriteData(5);
	countX();
}

void 
VIC::cycle6()
{
	checkDmaLineCondition();
	readSpriteData(5);
	readSpriteData(5);
	requestBusForSprite(7);
	releaseBusForSprite(4);
	countX();
}

void 
VIC::cycle7()
{
	checkDmaLineCondition();
	readSpritePtr(6);
	readSpriteData(6);
	countX();
}

void 
VIC::cycle8()
{
	checkDmaLineCondition();
	readSpriteData(6);
	readSpriteData(6);
	releaseBusForSprite(5);
	countX();
}

void 
VIC::cycle9()
{
	checkDmaLineCondition();
	readSpritePtr(7);
	readSpriteData(7);
	countX();
}

void 
VIC::cycle10()
{
	checkDmaLineCondition();
	readSpriteData(7);
	readSpriteData(7);
	releaseBusForSprite(6);
	countX();
}

void
VIC::cycle11()
{
	checkDmaLineCondition();
	countX();
}

void
VIC::cycle12()
{
	checkDmaLineCondition();
	if (dmaLine) {
		pullDownBA(0x100);
	}
	releaseBusForSprite(7);

	// Reset the X coordinate to 0
	xCounter = 0;
}

void
VIC::cycle13()
{
	checkDmaLineCondition();
	countX();
}

void
VIC::cycle14()
{
	checkDmaLineCondition();
	/* In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
	 (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
	 Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt. */
	registerVC = registerVCBASE;
	registerVMLI = 0;
	if (dmaLine) registerRC = 0;
	countX();
}

void
VIC::cycle15()
{
	checkDmaLineCondition();
	/* In der ersten Phase von Zyklus 15 wird geprüft, ob das
	 Expansions-Flipflop gesetzt ist. Wenn ja, wird MCBASE um 2 erhöht. */
	// Note: Done in cycle 16

	cAccess();
	countX();
}

void
VIC::cycle16()
{
	checkDmaLineCondition();
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
}

void
VIC::cycle17()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle18()
{
	checkDmaLineCondition();
	if (!isCSEL()) mainFrameFF = false;
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle19()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle20()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle21()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle22()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle23()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle24()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle25()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle26()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle27()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle28()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle29()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle30()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle31()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle32()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle33()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle34()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle35()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle36()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle37()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle38()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle39()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle40()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle41()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle42()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle43()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle44()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle45()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle46()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle47()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle48()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle49()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle50()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle51()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle52()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle53()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle54()
{
	checkDmaLineCondition();
	gAccess();
	cAccess();
	countX();
}

void
VIC::cycle55()
{
	checkDmaLineCondition();
	if (!isCSEL()) mainFrameFF = true;
	
	/* In der ersten Phase von Zyklus 55 wird das Expansions-Flipflop
	 invertiert, wenn das MxYE-Bit gesetzt ist. */
	expansionFF ^= iomem[0x17];
			
	/* In den ersten Phasen von Zyklus 55 und 56 wird für jedes Sprite geprüft,
	 ob das entsprechende MxE-Bit in Register $d015 gesetzt und die
	 Y-Koordinate des Sprites (ungerade Register $d001-$d00f) gleich den
	 unteren 8 Bits von RASTER ist. Ist dies der Fall und der DMA für das
	 Sprite noch ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht
	 und, wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelˆscht.
	 */
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
	releaseBA(0x100);
	requestBusForSprite(0);
	gAccess();
	countX();
}

void
VIC::cycle56()
{
	checkDmaLineCondition();
	countX();
}

void
VIC::cycle57()
{
	checkDmaLineCondition();
	if (isCSEL()) mainFrameFF = true;
	drawHorizontalFrame = mainFrameFF;
	requestBusForSprite(1);
	countX();
}

void
VIC::cycle58()
{
	checkDmaLineCondition();
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
	drawSpritesM();
			
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
	checkDmaLineCondition();
	readSpriteData(0);
	readSpriteData(0);
	// prepare bus access for sprite 0
	if (spriteDmaOnOff & 0x01) pullDownBA(0x01);
	requestBusForSprite(2);
	countX();
}

void
VIC::cycle60()
{
	checkDmaLineCondition();
	if (spriteDmaOnOff & (1 << 1))
		cpu->setRDY(2);
	readSpritePtr(1);
	readSpriteData(1);
	countX();
}

void
VIC::cycle61()
{
	checkDmaLineCondition();
	readSpriteData(1);
	readSpriteData(1);
	requestBusForSprite(3);
	releaseBusForSprite(0);
	countX();
}

void
VIC::cycle62()
{
	checkDmaLineCondition();
	if (spriteDmaOnOff & (1 << 2))
		cpu->setRDY(2);
	readSpritePtr(2);
	readSpriteData(2);
	countX();
}

void
VIC::cycle63()
{
	checkDmaLineCondition();
	readSpriteData(2);
	readSpriteData(2);
	// if (spriteDmaOnOff & 0x10) setBA(0);			
			
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
	if (getDisplayMode() > EXTENDED_BACKGROUND_COLOR_MODE) markLine(xStart(), xEnd(), colors[BLACK]);

	// draw debug markers
	if (markIRQLines && scanline == rasterInterruptLine()) markLine(0, TOTAL_SCREEN_WIDTH, colors[WHITE]);
	if (markDMALines && dmaLine) markLine(xCounter, 0, TOTAL_SCREEN_WIDTH);
	if (rasterlineDebug[scanline] >= 0) {
		markLine(0, TOTAL_SCREEN_WIDTH, colors[rasterlineDebug[scanline] % 16]);
		rasterlineDebug[scanline] = -1;
	}		
	requestBusForSprite(4);
	releaseBusForSprite(1);
	countX();
}

void
VIC::cycle64()
{
	checkDmaLineCondition();
	// NTSC only
	countX();
}

void
VIC::cycle65()
{
	checkDmaLineCondition();
	// NTSC only
	countX();
}

void 
VIC::dumpState()
{
	debug("Rasterline: %d (%x)\n", scanline, scanline);
	// debug("Mode: %s\n", cyclesPerRasterline == PAL_CYCLES_PER_RASTERLINE ? "PAL" : "NTSC");
	debug("Text resolution: %d x %d\n", numberOfRows(), numberOfColumns());
	debug("Vertical raster scroll: %d Horizontal raster scroll: %d\n\n", getVerticalRasterScroll(), getHorizontalRasterScroll());
	debug("Display mode: ");
	switch (getDisplayMode()) {
		case STANDARD_CHARACTER_MODE: 
			debug("Standard character mode\n");
			break;
		case MULTICOLOR_CHARACTER_MODE:
			debug("Multicolor character mode\n");
			break;
		case STANDARD_BITMAP_MODE:
			debug("Standard bitmap mode\n");
			break;
		case MULTICOLOR_BITMAP_MODE:
			debug("Multicolor bitmap mode\n");
			break;
		case EXTENDED_BACKGROUND_COLOR_MODE:
			debug("Extended background color mode\n");
			break;
		default:
			debug("Invalid display mode\n");
	}
	debug("Bank address: %d (%4X)", bankAddr, bankAddr);
	debug("Screen memory: %d (%4X)\n", screenMemoryAddr, screenMemoryAddr);
	debug("Character memory: %d (%4X) (RAM)\n", characterMemoryAddr, characterMemoryAddr);
	debug("State: %s\n", displayState ? "display" : "idle");
	debug("Registers: \n  VC: %d\n  VCBASE:%d\n  RC: %d\n  VMLI: %d\n", registerVC, registerVCBASE, registerRC, registerVMLI);
}

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
