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


VIC::VIC()
{
	name = "VIC";
	debug(2, "  Creating VIC at address %p...\n", this);
    
	// Start with all debug options disabled
	markIRQLines = false;
	markDMALines = false;
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &pixelEngine, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {

        // Configuration items
        { &chipModel,                   sizeof(chipModel),                      KEEP_ON_RESET },
        
        // Internal state
        { &vblank,                      sizeof(vblank),                         CLEAR_ON_RESET },
        { &xCounter,                    sizeof(xCounter),                       CLEAR_ON_RESET },
        { &yCounter,                    sizeof(yCounter),                       CLEAR_ON_RESET },
        { &yCounterEqualsIrqRasterline, sizeof(yCounterEqualsIrqRasterline),    CLEAR_ON_RESET },
        { &registerVC,                  sizeof(registerVC),                     CLEAR_ON_RESET },
        { &registerVCBASE,              sizeof(registerVCBASE),                 CLEAR_ON_RESET },
        { &registerRC,                  sizeof(registerRC),                     CLEAR_ON_RESET },
        { &registerVMLI,                sizeof(registerVMLI),                   CLEAR_ON_RESET },
        { &oldControlReg1,              sizeof(oldControlReg1),                 CLEAR_ON_RESET },
        { &refreshCounter,              sizeof(refreshCounter),                 CLEAR_ON_RESET },
        { &addrBus,                     sizeof(addrBus),                        CLEAR_ON_RESET },
        { &dataBus,                     sizeof(dataBus),                        CLEAR_ON_RESET },
        { &gAccessDisplayMode,          sizeof(gAccessDisplayMode),             CLEAR_ON_RESET },
        { &gAccessfgColor,              sizeof(gAccessfgColor),                 CLEAR_ON_RESET },
        { &gAccessbgColor,              sizeof(gAccessbgColor),                 CLEAR_ON_RESET },
        { &badLineCondition,            sizeof(badLineCondition),               CLEAR_ON_RESET },
        { &DENwasSetInRasterline30,     sizeof(DENwasSetInRasterline30),        CLEAR_ON_RESET },
        { &displayState,                sizeof(displayState),                   CLEAR_ON_RESET },
        { &BAlow,                       sizeof(BAlow),                          CLEAR_ON_RESET },
        { &BAwentLowAtCycle,            sizeof(BAwentLowAtCycle),               CLEAR_ON_RESET },
        { &mainFrameFF,                 sizeof(mainFrameFF),                    CLEAR_ON_RESET },
        { &verticalFrameFF,             sizeof(verticalFrameFF),                CLEAR_ON_RESET },
        
        { &iomem,                       sizeof(iomem),                          CLEAR_ON_RESET },
        { &bankAddr,                    sizeof(bankAddr),                       CLEAR_ON_RESET },
        
        { &g_data,                      sizeof(g_data),                         CLEAR_ON_RESET },
        { &g_character,                 sizeof(g_character),                    CLEAR_ON_RESET },
        { &g_color,                     sizeof(g_color),                        CLEAR_ON_RESET },
        { &g_mode,                      sizeof(g_mode),                         CLEAR_ON_RESET },
        
        { &mc,                          sizeof(mc),                             CLEAR_ON_RESET | BYTE_FORMAT },
        { &mcbase,                      sizeof(mcbase),                         CLEAR_ON_RESET | BYTE_FORMAT },
        { &spriteOnOff,                 sizeof(spriteOnOff),                    CLEAR_ON_RESET },
        { &spriteDmaOnOff,              sizeof(spriteDmaOnOff),                 CLEAR_ON_RESET },
        { &expansionFF,                 sizeof(expansionFF),                    CLEAR_ON_RESET },
        { &cleared_bits_in_d017,        sizeof(cleared_bits_in_d017),           CLEAR_ON_RESET },
        { &lightpenIRQhasOccured,       sizeof(lightpenIRQhasOccured),          CLEAR_ON_RESET },
        { NULL,                         0,                                      0 }};

    registerSnapshotItems(items, sizeof(items));
}

VIC::~VIC()
{
}

void 
VIC::reset()
{
    VirtualComponent::reset();
	
    // Establish bindungs
    cpu = c64->cpu;
    mem = c64->mem;
    
    // Reset subcomponents
    pixelEngine.reset();

    // Internal state
    yCounter = PAL_HEIGHT;
    iomem[0x20] = PixelEngine::LTBLUE; // Let the border color look correct right from the beginning
    iomem[0x21] = PixelEngine::BLUE;   // Let the background color look correct right from the beginning
	iomem[0x11] = 0x10;                // Make screen visible from the beginning
	expansionFF = 0xFF;
    
    // Remove startup graphics glitches by setting the initial value early
    setScreenMemoryAddr(0x400);
    
	// Debugging	
	drawSprites = true;
	spriteSpriteCollisionEnabled = 0xFF;
	spriteBackgroundCollisionEnabled = 0xFF;
}

void
VIC::ping()
{
    c64->putMessage(isPAL() ? MSG_PAL : MSG_NTSC);
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

void
VIC::setChipModel(ChipModel model)
{
    chipModel = model;
    pixelEngine.resetScreenBuffers();
    c64->putMessage(isPAL() ? MSG_PAL : MSG_NTSC);
}


// -----------------------------------------------------------------------------------------------
//                             I/O memory handling and RAM access
// -----------------------------------------------------------------------------------------------

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
    // return memAccess(0x3FFF);
    addrBus = bankAddr + 0x3FFF;
    return mem->ram[addrBus];
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
        g_data = memAccess(addr);
        g_character = characterSpace[registerVMLI];
        g_color = colorSpace[registerVMLI];
        g_mode = getDisplayMode();
        
        // "Nach jedem g-Zugriff im Display-Zustand werden VC und VMLI erhöht." [C.B.]
        registerVC++;
        registerVC &= 0x3FF; // 10 bit overflow
        registerVMLI++;
        registerVMLI &= 0x3F; // 6 bit overflow
        
    } else {
    
        // "Im Idle-Zustand erfolgen die g-Zugriffe immer an Videoadresse $3fff." [C.B.]
        addr = ECMbitInPreviousCycle() ? 0x39FF : 0x3FFF;
        
        // Prepare graphic sequencer
        g_data = memAccess(addr);
        g_character = 0;
        g_color = 0;
        g_mode = getDisplayMode();
    }
}

inline void VIC::pAccess(int sprite)
{
    // |VM13|VM12|VM11|VM10|  1 |  1 |  1 |  1 |  1 |  1 |  1 |  Spr.-Nummer |
    spritePtr[sprite] = memAccess((VM13VM12VM11VM10() << 6) | 0x03F8 | sprite) << 6;

}

// TODO: Change return type to void
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
    
    // load data into shift register
    // pixelEngine.sprite_sr[sprite].data.chunk[0] = data;
    pixelEngine.sprite_sr[sprite].data = data;
    return memAccessed;
}

// TODO: Change return type to void
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
    
    // load data into shift register
    // pixelEngine.sprite_sr[sprite].data.chunk[1] = data;
    pixelEngine.sprite_sr[sprite].data = (pixelEngine.sprite_sr[sprite].data << 8) | data;
    return memAccessed;
}

// TODO: Change return type to void
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
    
    // load data into shift register
    // pixelEngine.sprite_sr[sprite].data.chunk[2] = data;
    pixelEngine.sprite_sr[sprite].data = (pixelEngine.sprite_sr[sprite].data << 8) | data;
    return memAccessed;
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
VIC::turnSpriteDmaOff()
{
    // "7. In the first phase of cycle 16, [1] it is checked if the expansion flip flop
    //     is set. If so, [2] MCBASE load from MC (MC->MCBASE), [3] unless the CPU cleared
    //     the Y expansion bit in $d017 in the second phase of cycle 15, in which case
    //     [4] MCBASE is set to X = (101010 & (MCBASE & MC)) | (010101 & (MCBASE | MC)).
    //     After the MCBASE update, [5] the VIC checks if MCBASE is equal to 63 and [6] turns
    //     off the DMA of the sprite if it is." [VIC Addendum]
    
    for (unsigned i = 0; i < 8; i++) {
        if (GET_BIT(expansionFF,i)) { /* [1] */
            if (GET_BIT(cleared_bits_in_d017,i)) { /* [3] */
                uint8_t b101010 = 0x2A;
                uint8_t b010101 = 0x15;
                mcbase[i] = (b101010 & (mcbase[i] & mc[i])) | (b010101 & (mcbase[i] | mc[i])); /* [4] */
            } else {
                mcbase[i] = mc[i]; /* [2] */
            }
            
            if (mcbase[i] == 63) { /* [5] */
                CLR_BIT(spriteDmaOnOff,i); /* [6] */
            }
        }
    }

}

void
VIC::turnSpriteDmaOn()
{
    // "3. In den ersten Phasen von Zyklus 55 und 56 wird für jedes Sprite geprüft,
    //     ob [1] das entsprechende MxE-Bit in Register $d015 gesetzt und [2] die
    //     Y-Koordinate des Sprites (ungerade Register $d001-$d00f) gleich den
    //     unteren 8 Bits von RASTER ist. Ist dies der Fall und [3] der DMA für das
    //     Sprite noch ausgeschaltet, wird [4] der DMA angeschaltet, [5] MCBASE gelöscht[.]" [C.B.]
#if 0
    for (unsigned i = 0; i < 8; i++) {
        if (spriteIsEnabled(i)) { /* [1] */
            if (getSpriteY(i) == (yCounter & 0xff)) { /* [2] */
                if (!GET_BIT(spriteDmaOnOff,i)) { /* [3] */
                    SET_BIT(spriteDmaOnOff,i); /* [4] */
                    mcbase[i] = 0; /* [5] */
                    SET_BIT(expansionFF,i); // will be flipped for stretched sprites in cycle 56
                }
            }
        }
    }
#endif
    
    uint8_t risingEdges = ~spriteDmaOnOff & (iomem[0x15] & compareSpriteY(yCounter));
    for (unsigned i = 0; i < 8; i++)
        if (GET_BIT(risingEdges,i))
            mcbase[i] = 0;
    
    expansionFF |= risingEdges;
    spriteDmaOnOff |= risingEdges;
}

void
VIC::toggleExpansionFlipflop()
{
    // A '1' in D017 means that the sprite is vertically stretched
    expansionFF ^= iomem[0x17];
}

#if 0
void
VIC::turnSpriteDisplayOn()
{
    // "4. In der ersten Phase von Zyklus 58 wird [1] für jedes Sprite [2] MC mit MCBASE
    //     geladen (MCBASE->MC) und geprüft, [3] ob der DMA für das Sprite angeschaltet
    //     [und [3.1] das entsprechende MxE-Bit in Register $d015 immer noch gesetzt ist]
    //     und [4] die Y-Koordinate des Sprites gleich den unteren 8 Bits von RASTER
    //     ist. Ist dies der Fall, wird [5] die Darstellung des Sprites angeschaltet." [C.B.]
    // In [3], we need to check additionally, if sprite is still enabled.
    
    for (unsigned i = 0; i < 8; i++) { /* [1] */
        mc[i] = mcbase[i]; /* [2] */
        if (GET_BIT(spriteDmaOnOff, i) /* [3] */ && spriteIsEnabled(i) /* [3.1] */) { 
            if (getSpriteY(i) == (yCounter & 0xFF)) /* [4] */
                SET_BIT(spriteOnOff,i); /* [5] */
        }
    }
}

void
VIC::turnSpriteDisplayOff()
{
    // switch off sprite if dma is off
    for (int i = 0; i < 8; i++) {
        if (GET_BIT(spriteOnOff, i) && !GET_BIT(spriteDmaOnOff, i))
            CLR_BIT(spriteOnOff, i);
    }
}
#endif

// -----------------------------------------------------------------------------------------------
//                                      Frame flipflops
// -----------------------------------------------------------------------------------------------

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

        // "6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
        //     vertikale Rahmenflipflop gelöscht, wird das Haupt-Flipflop gelöscht." [C.B.]
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
    pixelEngine.beginFrame();
    
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
    pixelEngine.endFrame();
}


void 
VIC::beginRasterline(uint16_t line)
{
    verticalFrameFFsetCond = verticalFrameFFclearCond = false;

    // Determine if we're currently processing a VBLANK line (nothing is drawn in this area)
    if (isPAL()) {
        vblank = line < PAL_UPPER_VBLANK || line >= PAL_UPPER_VBLANK + PAL_RASTERLINES;
    } else {
        vblank = line < NTSC_UPPER_VBLANK || line >= NTSC_UPPER_VBLANK + NTSC_RASTERLINES;
    }

    /* OLD CODE
    if (line != 0) {
        //assert(yCounter == c64->getRasterline());
        yCounter = line; // Overflow case is handled in cycle 2
    }
    */
    // Increase yCounter. The overflow case is handled in cycle 2
    if (!yCounterOverflow())
        yCounter++;
    
    // Check for the DEN bit if we're processing rasterline 30
    // The initial value can change in the middle of a rasterline.
    if (line == 0x30)
        DENwasSetInRasterline30 = DENbit();

    // Check, if we are currently processing a DMA line. The result is stored in variable badLineCondition.
    // The initial value can change in the middle of a rasterline.
    updateBadLineCondition();
    
    pixelEngine.beginRasterline();
}

void 
VIC::endRasterline()
{
    // Set vertical flipflop if condition was hit
    if (verticalFrameFFsetCond) {
        verticalFrameFF = true;
    }
    
    // Draw debug markers
    if (markIRQLines && yCounter == rasterInterruptLine())
        pixelEngine.markLine(PixelEngine::WHITE);
    if (markDMALines && badLineCondition)
        pixelEngine.markLine(PixelEngine::RED);

    /*
    if (yCounter == 52 && !vblank) {
        pixelEngine.markLine(4);
    }
    */
    
    pixelEngine.endRasterline();
}

bool
VIC::yCounterOverflow()
{
    // PAL machines reset yCounter in cycle 2 in the first physical rasterline
    // NTSC machines reset yCounter in cycle 2 in the middle of the lower border area
    return (c64->isPAL() && c64->getRasterline() == 0) || (!c64->isPAL() && c64->getRasterline() == 238);
}

inline void
VIC::preparePixelEngine()
{
    pixelEngine.dc.yCounter = yCounter;
    pixelEngine.dc.xCounter = xCounter;
    pixelEngine.dc.verticalFrameFF = verticalFrameFF;
    pixelEngine.dc.mainFrameFF = mainFrameFF;
    pixelEngine.dc.data = g_data;
    pixelEngine.dc.character = g_character;
    pixelEngine.dc.color = g_color;
    pixelEngine.dc.mode = g_mode;
    pixelEngine.dc.delay = getHorizontalRasterScroll();
    
    for (unsigned i = 0; i < 8; i++) {
        pixelEngine.dc.spriteX[i] = getSpriteX(i);
    }
    pixelEngine.dc.spriteXexpand = iomem[0x1D];
}

void
VIC::cycle1()
{
    debug_cycle(1);
        
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
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
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR3 | SPR4));
    else
        setBAlow(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(2);

    // Check for yCounter overflows
    if (yCounterOverflow())
            yCounter = 0;
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
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
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR3 | SPR4 | SPR5));
    else
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5));

    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(3);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(4);
    else
        sSecondAccess(4);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5));
    else
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(4);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(4);
    else
        pAccess(5);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
        setBAlow(spriteDmaOnOff & (SPR4 | SPR5 | SPR6));
    } else {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6));
    }

    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(5);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(5);
    else
        sSecondAccess(5);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6));
    } else {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    }
        
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(6);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(5);
    else
        pAccess(6);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
        setBAlow(spriteDmaOnOff & (SPR5 | SPR6 | SPR7));
    } else {
        setBAlow(spriteDmaOnOff & (SPR6 | SPR7));

    }
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(7);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(6);
    else
        sSecondAccess(6);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR6 | SPR7));

    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(8);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(6);
    else
        pAccess(7);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR6 | SPR7));
    else
        setBAlow(spriteDmaOnOff & SPR7);
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(9);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(7);
    else
        sSecondAccess(7);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & SPR7);

    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(10);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(7);
    else
        rIdleAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
        setBAlow(spriteDmaOnOff & SPR7);
    } else {
        setBAlow(false);
    }
    
    // Phi2.5 Fetch
    if (isPAL())
        sThirdAccess(7);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle11()
{
    debug_cycle(11);
    
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
    debug_cycle(12);
    
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
    debug_cycle(13);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    xCounter = -4;
    preparePixelEngine(); // Prepare for next cycle (first border column)
    // Update pixelEngines color registers to get the first pixel right
    pixelEngine.updateColorRegisters();
    pixelEngine.updateBorderColorRegister();

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
    debug_cycle(14);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (first border column)
    preparePixelEngine(); // Prepare for next cycle (border column 2)

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
    debug_cycle(15);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (border column 2)
    preparePixelEngine(); // Prepare for next cycle (border column 3)
    
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
    debug_cycle(16);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (border column 3)
    preparePixelEngine(); // Prepare for next cycle (border column 4)
    
    // Phi1.3 Fetch
    gAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    turnSpriteDmaOff();
    
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
    debug_cycle(17);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(24);
    
    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (border column 4)
    preparePixelEngine(); // Prepare for next cycle (first canvas column)
    
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
    debug_cycle(18);

    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsLeft(31);
    
    // Phi1.2 Draw
    pixelEngine.draw17(); // Draw previous cycle (first canvas column)
    preparePixelEngine(); // Prepare for next cycle (canvas column 2)

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
    debug_cycle(19);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle
    preparePixelEngine(); // Prepare for next cycle
    
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
    debug_cycle(55);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (canvas column)
    preparePixelEngine(); // Prepare for next cycle (canvas column)
    
    // Phi1.3 Fetch
    gAccess();

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
	turnSpriteDmaOn();

    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
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
    debug_cycle(56);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(335);

    // Phi1.2 Draw
    pixelEngine.draw55(); // Draw previous cycle (canvas column)
    preparePixelEngine(); // Prepare for next cycle (last canvas column)
    
    // Phi1.3 Fetch
    rIdleAccess();
    g_data = 0; // no more gAccesses from now on (TODO: BETTER MOVE TO rIdleAccess(?))

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    turnSpriteDmaOn();
    toggleExpansionFlipflop();
    
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
    debug_cycle(57);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    checkFrameFlipflopsRight(344);
    
    // Phi1.2 Draw (border starts here)
    pixelEngine.draw(); // Draw previous cycle (last canvas column)
    preparePixelEngine(); // Prepare for next cycle (first column of right border)
    
    // Phi1.3 Fetch
    rIdleAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
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
    debug_cycle(58);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (first column of right border)
    preparePixelEngine(); // Prepare for next cycle (column 2 of right border)
    
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(0);
    else
        rIdleAccess();
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    
    // Reset mc with mcbase for all sprites
    for (unsigned i = 0; i < 8; i++)
        mc[i] = mcbase[i];
    
    // Turn display on for all sprites with a matching y coordinate
    // Sprite display remains off if sprite DMA is off or sprite is disabled (register 0x15)
    spriteOnOff |= spriteDmaOnOff & iomem[0x15] & compareSpriteY((uint8_t)yCounter);

    // Turn display off for all sprites that lost DMA.
    spriteOnOff &= spriteDmaOnOff;
    
    // turnSpriteDisplayOn();
    // turnSpriteDisplayOff();
    
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
    
    // Phi2.4 BA logic
    setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    if (isPAL())
        sFirstAccess(0);
    
    // Finalize
    updateDisplayState();
	countX();
}

void
VIC::cycle59()
{
    debug_cycle(59);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (column 2 of right border)
    preparePixelEngine(); // Prepare for next cycle (column 3 of right border)
    
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(0);
    else
        pAccess(0);
 
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    else
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1));
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(60);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw (last visible cycle)
    pixelEngine.draw(); // Draw previous cycle (column 3 of right border)
    preparePixelEngine(); // Prepare for next cycle (last column of right border)
    
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(1);
    else
        sSecondAccess(0);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2));
    else
        setBAlow(spriteDmaOnOff & (SPR0 | SPR1 | SPR2));
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(61);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    pixelEngine.draw(); // Draw previous cycle (last column of right border)
    
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(1);
    else
        pAccess(1);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    else
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2));
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(62);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();

    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        pAccess(2);
    else
        sSecondAccess(1);

    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL())
        setBAlow(spriteDmaOnOff & (SPR2 | SPR3));
    else
        setBAlow(spriteDmaOnOff & (SPR1 | SPR2 | SPR3));
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(63);
    
    // Phi1.1 Frame logic
    checkVerticalFrameFF();
    yCounterEqualsIrqRasterline = (yCounter == rasterInterruptLine());
        
    // Phi1.2 Draw
    // Phi1.3 Fetch
    if (isPAL())
        sSecondAccess(2);
    else
        pAccess(2);
    
    // Phi2.1 Rasterline interrupt
    // Phi2.2 Sprite logic
    // Phi2.3 VC/RC logic
    // Phi2.4 BA logic
    if (isPAL()) {
        setBAlow(spriteDmaOnOff & (SPR2 | SPR3 | SPR4));
    } else {
        setBAlow(spriteDmaOnOff & (SPR2 | SPR3));        
    }
    
    // Phi2.5 Fetch
    if (isPAL())
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
    debug_cycle(64);
    
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
    debug_cycle(65);
    
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

void
VIC::debug_cycle(unsigned c)
{
/*
    static cycle = 0;
    cycle = (c == 19) ? (cycle+1) : c;
*/

/*
     if (dirktrace == 1 && yCounter == DIRK_DEBUG_LINE) {
     printf("(%i,%i) (dx,yd):(%d,%d) D020:%d D021:%d BAlow:%d RDY:%d RC:%d VC:%d (VCbase:%d) VMLI:%d bad_line:%d disp_state:%d\n",
     yCounter, c64->rasterCycle,
     getHorizontalRasterScroll(), getVerticalRasterScroll(),
     iomem[0x20], iomem[0x21],
     BAlow, cpu->getRDY(),
     registerRC, registerVC, registerVCBASE, registerVMLI, badLineCondition, displayState);
     dirkcnt++;
     }
*/
}

