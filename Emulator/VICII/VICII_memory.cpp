// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "VICII.h"
#include "C64.h"

void
VICII::setUltimax(bool value) {
    
    // Details: http://www.harries.dk/files/C64MemoryMaps.pdf
    
    ultimax = value;
    
    if (value) {
        
        memSrc[0x0] = M_RAM;
        memSrc[0x1] = M_RAM;
        memSrc[0x2] = M_RAM;
        memSrc[0x3] = M_CRTHI;
        memSrc[0x4] = M_RAM;
        memSrc[0x5] = M_RAM;
        memSrc[0x6] = M_RAM;
        memSrc[0x7] = M_CRTHI;
        memSrc[0x8] = M_RAM;
        memSrc[0x9] = M_RAM;
        memSrc[0xA] = M_RAM;
        memSrc[0xB] = M_CRTHI;
        memSrc[0xC] = M_RAM;
        memSrc[0xD] = M_RAM;
        memSrc[0xE] = M_RAM;
        memSrc[0xF] = M_CRTHI;
    
    } else {
        
        memSrc[0x0] = M_RAM;
        memSrc[0x1] = M_CHAR;
        memSrc[0x2] = M_RAM;
        memSrc[0x3] = M_RAM;
        memSrc[0x4] = M_RAM;
        memSrc[0x5] = M_RAM;
        memSrc[0x6] = M_RAM;
        memSrc[0x7] = M_RAM;
        memSrc[0x8] = M_RAM;
        memSrc[0x9] = M_CHAR;
        memSrc[0xA] = M_RAM;
        memSrc[0xB] = M_RAM;
        memSrc[0xC] = M_RAM;
        memSrc[0xD] = M_RAM;
        memSrc[0xE] = M_RAM;
        memSrc[0xF] = M_RAM;
    }
}

void
VICII::switchBank(u16 addr) {

    if (config.glueLogic == GLUE_LOGIC_DISCRETE) {
        
        // Models with discrete glue logic switch banks immediately
        updateBankAddr();
        return;
     }
 
    /* Switch table for custom IC glue logic and PA / DDRA register changes
     * The tables have been derived from VICE test case fetchsplit.prg
     */
    u8 switchTablePA[4][4] = {
        { 0, 1, 2, 3 }, // From bank 0
        { 0, 1, 3, 3 }, // From bank 1
        { 0, 3, 2, 3 }, // From bank 2
        { 0, 1, 2, 3 }  // From bank 3
    };
    u8 switchTableDDRA[4][4] = {
        { 0, 1, 2, 3 }, // From bank 0
        { 1, 1, 3, 3 }, // From bank 1
        { 2, 3, 2, 3 }, // From bank 2
        { 0, 3, 3, 3 }  // From bank 3
    };

    // Determine old and new video bank
    u8 from = bankAddr >> 14;
    u8 to = (~cia2.getPA()) & 0x03;
    
    // Switch to the bank given by the switch table
    switch (addr) {
            
        case 0xDD00:
            
            // Change was triggered by writing into CIA2::PA
            updateBankAddr(switchTablePA[from][to]);
            break;
            
        case 0xDD02:
            
            // Change was triggered by writing into CIA2::DDRA
            updateBankAddr(switchTableDDRA[from][to]);
            break;
            
        default:
            fatalError;
    }
   
    // Switch to final bank one cycle later
    delay |= VICUpdateBankAddr;
}

void
VICII::updateBankAddr()
{
    updateBankAddr(~cia2.getPA() & 0x03);
}
    
u8
VICII::peek(u16 addr)
{
    u8 result;
    
    assert(addr <= 0x3F);
    
    switch(addr) {
        case 0x00: // Sprite X (lower 8 bits)
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x08:
        case 0x0A:
        case 0x0C:
        case 0x0E:
            
            result = (u8)reg.current.sprX[addr >> 1];
            break;
            
        case 0x01: // Sprite Y
        case 0x03:
        case 0x05:
        case 0x07:
        case 0x09:
        case 0x0B:
        case 0x0D:
        case 0x0F:
            
            result = reg.current.sprY[addr >> 1];
            break;
            
        case 0x10: // Sprite X (upper bits)
        
            result =
            ((reg.current.sprX[0] & 0x100) ? 0b00000001 : 0) |
            ((reg.current.sprX[1] & 0x100) ? 0b00000010 : 0) |
            ((reg.current.sprX[2] & 0x100) ? 0b00000100 : 0) |
            ((reg.current.sprX[3] & 0x100) ? 0b00001000 : 0) |
            ((reg.current.sprX[4] & 0x100) ? 0b00010000 : 0) |
            ((reg.current.sprX[5] & 0x100) ? 0b00100000 : 0) |
            ((reg.current.sprX[6] & 0x100) ? 0b01000000 : 0) |
            ((reg.current.sprX[7] & 0x100) ? 0b10000000 : 0);
            break;
                   
        case 0x11: // SCREEN CONTROL REGISTER #1
            
            result = (reg.current.ctrl1 & 0x7f) | (yCounter > 0xFF ? 0x80 : 0);
            break;
            
        case 0x12: // VIC_RASTER_READ_WRITE
            
            result = yCounter & 0xff;
            break;
            
        case 0x13: // LIGHTPEN X
            
            result = latchedLPX;
            break;
            
        case 0x14: // LIGHTPEN Y
            
            result = latchedLPY;
            break;
            
        case 0x15:
            
            result = reg.current.sprEnable;
            break;
            
        case 0x16:
            
            // The two upper bits always read back as '1'
            result = (reg.current.ctrl2 & 0xFF) | 0xC0;
            break;
            
        case 0x17:
            
            result = reg.current.sprExpandY;
            break;
            
        case 0x18:
            
            result = memSelect | 0x01; // Bit 1 is unused (always 1)
            break;
            
        case 0x19: // Interrupt Request Register (IRR)
            
            result = (irr & imr) ? (irr | 0xF0) : (irr | 0x70);
            break;
            
        case 0x1A: // Interrupt Mask Register (IMR)
            
            result = imr | 0xF0;
            break;
            
        case 0x1B:
            
            result = reg.current.sprPriority;
            break;
            
        case 0x1C:
            
            result = reg.current.sprMC;
            break;
            
        case 0x1D: // SPRITE_X_EXPAND
            
            result = reg.current.sprExpandX;
            break;
            
        case 0x1E: // Sprite-to-sprite collision
            
            result = spriteSpriteCollision;
            delay |= VICClrSprSprCollReg;
            break;
            
        case 0x1F: // Sprite-to-background collision
            
            result = spriteBackgroundColllision;
            delay |= VICClrSprBgCollReg;
            break;
            
        case 0x20: // Border color
            
            result = reg.current.colors[COLREG_BORDER] | 0xF0;
            break;
            
        case 0x21: // Background color 0
            
            result = reg.current.colors[COLREG_BG0] | 0xF0;
            break;
            
        case 0x22: // Background color 1
            
            result = reg.current.colors[COLREG_BG1] | 0xF0;
            break;
            
        case 0x23: // Background color 2
            
            result = reg.current.colors[COLREG_BG2] | 0xF0;
            break;
            
        case 0x24: // Background color 3
            
            result = reg.current.colors[COLREG_BG3] | 0xF0;
            break;
            
        case 0x25: // Sprite extra color 1 (for multicolor sprites)
            
            result = reg.current.colors[COLREG_SPR_EX1] | 0xF0;
            break;
            
        case 0x26: // Sprite extra color 2 (for multicolor sprites)
            
            result = reg.current.colors[COLREG_SPR_EX2] | 0xF0;
            break;
            
        case 0x27: // Sprite color 1
        case 0x28: // Sprite color 2
        case 0x29: // Sprite color 3
        case 0x2A: // Sprite color 4
        case 0x2B: // Sprite color 5
        case 0x2C: // Sprite color 6
        case 0x2D: // Sprite color 7
        case 0x2E: // Sprite color 8

            result = reg.current.colors[COLREG_SPR0 + addr - 0x27] | 0xF0;
            break;
            
        default:

            assert(addr >= 0x2F && addr <= 0x3F);
            result = 0xFF;
    }
    
    dataBusPhi2 = result;

    trace(VICREG_DEBUG, "peek(%x) = %x\n", addr, result);
    return result;
}

u8
VICII::spypeek(u16 addr) const
{
    assert(addr <= 0x3F);
    
    switch(addr) {
        case 0x00: // Sprite X (lower 8 bits)
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x08:
        case 0x0A:
        case 0x0C:
        case 0x0E:
            
            return (u8)reg.current.sprX[addr >> 1];
            
        case 0x01: // Sprite Y
        case 0x03:
        case 0x05:
        case 0x07:
        case 0x09:
        case 0x0B:
        case 0x0D:
        case 0x0F:
            
            return reg.current.sprY[addr >> 1];
            
        case 0x10: // Sprite X (upper bits)
            
            return
            ((reg.current.sprX[0] & 0x100) ? 0b00000001 : 0) |
            ((reg.current.sprX[1] & 0x100) ? 0b00000010 : 0) |
            ((reg.current.sprX[2] & 0x100) ? 0b00000100 : 0) |
            ((reg.current.sprX[3] & 0x100) ? 0b00001000 : 0) |
            ((reg.current.sprX[4] & 0x100) ? 0b00010000 : 0) |
            ((reg.current.sprX[5] & 0x100) ? 0b00100000 : 0) |
            ((reg.current.sprX[6] & 0x100) ? 0b01000000 : 0) |
            ((reg.current.sprX[7] & 0x100) ? 0b10000000 : 0);
            
        case 0x11: // SCREEN CONTROL REGISTER #1
            
            return (reg.current.ctrl1 & 0x7f) | (yCounter > 0xFF ? 0x80 : 0);
            
        case 0x12: // VIC_RASTER_READ_WRITE
            
            return yCounter & 0xff;
            
        case 0x13: // LIGHTPEN X
            
            return latchedLPX;
            
        case 0x14: // LIGHTPEN Y
            
            return latchedLPY;
            
        case 0x15:
            
            return reg.current.sprEnable;
            
        case 0x16:
            
            // The two upper bits always read back as '1'
            return (reg.current.ctrl2 & 0xFF) | 0xC0;
            
        case 0x17:
            
            return reg.current.sprExpandY;
            
        case 0x18:
            
            return memSelect | 0x01; // Bit 1 is unused (always 1)
            
        case 0x19: // Interrupt Request Register (IRR)
            
            return (irr & imr) ? (irr | 0xF0) : (irr | 0x70);
            
        case 0x1A: // Interrupt Mask Register (IMR)
            
            return imr | 0xF0;
            
        case 0x1B:
            
            return reg.current.sprPriority;
            
        case 0x1C:
            
            return reg.current.sprMC;
            
        case 0x1D: // SPRITE_X_EXPAND
            
            return reg.current.sprExpandX;
            
        case 0x1E: // Sprite-to-sprite collision
            
            return spriteSpriteCollision;
            
        case 0x1F: // Sprite-to-background collision
            
            return spriteBackgroundColllision;
            
        case 0x20: // Border color
            
            return reg.current.colors[COLREG_BORDER] | 0xF0;
            
        case 0x21: // Background color 0
            
            return reg.current.colors[COLREG_BG0] | 0xF0;
            
        case 0x22: // Background color 1
            
            return reg.current.colors[COLREG_BG1] | 0xF0;
            
        case 0x23: // Background color 2
            
            return reg.current.colors[COLREG_BG2] | 0xF0;
            
        case 0x24: // Background color 3
            
            return reg.current.colors[COLREG_BG3] | 0xF0;
            
        case 0x25: // Sprite extra color 1 (for multicolor sprites)
            
            return reg.current.colors[COLREG_SPR_EX1] | 0xF0;
            
        case 0x26: // Sprite extra color 2 (for multicolor sprites)
            
            return reg.current.colors[COLREG_SPR_EX2] | 0xF0;
            
        case 0x27: // Sprite color 1
        case 0x28: // Sprite color 2
        case 0x29: // Sprite color 3
        case 0x2A: // Sprite color 4
        case 0x2B: // Sprite color 5
        case 0x2C: // Sprite color 6
        case 0x2D: // Sprite color 7
        case 0x2E: // Sprite color 8
            
            return reg.current.colors[COLREG_SPR0 + addr - 0x27] | 0xF0;
            
        default:
            
            assert(addr >= 0x2F && addr <= 0x3F);
            return 0xFF;
    }
}

void
VICII::poke(u16 addr, u8 value)
{
    trace(VICREG_DEBUG, "poke(%x, %x)\n", addr, value);
    assert(addr < 0x40);
     
    dataBusPhi2 = value;
    
    switch(addr) {
        case 0x00: // Sprite X (lower 8 bits)
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x08:
        case 0x0A:
        case 0x0C:
        case 0x0E:
            
            reg.current.sprX[addr >> 1] &= 0x100;
            reg.current.sprX[addr >> 1] |= value;
            break;
        
        case 0x01: // Sprite Y
        case 0x03:
        case 0x05:
        case 0x07:
        case 0x09:
        case 0x0B:
        case 0x0D:
        case 0x0F:
            
            reg.current.sprY[addr >> 1] = value;
            break;
            
        case 0x10: // Sprite X (upper bit)
            
            for (isize i = 0; i < 8; i++) {
                reg.current.sprX[i] &= 0xFF;
                reg.current.sprX[i] |= GET_BIT(value, i) ? 0x100 : 0;
            }
            break;
            
        case 0x11: // Control register 1
            
            reg.current.ctrl1 = value;
            reg.current.mode = ((value & 0x60) | (reg.current.ctrl2 & 0x10)) >> 4;
            
            // Check the DEN bit. If it gets set somehwere in line 30, a bad
            // line conditions occurs.
            if (c64.scanline == 0x30 && (value & 0x10))
                DENwasSetInLine30 = true;
            
            if (badLine = badLineCondition(); badLine) {
                delay |= VICSetDisplayState;
            }
            upperComparisonVal = upperComparisonValue();
            lowerComparisonVal = lowerComparisonValue();
            
            rasterIrqLine = (u16)((rasterIrqLine & 0x00FF) | ((value & 0x80) << 1));
            checkForRasterIrq();
            break;
            
        case 0x12: // RASTER_COUNTER
            
            rasterIrqLine = (rasterIrqLine & 0xFF00) | value;
            checkForRasterIrq();
            return;
            
        case 0x13: // Lightpen X
        case 0x14: // Lightpen Y
            
            return;
            
        case 0x15: // SPRITE_ENABLED
            
            reg.current.sprEnable = value;
            break;
            
        case 0x16: // CONTROL_REGISTER_2
            
            reg.current.ctrl2 = value;
            reg.current.mode = ((reg.current.ctrl1 & 0x60) | (value & 0x10)) >> 4;
            reg.current.xscroll = value & 0x07;
            leftComparisonVal = leftComparisonValue();
            rightComparisonVal = rightComparisonValue();
            break;
            
        case 0x17: // SPRITE Y EXPANSION
           
            reg.current.sprExpandY = value;
            cleared_bits_in_d017 = (~value) & (~expansionFF);
            
            /* "The expansion flip flip is set as long as the bit in MxYE in
             *  register $d017 corresponding to the sprite is cleared." [C.B.]
             */
            expansionFF |= ~value;
            break;
            
        case 0x18: // Memory address pointers
            
            memSelect = value;
            return;
    
        case 0x19: // Interrupt Request Register (IRR)
            
            // Bits are cleared by writing '1'
            irr &= (~value) & 0x0F;
            delay |= VICUpdateIrqLine;
            return;
            
        case 0x1A: // Interrupt Mask Register (IMR)
            
            imr = value & 0x0F;
            delay |= VICUpdateIrqLine;
            return;
            
        case 0x1B: // Sprite priority
            
            reg.current.sprPriority = value;
            break;
            
        case 0x1C: // Sprite multicolor
            
            reg.current.sprMC = value;
            break;
            
        case 0x1D: // SPRITE_X_EXPAND
            reg.current.sprExpandX = value;
            break;
            
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
            
            reg.current.colors[addr - 0x20] = value & 0xF;
            
            // Emulate the gray dot bug
            if (config.grayDotBug) reg.delayed.colors[addr - 0x20] = 0xF;

            break;
    }
    
    delay |= VICUpdateRegisters;
}

u8
VICII::memAccess(u16 addr)
{
    assert((addr & 0xC000) == 0); // 14 bit address
    assert((bankAddr & 0x3FFF) == 0); // multiple of 16 KB
    
    addrBus = bankAddr | addr;
    switch (memSrc[addrBus >> 12]) {
            
        case M_RAM:
            return mem.ram[addrBus];
            
        case M_CHAR:
            return mem.rom[0xC000 + addr];

        case M_CRTHI:
            return expansionport.peek(addrBus | 0xF000);
            
        default:
            return mem.ram[addrBus];
    }
}

u8
VICII::memSpyAccess(u16 addr)
{
    u8 result;
    
    assert((addr & 0xC000) == 0);
    assert((bankAddr & 0x3FFF) == 0);
    
    u16 addrBus = bankAddr | addr;
    
    if (getUltimax()) {
        
        switch (addrBus >> 12) {
            case 0xF:
            case 0xB:
            case 0x7:
            case 0x3:
                result = expansionport.spypeek(addrBus | 0xF000);
                break;
            case 0xE:
            case 0xD:
            case 0x9:
            case 0x8:
            case 0x0:
                result = mem.ram[addrBus];
                break;
            default:
                result = mem.ram[addrBus];
        }
        
    } else {
        
        if (isCharRomAddr(addr)) {
            result = mem.rom[0xC000 + addr];
        } else {
            result = mem.ram[addrBus];
        }
    }
    
    return result;
}

bool
VICII::isCharRomAddr(u16 addr) const
{
    addr = (addr | bankAddr) >> 12;
    return addr == 1 || addr == 9;
}
