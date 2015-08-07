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


// TODO: Delay drawing by one cycle
// To do so, ...
// add typedef struct drawingContext
// Stores everything that is needed to draw a chunk of pixels
// Enables us to delay drawing by some cycles

// Old sequence is simulated by:
// 1. updateDrawingContext
// 1. draw
// 2. updateDrawingContext
// 2. draw

// Move draw one cycle behind
// 1. updateDrawingContext
// 2. draw
// 2. updateDrawingContext
// 3. draw

#include "C64.h"

// DIRK
unsigned dirktrace = 0;
unsigned dirkcnt = 0;



VIC::VIC(C64 *c64)
{
	name = "VIC";

	debug(2, "  Creating VIC at address %p...\n", this);

	this->c64 = c64;
	
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
	
    // Establish bindungs
    cpu = c64->cpu;
    mem = c64->mem;
    
	// Internal registers
    frame = 0;
    xCounter = 0;
    yCounter = 0;
    yCounterEqualsIrqRasterline = false;
	registerVC = 0;
	registerVCBASE = 0;
	registerRC = 0;
	registerVMLI = 0;
    refreshCounter = 0;
    addrBus = 0;
    dataBus = 0;
    gAccessDisplayMode = 0;
    gAccessfgColor = 0;
    gAccessbgColor = 0;
	badLineCondition = false;
    DENwasSetInRasterline30 = false;
	displayState = false;
	BAlow = 0;
    BAwentLowAtCycle = 0;
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
	
    // Graphic sequencer
    gs_data = 0;
    gs_data_old = 0;
    gs_mode = STANDARD_TEXT;
    
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
	expansionFF = 0xFF;
    cleared_bits_in_d017 = 0;
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
			
uint32_t
VIC::stateSize()
{
    return 158;
}

void
VIC::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

	// Internal registers
    xCounter = read16(buffer);
	yCounter = read32(buffer);
    yCounterEqualsIrqRasterline = (bool)read8(buffer);
    registerVC = read16(buffer);
	registerVCBASE = read16(buffer);
	registerRC = read8(buffer);
	registerVMLI = read8(buffer);
    oldControlReg1 = read8(buffer);
    refreshCounter = read8(buffer);
    addrBus = read16(buffer);
    dataBus = read8(buffer);
    gAccessDisplayMode = read8(buffer);
    gAccessfgColor = read8(buffer);
    gAccessbgColor = read8(buffer);
	badLineCondition = (bool)read8(buffer);
	DENwasSetInRasterline30 = (bool)read8(buffer);
	displayState = (bool)read8(buffer);
	BAlow = read16(buffer);
    BAwentLowAtCycle = read64(buffer);
	mainFrameFF = (bool)read8(buffer);
	verticalFrameFF = (bool)read8(buffer);
    
	// Memory
    readBlock(buffer, iomem, sizeof(iomem));
	bankAddr = read16(buffer);
    // readBlock(buffer, characterSpace, sizeof(characterSpace));
    // readBlock(buffer, colorSpace, sizeof(colorSpace));

    // Sequencer
    gs_shift_reg = read8(buffer);
    gs_mc_flop = (bool)read8(buffer);
    LatchedCharacterSpace = read8(buffer);
    LatchedColorSpace = read8(buffer);
    gs_data = read8(buffer);
    gs_characterSpace = read8(buffer);
    gs_colorSpace = read8(buffer);
    gs_data_old = read8(buffer);
    gs_mode = (DisplayMode)read8(buffer);
    gs_delay = read8(buffer);
    
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
    cleared_bits_in_d017 = read8(buffer);
    
	// Lightpen
	lightpenIRQhasOccured = (bool)read8(buffer);
    
    debug(2, "  VIC state loaded (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void
VIC::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;

	// Internal registers
    write16(buffer, xCounter);
    write32(buffer, yCounter);
    write8(buffer, (uint8_t)yCounterEqualsIrqRasterline);
    write16(buffer, registerVC);
	write16(buffer, registerVCBASE);
	write8(buffer, registerRC);
	write8(buffer, registerVMLI);
    write8(buffer, oldControlReg1);
    write8(buffer, refreshCounter);
    write16(buffer, addrBus);
    write8(buffer, dataBus);
    write8(buffer, gAccessDisplayMode);
    write8(buffer, gAccessfgColor);
    write8(buffer, gAccessbgColor);
	write8(buffer, (uint8_t)badLineCondition);
	write8(buffer, (uint8_t)DENwasSetInRasterline30);
	write8(buffer, (uint8_t)displayState);
	write16(buffer, BAlow);
    write64(buffer, BAwentLowAtCycle);
	write8(buffer, (uint8_t)mainFrameFF);
	write8(buffer, (uint8_t)verticalFrameFF);
	
	// Memory
    writeBlock(buffer, iomem, sizeof(iomem));
	write16(buffer, bankAddr);
    // writeBlock(buffer, characterSpace, sizeof(characterSpace));
    // writeBlock(buffer, colorSpace, sizeof(colorSpace));

    // Sequencer
    write8(buffer, gs_shift_reg);
    write8(buffer, (uint8_t)gs_mc_flop);
    write8(buffer, LatchedCharacterSpace);
    write8(buffer, LatchedColorSpace);
    write8(buffer, gs_data);
    write8(buffer, gs_characterSpace);
    write8(buffer, gs_colorSpace);
    write8(buffer, gs_data_old);
    write8(buffer, (uint8_t)gs_mode);
    write8(buffer, gs_delay);
    
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
    write8(buffer, cleared_bits_in_d017);

	// Lightpen
	write8(buffer, lightpenIRQhasOccured);
    
    debug(4, "  VIC state saved (%d bytes)\n", *buffer - old);
    assert(*buffer - old == stateSize());
}

void 
VIC::dumpState()
{
	msg("VIC\n");
	msg("---\n\n");
	msg("     Bank address : %04X\n", bankAddr, bankAddr);
    msg("    Screen memory : %04X\n", getScreenMemoryAddr());
	msg(" Character memory : %04X\n", getCharacterMemoryAddr());
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
	msg("            (X,Y) : (%d,%d) %s %s\n", xCounter, yCounter,  badLineCondition ? "(DMA line)" : "", DENwasSetInRasterline30 ? "" : "(DMA lines disabled, no DEN bit in rasterline 30)");
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
    isPAL = true;
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
    isPAL = false;
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

// TODO: VICE distinguishes between phi1 accesses (clock LOW) and phi2 accesses (clock HIGH)
uint8_t VIC::memAccess(uint16_t addr)
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
    
    addrBus = bankAddr + addr;
    
    if ((addrBus & 0x7000) == 0x1000) {

        // Accessing range 0x1000 - 0x1FFF or 0x9000 - 0x9FFF
        // Character ROM is blended in here
        assert ((0xC000 + addr) >= 0xD000 && (0xC000 + addr) <= 0xDFFF);
        dataBus = mem->rom[0xC000 + addr];

    } else {
        dataBus = mem->ram[addrBus];
    }
    
    return dataBus;
}

uint8_t VIC::memIdleAccess()
{
    // TODO: Optimize
    return memAccess(0x3FFF);
}

inline void VIC::cAccess()
{
    // Only proceed if the BA line is pulled down
    if (!badLineCondition)
        return;

    // If BA is pulled down for at least three cycles, perform memory access
    if (BApulledDownForAtLeastThreeCycles()) {
        
        // |VM13|VM12|VM11|VM10| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0|
        uint16_t addr = (VM13VM12VM11VM10() << 6) | registerVC;
        
        characterSpace[registerVMLI] = memAccess(addr);
        colorSpace[registerVMLI] = mem->colorRam[registerVC] & 0x0F;
    }
    
    // VIC has no access, yet
    else {
        
        /* "Trotzdem greift der VIC auf die Videomatrix zu, oder versucht es zumindest,
            denn solange AEC in der zweiten Taktphase noch High ist, sind die
            Adressbustreiber und Datenbustreiber D0-D7 des VIC im Tri-State und der VIC
            liest statt der Daten aus der Videomatrix in den ersten drei Zyklen den
            Wert $ff an D0-D7. Die Datenleitungen D8-D13 des VIC haben allerdings
            keinen Tri-State-Treiber und sind immer auf Eingang geschaltet. Allerdings
            bekommt der VIC auch dort keine g¸ltigen Farb-RAM-Daten, denn da AEC High
            ist, kontrolliert offiziell der 6510 noch den Bus und sofern dieser nicht
            zufällig gerade den nächsten Opcode vom Farb-RAM lesen will, ist der
            Chip-Select-Eingang des Farb-RAMs nicht aktiv.
         
            Lange Rede, kurzer Sinn: Der VIC liest in den ersten drei
            Zyklen, nachdem BA auf Low gegangen ist als Zeichenzeiger $ff und als
            Farbinformation die untersten 4 Bit des Opcodes nach dem Zugriff auf $d011.
            Erst danach werden wieder reguläre Videomatrixdaten gelesen." [C.B.] */
        
        characterSpace[registerVMLI] = 0xFF;
        colorSpace[registerVMLI] = c64->mem->ram[cpu->getPC()] & 0x0F;
    }
}

inline void VIC::gAccess()
{
    uint16_t addr;
    
    assert ((registerVC & 0xFC00) == 0); // 10 bit register
    assert ((registerRC & 0xF8) == 0);   // 3 bit register

    if (displayState) {

        // "Der Adressgenerator für die Text-/Bitmap-Zugriffe (c- und g-Zugriffe)
        //  besitzt bei den g-Zugriffen im wesentlichen 3 Modi (die c-Zugriffe erfolgen
        //  immer nach dem selben Adressschema). Im Display-Zustand wählt das BMM-Bit
        //  entweder Zeichengenerator-Zugriffe (BMM=0) oder Bitmap-Zugriffe (BMM=1)
        //  aus" [C.B.]
        
        //  BMM = 1 : |CB13| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0| RC2| RC1| RC0|
        //  BMM = 0 : |CB13|CB12|CB11| D7 | D6 | D5 | D4 | D3 | D2 | D1 | D0 | RC2| RC1| RC0|
        
        if (BMMbitInPreviousCycle()) {
            addr = (CB13() << 10) | (registerVC << 3) | registerRC;
        } else {
            addr = (CB13CB12CB11() << 10) | (characterSpace[registerVMLI] << 3) | registerRC;
        }

        // "Bei gesetztem ECM-Bit schaltet der Adressgenerator bei den g-Zugriffen die
        //  Adressleitungen 9 und 10 immer auf Low, bei ansonsten gleichem Adressschema
        //  (z.B. erfolgen dann die g-Zugriffe im Idle-Zustand an Adresse $39ff)." [C.B.]
        
        if (ECMbitInPreviousCycle())
            addr &= 0xF9FF;

        // Prepare graphic sequencer
        gs_data = memAccess(addr);
        gs_delay = getHorizontalRasterScroll();
        gs_characterSpace = characterSpace[registerVMLI];
        gs_colorSpace = colorSpace[registerVMLI];
        gs_mode = getDisplayMode();

        // "Nach jedem g-Zugriff im Display-Zustand werden VC und VMLI erhöht." [C.B.]
        registerVC++;
        registerVC &= 0x3FF; // 10 bit overflow
        registerVMLI++;
        registerVMLI &= 0x3F; // 6 bit overflow
        
    } else {
    
        // "Im Idle-Zustand erfolgen die g-Zugriffe immer an Videoadresse $3fff." [C.B.]
        addr = ECMbitInPreviousCycle() ? 0x39FF : 0x3FFF;
        
        // Prepare graphic sequencer
        gs_data = memAccess(addr);
        gs_delay = getHorizontalRasterScroll();
        gs_characterSpace = 0;
        gs_colorSpace = 0;
        gs_mode = getDisplayMode();

    }
}

inline void VIC::pAccess(int sprite)
{
    // |VM13|VM12|VM11|VM10|  1 |  1 |  1 |  1 |  1 |  1 |  1 |  Spr.-Nummer |
    spritePtr[sprite] = memAccess((VM13VM12VM11VM10() << 6) | 0x03F8 | sprite) << 6;

}

inline bool VIC::sFirstAccess(int sprite)
{
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    bool memAccessed = false;
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        if (BApulledDownForAtLeastThreeCycles()) {
            data = memAccess(spritePtr[sprite] | mc[sprite]);
            memAccessed = true;
        }

        mc[sprite]++;
        mc[sprite] &= 0x3F; // 6 bit overflow
    }
    
    spriteShiftReg[sprite][0] = data;
    return memAccessed;
}

inline bool VIC::sSecondAccess(int sprite)
{
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    bool memAccessed = false;
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        if (BApulledDownForAtLeastThreeCycles()) {
            data = memAccess(spritePtr[sprite] | mc[sprite]);
            memAccessed = true;
        }
        
        mc[sprite]++;
        mc[sprite] &= 0x3F; // 6 bit overflow
    }
    
    // If no memory access has happened here, we perform an idle access
    // The obtained data might be overwritten by the third sprite access
    if (!memAccessed)
        memIdleAccess();
    
    spriteShiftReg[sprite][1] = data;
    return memAccessed;
}

inline bool VIC::sThirdAccess(int sprite)
{
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    bool memAccessed = false;
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        if (BApulledDownForAtLeastThreeCycles()) {
            data = memAccess(spritePtr[sprite] | mc[sprite]);
            memAccessed = true;
        }

        mc[sprite]++;
        mc[sprite] &= 0x3F; // 6 bit overflow
    }
    
    spriteShiftReg[sprite][2] = data;
    return memAccessed;
}

// -----------------------------------------------------------------------------------------------
//                                         Graphics sequencer
// -----------------------------------------------------------------------------------------------

void
VIC::prepareDrawingContextForCycle(uint8_t cycle)
{
    dc.cycle = cycle;
    dc.yCounter = yCounter;
    dc.xCounter = xCounter;
    dc.verticalFrameFF = verticalFrameFF;
    dc.mainFrameFF = mainFrameFF;
    dc.data = gs_data;
    dc.characterSpace = gs_characterSpace;
    dc.colorSpace = gs_colorSpace;
    dc.mode = gs_mode;
}

void
VIC::updateDrawingContext()
{
    dc.borderColor = getBorderColor();
    dc.backgroundColor[0] = getBackgroundColor();
    dc.backgroundColor[1] = getExtraBackgroundColor(1);
    dc.backgroundColor[2] = getExtraBackgroundColor(2);
    dc.backgroundColor[3] = getExtraBackgroundColor(3);
    dc.delay = gs_delay;
}

void
VIC::draw()
{
    updateDrawingContext();
    drawPixels(dc.cycle); // get rid of parameter
    // drawSprites()
    drawBorder();
    // synthesizePixels();
}

void
VIC::drawBorder()
{
    uint16_t xCoord = (dc.xCounter - 28) + leftBorderWidth;

    // Take special care of 38 column mode
    
    if (dc.cycle == 17 && dc.mainFrameFF && !mainFrameFF) {
        int border_rgba = colors[dc.borderColor];
        drawSevenFramePixels(xCoord, border_rgba);
        // gs_data = 0; ????
        return;
    }
    
    if (dc.cycle == 55 && !dc.mainFrameFF && mainFrameFF) {
        int border_rgba = colors[dc.borderColor];
        setFramePixel(xCoord+7, border_rgba);
        // gs_data = 0; ????
        return;
    }

    // Standard case
    
    if (dc.mainFrameFF) {
        int border_rgba = colors[dc.borderColor];
        drawEightFramePixels(xCoord, border_rgba);
        gs_data = 0; // ????? What is that for?
        return;
    }
    
    /* "Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    // SHOULD BE DONE IN DRAW PIXELS ???? MIGHT BE WRONG ANYWAY ...
    if ((dc.cycle >= 13 && dc.cycle <= 16) || (dc.cycle >= 57 && dc.cycle <= 60))
        drawEightBehindBackgroudPixels(xCoord);

}

void
VIC::drawBorderDeprecated()
{
    updateDrawingContext();
    drawBorderArea(dc.cycle);
}

void VIC::drawPixels(uint8_t cycle)
{
    // assert(cycle >= 17 && cycle <= 56);
    assert(cycle >= 13 && cycle <= 60);
    
    uint16_t xCoord = (dc.xCounter - 28) + leftBorderWidth;
    
    /* "Der Sequenzer gibt die Grafikdaten in jeder Rasterzeile im Bereich der
     Anzeigespalte aus, sofern das vertikale Rahmenflipflop gelöscht ist (siehe
     Abschnitt 3.9.). Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    
    if (!dc.verticalFrameFF) {
        
        drawPixel(xCoord, 0);
        drawPixel(xCoord + 1, 1);
        drawPixel(xCoord + 2, 2);
        drawPixel(xCoord + 3, 3);
        drawPixel(xCoord + 4, 4);
        drawPixel(xCoord + 5, 5);
        drawPixel(xCoord + 6, 6);
        drawPixel(xCoord + 7, 7);
        
    } else {
        drawEightBehindBackgroudPixels(xCoord);
    }
}

void VIC::drawBorderArea(uint8_t cycle)
{
    assert((cycle >= 13 && cycle <= 16) || (cycle >= 57 && cycle <= 60));
    
    uint16_t xCoord = (dc.xCounter - 28) + leftBorderWidth;
    
    // draw border
    if (dc.mainFrameFF) {
        
        int border_rgba = colors[dc.borderColor];

        /*
        if (dc.borderColor == 11 && dirktrace == 0) {
            dirktrace = 1; // ON
        }
        
        if (dirktrace == 1) {
            printf("Drawing with border color %d %s (left/right border area)\n",
                   dc.borderColor, dc.borderColor != 10 ? "******" : "");
        }
        */
        
        drawEightFramePixels(xCoord, border_rgba);
        return;
    }
    
    /* "Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
     die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
     normalerweise vom Rahmen überdeckt)." [C.B.] */
    drawEightBehindBackgroudPixels(xCoord);
    
}

void VIC::loadPixelSynthesizerWithColors(DisplayMode mode, uint8_t characterSpace, uint8_t colorSpace)
{
    switch (gs_mode) {
            
        case STANDARD_TEXT:
            col_rgba[0] = colors[dc.backgroundColor[0]];
            col_rgba[1] = colors[dc.colorSpace];
            multicol = false;
            break;
            
        case MULTICOLOR_TEXT:
            if (colorSpace & 0x8 /* MC flag */) {
                col_rgba[0] = colors[dc.backgroundColor[0]];
                col_rgba[1] = colors[dc.backgroundColor[1]];
                col_rgba[2] = colors[dc.backgroundColor[2]];
                col_rgba[3] = colors[dc.colorSpace & 0x07];
                multicol = true;
            } else {
                col_rgba[0] = colors[dc.backgroundColor[0]];
                col_rgba[1] = colors[dc.colorSpace];
                multicol = false;
            }
            break;
            
        case STANDARD_BITMAP:
            col_rgba[0] = colors[dc.characterSpace & 0x0F];
            col_rgba[1] = colors[dc.characterSpace >> 4];
            multicol = false;
            break;
            
        case MULTICOLOR_BITMAP:
            col_rgba[0] = colors[dc.backgroundColor[0]];
            col_rgba[1] = colors[dc.characterSpace >> 4];
            col_rgba[2] = colors[dc.characterSpace & 0x0F];
            col_rgba[3] = colors[dc.colorSpace];
            multicol = true;
            break;
            
        case EXTENDED_BACKGROUND_COLOR:
            col_rgba[0] = colors[dc.backgroundColor[characterSpace >> 6]];
            col_rgba[1] = colors[dc.colorSpace];
            multicol = false;
            break;
            
        case INVALID_TEXT:
            col_rgba[0] = colors[BLACK];
            col_rgba[1] = colors[BLACK];
            col_rgba[2] = colors[BLACK];
            col_rgba[3] = colors[BLACK];
            multicol = (dc.colorSpace & 0x8 /* MC flag */);
            break;
            
        case INVALID_STANDARD_BITMAP:
            col_rgba[0] = colors[BLACK];
            col_rgba[1] = colors[BLACK];
            multicol = false;
            break;
            
        case INVALID_MULTICOLOR_BITMAP:
            col_rgba[0] = colors[BLACK];
            col_rgba[1] = colors[BLACK];
            col_rgba[2] = colors[BLACK];
            col_rgba[3] = colors[BLACK];
            multicol = true;
            break;
            
        default:
            assert(0);
            break;
    }
}

void VIC::drawPixel(uint16_t offset, uint8_t pixel)
{
    assert(pixel < 8);
    
    if (pixel == dc.delay) {
        
        // Load shift register
        gs_shift_reg = dc.data;
        // Remember how to synthesize pixels
        LatchedCharacterSpace = dc.characterSpace;
        LatchedColorSpace = dc.colorSpace;
        gs_mc_flop = true;
    }
    
    if (gs_mc_flop) {
        // Determine pixel colors and render
        loadPixelSynthesizerWithColors(dc.mode,LatchedCharacterSpace,LatchedColorSpace);
        if (multicol) {
            renderTwoMultiColorPixels(gs_shift_reg >> 6);
        } else {
            renderTwoSingleColorPixels(gs_shift_reg >> 6);
        }
    }
    
    // Copy pixel to pixel buffer
    pixelBuffer[offset] = pixelBufferTmp[0];
    pixelBufferTmp[0] = pixelBufferTmp[1];
    // pixelBufferTmp[1] = 0x00;
    
    zBuffer[offset] = zBufferTmp[0];
    zBufferTmp[0] = zBufferTmp[1];
    // zBufferTmp[1] = 0x00;
    
    pixelSource[offset] = pixelSourceTmp[0];
    pixelSourceTmp[0] = pixelSourceTmp[1];
    // pixelSourceTmp[1] = 0x00;

    // Shift register and toggle flipflop
    gs_shift_reg <<= 1;
    gs_mc_flop = !gs_mc_flop;
}



// -----------------------------------------------------------------------------------------------
//                                           Drawing
// -----------------------------------------------------------------------------------------------

inline void
VIC::setFramePixel(unsigned offset, int rgba)
{
    zBuffer[offset] = BORDER_LAYER_DEPTH;
    pixelBuffer[offset] = rgba;
    pixelSource[offset] &= (~0x80); // disable sprite/foreground collision detection in border
}

inline void
VIC::setSpritePixel(unsigned offset, int rgba, int depth, int source)
{
    assert (depth >= SPRITE_LAYER_FG_DEPTH && depth <= SPRITE_LAYER_BG_DEPTH + 8);

    if (depth <= zBuffer[offset]) {
        zBuffer[offset] = depth;
        pixelBuffer[offset] = rgba;
    }
    pixelSource[offset] |= source;
}

// DEPRECATED
inline void
VIC::setForegroundPixel(unsigned offset, int rgba)
{
    if (FOREGROUND_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = FOREGROUND_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
        pixelSource[offset] |= 0x80;
    }
}

inline void
VIC::renderForegroundPixel(unsigned offset, int rgba)
{
    assert(offset == 0 || offset == 1);
    
    zBufferTmp[offset] = FOREGROUND_LAYER_DEPTH;
    pixelBufferTmp[offset] = rgba;
    pixelSourceTmp[offset] |= 0x80;
}


// DEPRECATED
inline void
VIC::setBackgroundPixel(unsigned offset, int rgba)
{
    if (BACKGROUD_LAYER_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = BACKGROUD_LAYER_DEPTH;
        pixelBuffer[offset] = rgba;
    }
}

inline void
VIC::renderBackgroundPixel(unsigned offset, int rgba)
{
    assert(offset == 0 || offset == 1);
    
    zBufferTmp[offset] = BACKGROUD_LAYER_DEPTH;
    pixelBufferTmp[offset] = rgba;
    pixelSourceTmp[offset] = 0;
}

// DEPRECATED
inline void
VIC::setBehindBackgroundPixel(unsigned offset, int rgba)
{
    if (BEIND_BACKGROUND_DEPTH <= zBuffer[offset]) {
        zBuffer[offset] = BEIND_BACKGROUND_DEPTH;
        pixelBuffer[offset] = rgba;
    }
}

// DEPRECATED
void
VIC::drawEightBehindBackgroudPixels(unsigned offset)
{
    /* "Der Sequenzer gibt die Grafikdaten in jeder Rasterzeile im Bereich der
        Anzeigespalte aus, sofern das vertikale Rahmenflipflop gelöscht ist (siehe
        Abschnitt 3.9.). Außerhalb der Anzeigespalte und bei gesetztem Flipflop wird
        die letzte aktuelle Hintergrundfarbe dargestellt (dieser Bereich ist
        normalerweise vom Rahmen überdeckt)." [C.B.] */

    // int bg_rgba = colors[gs_bg_color_old]; // TODO: WE PROBABLY SELECT THE WRONG COLOR HERE
    loadPixelSynthesizerWithColors(gs_mode,LatchedCharacterSpace,LatchedColorSpace);
    for (unsigned i = 0; i < 8; i++) {
        setBehindBackgroundPixel(offset++, col_rgba[0]);
    }    
}

#if 0
inline void
VIC::renderSingleColorPixels(unsigned offset)
{
}
#endif

inline void
VIC::renderTwoSingleColorPixels(uint8_t bits)
{
    if (bits & 0x02)
        renderForegroundPixel(0, col_rgba[1]);
    else
        renderBackgroundPixel(0, col_rgba[0]);
    
    if (bits & 0x01)
        renderForegroundPixel(1, col_rgba[1]);
    else
        renderBackgroundPixel(1, col_rgba[0]);
}

// DEPRECATED
inline void
VIC::drawTwoSingleColorPixels(unsigned offset, uint8_t bits)
{
    if (bits & 0x02)
        setForegroundPixel(offset++, col_rgba[1]);
    else
        setBackgroundPixel(offset++, col_rgba[0]);

    if (bits & 0x01)
        setForegroundPixel(offset, col_rgba[1]);
    else
        setBackgroundPixel(offset, col_rgba[0]);
}

// DEPRECATED
inline void 
VIC::drawSingleColorCharacter(unsigned offset)
{
    // int fg_rgba = colors[gs_fg_color];
    // int bg_rgba = colors[gs_bg_color];
    
	assert(offset >= 0 && offset+7 < MAX_VIEWABLE_PIXELS);

    drawTwoSingleColorPixels(offset, gs_data >> 6);
    drawTwoSingleColorPixels(offset + 2, gs_data >> 4);
    drawTwoSingleColorPixels(offset + 4, gs_data >> 2);
    drawTwoSingleColorPixels(offset + 6, gs_data);
}

inline void
VIC::renderTwoMultiColorPixels(uint8_t bits)
{
    int rgba = col_rgba[bits & 0x03];
    
    if (bits & 0x02) {
        renderForegroundPixel(0, rgba);
        renderForegroundPixel(1, rgba);
    } else {
        renderBackgroundPixel(0, rgba);
        renderBackgroundPixel(1, rgba);
    }
}

inline void
VIC::drawTwoMultiColorPixels(unsigned offset, uint8_t bits)
{
    int rgba = col_rgba[bits & 0x03];
    
    if (bits & 0x02) {
        setForegroundPixel(offset++, rgba);
        setForegroundPixel(offset++, rgba);
    } else {
        setBackgroundPixel(offset++, rgba);
        setBackgroundPixel(offset++, rgba);
    }
}

inline void
VIC::drawMultiColorCharacter(unsigned offset)
{
	assert(offset+7 < MAX_VIEWABLE_PIXELS);

    drawTwoMultiColorPixels(offset, gs_data >> 6);
    drawTwoMultiColorPixels(offset + 2, gs_data >> 4);
    drawTwoMultiColorPixels(offset + 4, gs_data >> 2);
    drawTwoMultiColorPixels(offset + 6, gs_data);
}

inline void
VIC::drawTwoInvalidSingleColorPixels(unsigned offset, uint8_t bits)
{
    drawTwoSingleColorPixels(offset, bits);
}

inline void
VIC::drawInvalidSingleColorCharacter(unsigned offset)
{
    assert(offset+7 < MAX_VIEWABLE_PIXELS);

    drawTwoInvalidSingleColorPixels(offset, gs_data >> 6);
    drawTwoInvalidSingleColorPixels(offset + 2, gs_data >> 4);
    drawTwoInvalidSingleColorPixels(offset + 4, gs_data >> 2);
    drawTwoInvalidSingleColorPixels(offset + 6, gs_data);
}

inline void
VIC::drawTwoInvalidMultiColorPixels(unsigned offset, uint8_t bits)
{
    drawTwoMultiColorPixels(offset, bits);
}

inline void
VIC::drawInvalidMultiColorCharacter(unsigned offset)
{
    assert(offset+7 < MAX_VIEWABLE_PIXELS);
    
    drawTwoInvalidMultiColorPixels(offset, gs_data >> 6);
    drawTwoInvalidMultiColorPixels(offset + 2, gs_data >> 4);
    drawTwoInvalidMultiColorPixels(offset + 4, gs_data >> 2);
    drawTwoInvalidMultiColorPixels(offset + 6, gs_data);
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
		if (spriteSpriteCollisionEnabled && (pixelSource[offset] & 0x7F)) {
			iomem[0x1E] |= ((pixelSource[offset] & 0x7F) | mask);
			triggerIRQ(4);
		}
		
		// Check sprite/background collision
		if (spriteBackgroundCollisionEnabled && (pixelSource[offset] & 0x80)) {
			iomem[0x1F] |= mask;
			triggerIRQ(2);
		}
		
        if (nr == 7)
            mask = 0;
        
        setSpritePixel(offset, color, spriteDepth(nr), mask);
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
		offset = spriteX + (leftBorderWidth - 24);
	else
		offset = spriteX + (leftBorderWidth - 24) - 488;
	
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
}

uint16_t
VIC::getScreenMemoryAddr()
{
    return VM13VM12VM11VM10() << 6;
}

void
VIC::setScreenMemoryAddr(uint16_t addr)
{
    assert((addr & ~0x3C00) == 0);
    
    addr >>= 6;
    iomem[0x18] = (iomem[0x18] & ~0xF0) | (addr & 0xF0);
}

uint16_t
VIC::getCharacterMemoryAddr()
{
    return (CB13CB12CB11() << 10) % 0x4000;
}


void 
VIC::setCharacterMemoryAddr(uint16_t addr)
{
    assert((addr & ~0x3800) == 0);
	
    addr >>= 10;
    iomem[0x18] = (iomem[0x18] & ~0x0E) | (addr & 0x0E);
}

uint8_t 
VIC::peek(uint16_t addr)
{
	uint8_t result;
		
	assert(addr <= VIC_END_ADDR - VIC_START_ADDR);
	
	switch(addr) {
		case 0x11: // SCREEN CONTROL REGISTER #1
			result = (iomem[addr] & 0x7f) + (yCounter > 0xff ? 128 : 0);
			return result;
            
		case 0x12: // VIC_RASTER_READ_WRITE
			result = yCounter & 0xff;
			return result;
            
		case 0x13: // LIGHTPEN X
			return iomem[addr];
            
		case 0x14: // LIGHTPEN Y
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
				if (yCounter == rasterInterruptLine())
					triggerIRQ(1);
			} else {
				iomem[addr] = value;
			}
			
			// Check the DEN bit if we're in rasterline 30
            // If it's set at some point in that line, bad line conditions can occur
			if (yCounter == 0x30 && (value & 0x10) != 0)
                DENwasSetInRasterline30 = true;
			
			// Bits 0 - 3 determine the vertical scroll offset.
            // Changing these bits directly affects the badline line condition the middle of a rasterline
			updateBadLineCondition();
			return;
			
		case 0x12: // RASTER_COUNTER
			if (iomem[addr] != value) {
				// Value changed: Check if we need to trigger an interrupt immediately
				iomem[addr] = value;
				if (yCounter == rasterInterruptLine())
					triggerIRQ(1);
			} else {
				iomem[addr] = value;
			}
			return;
						
		case 0x17: // SPRITE Y EXPANSION
			iomem[addr] = value;
            cleared_bits_in_d017 = (~value) & (~expansionFF);
            
            /* "1. Das Expansions-Flipflop ist gesetzt, solange das zum jeweiligen Sprite
                   gehörende Bit MxYE in Register $d017 gelöscht ist." [C.B.] */
            
			expansionFF |= ~value;
			return;
			
		case 0x18: // MEMORY_SETUP_REGISTER
            iomem[addr] = value;
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

inline void
VIC::setBAlow(bool value)
{
    if (!BAlow && value) {
        BAwentLowAtCycle = c64->getCycles();
    }
    BAlow = value;
    cpu->setRDY(value == 0);
}

inline bool
VIC::BApulledDownForAtLeastThreeCycles()
{
    return BAlow && (c64->getCycles() - BAwentLowAtCycle > 2);
}

void 
VIC::triggerIRQ(uint8_t source)
{
	iomem[0x19] |= source;
	if (iomem[0x1A] & source) {
		// Interrupt is enabled
		iomem[0x19] |= 128;
		cpu->setIRQLineVIC();
		// debug("Interrupting at rasterline %x %d\n", yCounter, yCounter);
	}
}

void
VIC::triggerLightPenInterrupt()
{
    // https://svn.code.sf.net/p/vice-emu/code/testprogs/VICII/lp-trigger/

    if (!lightpenIRQhasOccured) {

		// lightpen interrupts can only occur once per frame
		lightpenIRQhasOccured = true;

		// determine current coordinates
        int x = xCounter;
        int y = yCounter;
				
		// latch coordinates 
		iomem[0x13] = x / 2; // value equals the current x coordinate divided by 2
		iomem[0x14] = y;

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
			if (y == (yCounter & 0xff)) {
				spriteDmaOnOff |= (1 << i);
				mcbase[i] = 0;
				if (spriteHeightIsDoubled(i))
					expansionFF &= ~(1 << i);
			}
		}
	}
}

// -----------------------------------------------------------------------------------------------
//                                      Frame flipflops
// -----------------------------------------------------------------------------------------------

void
VIC::dirk()
{
/*
    
    unsigned cycle = c64->rasterlineCycle;

    if (dirktrace == 1) {
        printf("(%i,%i) D012:%d BAlow:%d RDY:%d displayState:%d RC:%d VC:%d VCbase:%d VMLI:%d\n",
               yCounter,cycle,iomem[0x12],BAlow,cpu->getRDY(),
               displayState, registerRC, registerVC, registerVCBASE, registerVMLI);
    }
*/
}

void
VIC::checkVerticalFrameFF()
{
    // Check for upper border
    if (yCounter == upperComparisonValue() && DENbit()) {
        verticalFrameFFclearCond = true;
    }
    // Trigger immediately (similar to VICE)
    if (verticalFrameFFclearCond) {
        verticalFrameFF = false;
    }
    
    // Check for lower border
    if (yCounter == lowerComparisonValue()) {
        verticalFrameFFsetCond = true;
        verticalFrameFF = true;
    }
    // Trigger immediately (VICE does this in cycle 1)
    if (verticalFrameFFsetCond) {
        verticalFrameFF = true;
    }

}

void
VIC::checkFrameFlipflopsLeft(uint16_t comparisonValue)
{
    if (comparisonValue == leftComparisonValue()) {
        
        // "4. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
        //     den unteren, wird das vertikale Rahmenflipflop gesetzt." [C.B.]
        
/* OLD CODE
        if (yCounter == lowerComparisonValue()) {
            verticalFrameFF = true;
        }
*/
        /* VICE HANDLES THIS IN CYCLE 1 */
/*
        if (verticalFrameFFsetCond) {
            verticalFrameFF = true;
        }
*/
        // "5. Erreicht die X-Koordinate den linken Vergleichswert und die Y-Koordinate
        //     den oberen und ist das DEN-Bit in Register $d011 gesetzt, wird das
        //     vertikale Rahmenflipflop gelöscht." [C.B.]
        
/* OLD CODE
        else if (yCounter == upperComparisonValue() && DENbit()) {
            verticalFrameFF = false;
        }
*/
        // Now handled in 'checkVerticalFrameFF'
        
        // "6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
        //     vertikale Rahmenflipflop gelöscht, wird das Haupt-Flipflop gelöscht." [C.B.]
        // verticalFrameFF = false;
        clearMainFrameFF();
    }

}

void
VIC::checkFrameFlipflopsRight(uint16_t comparisonValue)
{
    // "1. Erreicht die X-Koordinate den rechten Vergleichswert, wird das
    //     Haupt-Rahmenflipflop gesetzt." [C.B.]
    
    if (comparisonValue == rightComparisonValue()) {
        mainFrameFF = true;
    }

}

// -----------------------------------------------------------------------------------------------
//                                    Execution functions
//
// All cycles are processed in this order:
//
//   Phi1.1 Frame logic
//   Phi1.2 Draw
//   Phi1.3 Fetch
//   Phi2.1 Rasterline interrupt
//   Phi2.2 Sprite logic
//   Phi2.3 VC/RC logic
//   Phi2.4 BA logic
//   Phi2.5 Fetch
// -----------------------------------------------------------------------------------------------


void 
VIC::beginFrame()
{
    frame++;
    
	lightpenIRQhasOccured = false;

    /* "Der [Refresh-]Zähler wird in Rasterzeile 0 mit
        $ff gelöscht und nach jedem Refresh-Zugriff um 1 verringert.
        Der VIC greift also in Zeile 0 auf die Adressen $3fff, $3ffe, $3ffd, $3ffc
        und $3ffb zu, in Zeile 1 auf $3ffa, $3ff9, $3ff8, $3ff7 und $3ff6 usw." [C.B.] */
   
    refreshCounter = 0xFF;

    /* "1. Irgendwo einmal auﬂerhalb des Bereiches der Rasterzeilen $30-$f7 (also
           außerhalb des Bad-Line-Bereiches) wird VCBASE auf Null gesetzt.
           Vermutlich geschieht dies in Rasterzeile 0, der genaue Zeitpunkt ist
           nicht zu bestimmen, er spielt aber auch keine Rolle." [C.B.] */
 
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
    if (line != 0) {
        yCounter = line; // Overflow case is handled in cycle 2
    }
    
    verticalFrameFFsetCond = verticalFrameFFclearCond = false;
    
	// Clear z buffer. The buffer is initialized with a high, positive value (meaning the pixel is far away)
	memset(zBuffer, 0x7f, sizeof(zBuffer)); // Why don't we use 0xFF???

	// Clear pixel source
	memset(pixelSource, 0x00, sizeof(pixelSource));

    // Clear pixel buffer
    memset(pixelBuffer, 0x00, sizeof(pixelSource));

    // Check, if we are currently processing a DMA line. The result is stored in variable badLineCondition.
    // The initial value can change in the middle of a rasterline.
    updateBadLineCondition();

    // Check for the DEN bit if we're processing rasterline 30
    // The initial value can change in the middle of a rasterline.
    if (line == 0x30)
        DENwasSetInRasterline30 = DENbit();

    // Reset graphic sequencer
    // gs_data = 0;
    gs_shift_reg = 0;
    // pixelBufferTmp[0] = pixelBuffer[1] = 0;
    zBufferTmp[0] = zBufferTmp[1] = 0;
    pixelSourceTmp[0] = pixelSourceTmp[1] = 0;
}

void 
VIC::endRasterline()
{
    // Set vertical flipflop is condition was hit
    if (verticalFrameFFsetCond) {
        verticalFrameFF = true;
    }
    
	// Copy pixel buffer of old line to screen buffer
	pixelBuffer += totalScreenWidth;
}

void 
VIC::cycle1()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(3);
    else
        sSecondAccess(3);
    
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (c64->getRasterline() != 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIRQ(1);
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR3 | SPR4));
    else
        setBAlow(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    
    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(3);
    else
        sThirdAccess(3);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle2()
{
    yCounter = c64->getRasterline();

    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(3);
    else
        pAccess(4);
    
    // Phi2.2 Sprite logic
    // Phi2.1 Rasterline interrupt (edge triggered)
    bool edgeOnYCounter = (yCounter == 0);
    bool edgeOnIrqCond  = (yCounter == rasterInterruptLine() && !yCounterEqualsIrqRasterline);
    if (edgeOnYCounter && edgeOnIrqCond)
        triggerIRQ(1);

    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    else
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5));

    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(3);
    else
        sFirstAccess(4);
    
    // Finalize
    updateDisplayState();
    countX();
}

void 
VIC::cycle3()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(4);
    else
        sSecondAccess(4);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5));
    else
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    
    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(4);
    else
        sThirdAccess(4);
    
    // Finalize
    updateDisplayState();
	countX();
}

void 
VIC::cycle4()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(4);
    else
        pAccess(5);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    } else {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6));
    }

    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(4);
    else
        sFirstAccess(5);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle5()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(5);
    else
        sSecondAccess(5);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6));
    } else {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    }
        
    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(5);
    else
        sThirdAccess(5);
    
    // Finalize
    updateDisplayState();
    countX();
}

void 
VIC::cycle6()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(5);
    else
        pAccess(6);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    } else {
        setBAlow(spriteDmaOnOff & (SPR6 | SPR7));

    }
    
    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(5);
    else
        sFirstAccess(6);
    
    // Finalize
    updateDisplayState();
    countX();
}

void 
VIC::cycle7()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(6);
    else
        sSecondAccess(6);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR6 | SPR7));

    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(6);
    else
        sThirdAccess(6);
    
    // Finalize
    updateDisplayState();
	countX();
}

void 
VIC::cycle8()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(6);
    else
        pAccess(7);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR6 | SPR7));
    else
        setBAlow(spriteDmaOnOff & SPR7);
    
    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(6);
    else
        sFirstAccess(7);
    
    // Finalize
    updateDisplayState();
    countX();
}

void 
VIC::cycle9()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(7);
    else
        sSecondAccess(7);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR7);

    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(7);
    else
        sThirdAccess(7);
    
    // Finalize
    updateDisplayState();
	countX();
}

void 
VIC::cycle10()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(7);
    else
        rIdleAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & SPR7);
    } else {
        setBAlow(false);
    }
    
    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(7);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle11()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch (first out of five DRAM refreshs)
    rAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(false);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle12()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch (second out of five DRAM refreshs)
    rAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic

    /* "3. Liegt in den Zyklen 12-54 ein Bad-Line-Zustand vor, wird BA auf Low
        gelegt und die c-Zugriffe gestartet. Einmal gestartet, findet in der
        zweiten Phase jedes Taktzyklus im Bereich 15-54 ein c-Zugriff statt. Die
        gelesenen Daten werden in der Videomatrix-/Farbzeile an der durch VMLI
        angegebenen Position abgelegt. Bei jedem g-Zugriff im Display-Zustand
        werden diese Daten ebenfalls an der durch VMLI spezifizierten Position
        wieder intern gelesen." [C.B.] */
    
    setBAlow(badLineCondition);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle13() // X Coordinate -3 - 4 (?)
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw (border starts here)
    prepareDrawingContextForCycle(13);

    // Phi1.3 Fetch (third out of five DRAM refreshs)
    rAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle14() // SpriteX: 0 - 7 (?)
{
    dirk();
    
    xCounter = 4;
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(14); // Prepare for next cycle

    // Phi1.3 Fetch (forth out of five DRAM refreshs)
    rAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    
	// "2. In der ersten Phase von Zyklus 14 jeder Zeile wird VC mit VCBASE geladen
    //     (VCBASE->VC) und VMLI gelöscht. Wenn zu diesem Zeitpunkt ein
    //     Bad-Line-Zustand vorliegt, wird zusätzlich RC auf Null gesetzt." [C.B.]

    registerVC = registerVCBASE;
	registerVMLI = 0;
	if (badLineCondition)
		registerRC = 0;

    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle15() // SpriteX: 8 - 15 (?)
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(15); // Prepare for next cycle
    
    // Phi1.3 Fetch (last DRAM refresh)
    rAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
	cAccess();

    // Finalize
    cleared_bits_in_d017 = 0;
    updateDisplayState();
    countX();
}

void
VIC::cycle16() // SpriteX: 16 - 23 (?)
{
    dirk();

    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(16); // Prepare for next cycle
    
    // Phi1.3 Fetch
    gAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic

    // "7. In the first phase of cycle 16, it is checked if the expansion flip flop
    //     is set. If so, MCBASE load from MC (MC->MCBASE), unless the CPU cleared
    //     the Y expansion bit in $d017 in the second phase of cycle 15, in which case
    //     MCBASE is set to X = (101010 & (MCBASE & MC)) | (010101 & (MCBASE | MC)).
    //     After the MCBASE update, the VIC checks if MCBASE is equal to 63 and turns
    //     off the DMA of the sprite if it is." [VIC Addendum]
    
    for (int i = 0; i < 8; i++) {
        uint8_t mask = (1 << i);
        if (expansionFF & mask) {
            if (cleared_bits_in_d017 & mask) {
                mcbase[i] = (0x2A /* 101010 */ & (mcbase[i] & mc[i])) | (0x15 /* 010101 */ & (mcbase[i] | mc[i]));
            } else {
                mcbase[i] = mc[i];
            }
        }
        
        if (mcbase[i] == 63) {
            spriteDmaOnOff &= ~mask;
        }
    }

    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle17() // SpriteX: 24 - 31 (?)
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(24);
    
    // Phi1.2 Draw (main screen area starts here)
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(17); // Prepare for next cycle
    
    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
    cAccess();

    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle18() // SpriteX: 32 - 39
{
    dirk();

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(31);
    
    // Phi1.2 Draw
    draw(); // Draw previous cycle
    prepareDrawingContextForCycle(18); // Prepare for next cycle

    // Phi1.3 Fetch
    gAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
    cAccess();

    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle19to54()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    draw(); // Draw previous cycle
    prepareDrawingContextForCycle(19); // Prepare for next cycle
    
    // Phi1.3 Fetch
    gAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(badLineCondition);

    // Phi2.5 Fetch
    cAccess();
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle55()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    draw(); // Draw previous cycle
    prepareDrawingContextForCycle(55); // Prepare for next cycle
    
    // Phi1.3 Fetch
    gAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic

    // "In der ersten Phase von Zyklus 55 wird das Expansions-Flipflop
    //  invertiert, wenn das MxYE-Bit gesetzt ist." [C.B.]
    expansionFF ^= iomem[0x17];

	// "In den ersten Phasen von Zyklus 55 und 56 wird für jedes Sprite geprüft,
    //  ob das entsprechende MxE-Bit in Register $d015 gesetzt und die
    //  Y-Koordinate des Sprites (ungerade Register $d001-$d00f) gleich den
    //  unteren 8 Bits von RASTER ist. Ist dies der Fall und der DMA für das
    //  Sprite noch ausgeschaltet, wird der DMA angeschaltet, MCBASE gelöscht
    //  und, wenn das MxYE-Bit gesetzt ist, das Expansions-Flipflop gelöscht." [C.B.]
	 
	updateSpriteDmaOnOff();

    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & SPR0);
    } else {
        setBAlow(false);
    }
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle56()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(335);

    // Phi1.2 Draw
    draw(); // Draw previous cycle
    prepareDrawingContextForCycle(56); // Prepare for next cycle
    
    // Phi1.3 Fetch
    rIdleAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
	updateSpriteDmaOnOff();
    
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR0);
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle57()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(344);
    
    // Phi1.2 Draw (border starts here)
    draw(); // Draw previous cycle
    prepareDrawingContextForCycle(57); // Prepare for next cycle
    
    // Phi1.3 Fetch
    rIdleAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    } else {
        setBAlow(spriteDmaOnOff & SPR0);
    }
    
    // Phi2.5 Fetch
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle58()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(58); // Prepare for next cycle
    
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(0);
    else
        rIdleAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
			
	// "4. In der ersten Phase von Zyklus 58 wird für jedes Sprite MC mit MCBASE
    //     geladen (MCBASE->MC) und geprüft, ob der DMA für das Sprite angeschaltet
    //     und die Y-Koordinate des Sprites gleich den unteren 8 Bits von RASTER
    //     ist. Ist dies der Fall, wird die Darstellung des Sprites angeschaltet." [C.B.]

    oldSpriteOnOff = spriteOnOff; // remember last value
	for (int i = 0; i < 8; i++) {
		mc[i] = mcbase[i];
		uint8_t mask = (1 << i);
		if (spriteDmaOnOff & mask) {
			uint8_t y = getSpriteY(i);
			if (y == (yCounter & 0xff))
				spriteOnOff |= mask;
		}
	}
		
	// Draw rasterline into pixel buffer
	drawAllSprites();
			
	// switch off sprites if dma is off
	for (int i = 0; i < 8; i++) {
		uint8_t mask = (1 << i);
		if ((spriteOnOff & mask) && !(spriteDmaOnOff & mask))
			spriteOnOff &= ~mask;
	}

    // Phi2.3 VC/RC logic
    
    // "5. In der ersten Phase von Zyklus 58 wird geprüft, ob RC=7 ist. Wenn ja,
    //     geht die Videologik in den Idle-Zustand und VCBASE wird mit VC geladen
    //     (VC->VCBASE)." [C.B.]

    // "Der Übergang vom Display- in den Idle-Zustand erfolgt in Zyklus 58 einer Zeile,
    //  wenn der RC den Wert 7 hat und kein Bad-Line-Zustand vorliegt."
    
    
    if (registerRC == 7) {
        registerVCBASE = registerVC;
        if (!badLineCondition)
            displayState = false;
    }

    updateDisplayState();
    
    if (displayState) {
        // 3 bit overflow register
        registerRC = (registerRC + 1) & 0x07;
    }
    
    /* OLD
    //if (displayState && registerRC == 7 && !badLineCondition) {
    //    displayState = false;
    //    registerVCBASE = registerVC;
    // }
    
    // "Ist die Videologik danach im Display-Zustand (liegt ein
    //  Bad-Line-Zustand vor, ist dies immer der Fall), wird RC erhöht." [C.B.]
    
    if (displayState) {
        // 3 bit overflow register
        registerRC = (registerRC + 1) & 0x07;
    } else {
        // "(liegt ein Bad-Line-Zustand vor, ist dies immer der Fall)"
        assert(!badLineCondition);
    }
    */
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(0);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle59()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(59); // Prepare for next cycle
    
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(0);
    else
        pAccess(0);
 
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    else
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(0);
    else
        sFirstAccess(0);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle60()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw (last visible cycle)
    drawBorderDeprecated(); // Draw previous cycle
    prepareDrawingContextForCycle(60); // Prepare for next cycle
    
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(1);
    else
        sSecondAccess(0);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2));
    else
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    
    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(1);
    else
        sThirdAccess(0);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle61()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    drawBorderDeprecated(); // Draw previous cycle
    
    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(1);
    else
        pAccess(1);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    else
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2));
    
    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(1);
    else
        sFirstAccess(1);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle62()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL)
        pAccess(2);
    else
        sSecondAccess(1);

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL)
        setBAlow(spriteDmaOnOff & (SPR2 | SPR3));
    else
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    
    // Phi2.5 Fetch
    if (isPAL)
        sFirstAccess(2);
    else
        sThirdAccess(1);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle63()
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
    
    // "2. Erreicht die Y-Koordinate den unteren Vergleichswert in Zyklus 63, wird
    //     das vertikale Rahmenflipflop gesetzt." [C.B.]
    /*
    if (yCounter == lowerComparisonValue()) {
        verticalFrameFF = true;
    }
    */
    // "3. Erreicht die Y-Koordinate den oberern Vergleichswert in Zyklus 63 und
    //     ist das DEN-Bit in Register $d011 gesetzt, wird das vertikale
    //     Rahmenflipflop gelöscht." [C.B.]
    /*
    else if (yCounter == upperComparisonValue() && DENbit()) {
        verticalFrameFF = false;
    }
    */
    
    // Phi1.2 Draw

    // Extend pixel buffer to the left and right to make it look nice
    int color = pixelBuffer[22];
    for (unsigned i = 0; i <= 22; i++) {
        pixelBuffer[i] = color;
    }
    
    color = pixelBuffer[389];
    for (unsigned i = 390; i < totalScreenWidth; i++) {
        pixelBuffer[i] = color;
    }

	// draw debug markers
	if (markIRQLines && yCounter == rasterInterruptLine())
		markLine(0, totalScreenWidth, colors[WHITE]);
	if (markDMALines && badLineCondition)	
		markLine(0, totalScreenWidth, colors[RED]);
	if (rasterlineDebug[yCounter] >= 0) {
		markLine(0, totalScreenWidth, colors[rasterlineDebug[yCounter] % 16]);
		rasterlineDebug[yCounter] = -1;
	}		

    // Phi1.3 Fetch
    if (isPAL)
        sSecondAccess(2);
    else
        pAccess(2);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL) {
        setBAlow(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));
    } else {
        setBAlow(spriteDmaOnOff & (SPR2 | SPR3));        
    }
    
    // Phi2.5 Fetch
    if (isPAL)
        sThirdAccess(2);
    else
        sFirstAccess(2);
    
    // Finalize
    updateDisplayState();
    countX();
}

void
VIC::cycle64() 	// NTSC only
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    sSecondAccess(2);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));

     // Phi2.5 Fetch
    sThirdAccess(2);
    
    // Finalize
	updateDisplayState();
    countX();
}

void
VIC::cycle65() 	// NTSC only
{
    dirk();
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());

    // Phi1.2 Draw
    // Phi1.3 Fetch
    pAccess(3);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR3 | SPR4));

    // Phi2.5 Fetch
    sFirstAccess(3);

    // Finalize
    updateDisplayState();
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

