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

	frame    = 0;
	bankAddr = 0;
	scanline = 0;
	drawSprites  = true;
	markIRQLines = false;
	lastScanline = NTSC_RASTERLINES-1;
	screenMemory = mem->getRam();
	screenMemoryAddr = 0x0000;
	spriteMemory = screenMemory;
	characterMemory = mem->getRam();
	characterMemoryAddr = 0x0000;
	
	memset(spriteLineToDraw, 0xff, sizeof(spriteLineToDraw));
	memset(iomem, 0x00, sizeof(iomem));
	
	for (int i = 0; i < 8; i++) {
		spriteSpriteCollisionEnabled[i] = true;
		spriteBackgroundCollisionEnabled[i] = true;
	}
	
	// Let the color look correct right from the beginning
	iomem[0x20] = 14; // Light blue
	iomem[0x21] = 6;  // Blue
}
			
// Loading and saving snapshots
bool
VIC::load(FILE *file)
{
	debug("  Loading VIC state...\n");
	frame            = read64(file);
	scanline         = read32(file);
	lastScanline     = read32(file);
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
	for (int i = 0; i < sizeof(spriteLineToDraw); i++) {
		((char *)spriteLineToDraw)[i] = read8(file);
	}		
	return true;
}

bool
VIC::save(FILE *file)
{
	debug("  Saving VIC state...\n");
	write64(file, frame);
	write32(file, scanline);
	write32(file, lastScanline);
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
	for (int i = 0; i < sizeof(spriteLineToDraw); i++) {
		write8(file, ((char *)spriteLineToDraw)[i]);
	}	
	return true;
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
		uint8_t oldValue;
		
		case 0x01:
			iomem[addr] = value;
			updateSpriteLineToDraw(0);
			return;
		case 0x03:
			iomem[addr] = value;
			updateSpriteLineToDraw(1);
			return;
		case 0x05:
			iomem[addr] = value;
			updateSpriteLineToDraw(2);
			return;
		case 0x07:
			iomem[addr] = value;
			updateSpriteLineToDraw(3);
			return;
		case 0x09:
			iomem[addr] = value;
			updateSpriteLineToDraw(4);
			return;
		case 0x0B:
			iomem[addr] = value;
			updateSpriteLineToDraw(5);
			return;
		case 0x0D:
			iomem[addr] = value;
			updateSpriteLineToDraw(6);
			return;
		case 0x0F:
			iomem[addr] = value;
			updateSpriteLineToDraw(7);
			return;
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
		case 0x15: // SPRITE ENABLED
			oldValue = iomem[addr]; 
			iomem[addr] = value;
			for (int i = 0; i < 8; i++) {
				if ((value & (1 << i)) != (oldValue & (1 << i)))
					updateSpriteLineToDraw(i);
			}
			return;
		case 0x16:
			iomem[addr] = value | (128 + 64); // The upper two bits are unused and always return 1 when read
			return;
		case 0x17: // VERTICAL STRETCH
			oldValue = iomem[addr]; 
			iomem[addr] = value;
			for (int i = 0; i < 8; i++)
				if ((value & (1 << i)) != (oldValue & (1 << i)))
					updateSpriteLineToDraw(i);
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
	assert(false);
}

void 
VIC::setScreenGeometry(ScreenGeometry mode)
{
	setNumberOfRows((mode == COL_40_ROW_25 || mode == COL_38_ROW_25) ? 25 : 24);
	setNumberOfColumns((mode == COL_40_ROW_25 || mode == COL_40_ROW_24) ? 40 : 38);
}

void 
VIC::updateSpriteLineToDraw(uint8_t nr) 
{
	assert (nr < 8);

	// TODO: Optimize, so that only the non-zero values get zeroes out
	memset(spriteLineToDraw[nr], 0xff, sizeof(spriteLineToDraw[nr]));
	
	if (spriteIsEnabled(nr)) {
		int offset = 1 + getSpriteY(nr); 		
		if (spriteHeightIsDoubled(nr)) {
			for (int i = 0; i < 21; i++) {
				spriteLineToDraw[nr][offset++] = i;
				spriteLineToDraw[nr][offset++] = i;
			}
		} else {
			for (int i = 0; i < 21; i++) {
				spriteLineToDraw[nr][offset++] = i;
			}
		}
	}
}

//! Updates the \a spriteLineToDraw array for all sprites
void 
VIC::updateSpriteLinesToDraw()
{
	for (int i = 0; i < 8; i++)
		updateSpriteLineToDraw(i);
}

inline void 
VIC::setForegroundPixel(int offset, int color) 
{
	pixelBuffer[offset] = color;
	zBuffer[offset]     = 0x10;
	pixelSource[offset] = 0x80;
}

inline void 
VIC::setBackgroundPixel(int offset, int color) 
{
	pixelBuffer[offset] = color;
	// zBuffer[offset]     = 0x00;
	// pixelSource[offset] = 0x00;
}

#if 0
inline void 
VIC::clearPixel(int offset, int color) 
{
	pixelBuffer[offset] = color;
	// zBuffer[offset]     = 0x00;
	// pixelSource[offset] = 0x00;
}
#endif

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
VIC::drawSprite(uint8_t line, uint8_t nr)
{
	uint8_t  pattern;
	uint16_t spriteData;
	int spriteX, offset; // = getSpriteX(nr) + getHorizontalRasterScroll();

	assert(nr < 8);
	assert(line < 21);

	spriteData = getSpriteData(nr) + 3 * line;
	spriteX    = getSpriteX(nr);

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
			pattern = mem->peekRam(spriteData + i);
			
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
			pattern = mem->peekRam(spriteData + i);
			
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
//VIC::drawSingleColorCharacter(int *buffer, uint8_t pattern, int fgcolor, int bgcolor)
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
VIC::drawSingleColorBitmap(int offset, uint8_t pattern, int fgcolor, int bgcolor)
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


bool
VIC::drawInnerScanLine()
{
	uint16_t xCoord    = BORDER_WIDTH + getHorizontalRasterScroll();
	int colorLookup[4];
	uint16_t offset;
	DisplayMode displayMode = getDisplayMode();
	
#if 0
	static int oldDisplayMode = displayMode;
	if (oldDisplayMode != displayMode) {
		oldDisplayMode = displayMode;
	}
#endif
	
	switch(displayMode) {
		
		case STANDARD_BITMAP_MODE:
			offset = getRowNumberForRasterline(scanline) * 320 + getRowOffsetForRasterline(scanline);
			for (int column = 0; column < 40; column++, xCoord += 8) {
				uint8_t pattern = characterMemory[offset + 8 * column];
				uint8_t fgcolor = characterSpace[column] >> 4;
				uint8_t bgcolor = characterSpace[column] & 0x0F;
				drawSingleColorBitmap(xCoord, pattern, colors[fgcolor], colors[bgcolor]);
			}
			break;

		case MULTICOLOR_BITMAP_MODE:
			offset = getRowNumberForRasterline(scanline) * 320 + getRowOffsetForRasterline(scanline);
			for (int column = 0; column < 40; column++, xCoord += 8) {
				uint8_t pattern = characterMemory[offset + 8 * column];
				colorLookup[0]  = colors[getBackgroundColor()];
				colorLookup[1]  = colors[characterSpace[column] >> 4];
				colorLookup[2]  = colors[characterSpace[column] & 0x0F];
				colorLookup[3]  = colors[colorSpace[column]];
				drawMultiColorCharacter(xCoord, pattern, colorLookup);
			}
			break;
			
		case STANDARD_CHARACTER_MODE:
			offset = getRowOffsetForRasterline(scanline); 
			for (int column = 0; column < 40; column++, xCoord += 8) {
				uint8_t character = characterSpace[column];
				uint8_t pattern   = characterMemory[offset +  8 * character];
				uint8_t fgcolor   = colorSpace[column]; 
				uint8_t bgcolor   = getBackgroundColor();
				drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[bgcolor]);
			}
			break;
			
		case MULTICOLOR_CHARACTER_MODE:
			offset = getRowOffsetForRasterline(scanline); 
			for (int column = 0; column < 40; column++, xCoord += 8) {
				uint8_t character = characterSpace[column];
				uint8_t pattern   = characterMemory[offset +  8 * character];
				uint8_t fgcolor   = colorSpace[column];
				if (fgcolor & 0x08) {
					colorLookup[0] = colors[getBackgroundColor()];
					colorLookup[1] = colors[getExtraBackgroundColor(1)];
					colorLookup[2] = colors[getExtraBackgroundColor(2)];
					colorLookup[3] = colors[fgcolor & 0x07];
					drawMultiColorCharacter(xCoord, pattern, colorLookup);
				} else {
					drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[getBackgroundColor()]);
				}
			}
			break;
			
					
		case EXTENDED_BACKGROUND_COLOR_MODE:
			offset = getRowOffsetForRasterline(scanline); 
			for (int column = 0; column < 40; column++, xCoord += 8) {
				uint8_t character = characterSpace[column];
				uint8_t pattern = characterMemory[offset +  8 * character];
				uint8_t fgcolor = colorSpace[column]; 
				// In extended background color mode, the two upper bits of the character code determine
				// the background color. Therefore, only 64 characters can be displayed
				uint8_t bgColor = getExtraBackgroundColor(character >> 6);
				character &= 0x3F;
				if (fgcolor & 0x08) {
					colorLookup[0] = colors[bgColor];
					colorLookup[1] = colors[getExtraBackgroundColor(1)];
					colorLookup[2] = colors[getExtraBackgroundColor(2)];
					colorLookup[3] = colors[fgcolor & 0x07];
					drawMultiColorCharacter(xCoord, pattern, colorLookup);
				} else {
					drawSingleColorCharacter(xCoord, pattern, colors[fgcolor], colors[getBackgroundColor()]);
				}
			}
			break;
			
		default:
			// Invalid display mode
			return false;
	}
	return true;
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

inline void 
VIC::setRasterline(int line)
{
	scanline = line;
	
	// New frame?
	if (scanline == 0) {
		frame++;

		// Frame complete. Notify listener...
		getListener()->drawAction(currentScreenBuffer);

		// Switch frame buffer
		currentScreenBuffer = (currentScreenBuffer == screenBuffer1) ? screenBuffer2 : screenBuffer1;
	}
	
	// Check, if scanline matches the preset raster value for interrupts
	if (scanline == rasterInterruptLine()) {
		//debug("Triggering rasterline IRQ at scanline %d (%2X)\n", scanline, scanline);
		triggerIRQ(1);
	}

	// Check, if DMA lines are enabled or disabled (in line 30 only)
	if (scanline == 30) {
		dmaLinesEnabled = iomem[0x11] & 0x10;
	}
}	

inline void
VIC::fetchData(uint16_t line)
{
	uint8_t row     = getRowNumberForRasterline(line);	
	uint16_t offset = 40 * row;
	
	assert(row < 25);
	for (int column = 0; column < 40; column++) {
		characterSpace[column] = screenMemory[offset];
		colorSpace[column]     = mem->peekColorRam(offset) & 15;
		offset++;
	}
}

bool 
VIC::executeOneLine(int line, int *deadCycles)
{
	bool validDisplayMode = true;
	
	setRasterline(line);
		
	// Draw directly into screenBuffer
	// pixelBuffer = currentScreenBuffer + (scanline * TOTAL_SCREEN_WIDTH);
	
	// Clear z buffer
	// The z buffer is initialized with a high, positive value (meaning the pixel is far away)
	memset(zBuffer, 0x7f, sizeof(zBuffer));

	// Clear pixel source
	memset(pixelSource, 0x00, sizeof(pixelSource));
	
	// Determine start address of the current line in the screenbuffer
	// int *buffer = screenBuffer + (scanline * TOTAL_SCREEN_WIDTH);

	// Fetch data, if the current line is a DMA line

	// TODO:
	// I think the real C64 has a counter that is incremented in each fetch.
	// When multiple bad lines are induced (not supported now), consecutive cells are loaded
	// In this implementation, always the same data is fetched 
	// (this is because the fetched data is determined by the rasterline and not by a steadily increasing counter).
	// This might be the reason why "shadow"'s screen scrolling does not work as expected
	if (isDMALine(scanline) && dmaLinesEnabled) {
		fetchData(scanline);
		*deadCycles = 40;
	} else {
		*deadCycles = 0;
	}

	// Fill screen buffer with background color
	int bgcolor = colors[getBorderColor()];

	if (scanline < yStart() || scanline > yEnd()) {

		//
		// Upper and lower border
		//
		for (int i = 0; i < TOTAL_SCREEN_WIDTH; i++)
			pixelBuffer[i] = bgcolor;

	} else {	

		//
		// Drawable area
		//		
		validDisplayMode = drawInnerScanLine();
	}
	
	// Draw sprites
	// iomem[0x1E] = 0x00; // Clear sprite-to-sprite collision register
	// iomem[0x1F] = 0x00; // Clear sprite-to-background collision register
	if (drawSprites) {
		for (int i = 0; i < 8; i++) {
			uint8_t spriteLine = spriteLineToDraw[i][scanline];
			if (spriteLine != 0xff)
				drawSprite(spriteLine, i);
		}
	}
	
	// Fill left border
	for (int i = 0; i < xStart(); i++) {
		pixelBuffer[i] = bgcolor;
		// zBuffer[i] = 0xff;
	}
	// Fill right border
	for (int i = xEnd()+1; i < TOTAL_SCREEN_WIDTH; i++) {
		pixelBuffer[i] = bgcolor;
		// zBuffer[i] = 0xff;
	}
	
	// Clear line if the invalid display mode is invalid
	// Fill left border
	if (!validDisplayMode) {
		for (int i = xStart(); i <= xEnd(); i++) {
			pixelBuffer[i] = colors[WHITE]; // bgcolor;
		}	
	}
	
	// Mark raster interrupt lines if requested (for debugging purposes only)
	if (markIRQLines && scanline == rasterInterruptLine()) {
		for (int i = 0; i < TOTAL_SCREEN_WIDTH; i++) {
			pixelBuffer[i] = colors[WHITE];
		}
	}
	
	// Copy pixel buffer to screen buffer
	memcpy(currentScreenBuffer + (scanline * TOTAL_SCREEN_WIDTH), pixelBuffer, sizeof(pixelBuffer));
	
	return true;
}

void VIC::dumpState()
{
	debug("Rasterline: %d (%x)\n", scanline, scanline);
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
}
