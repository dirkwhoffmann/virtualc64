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

#define SPR0 0x01
#define SPR1 0x02
#define SPR2 0x04
#define SPR3 0x08
#define SPR4 0x10
#define SPR5 0x20
#define SPR6 0x40
#define SPR7 0x80

VIC::VIC()
{
	setDescription("VIC");
	debug(3, "  Creating VIC at address %p...\n", this);
    
	markIRQLines = false;
	markDMALines = false;
    emulateGrayDotBug = true;
    
    // Register sub components
    VirtualComponent *subcomponents[] = { &pixelEngine, NULL };
    registerSubComponents(subcomponents, sizeof(subcomponents));

    // Register snapshot items
    SnapshotItem items[] = {

        // Configuration items
        { &chipModel,                   sizeof(chipModel),                      KEEP_ON_RESET },
        { &emulateGrayDotBug,           sizeof(emulateGrayDotBug),              KEEP_ON_RESET },

        // Internal state
        { &delay,                       sizeof(delay),                          CLEAR_ON_RESET },
        { &lp,                          sizeof(lp),                             CLEAR_ON_RESET },
        { &addrBus,                     sizeof(addrBus),                        CLEAR_ON_RESET },
        { &dataBus,                     sizeof(dataBus),                        CLEAR_ON_RESET },
        { &irr,                         sizeof(irr),                            CLEAR_ON_RESET },
        { &imr,                         sizeof(imr),                            CLEAR_ON_RESET },
        { &vblank,                      sizeof(vblank),                         CLEAR_ON_RESET },
        { &registerVC,                  sizeof(registerVC),                     CLEAR_ON_RESET },
        { &registerVCBASE,              sizeof(registerVCBASE),                 CLEAR_ON_RESET },
        { &registerRC,                  sizeof(registerRC),                     CLEAR_ON_RESET },
        { &registerVMLI,                sizeof(registerVMLI),                   CLEAR_ON_RESET },
        { &xCounter,                    sizeof(xCounter),                       CLEAR_ON_RESET },
        { &yCounter,                    sizeof(yCounter),                       CLEAR_ON_RESET },
        { &verticalFrameFFsetCond,      sizeof(verticalFrameFFsetCond),         CLEAR_ON_RESET },
        { &verticalFrameFFclearCond,    sizeof(verticalFrameFFclearCond),       CLEAR_ON_RESET },
        { &refreshCounter,              sizeof(refreshCounter),                 CLEAR_ON_RESET },
        { &gAccessDisplayMode,          sizeof(gAccessDisplayMode),             CLEAR_ON_RESET },
        { &gAccessfgColor,              sizeof(gAccessfgColor),                 CLEAR_ON_RESET },
        { &gAccessbgColor,              sizeof(gAccessbgColor),                 CLEAR_ON_RESET },
        { &badLineCondition,            sizeof(badLineCondition),               CLEAR_ON_RESET },
        { &DENwasSetInRasterline30,     sizeof(DENwasSetInRasterline30),        CLEAR_ON_RESET },
        { &displayState,                sizeof(displayState),                   CLEAR_ON_RESET },
        { &BAlow,                       sizeof(BAlow),                          CLEAR_ON_RESET },
        { &BAwentLowAtCycle,            sizeof(BAwentLowAtCycle),               CLEAR_ON_RESET },
        { &iomem,                       sizeof(iomem),                          CLEAR_ON_RESET },
        { &bankAddr,                    sizeof(bankAddr),                       CLEAR_ON_RESET },
        { &characterSpace,              sizeof(characterSpace),                 CLEAR_ON_RESET },
        { &colorSpace,                  sizeof(colorSpace),                     CLEAR_ON_RESET },
        { &isFirstDMAcycle,             sizeof(isFirstDMAcycle),                CLEAR_ON_RESET },
        { &isSecondDMAcycle,            sizeof(isSecondDMAcycle),               CLEAR_ON_RESET },
        { &mc,                          sizeof(mc),                             CLEAR_ON_RESET | BYTE_FORMAT },
        { &mcbase,                      sizeof(mcbase),                         CLEAR_ON_RESET | BYTE_FORMAT },
        { spritePtr,                    sizeof(spritePtr),                      CLEAR_ON_RESET | WORD_FORMAT },
        { &spriteOnOff,                 sizeof(spriteOnOff),                    CLEAR_ON_RESET },
        { &spriteDmaOnOff,              sizeof(spriteDmaOnOff),                 CLEAR_ON_RESET },
        { &expansionFF,                 sizeof(expansionFF),                    CLEAR_ON_RESET },
        { &cleared_bits_in_d017,        sizeof(cleared_bits_in_d017),           CLEAR_ON_RESET },
        { &lightpenIRQhasOccured,       sizeof(lightpenIRQhasOccured),          CLEAR_ON_RESET },
        { &yCounterEqualsIrqRasterline, sizeof(yCounterEqualsIrqRasterline),    CLEAR_ON_RESET },

        // Pixel engine pipe
        { p.spriteX,                    sizeof(p.spriteX),                      CLEAR_ON_RESET | WORD_FORMAT },
        { &p.spriteXexpand,             sizeof(p.spriteXexpand),                CLEAR_ON_RESET },
        { &p.previousCTRL1,             sizeof(p.previousCTRL1),                CLEAR_ON_RESET },
        { &p.g_data,                    sizeof(p.g_data),                       CLEAR_ON_RESET },
        { &p.g_character,               sizeof(p.g_character),                  CLEAR_ON_RESET },
        { &p.g_color,                   sizeof(p.g_color),                      CLEAR_ON_RESET },
        { &p.mainFrameFF,               sizeof(p.mainFrameFF),                  CLEAR_ON_RESET },
        { &p.verticalFrameFF,           sizeof(p.verticalFrameFF),              CLEAR_ON_RESET },
        
        { NULL,                         0,                                      0 }};

    registerSnapshotItems(items, sizeof(items));
}

VIC::~VIC()
{
}

void
VIC::setC64(C64 *c64)
{
    VirtualComponent::setC64(c64);

    // Assign reference clock to all time delayed variables
    control1.setClock(&c64->cpu.cycle);
    control2.setClock(&c64->cpu.cycle);
    borderColor.setClock(&c64->cpu.cycle);
    for (unsigned i = 0; i < 4; i++)
        bgColor[i].setClock(&c64->cpu.cycle);
    sprExtraColor1.setClock(&c64->cpu.cycle);
    sprExtraColor2.setClock(&c64->cpu.cycle);
    for (unsigned i = 0; i < 8; i++)
        sprColor[i].setClock(&c64->cpu.cycle);
}

void 
VIC::reset()
{
    VirtualComponent::reset();
    
    // Internal state
    yCounter = PAL_HEIGHT;
    
    // Preset some video parameters to show a blank sreen on power up
    setScreenMemoryAddr(0x400);
    memset(&c64->mem.ram[0x400], 32, 40*25);
	expansionFF = 0xFF;
    
    // Reset timed delay variables
    control1.reset(0x10);
    control2.reset(0);
    borderColor.reset(pattern[14]); // Light blue
    bgColor[0].reset(pattern[6]);   // Blue
    bgColor[1].reset(0);
    bgColor[2].reset(0);
    bgColor[3].reset(0);
    sprExtraColor1.reset(0);
    sprExtraColor2.reset(0);
    for (unsigned i = 0; i < 8; i++)
        sprColor[i].reset(0);
    
	// Disable cheating by default
	drawSprites = true;
	spriteSpriteCollisionEnabled = 0xFF;
	spriteBackgroundCollisionEnabled = 0xFF;
}

void
VIC::ping()
{
    VirtualComponent::ping();
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
	msg("      MainFrameFF : %d\n", p.mainFrameFF);
	msg("  VerticalFrameFF : %d\n", p.verticalFrameFF);
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

size_t
VIC::stateSize()
{
    size_t result = VirtualComponent::stateSize();

    result += control1.stateSize();
    result += control2.stateSize();
    result += borderColor.stateSize();
    for (unsigned i = 0; i < 4; i++)
        result += bgColor[i].stateSize();
    result += sprExtraColor1.stateSize();
    result += sprExtraColor2.stateSize();
    for (unsigned i = 0; i < 8; i++)
        result += sprColor[i].stateSize();

    return result;
}

void
VIC::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    VirtualComponent::loadFromBuffer(buffer);

    control1.loadFromBuffer(buffer);
    control2.loadFromBuffer(buffer);
    borderColor.loadFromBuffer(buffer);
    for (unsigned i = 0; i < 4; i++)
        bgColor[i].loadFromBuffer(buffer);
    sprExtraColor1.loadFromBuffer(buffer);
    sprExtraColor2.loadFromBuffer(buffer);
    for (unsigned i = 0; i < 8; i++)
        sprColor[i].loadFromBuffer(buffer);
    
    assert(*buffer - old == stateSize());
}

void
VIC::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    VirtualComponent::saveToBuffer(buffer);
    
    control1.saveToBuffer(buffer);
    control2.saveToBuffer(buffer);
    borderColor.saveToBuffer(buffer);
    for (unsigned i = 0; i < 4; i++)
        bgColor[i].saveToBuffer(buffer);
    sprExtraColor1.saveToBuffer(buffer);
    sprExtraColor2.saveToBuffer(buffer);
    for (unsigned i = 0; i < 8; i++)
        sprColor[i].saveToBuffer(buffer);
    
    assert(*buffer - old == stateSize());
}

VICInfo
VIC::getInfo()
{
    VICInfo info;
    
    info.rasterline = c64->rasterline;
    info.cycle = c64->rasterlineCycle;
    info.xCounter = xCounter;
    info.badLine = badLineCondition;
    info.ba = (BAlow == 0);
    info.displayMode = getDisplayMode();
    info.borderColor = borderColor.current() & 0xF;
    info.backgroundColor0 = bgColor[0].current() & 0xF;
    info.backgroundColor1 = bgColor[1].current() & 0xF;
    info.backgroundColor2 = bgColor[2].current() & 0xF;
    info.backgroundColor3 = bgColor[3].current() & 0xF;
    info.screenGeometry = getScreenGeometry();
    info.dx = getHorizontalRasterScroll();
    info.dy = getVerticalRasterScroll();
    info.verticalFrameFlipflop = p.verticalFrameFF;
    info.horizontalFrameFlipflop = p.mainFrameFF;
    info.memoryBankAddr = getMemoryBankAddr();
    info.screenMemoryAddr = getScreenMemoryAddr();
    info.characterMemoryAddr = getCharacterMemoryAddr();
    info.imr = imr;
    info.irr = irr;
    info.spriteCollisionIrqEnabled = irqOnSpriteSpriteCollision();
    info.backgroundCollisionIrqEnabled = irqOnSpriteBackgroundCollision();
    info.rasterIrqEnabled = rasterInterruptEnabled();
    info.irqRasterline = rasterInterruptLine();
    info.irqLine = (imr & irr) != 0;
    
    return info;
}

SpriteInfo
VIC::getSpriteInfo(unsigned i)
{
    SpriteInfo info;
    
    info.enabled = spriteEnabled(i);
    info.x = getSpriteX(i);
    info.y = getSpriteY(i);
    info.color = sprColor[i].current() & 0xF;
    info.multicolor = spriteIsMulticolor(i);
    info.extraColor1 = sprExtraColor1.current() & 0xF;
    info.extraColor2 = sprExtraColor2.current() & 0xF;
    info.expandX = spriteWidthIsDoubled(i);
    info.expandY = spriteHeightIsDoubled(i);
    info.priority = spritePriority(i);
    info.collidesWithSprite = spriteCollidesWithSprite(i);
    info.collidesWithBackground = spriteCollidesWithBackground(i);
    
    return info;
}

void
VIC::setChipModel(VICChipModel model)
{
    debug(2, "VIC::setChipModel(%d)\n", model);
    
    c64->suspend();
    
    chipModel = model;
    updatePalette();
    pixelEngine.resetScreenBuffers();
    c64->updateVicFunctionTable();
    
    switch(chipModel) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:
        case PAL_8565:
            c64->setClockFrequency(PAL_CLOCK_FREQUENCY);
            c64->putMessage(MSG_PAL);
            break;
            
        case NTSC_6567:
        case NTSC_6567_R56A:
        case NTSC_8562:
            c64->setClockFrequency(NTSC_CLOCK_FREQUENCY);
            c64->putMessage(MSG_NTSC);
            break;
            
        default:
            assert(false);
    }
    
    c64->resume();
}

unsigned
VIC::getClockFrequency()
{
    switch (chipModel) {
            
        case NTSC_6567:
        case NTSC_8562:
        case NTSC_6567_R56A:
            return NTSC_CLOCK_FREQUENCY;
            
        default:
            return PAL_CLOCK_FREQUENCY;
    }
}

unsigned
VIC::getCyclesPerRasterline()
{
    switch (chipModel) {
            
        case NTSC_6567_R56A:
            return 64;
            
        case NTSC_6567:
        case NTSC_8562:
            return 65;
            
        default:
            return 63;
    }
}

bool
VIC::isLastCycleInRasterline(unsigned cycle)
{
    return cycle >= getCyclesPerRasterline();
}

unsigned
VIC::getRasterlinesPerFrame()
{
    switch (chipModel) {
            
        case NTSC_6567_R56A:
            return 262;
            
        case NTSC_6567:
        case NTSC_8562:
            return 263;
            
        default:
            return 312;
    }
}

bool
VIC::isVBlankLine(unsigned rasterline)
{
    switch (chipModel) {
            
        case NTSC_6567_R56A:
            return rasterline < 16 || rasterline >= 16 + 234;
            
        case NTSC_6567:
        case NTSC_8562:
            return rasterline < 16 || rasterline >= 16 + 235;
            
        default:
            return rasterline < 16 || rasterline >= 16 + 284;
    }
}


//
// Getter and setter
//

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

void
VIC::setDisplayMode(DisplayMode m) {
    
    c64->suspend();
    
    uint8_t newCntrl1 = (control1.current() & ~0x60) | (m & 0x60);
    uint8_t newCntrl2 = (control2.current() & ~0x10) | (m & 0x10);
    control1.write(repeated(newCntrl1));
    control2.write(repeated(newCntrl2));

    c64->resume();
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
		
	assert(addr <= 0x3F);
	
	switch(addr) {
		case 0x11: // SCREEN CONTROL REGISTER #1
            return (control1.current() & 0x7f) | (yCounter > 0xFF ? 0x80 : 0);
            
		case 0x12: // VIC_RASTER_READ_WRITE
			return yCounter & 0xff;
            
		case 0x13: // LIGHTPEN X
			return iomem[addr];
            
		case 0x14: // LIGHTPEN Y
			return iomem[addr];
            
        case 0x16:
            // The two upper bits always read back as '1'
            return (control2.current() & 0xFF) | 0xC0;

   		case 0x18:
            return iomem[addr] | 0x01; // Bit 1 is unused (always 1)
            
		case 0x19: // Interrupt Request Register (IRR)
            return (irr & imr) ? (irr | 0xF0) : (irr | 0x70);
                        
		case 0x1A: // Interrupt Mask Register (IMR)
            return imr | 0xF0;
            
        case 0x1D: // SPRITE_X_EXPAND
            return p.spriteXexpand;

		case 0x1E: // Sprite-to-sprite collision
			result = iomem[addr];
			iomem[addr] = 0x00;  // Clear on read
			return result;
            
		case 0x1F: // Sprite-to-background collision
			result = iomem[addr];
			iomem[addr] = 0x00;  // Clear on read
			return result;

        case 0x20:
            return (borderColor.current() & 0x0F) | 0xF0;
            
        case 0x21: // Background color 0
        case 0x22: // Background color 1
        case 0x23: // Background color 2
        case 0x24: // Background color 3
            
            return (bgColor[addr - 0x21].current() & 0x0F) | 0xF0;
      
        case 0x25: // Sprite extra color 1 (for multicolor sprites)
            return (sprExtraColor1.current() & 0x0F) | 0xF0;
            
        case 0x26: // Sprite extra color 2 (for multicolor sprites)
            return (sprExtraColor2.current() & 0x0F) | 0xF0;
            
        case 0x27: // Sprite color 1
        case 0x28: // Sprite color 2
        case 0x29: // Sprite color 3
        case 0x2A: // Sprite color 4
        case 0x2B: // Sprite color 5
        case 0x2C: // Sprite color 6
        case 0x2D: // Sprite color 7
        case 0x2E: // Sprite color 8
            return (sprColor[addr - 0x27].current() & 0x0F) | 0xF0;
    }
		
	if (addr >= 0x2F && addr <= 0x3F) {
		// Unusable register area
		return 0xFF; 
	}
	
	// Default action
	return iomem[addr];
}

uint8_t
VIC::spypeek(uint16_t addr)
{
    assert(addr <= 0x003F);
    
    switch(addr) {
            
        case 0x1E:
            return iomem[addr];
            
        case 0x1F:
            return iomem[addr];
            
        default:
            return peek(addr);
    }
}

void
VIC::poke(uint16_t addr, uint8_t value)
{
	assert(addr < 0x40);
    
	switch(addr) {		
        case 0x00: // SPRITE_0_X
            p.spriteX[0] = value | ((iomem[0x10] & 0x01) << 8);
            break;

        case 0x02: // SPRITE_1_X
            p.spriteX[1] = value | ((iomem[0x10] & 0x02) << 7);
            break;

        case 0x04: // SPRITE_2_X
            p.spriteX[2] = value | ((iomem[0x10] & 0x04) << 6);
            break;

        case 0x06: // SPRITE_3_X
            p.spriteX[3] = value | ((iomem[0x10] & 0x08) << 5);
            break;

        case 0x08: // SPRITE_4_X
            p.spriteX[4] = value | ((iomem[0x10] & 0x10) << 4);
            break;

        case 0x0A: // SPRITE_5_X
            p.spriteX[5] = value | ((iomem[0x10] & 0x20) << 3);
            break;
            
        case 0x0C: // SPRITE_6_X
            p.spriteX[6] = value | ((iomem[0x10] & 0x40) << 2);
            break;
            
        case 0x0E: // SPRITE_7_X
            p.spriteX[7] = value | ((iomem[0x10] & 0x80) << 1);
            break;

        case 0x10: // SPRITE_X_UPPER_BITS
            p.spriteX[0] = (p.spriteX[0] & 0xFF) | ((value & 0x01) << 8);
            p.spriteX[1] = (p.spriteX[1] & 0xFF) | ((value & 0x02) << 7);
            p.spriteX[2] = (p.spriteX[2] & 0xFF) | ((value & 0x04) << 6);
            p.spriteX[3] = (p.spriteX[3] & 0xFF) | ((value & 0x08) << 5);
            p.spriteX[4] = (p.spriteX[4] & 0xFF) | ((value & 0x10) << 4);
            p.spriteX[5] = (p.spriteX[5] & 0xFF) | ((value & 0x20) << 3);
            p.spriteX[6] = (p.spriteX[6] & 0xFF) | ((value & 0x40) << 2);
            p.spriteX[7] = (p.spriteX[7] & 0xFF) | ((value & 0x80) << 1);
            break;

        case 0x11: // CONTROL_REGISTER_1

            if ((control1.current() & 0x80) != (value & 0x80)) {
                // Value changed: Check if we need to trigger an interrupt immediately
                control1.write(repeated(value));
                if (yCounter == rasterInterruptLine())
                    triggerDelayedIRQ(1);
            } else {
                control1.write(repeated(value));
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
					triggerDelayedIRQ(1);
			}
			return;

        case 0x16: // CONTROL_REGISTER_2

            control2.write(repeated(value));
            return;
            
		case 0x17: // SPRITE Y EXPANSION
			iomem[addr] = value;
            cleared_bits_in_d017 = (~value) & (~expansionFF);
            
            /* "1. Das Expansions-Flipflop ist gesetzt, solange das zum jeweiligen Sprite
                   gehörende Bit MxYE in Register $d017 gelöscht ist." [C.B.] */
            
			expansionFF |= ~value;
			return;
			
        case 0x18: { // MEMORY_SETUP_REGISTER
            
            uint8_t oldValue = iomem[addr];
            iomem[addr] = value;
            
            // The GUI needs to know when the second bit changes. This bit
            // lets us distinguish between uppercase / lowercase character mode
            if ((oldValue ^ value) & 0x02) {
                c64->putMessage(MSG_CHARSET);
            }
			return;
        }
		case 0x19: // Interrupt Request Register (IRR)
            
            // Bits are cleared by writing '1'
            irr &= (~value) & 0x0F;
    
            if (!(irr & imr)) {
                delay |= VICReleaseIrq1;
            }
			return;
            
		case 0x1A: // Interrupt Mask Register (IMR)
            
            imr = value & 0x0F;
            
            if (irr & imr) {
                triggerDelayedIRQ(1);
                // c64->cpu.pullDownIrqLine(CPU::INTSRC_VIC);
            } else {
                delay |= VICReleaseIrq1;
                // c64->cpu.releaseIrqLine(CPU::INTSRC_VIC);
            }
			return;		
			
        case 0x1D: // SPRITE_X_EXPAND
            p.spriteXexpand = value;
            return;
            
		case 0x1E:
		case 0x1F:
			// Writing has no effect
			return;
            
        case 0x20: // Color registers
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
            
            pokeColorReg(addr, value & 0x0F);
            return;
    }
	
	// Default action
	iomem[addr] = value;
}

void
VIC::pokeColorReg(uint16_t addr, uint8_t value)
{
    assert(addr >= 0x20 && addr <= 0x2E);
    assert(is_uint4_t(value));
    
    // Setup the color mask for the gray dot bug
    uint64_t grayDot = (hasGrayDotBug() && emulateGrayDotBug) ? 0xF : 0x0;
  
    switch(addr) {
            
        case 0x20: // Border color
     
            borderColor.write(pattern[value]);
            borderColor.pipeline[1] |= grayDot;
            return;
            
        case 0x21: // Background color 0
        case 0x22: // Background color 1
        case 0x23: // Background color 2
        case 0x24: // Background color 3
            
            bgColor[addr - 0x21].write(pattern[value]);
            bgColor[addr - 0x21].pipeline[1] |= grayDot;
            return;
            
        case 0x25: // Sprite extra color 1 (for multicolor sprites)
            
            sprExtraColor1.write(pattern[value]);
            sprExtraColor1.pipeline[1] |= grayDot;
            return;
            
        case 0x26: // Sprite extra color 2 (for multicolor sprites)

            sprExtraColor2.write(pattern[value]);
            sprExtraColor2.pipeline[1] |= grayDot;
            return;
            
        case 0x27: // Sprite color 1
        case 0x28: // Sprite color 2
        case 0x29: // Sprite color 3
        case 0x2A: // Sprite color 4
        case 0x2B: // Sprite color 5
        case 0x2C: // Sprite color 6
        case 0x2D: // Sprite color 7
        case 0x2E: // Sprite color 8

            sprColor[addr - 0x27].write(pattern[value]);
            sprColor[addr - 0x27].pipeline[1] |= grayDot;
            return;
            
        default:
            assert(false);
    }
}


//
// I/O memory handling and RAM access
//

uint8_t VIC::memAccess(uint16_t addr)
{
    // VIC has only 14 address lines. To be able to access the complete 64KB main memory,
    // it inverts bit 0 and bit 1 of the CIA2 portA register and uses these values as the
    // upper two address bits.
    
    assert((addr & 0xC000) == 0); /* 14 bit address */
    assert((bankAddr & 0x3FFF) == 0); /* multiple of 16 KB */
    
    addrBus = bankAddr | addr;
    
    // VIC memory mapping (http://www.harries.dk/files/C64MemoryMaps.pdf)
    // Note: Final Cartridge III (freezer mode) only works when BLANK is replaced
    //       by RAM. So this mapping might not be 100% correct.
    //
    //          Ultimax  Standard
    // 0xF000:   ROMH      RAM
    // 0xE000:   RAM       RAM
    // 0xD000:   RAM       RAM
    // 0xC000:   BLANK     RAM
    // --------------------------
    // 0xB000:   ROMH      RAM
    // 0xA000:   BLANK     RAM
    // 0x9000:   RAM       CHAR
    // 0x8000:   RAM       RAM
    // --------------------------
    // 0x7000:   ROMH      RAM
    // 0x6000:   BLANK     RAM
    // 0x5000:   BLANK     RAM
    // 0x4000:   BLANK     RAM
    // --------------------------
    // 0x3000:   ROMH      RAM
    // 0x2000:   BLANK     RAM
    // 0x1000:   BLANK     CHAR
    // 0x0000:   RAM       RAM
    
    if (!c64->getUltimax()) {
        switch (addrBus >> 12) {
            case 0x9:
            case 0x1:
                dataBus = c64->mem.rom[0xC000 + addr];
                break;
            default:
                dataBus = c64->mem.ram[addrBus];
        }
    } else {
        switch (addrBus >> 12) {
            case 0xF:
            case 0xB:
            case 0x7:
            case 0x3:
                dataBus = c64->expansionport.peek(addrBus | 0xF000);
                // dataBus = c64->expansionport.peekRomH(addrBus | 0xF000);
                break;
            case 0xE:
            case 0xD:
            case 0x9:
            case 0x8:
            case 0x0:
                dataBus = c64->mem.ram[addrBus];
                break;
            default:
                dataBus = c64->mem.ram[addrBus];
        }
    }
    
    return dataBus;
}

uint8_t VIC::memIdleAccess()
{
    return memAccess(0x3FFF);
}

void VIC::cAccess()
{
    // Only proceed if the BA line is pulled down
    if (!badLineCondition)
        return;
    
    // If BA is pulled down for at least three cycles, perform memory access
    if (BApulledDownForAtLeastThreeCycles()) {
        
        // |VM13|VM12|VM11|VM10| VC9| VC8| VC7| VC6| VC5| VC4| VC3| VC2| VC1| VC0|
        uint16_t addr = (VM13VM12VM11VM10() << 6) | registerVC;
        
        characterSpace[registerVMLI] = memAccess(addr);
        colorSpace[registerVMLI] = c64->mem.colorRam[registerVC] & 0x0F;
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
        colorSpace[registerVMLI] = c64->mem.ram[c64->cpu.getPC()] & 0x0F;
    }
}

void VIC::gAccess()
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
        
        if (BMMbitInPreviousCycle() | BMMbit()) {
            addr = (CB13() << 10) | (registerVC << 3) | registerRC;
        } else {
            addr = (CB13CB12CB11() << 10) | (characterSpace[registerVMLI] << 3) | registerRC;
        }
        
        // "Bei gesetztem ECM-Bit schaltet der Adressgenerator bei den g-Zugriffen die
        //  Adressleitungen 9 und 10 immer auf Low, bei ansonsten gleichem Adressschema
        //  (z.B. erfolgen dann die g-Zugriffe im Idle-Zustand an Adresse $39ff)." [C.B.]
        
        if (ECMbit())
            addr &= 0xF9FF;
        
        // Prepare graphic sequencer
        p.g_data = memAccess(addr);
        p.g_character = characterSpace[registerVMLI];
        p.g_color = colorSpace[registerVMLI];
        
        // "Nach jedem g-Zugriff im Display-Zustand werden VC und VMLI erhöht." [C.B.]
        registerVC++;
        registerVC &= 0x3FF; // 10 bit overflow
        registerVMLI++;
        registerVMLI &= 0x3F; // 6 bit overflow
        
    } else {
        
        // "Im Idle-Zustand erfolgen die g-Zugriffe immer an Videoadresse $3fff." [C.B.]
        addr = ECMbit() ? 0x39FF : 0x3FFF;
        
        // Prepare graphic sequencer
        p.g_data = memAccess(addr);
        p.g_character = 0;
        p.g_color = 0;
    }
}

void VIC::pAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    // |VM13|VM12|VM11|VM10|  1 |  1 |  1 |  1 |  1 |  1 |  1 |  Spr.-Nummer |
    spritePtr[sprite] = memAccess((VM13VM12VM11VM10() << 6) | 0x03F8 | sprite) << 6;
    
}

void VIC::sFirstAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    
    isFirstDMAcycle = (1 << sprite);
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        if (BApulledDownForAtLeastThreeCycles())
            data = memAccess(spritePtr[sprite] | mc[sprite]);
        
        mc[sprite]++;
        mc[sprite] &= 0x3F; // 6 bit overflow
    }
    
    pixelEngine.sprite_sr[sprite].chunk1 = data;
}

void VIC::sSecondAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    bool memAccessed = false;
    
    isFirstDMAcycle = 0;
    isSecondDMAcycle = (1 << sprite);
    
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
    
    pixelEngine.sprite_sr[sprite].chunk2 = data;
}

void VIC::sThirdAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        if (BApulledDownForAtLeastThreeCycles())
            data = memAccess(spritePtr[sprite] | mc[sprite]);
        
        mc[sprite]++;
        mc[sprite] &= 0x3F; // 6 bit overflow
    }
    
    pixelEngine.sprite_sr[sprite].chunk3 = data;
}


void VIC::sFinalize(unsigned sprite)
{
    assert(sprite < 8);
    isSecondDMAcycle = 0;
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

ScreenGeometry 
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


//
// DMA lines, BA signal, and IRQs
//

void
VIC::setBAlow(uint8_t value)
{
    if (!BAlow && value)
        BAwentLowAtCycle = c64->currentCycle();
    
    BAlow = value;
    c64->cpu.setRDY(value == 0);
}

bool
VIC::BApulledDownForAtLeastThreeCycles()
{
    return BAlow && (c64->currentCycle() - BAwentLowAtCycle > 2);
}

void 
VIC::triggerIRQ(uint8_t source, unsigned cycleDelay)
{
    assert(source == 1 || source == 2 || source == 4 || source == 8);
    
    irr |= source;
    if (irr & imr) {
        switch (cycleDelay) {
            case 0:
                c64->cpu.pullDownIrqLine(CPU::INTSRC_VIC);
                return;
            case 1:
                delay |= VICTriggerIrq1;
                return;
            default:
                assert(false);
        }
    }
}

uint16_t
VIC::vicXPosFromCycle(uint8_t cycle, uint16_t offset)
{
    // uint16_t x = (cycle < 14) ? (cycle * 8 + 0x18C) : ((cycle - 14) * 8 + 4);
    // uint16_t result = (x + offset + 0x1f8) % 0x1f8;
    uint16_t x = (cycle < 14) ? (cycle * 8 + 396) : ((cycle - 14) * 8 + 4);
    uint16_t result = (x + offset + 504) % 504;
    // cycle 1: 404  VICE: 404
    // cycle 2:
    
    if (result != xCounter) {
        // debug("result = %d xCounter = %d", result, xCounter);
    }

    return result;
}

uint16_t
VIC::lightpenX()
{
    uint8_t cycle = c64->getRasterlineCycle();
    
    switch (chipModel) {
            
        case PAL_6569_R1:
        case PAL_6569_R3:

            return 4 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);

        case PAL_8565:
            
            return 2 + (cycle < 14 ? 392 + (8 * cycle) : (cycle - 14) * 8);
            
        case NTSC_6567:
        case NTSC_6567_R56A:
            
            return 4 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        case NTSC_8562:
            
            return 2 + (cycle < 14 ? 400 + (8 * cycle) : (cycle - 14) * 8);
            
        default:
            assert(false);
    }
}

uint16_t
VIC::lightpenY()
{
    return getRasterline();
}

void
VIC::setLP(bool value)
{
    // A negative transition on LP triggers a lightpen event.
    if (FALLING_EDGE(lp, value)) {
        delay |= VICLpTransition0;
    }
    
    lp = value;
}

void
VIC::triggerLightpenInterrupt()
{
    uint8_t vicCycle = c64->getRasterlineCycle();
    debug("Negative LP transition at rastercycle %d\n", vicCycle);

    // An interrupt is suppressed if ...
    
    // ... a previous interrupt has occured in the current frame.
    if (lightpenIRQhasOccured)
        return;

    // ... we are in the last PAL rasterline and not in cycle 1.
    if (yCounter == PAL_HEIGHT - 1 && vicCycle != 1)
        return;
    
    // Latch coordinates
    iomem[0x13] = lightpenX() / 2;
    iomem[0x14] = lightpenY();
    debug("Lightpen x = %d\n", lightpenX());
    // Newer VIC models trigger an interrupt immediately
    if (chipModel != PAL_6569_R1 && chipModel != NTSC_6567_R56A) {
        triggerIRQ(8);
    }
    
    // Lightpen interrupts can only occur once per frame
    lightpenIRQhasOccured = true;
}

void
VIC::retriggerLightpenInterrupt()
{
    // This function is called at the beginning of a frame, only.
    assert(c64->getRasterline() == 0);
    assert(c64->getRasterlineCycle() == 1);
    assert(lightpenIRQhasOccured == false);
 
    // Determine lightpen coordinates and trigger an interrupt if an old VICII
    // model is emulated. 
    uint16_t x, y;

    switch (chipModel) {
            
        case PAL_6569_R1:
            
            x = 0xD1;vicXPosFromCycle(c64->getRasterlineCycle(), 6 /* Hoxs64 */);
            y = yCounter;
            triggerIRQ(8);
            break;
            
        case PAL_6569_R3:
            
            x = vicXPosFromCycle(c64->getRasterlineCycle(), 6 /* Hoxs64 */);
            y = yCounter;
            if (x + 2 >= 0x194 && (x + 2 - 6) < 0x194) y++; /* Hoxs64 */
            break;
            
        case PAL_8565:
            
            x = vicXPosFromCycle(c64->getRasterlineCycle(), 6 /* Hoxs64 */);
            y = yCounter;
            if (x + 2 >= 0x194 && (x + 2 - 6) < 0x194) y++; /* Hoxs64 */
            break;
            
        case NTSC_6567:
            
            x = vicXPosFromCycle(c64->getRasterlineCycle(), 6 /* Hoxs64 */);
            y = yCounter;
            if (x + 2 >= 0x194 && (x + 2 - 6) < 0x194) y++; /* Hoxs64 */
            break;
            
        case NTSC_6567_R56A:
            
            x = vicXPosFromCycle(c64->getRasterlineCycle(), 6 /* Hoxs64 */);
            y = yCounter;
            triggerIRQ(8);
            break;
            
        case NTSC_8562:
            
            x = vicXPosFromCycle(c64->getRasterlineCycle(), 6 /* Hoxs64 */);
            y = yCounter;
            if (x + 2 >= 0x194 && (x + 2 - 6) < 0x194) y++; /* Hoxs64 */
            break;
            
        default:
            assert(false);
    }
    
    // Latch coordinates
    // iomem[0x13] = x / 2;
    // iomem[0x14] = y;

    // Lightpen interrupts can only occur once per frame
    lightpenIRQhasOccured = true;
}


//
// Sprites
//

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

void
VIC::setSpriteColor(uint8_t nr, uint8_t color)
{
    assert(nr < 8);
    c64->suspend();
    sprColor[nr].write(pattern[color]);
    c64->resume();
}


//
// Frame flipflops
//

void
VIC::checkVerticalFrameFF()
{
    // Check for upper border
    if (yCounter == upperComparisonValue() && DENbit()) {
        verticalFrameFFclearCond = true;
    }
    // Trigger immediately (similar to VICE)
    if (verticalFrameFFclearCond) {
        p.verticalFrameFF = false;
    }
    
    // Check for lower border
    if (yCounter == lowerComparisonValue()) {
        verticalFrameFFsetCond = true;
    }
    // Trigger in cycle 1 (similar to VICE)
}

void
VIC::checkFrameFlipflopsLeft(uint16_t comparisonValue)
{
    // "6. Erreicht die X-Koordinate den linken Vergleichswert und ist das
    //     vertikale Rahmenflipflop gelöscht, wird das Haupt-Flipflop gelöscht." [C.B.]

    if (comparisonValue == leftComparisonValue()) {
        clearMainFrameFF();
    }
}

void
VIC::checkFrameFlipflopsRight(uint16_t comparisonValue)
{
    // "1. Erreicht die X-Koordinate den rechten Vergleichswert, wird das
    //     Haupt-Rahmenflipflop gesetzt." [C.B.]
    
    if (comparisonValue == rightComparisonValue()) {
        p.mainFrameFF = true;
    }
}

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
    
    // Retrigger lightpen interrupt if lp line is still pulled down
    if (!lp)
        retriggerLightpenInterrupt();
}

void
VIC::endFrame()
{
    pixelEngine.endFrame();
}

void
VIC::preparePixelEngine() {
    
    uint8_t ctrl1 = control1.delayed() & 0xFF;
    pixelEngine.pipe = p;
    pixelEngine.pipe.previousCTRL1 = ctrl1;
}

void 
VIC::beginRasterline(uint16_t line)
{
    verticalFrameFFsetCond = verticalFrameFFclearCond = false;

    // Determine if we're currently processing a VBLANK line (nothing is drawn in this area)
    vblank = isVBlankLine(line);
    /*
    if (isPAL()) {
        vblank = line < PAL_UPPER_VBLANK || line >= PAL_UPPER_VBLANK + PAL_RASTERLINES;
    } else {
        vblank = line < NTSC_UPPER_VBLANK || line >= NTSC_UPPER_VBLANK + NTSC_RASTERLINES;
    }
    */
    
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

    // Check, if we are currently processing a DMA line. The result is stored
    // in variable badLineCondition. The initial value can change in the middle
    // of a rasterline.
    updateBadLineCondition();
    
    pixelEngine.beginRasterline();
}

void 
VIC::endRasterline()
{
    // Set vertical flipflop if condition was hit
    if (verticalFrameFFsetCond) {
        p.verticalFrameFF = true;
    }
    
    // Draw debug markers
    if (markIRQLines && yCounter == rasterInterruptLine())
        pixelEngine.markLine(VICII_WHITE);
    if (markDMALines && badLineCondition)
        pixelEngine.markLine(VICII_RED);

    /*
    if (c64->rasterline == 51 && !vblank) {
        pixelEngine.markLine(4);
    }
    */
    
    pixelEngine.endRasterline();
}

bool
VIC::yCounterOverflow()
{
    /* PAL machines reset the yCounter in cycle 2 in the first rasterline.
     * NTSC machines reset the yCounter in cycle 2 in the middle of the lower
     * border area.
     */
    return c64->getRasterline() == (c64->vic.isPAL() ? 0 : 238);
}


//
// Execution functions
//










