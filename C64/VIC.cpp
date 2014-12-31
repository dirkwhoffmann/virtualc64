/*
 * Author: Dirk W. Hoffmann
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
   Mostly based on the extensive VIC II documentation by Christian Bauer ([C.B.])
   Many thanks, Christian! 
*/

#include "C64.h"


// Update value of the display variable according to the dma line condition 
#define update_display if (badLineCondition) { displayState = true; }

// Update value of the display variable and the BA line according to the dma line condition
/* "3. Liegt in den Zyklen 12-54 ein Bad-Line-Zustand vor, wird BA auf Low
    gelegt und die c-Zugriffe gestartet. Einmal gestartet, findet in der
    zweiten Phase jedes Taktzyklus im Bereich 15-54 ein c-Zugriff statt. Die
    gelesenen Daten werden in der Videomatrix-/Farbzeile an der durch VMLI
    angegebenen Position abgelegt. Bei jedem g-Zugriff im Display-Zustand
    werden diese Daten ebenfalls an der durch VMLI spezifizierten Position
    wieder intern gelesen." [C.B.] */
 
#define update_display_and_ba if (badLineCondition) { displayState = true; pullDownBA(0x100); }


VIC::VIC()
{
	name = "VIC";

	debug(2, "  Creating VIC at address %p...\n", this);

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
	badLineCondition = false;
    DENwasSetInRasterline30 = false;
	displayState = false;
	BAlow = 0;
	mainFrameFF = false;
	verticalFrameFF = false;
	// drawVerticalFrame = false;
	// drawHorizontalFrame = false;
	
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
		spriteShiftReg[i][2] = 0; 
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
void
VIC::loadFromBuffer(uint8_t **buffer)
{
	debug(2, "  Loading VIC state...\n");
	
	// Internal registers
	scanline = read32(buffer);
	xCounter = read16(buffer);
	registerVC = read16(buffer);
	registerVCBASE = read16(buffer);
	registerRC = read16(buffer);
	registerVMLI = read16(buffer);
	badLineCondition = (bool)read8(buffer);
	DENwasSetInRasterline30 = (bool)read8(buffer);
	displayState = (bool)read8(buffer);
	BAlow = (bool)read8(buffer);
	mainFrameFF = (bool)read8(buffer);
	verticalFrameFF = (bool)read8(buffer);
	// drawVerticalFrame = (bool)read8(buffer);
	// drawHorizontalFrame = (bool)read8(buffer);
    (void)read8(buffer);
    (void)read8(buffer);
    
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
		spriteShiftReg[i][2] = read8(buffer);
	}
	spriteOnOff = read8(buffer);
	oldSpriteOnOff = read8(buffer);
	spriteDmaOnOff = read8(buffer);
	expansionFF = read8(buffer);
	
	// Lightpen
	lightpenIRQhasOccured = (bool)read8(buffer);
}

void
VIC::saveToBuffer(uint8_t **buffer)
{
	debug(2, "  Saving VIC state...\n");

	// Internal registers
	write32(buffer, scanline);
	write16(buffer, xCounter);
	write16(buffer, registerVC);
	write16(buffer, registerVCBASE);
	write16(buffer, registerRC);
	write16(buffer, registerVMLI);
	write8(buffer, (uint8_t)badLineCondition);
	write8(buffer, (uint8_t)DENwasSetInRasterline30);
	write8(buffer, (uint8_t)displayState);
	write8(buffer, (uint8_t)BAlow);
	write8(buffer, (uint8_t)mainFrameFF);
	write8(buffer, (uint8_t)verticalFrameFF);
	// write8(buffer, (uint8_t)drawVerticalFrame);
	// write8(buffer, (uint8_t)drawHorizontalFrame);
    write8(buffer, (uint8_t)0);
    write8(buffer, (uint8_t)0);
	
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
		write8(buffer, spriteShiftReg[i][2]); 
	}
	write8(buffer, spriteOnOff);
	write8(buffer, oldSpriteOnOff);
	write8(buffer, spriteDmaOnOff);
	write8(buffer, expansionFF);
	
	// Lightpen
	write8(buffer, lightpenIRQhasOccured);
}

void 
VIC::dumpState()
{
	msg("VIC\n");
	msg("---\n\n");
	msg("     Bank address : %04X\n", bankAddr, bankAddr);
	msg("    Screen memory : %04X\n", screenMemoryAddr);
	msg(" Character memory : %04X (%s)\n", characterMemoryAddr, characterMemoryMappedToROM ? "ROM" : "RAM");
	msg("  Text resolution : %d x %d\n", numberOfRows(), numberOfColumns());
	msg("X/Y raster scroll : %d / %d\n", getHorizontalRasterScroll(), getVerticalRasterScroll());
	msg("     Display mode : ");
	switch (getDisplayMode()) {
		case STANDARD_TEXT: 
			msg("Standard character mode\n");
			break;
		case MULTICOLOR_TEXT:
			msg("Multicolor character mode\n");
			break;
		case STANDARD_BITMAP:
			msg("Standard bitmap mode\n");
			break;
		case MULTICOLOR_BITMAP:
			msg("Multicolor bitmap mode\n");
			break;
		case EXTENDED_BACKGROUND_COLOR:
			msg("Extended background color mode\n");
			break;
		default:
			msg("Invalid\n");
	}
	msg("            (X,Y) : (%d,%d) %s %s\n", xCounter, scanline,  badLineCondition ? "(DMA line)" : "", DENwasSetInRasterline30 ? "" : "(DMA lines disabled, no DEN bit in rasterline 30)");
	msg("               VC : %02X\n", registerVC);
	msg("           VCBASE : %02X\n", registerVCBASE);
	msg("               RC : %02X\n", registerRC);
	msg("             VMLI : %02X\n", registerVMLI);
	msg("          BA line : %s\n", BAlow ? "low" : "high");
	msg("      MainFrameFF : %d\n", mainFrameFF);
	msg("  VerticalFrameFF : %d\n", verticalFrameFF);
	msg("     DisplayState : %s\n", displayState ? "on" : "off");
	msg("         SpriteOn : %02X ( ", spriteOnOff);
	for (int i = 0; i < 8; i++) 
		msg("%d ", (spriteOnOff & (1 << i)) != 0);
	msg(")\n");
	msg("        SpriteDma : %02X ( ", spriteDmaOnOff);
	for (int i = 0; i < 8; i++) 
		msg("%d ", (spriteDmaOnOff & (1 << i)) != 0 );
	msg(")\n");
	msg("      Y expansion : %02X ( ", expansionFF);
	for (int i = 0; i < 8; i++) 
		msg("%d ", (expansionFF & (1 << i)) != 0);
	msg(")\n");
	
	msg("        IO memory : ");
	for (unsigned i = 0; i < sizeof(iomem); i += 16) {
		for (unsigned j = 0; j < 16; j ++) {
			msg("%02X ", iomem[i + j]);
		}
		msg("\n                    ");
	}
	msg("\n");
}


// -----------------------------------------------------------------------------------------------
//                                         Configuring
// -----------------------------------------------------------------------------------------------

void 
VIC::setPAL()
{ 
	leftBorderWidth = PAL_LEFT_BORDER_WIDTH;
	rightBorderWidth = PAL_RIGHT_BORDER_WIDTH;
	upperBorderHeight = PAL_UPPER_BORDER_HEIGHT;
	lowerBorderHeight = PAL_LOWER_BORDER_HEIGHT;
	totalScreenWidth = PAL_VIEWABLE_PIXELS;
	totalScreenHeight = PAL_VIEWABLE_RASTERLINES;
	firstVisibleLine = PAL_UPPER_INVISIBLE;
	lastVisibleLine = PAL_UPPER_INVISIBLE + PAL_VIEWABLE_RASTERLINES;
	pixelAspectRatio = 0.9365;
}

void
VIC::setNTSC()
{
	leftBorderWidth = NTSC_LEFT_BORDER_WIDTH;
	rightBorderWidth = NTSC_RIGHT_BORDER_WIDTH;
	upperBorderHeight = NTSC_UPPER_BORDER_HEIGHT;
	lowerBorderHeight = NTSC_LOWER_BORDER_HEIGHT;
	totalScreenWidth = NTSC_VIEWABLE_PIXELS;
	totalScreenHeight = NTSC_VIEWABLE_RASTERLINES;
	firstVisibleLine = NTSC_UPPER_INVISIBLE;
	lastVisibleLine = NTSC_UPPER_INVISIBLE + NTSC_VIEWABLE_RASTERLINES;
	pixelAspectRatio = 0.75;
}

// -----------------------------------------------------------------------------------------------
//                             I/O memory handling and RAM access
// -----------------------------------------------------------------------------------------------

inline uint8_t VIC::memAccess(uint16_t addr)
{
    /* "Der VIC besitzt nur 14 Adreﬂleitungen, kann also nur 16KB Speicher
        adressieren. Er kann trotzdem auf die kompletten 64KB Hauptspeicher
        zugreifen, denn die 2 fehlenden oberen Adressbits werden von einem der
        CIA-I/O-Chips zur Verfügung gestellt (es sind dies die invertierten Bits 0
        und 1 von Port A der CIA 2). Damit kann jeweils eine von 4 16KB-Bänken für
        den VIC eingestellt werden." [C.B.]

       "Das Char-ROM wird in den Bänken 0 und 2 jeweils an den VIC-Adressen
        $1000-$1fff eingeblendet" [C.B.] */
    
    assert((addr & 0xC000) == 0); /* 14 bit address */
    
    uint16_t extaddr = bankAddr + addr;
    
    if ((extaddr & 0x7000) == 0x1000) { // Address points into character ROM
        assert ((0xC000 + addr) >= 0xD000 && (0xC000 + addr) <= 0xDFFF); // Really?
        return mem->rom[0xC000 + addr]; // Read from character ROM
    } else {
        return mem->ram[extaddr];
    }
}

inline void VIC::cAccess()
{
    // Only proceed if the BA line is pulled down
    // TODO: Checking badLineCondition is not accurate here.
    if (!badLineCondition)
        return;

    // If BA is pulled down for at least three cycles, perform memory access
    if (BApulledDownForAtLeastThreeCycles()) {
        
        // |VM13|VM12|VM11|VM10| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0|
        uint16_t addr = (VM13VM12VM11VM10() << 6) | registerVC;
        
        characterSpace[registerVMLI] = memAccess(addr);
            colorSpace[registerVMLI] = mem->colorRam[registerVC] & 0x0F;
    }
    
    // Otherwise, 0xFF is read
    else {
        characterSpace[registerVMLI] = 0xFF;
            colorSpace[registerVMLI] = 0xFF;
    }
}

inline void VIC::gAccess()
{
    uint16_t addr;

    assert ((registerVC & 0xFC00) == 0); // 10 bit register
    assert ((registerRC & 0xF8) == 0);   // 3 bit register
    

    // Run address generator
    
    /* "Der Adressgenerator für die Text-/Bitmap-Zugriffe (c- und g-Zugriffe)
        besitzt bei den g-Zugriffen im wesentlichen 3 Modi (die c-Zugriffe erfolgen
        immer nach dem selben Adressschema). Im Display-Zustand wählt das BMM-Bit
        entweder Zeichengenerator-Zugriffe (BMM=0) oder Bitmap-Zugriffe (BMM=1)
        aus" [C.B.] */

    if (displayState) {
        
        if (BMMbit()) {
            
            // |CB13| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0| RC2| RC1| RC0|
            addr = (CB13() << 10) | (registerVC << 3) | registerRC;

        } else {

            // |CB13|CB12|CB11| D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | RC2| RC1| RC0|
            addr = (CB13CB12CB11() << 10) | (characterSpace[registerVMLI] << 3) | registerRC;
        
        }
    }
                   
    /* "Im Idle-Zustand erfolgen die g-Zugriffe immer an Videoadresse $3fff." [C.B.] */

    else {
        addr = 0x3FFF;
    }

    /* "Bei gesetztem ECM-Bit schaltet der Adreﬂgenerator bei den g-Zugriffen die
        Adreﬂleitungen 9 und 10 immer auf Low, bei ansonsten gleichem Adressschema
        (z.B. erfolgen dann die g-Zugriffe im Idle-Zustand an Adresse $39ff)." [C.B.] */

    if (ECMbit()) {
        addr &= 0xF9FF;
    }
    
    // Load graphics sequencer
    loadGraphicSequencer(memAccess(addr), getHorizontalRasterScroll());
    
    
    gAccessResult = memAccess(addr); // DEPRECATED
    
    // VC and VMLI are increased after each gAccess
    if (displayState) {
        registerVC++;
        registerVC &= 0x3FF; // 10 bit overflow
        registerVMLI++;
        registerVMLI &= 0x3F; // 6 bit overflow;
    }
}

inline void VIC::pAccess()
{
    
}

inline void VIC::sAccess()
{

}

// -----------------------------------------------------------------------------------------------
//                                         Graphics sequencer
// -----------------------------------------------------------------------------------------------

void VIC::loadGraphicSequencer(uint8_t data, uint8_t load_delay)
{
    gs_data = data;
    gs_delay = load_delay;
    
    // Determine how to interpret the data
    gs_mode = getDisplayMode();
    switch (gs_mode) {
            
        case STANDARD_TEXT:
            gs_fg_color = colorSpace[registerVMLI];
            gs_bg_color = getBackgroundColor();
            break;
            
        case MULTICOLOR_TEXT:
            gs_fg_color = colorSpace[registerVMLI];
            if (gs_fg_color & 0x8 /* MC flag */) {
                gs_multicol0 = getBackgroundColor();
                gs_multicol1 = getExtraBackgroundColor(1);
                gs_multicol2 = getExtraBackgroundColor(2);
                gs_multicol3 = gs_fg_color & 0x07;
            } else {
                gs_bg_color = getBackgroundColor();
            }
            break;
            
        case STANDARD_BITMAP:
            gs_fg_color = characterSpace[registerVMLI] >> 4;
            gs_bg_color = characterSpace[registerVMLI] & 0x0F;
            break;
            
        case MULTICOLOR_BITMAP:
            gs_multicol0 = getBackgroundColor();
            gs_multicol1 = characterSpace[registerVMLI] >> 4;
            gs_multicol2 = characterSpace[registerVMLI] & 0x0F;
            gs_multicol3 = colorSpace[registerVMLI];
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            gs_fg_color = colorSpace[registerVMLI];
            if (gs_fg_color & 0x8 /* MC flag */) {
                gs_multicol0 = getExtraBackgroundColor(characterSpace[registerVMLI] >> 6);;
                gs_multicol1 = getExtraBackgroundColor(1);
                gs_multicol2 = getExtraBackgroundColor(2);
                gs_multicol3 = gs_fg_color & 0x07;
            } else {
                gs_bg_color = getBackgroundColor();
            }
            break;
            
        case INVALID_TEXT:
        case INVALID_STANDARD_BITMAP:
        case INVALID_MULTICOLOR_BITMAP:
            // Nothing to do
            break;

        default:
            assert(0);
            break;
    }
}

void VIC::runGraphicSequencerAtBorder()
{
    uint16_t xCoord;
    
    xCoord = ((int16_t)xCounter - 28) + leftBorderWidth;
    
    if (mainFrameFF || verticalFrameFF) {
        int bordercolor = colors[getBorderColor()]; // colors[7];
        for (unsigned i = 0; i < 8; i++) {
            setFramePixel(xCoord+i, bordercolor);
        }
    }

    // What do we do if frame flipflops are off?
}

void VIC::runGraphicSequencer()
{
    uint16_t xCoord = (xCounter - 28) + leftBorderWidth;

    // Only run the sequencer if no frame flipflop is set
    if (mainFrameFF || verticalFrameFF) {
        int bordercolor = colors[getBorderColor()]; // colors[4];
        for (unsigned i = 0; i < 8; i++) {
            setFramePixel(xCoord+i, bordercolor);
        }
        return;
    }
    
    xCoord += gs_delay;
    
    // Synthesize pixels
    switch (getDisplayMode()) {
            
        case STANDARD_TEXT:
            drawSingleColorCharacter(xCoord);
            break;
            
        case MULTICOLOR_TEXT:
            if (gs_fg_color & 0x8 /* MC flag */) {
                drawMultiColorCharacter(xCoord);
            } else {
                drawSingleColorCharacter(xCoord);
            }
            break;
            
        case STANDARD_BITMAP:
            drawSingleColorCharacter(xCoord);
            break;
            
        case MULTICOLOR_BITMAP:
            drawMultiColorCharacter(xCoord);
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            if (gs_fg_color & 0x8 /* MC flag */) {
                drawMultiColorCharacter(xCoord);
            } else {
                drawSingleColorCharacter(xCoord);
            }
            break;		

        case INVALID_TEXT:
            if (gs_fg_color & 0x8 /* MC flag */) {
                drawInvalidMultiColorCharacter(xCoord);
            } else {
                drawInvalidSingleColorCharacter(xCoord);
            }
            break;
            
        case INVALID_STANDARD_BITMAP:
            drawInvalidSingleColorCharacter(xCoord);
            break;
            
        case INVALID_MULTICOLOR_BITMAP:
            drawInvalidMultiColorCharacter(xCoord);
            break;

        default:
            assert(0);
            break;
    }
}

// -----------------------------------------------------------------------------------------------
//                                           Drawing
// -----------------------------------------------------------------------------------------------

inline void
VIC::setPixel(unsigned offset, int color, int depth, int source)
{
    pixelSource[offset] |= source;
    
    if (depth < zBuffer[offset]) {
        zBuffer[offset] = depth;
        pixelBuffer[offset] = color;
    }
}

inline void
VIC::setFramePixel(unsigned offset, int color)
{
    setPixel(offset, color, 0x00 /* in front of everything */, 0x00);
}

inline void
VIC::setForegroundPixel(unsigned offset, int color)
{
    setPixel(offset, color, 0x20, 0x80);
}

inline void
VIC::setBackgroundPixel(unsigned offset, int color)
{
    pixelBuffer[offset] = color;
}

inline void 
VIC::drawSingleColorCharacter(unsigned offset)
{
    int fg_rgba = colors[gs_fg_color];
    int bg_rgba = colors[gs_bg_color];
    
	assert(offset >= 0 && offset+7 < MAX_VIEWABLE_PIXELS);
    
	if (gs_data & 128) setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 64)  setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 32)  setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 16)  setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 8)   setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 4)   setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 2)   setForegroundPixel(offset++, fg_rgba); else setBackgroundPixel(offset++, bg_rgba);
	if (gs_data & 1)   setForegroundPixel(offset, fg_rgba); else setBackgroundPixel(offset, bg_rgba);
}

inline void 
VIC::drawMultiColorCharacter(unsigned offset)
{
    int col, colorLookup[4];;
    uint8_t colBits;
    
	assert(offset+7 < MAX_VIEWABLE_PIXELS);

    colorLookup[0] = colors[gs_multicol0];
    colorLookup[1] = colors[gs_multicol1];
    colorLookup[2] = colors[gs_multicol2];
    colorLookup[3] = colors[gs_multicol3];
    
    for (unsigned i = 0; i < 4; i++, gs_data <<= 2) {

        colBits = (gs_data & 0xC0) >> 6;
        col = colorLookup[colBits];
        
        if (colBits & 0x02) {
            setForegroundPixel(offset++, col);
            setForegroundPixel(offset++, col);
        } else {
            setBackgroundPixel(offset++, col);
            setBackgroundPixel(offset++, col);
        }
    }
}

inline void
VIC::drawInvalidSingleColorCharacter(unsigned offset)
{
    int col = colors[BLACK];

    assert(offset+7 < MAX_VIEWABLE_PIXELS);
    
    for (unsigned i = 0; i < 8; i++, gs_data <<= 1) {
        
        if (gs_data & 0xF0) {
            setForegroundPixel(offset++, col);
        } else {
            setBackgroundPixel(offset++, col);
        }
    }
}

inline void
VIC::drawInvalidMultiColorCharacter(unsigned offset)
{
    int col = colors[BLACK];

    assert(offset+7 < MAX_VIEWABLE_PIXELS);
    
    for (unsigned i = 0; i < 4; i++, gs_data <<= 2) {
        
        if (gs_data & 0xF0) {
            setForegroundPixel(offset++, col);
            setForegroundPixel(offset++, col);
        } else {
            setBackgroundPixel(offset++, col);
            setBackgroundPixel(offset++, col);
        }
    }
}

inline void 
VIC::setSpritePixel(unsigned offset, int color, int nr) 
{	
	uint8_t mask = (1 << nr);
	
	if (offset < totalScreenWidth) {

        //int depth = spriteDepth(nr);
		//if (depth < zBuffer[offset]) {
		//	pixelBuffer[offset] = color;
		//	zBuffer[offset] = depth;
		// }
		
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
		
        if (nr == 7)
            mask = 0;
        
        setPixel(offset, color, spriteDepth(nr), mask);
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
		offset = spriteX + (leftBorderWidth - 22);
	else
		offset = spriteX + (leftBorderWidth - 22) - 488; 
	
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
	
	for (unsigned i = 0; i < (unsigned)xStart(); i++) {
		pixelBuffer[i] = bcolor;
	}
	for (unsigned i = xEnd(); i < totalScreenWidth; i++) {
		pixelBuffer[i] = bcolor;
	}
}

void inline
VIC::drawVerticalBorder()
{
	int bcolor = colors[getBorderColor()];
	
	for (unsigned i = 0; i < totalScreenWidth; i++) {
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
	setScreenMemoryAddr((iomem[0x18] & 0xF0) << 6); // DEPRECTAD
	setCharacterMemoryAddr((iomem[0x18] & 0x0E) << 10); // DEPRECTAD
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
            
        case 0x16:
            result = iomem[addr] | 0xC0; // Bits 7 and 8 are unused (always 1)
            return result;
            
   		case 0x18:
            result = iomem[addr] | 0x01; // Bit 1 is unused (always 1)
            return result;
            
		case 0x19:
			result = iomem[addr] | 0x70; // Bits 4 to 6 are unused (always 1)
			return result;
            
		case 0x1A:
			result = iomem[addr] | 0xF0; // Bits 4 to 7 are unsed (always 1)
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
		// Color registers
		return iomem[addr] | 0xF0; // Bits 4 to 7 are unsed (always 1)
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
			
			// Check the DEN bit if we're in rasterline 30
            // If it's set at some point in that line, bad line conditions can occur
			if (scanline == 0x30 && (value & 0x10) != 0)
                DENwasSetInRasterline30 = true;
			
			// Bit 0 - 3 determine the vertical scroll offset. By changing these bits, the DMA line condition 
			// can appear or disappear in the middle of a rasterline.
			updateBadLineCondition();
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
						
		case 0x17:
			iomem[addr] = value;
			expansionFF |= ~value;
			return;
			
		case 0x18: // MEMORY_SETUP_REGISTER
            iomem[addr] = value;
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
    assert (source != 0);
    
    if (!BAlow)
        BAwentLowAtCycle = c64->getCycles();
    
	BAlow |= source;
	cpu->setRDY(BAlow == 0); 
}

void 
VIC::releaseBA(uint16_t source)
{ 
	BAlow &= ~source;
	cpu->setRDY(BAlow == 0); 
}

inline bool
VIC::BApulledDownForAtLeastThreeCycles()
{
    return (c64->getCycles() - BAwentLowAtCycle) > 2;
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
	pixelBuffer += totalScreenWidth;
}

void 
VIC::cycle1()
{
	// Check, if we are currently processing a DMA line. The result is stored in variable badLineCondition.
	// Be aware that the value of variable dmaLine can change in the middle of a rasterline as a side effect 
	// of modifying the y scroll offset.
	updateBadLineCondition();

	// Trigger rasterline interrupt if applicable
	// In line 0, the interrupt is triggered in cycle 2
    // In all other lines, it is triggered in cycle 1
	if (scanline == rasterInterruptLine() && scanline != 0)
		triggerIRQ(1);
			
	// Check for the DEN bit if we're processing rasterline 30
    if (scanline == 0x30 && (iomem[0x11] & 0x10) != 0)
        DENwasSetInRasterline30 = true;
    
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
    // In line 0, the interrupt is triggered in cycle 2
    // In all other lines, it is triggered in cycle 1
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
	if (badLineCondition) {
        // Freeze CPU because c-accesses are coming up...
		pullDownBA(0x100);
	}
	releaseBusForSprite(7);

	// Reset the X coordinate to 0
	// xCounter = 0;
	update_display_and_ba;
}

void
VIC::cycle13()
{
    xCounter = -4;
    // Frodo: xCounter = 0xFFFC; // -3??
    
    // We reach the left border here and start drawing
    runGraphicSequencerAtBorder();

	countX();
	update_display_and_ba;
}

void
VIC::cycle14()
{
	// NEW CODE
	// xCounter = 0x04;
	
    /* WHERE DO WE IMPLEMENT THIS ONE?
       "1. Irgendwo einmal auﬂerhalb des Bereiches der Rasterzeilen $30-$f7 (also
           außerhalb des Bad-Line-Bereiches) wird VCBASE auf Null gesetzt.
           Vermutlich geschieht dies in Rasterzeile 0, der genaue Zeitpunkt ist
           nicht zu bestimmen, er spielt aber auch keine Rolle." [C.B.] */
    
	/* "2. In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
	       (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
           Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt." [C.B.] */
    registerVC = registerVCBASE;
	registerVMLI = 0;
	if (badLineCondition)
		registerRC = 0;
    
    runGraphicSequencerAtBorder();

	countX();
	update_display_and_ba;
}

void
VIC::cycle15()
{
	/* "7. In der ersten Phase von Zyklus 15 wird geprüft, ob das
	       Expansions-Flipflop gesetzt ist. Wenn ja, wird MCBASE um 2 erhöht." [C.B.] */
    for (int i = 0; i < 8; i++) {
        uint8_t mask = (1 << i);
        if (expansionFF & mask) {
            mcbase[i] += 2;
            mcbase[i] &= 0x3F; // 6 bit counter
        }
    }

    runGraphicSequencerAtBorder();

    // Second clock phase
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle16()
{
    /* CORRECTED RULE:
       "7. In the first phase of cycle 16, it is checked if the expansion flip flop
        is set. If so, MCBASE load from MC (MC->MCBASE), unless the CPU cleared
        the Y expansion bit in $d017 in the second phase of cycle 15, in which case
        MCBASE is set to X = (101010 & (MCBASE & MC)) | (010101 & (MCBASE | MC)).
        After the MCBASE update, the VIC checks if MCBASE is equal to 63 and turns
        off the DMA of the sprite if it is." [VIC Addendum]
     */
    
	/* "8. In der ersten Phase von Zyklus 16 wird geprüft, ob das
	       Expansions-Flipflop gesetzt ist. Wenn ja, wird MCBASE um 1 erhöht.
	       Dann wird geprüft, ob MCBASE auf 63 steht und bei positivem Vergleich
	       der DMA und die Darstellung für das jeweilige Sprite abgeschaltet." [C.B.] */
			
	for (int i = 0; i < 8; i++) {
		uint8_t mask = (1 << i);
		if (expansionFF & mask) {
			mcbase[i] += 1;
			mcbase[i] &= 0x3F; // 6 bit counter
		}
		if (mcbase[i] == 63) {			
			spriteDmaOnOff &= ~mask;
		}
	}		
    runGraphicSequencerAtBorder();
    
    // First clock phase (LOW)
    gAccess();
    
    // Second clock phase (HIGH)
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle17()
{
    const uint16_t x_coord = 24;

    // Set or clear frame flipflops in 40 column mode
    if (x_coord == leftComparisonValue()) {

        // "4. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
        //     den unteren, wird das vertikale Rahmenflipflop gesetzt." [C.B.]

        if (scanline == lowerComparisonValue()) {
            verticalFrameFF = true;
        }

        // "5. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
        //     den oberen und ist das DEN-Bit in Register $d011 gesetzt, wird das
        //     vertikale Rahmenflipflop gelöscht." [C.B.]

        else if (scanline == upperComparisonValue() && DENbit()) {
            verticalFrameFF = false;
        }
    
        // "6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
        //     vertikale Rahmenflipflop gelöscht, wird das Haupt-Flipflop gelöscht." [C.B.]

        clearMainFrameFF();
    }
    
    // We reach the main screen area here (start drawing pixels)
    runGraphicSequencer();

    // First clock phase (LOW)
    gAccess();

    // Second clock phase (HIGH)
    cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle18()
{
    const uint16_t x_coord = 31;
    
    // Set or clear frame flipflops in 38 column mode
    if (x_coord == leftComparisonValue()) {
        
        // "4. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
        //     den unteren, wird das vertikale Rahmenflipflop gesetzt." [C.B.]
        
        if (scanline == lowerComparisonValue()) {
            verticalFrameFF = true;
        }
        
        // "5. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
        //     den oberen und ist das DEN-Bit in Register $d011 gesetzt, wird das
        //     vertikale Rahmenflipflop gelöscht." [C.B.]
        
        else if (scanline == upperComparisonValue() && DENbit()) {
            verticalFrameFF = false;
        }
        
        // "6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
        //     vertikale Rahmenflipflop gelöscht, wird das Haupt-Flipflop gelöscht." [C.B.]
        
        clearMainFrameFF();
    }

    runGraphicSequencer();

    // First clock phase (LOW)
    gAccess();

    // Second clock phase (HIGH)
    cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle19to54()
{
    runGraphicSequencer();
    
    // First clock phase (LOW)
    gAccess();

    // Second clock phase (HIGH)
	cAccess();
	countX();
	update_display_and_ba;
}

void
VIC::cycle55()
{
    runGraphicSequencer();

    // First clock phase (LOW)

    /* "In der ersten Phase von Zyklus 55 wird das Expansions-Flipflop
     invertiert, wenn das MxYE-Bit gesetzt ist." [C.B.] */
    expansionFF ^= iomem[0x17];

    gAccess();

    
	/* In den ersten Phasen von Zyklus 55 und 56 wird für jedes Sprite geprüft,
	 ob das entsprechende MxE-Bit in Register $d015 gesetzt und die
	 Y-Koordinate des Sprites (ungerade Register $d001-$d00f) gleich den
	 unteren 8 Bits von RASTER ist. Ist dies der Fall und der DMA für das
	 Sprite noch ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht
	 und, wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht.
	 */
	updateSpriteDmaOnOff();
    
    releaseBA(0x100);
	requestBusForSprite(0);
	countX();
	update_display;
}

void
VIC::cycle56()
{
    const uint16_t x_coord = 335;
    
    // "1. Erreicht die X-Koordinate den rechten Vergleichswert, wird das
     //     Haupt-Rahmenflipflop gesetzt." [C.B.]

     if (x_coord == rightComparisonValue()) {
         mainFrameFF = true;
     }

    runGraphicSequencer();
    
	updateSpriteDmaOnOff();
    requestBusForSprite(0); // Bus is again requested because DMA conditions may have changed
	countX();
	update_display;
}

void
VIC::cycle57()
{
    const uint16_t x_coord = 344;
    
    // "1. Erreicht die X-Koordinate den rechten Vergleichswert, wird das
    //     Haupt-Rahmenflipflop gesetzt." [C.B.]
    
    if (x_coord == rightComparisonValue()) {
        mainFrameFF = true;
    }
    
    // We reach the right border here (stop drawing pixels)
    runGraphicSequencerAtBorder();
    
	requestBusForSprite(1);
	countX();
	update_display;
}

void
VIC::cycle58()
{
    runGraphicSequencerAtBorder();

	/* "Der Übergang vom Display- in den Idle-Zustand erfolgt in Zyklus 58 einer Zeile,
	    wenn der RC den Wert 7 hat und kein Bad-Line-Zustand vorliegt."
	    "5. In der ersten Phase von Zyklus 58 wird geprüft, ob RC=7 ist. Wenn ja,
            geht die Videologik in den Idle-Zustand und VCBASE wird mit VC geladen
	        (VC->VCBASE). Ist die Videologik danach im Display-Zustand (liegt ein
            Bad-Line-Zustand vor, ist dies immer der Fall), wird RC erhöht." [C.B.] */
    if (displayState && registerRC == 7 && !badLineCondition) {
        displayState = false;
		registerVCBASE = registerVC;	
	}
    // "(liegt ein Bad-Line-Zustand vor, ist dies immer der Fall)"
    // According to this, do we really need || badLineCondition() in the next statement?
	if (displayState || badLineCondition) {
		registerRC++;
		registerRC &= 0x07;  // 3 bit overflow
	}
			
	/* "4. In der ersten Phase von Zyklus 58 wird für jedes Sprite MC mit MCBASE
	    geladen (MCBASE->MC) und geprüft, ob der DMA für das Sprite angeschaltet
	    und die Y-Koordinate des Sprites gleich den unteren 8 Bits von RASTER
	    ist. Ist dies der Fall, wird die Darstellung des Sprites angeschaltet." [C.B.] */
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
    runGraphicSequencerAtBorder();

	readSpriteData(0);
	readSpriteData(0);
	requestBusForSprite(2);
	countX();
	update_display;
}

void
VIC::cycle60()
{
    // This is the last invocation of the graphic sequencer
    runGraphicSequencerAtBorder();

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

    // "2. Erreicht die Y-Koordinate den unteren Vergleichswert in Zyklus 63, wird
    //     das vertikale Rahmenflipflop gesetzt." [C.B.]

    if (scanline == lowerComparisonValue()) {
        verticalFrameFF = true;
    }

    // "3. Erreicht die Y-Koordinate den oberern Vergleichswert in Zyklus 63 und
    //     ist das DEN-Bit in Register $d011 gesetzt, wird das vertikale
    //     Rahmenflipflop gelöscht." [C.B.]
    
    else if (scanline == upperComparisonValue() && DENbit()) {
        verticalFrameFF = false;
    }
			
	// draw border
    // TODO: HANDLE IN EACH CYCLE
    // drawBorder();
			
	// illegal display modes cause a black line to appear
	if (getDisplayMode() > EXTENDED_BACKGROUND_COLOR) markLine(xStart(), SCREEN_WIDTH, colors[BLACK]);

	// draw debug markers
	if (markIRQLines && scanline == rasterInterruptLine()) 
		markLine(0, totalScreenWidth, colors[WHITE]);
	if (markDMALines && badLineCondition)	
		markLine(0, totalScreenWidth, colors[RED]);
	if (rasterlineDebug[scanline] >= 0) {
		markLine(0, totalScreenWidth, colors[rasterlineDebug[scanline] % 16]);
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
VIC::markLine(int start, unsigned length, int color)
{
	for (unsigned i = 0; i < length; i++) {
		pixelBuffer[start + i] = color;
	}	
}

