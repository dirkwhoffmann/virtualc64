/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, 2018
 */
/* This program is free software; you can redistribute it and/or modify
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

uint8_t
VIC::peek(uint16_t addr)
{
    uint8_t result;
    
    assert(addr <= 0x3F);
    
    switch(addr) {
        case 0x00:
        case 0x02:
        case 0x04:
        case 0x06:
        case 0x08:
        case 0x0A:
        case 0x0C:
        case 0x0E:
            
            assert(iomem[addr] == (sprXCoord[addr >> 1].current() & 0xFF));
            return iomem[addr];
            
        case 0x10: { uint8_t result;
            
            result =
            ((sprXCoord[0].current() & 0x100) ? 0b00000001 : 0) |
            ((sprXCoord[1].current() & 0x100) ? 0b00000010 : 0) |
            ((sprXCoord[2].current() & 0x100) ? 0b00000100 : 0) |
            ((sprXCoord[3].current() & 0x100) ? 0b00001000 : 0) |
            ((sprXCoord[4].current() & 0x100) ? 0b00010000 : 0) |
            ((sprXCoord[5].current() & 0x100) ? 0b00100000 : 0) |
            ((sprXCoord[6].current() & 0x100) ? 0b01000000 : 0) |
            ((sprXCoord[7].current() & 0x100) ? 0b10000000 : 0);

            assert(result == iomem[0x10]);
            return result;
        }
                   
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
            sprXCoord[0].write((sprXCoord[0].current() & 0x100) | value);
            assert(p.spriteX[0] == sprXCoord[0].current());
            break;
            
        case 0x02: // SPRITE_1_X
            p.spriteX[1] = value | ((iomem[0x10] & 0x02) << 7);
            sprXCoord[1].write((sprXCoord[1].current() & 0x100) | value);
            assert(p.spriteX[1] == sprXCoord[1].current());
            break;
            
        case 0x04: // SPRITE_2_X
            p.spriteX[2] = value | ((iomem[0x10] & 0x04) << 6);
            sprXCoord[2].write((sprXCoord[2].current() & 0x100) | value);
            assert(p.spriteX[2] == sprXCoord[2].current());
            break;
            
        case 0x06: // SPRITE_3_X
            p.spriteX[3] = value | ((iomem[0x10] & 0x08) << 5);
            sprXCoord[3].write((sprXCoord[3].current() & 0x100) | value);
            assert(p.spriteX[3] == sprXCoord[3].current());
            break;
            
        case 0x08: // SPRITE_4_X
            p.spriteX[4] = value | ((iomem[0x10] & 0x10) << 4);
            sprXCoord[4].write((sprXCoord[4].current() & 0x100) | value);
            assert(p.spriteX[4] == sprXCoord[4].current());
            break;
            
        case 0x0A: // SPRITE_5_X
            p.spriteX[5] = value | ((iomem[0x10] & 0x20) << 3);
            sprXCoord[5].write((sprXCoord[5].current() & 0x100) | value);
            assert(p.spriteX[5] == sprXCoord[5].current());
            break;
            
        case 0x0C: // SPRITE_6_X
            p.spriteX[6] = value | ((iomem[0x10] & 0x40) << 2);
            sprXCoord[6].write((sprXCoord[6].current() & 0x100) | value);
            assert(p.spriteX[6] == sprXCoord[6].current());
            break;
            
        case 0x0E: // SPRITE_7_X
            p.spriteX[7] = value | ((iomem[0x10] & 0x80) << 1);
            sprXCoord[7].write((sprXCoord[7].current() & 0x100) | value);
            assert(p.spriteX[7] == sprXCoord[7].current());
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
            for (unsigned i = 0; i < 8; i++) {
                uint16_t upperBit = GET_BIT(value, i) ? 0x100 : 0;
                sprXCoord[i].write(upperBit | (sprXCoord[i].current() & 0xFF));
                assert(sprXCoord[i].current() == p.spriteX[i]); 
            }
            break;
            
        case 0x11: // CONTROL_REGISTER_1
            
            if ((control1.current() & 0x80) != (value & 0x80)) {
                // Value changed: Check if we need to trigger an interrupt immediately
                control1.write(value);
                if (yCounter == rasterInterruptLine())
                    triggerDelayedIRQ(1);
            } else {
                control1.write(value);
            }
            
            // Check the DEN bit if we're in rasterline 30
            // If it's set at some point in that line, bad line conditions can occur
            // if (yCounter == 0x30 && (value & 0x10) != 0)
            if (c64->rasterline == 0x30 && (value & 0x10) != 0)
                DENwasSetInRasterline30 = true;
            
            // Bits 0 to 3 are the vertical scroll offset. If they change in
            // the middle of the rasterline, the bad line condition might change
            // as well.
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
            
        case 0x15: // SPRITE_ENABLED
            
            iomem[addr] = value;
            return;
            
        case 0x16: // CONTROL_REGISTER_2
            
            control2.write(value);
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
            
            borderColor.write(value);
            borderColor.pipeline[1] |= grayDot;
            return;
            
        case 0x21: // Background color 0
        case 0x22: // Background color 1
        case 0x23: // Background color 2
        case 0x24: // Background color 3
            
            bgColor[addr - 0x21].write(value);
            bgColor[addr - 0x21].pipeline[1] |= grayDot;
            return;
            
        case 0x25: // Sprite extra color 1 (for multicolor sprites)
            
            sprExtraColor1.write(value);
            sprExtraColor1.pipeline[1] |= grayDot;
            return;
            
        case 0x26: // Sprite extra color 2 (for multicolor sprites)
            
            sprExtraColor2.write(value);
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
            
            sprColor[addr - 0x27].write(value);
            sprColor[addr - 0x27].pipeline[1] |= grayDot;
            return;
            
        default:
            assert(false);
    }
}

uint8_t
VIC::memAccess(uint16_t addr)
{
    /* VIC has only 14 address lines. To be able to access the complete 64KB
     * main memory, it inverts bit 0 and bit 1 of the CIA2 portA register and
     * uses these values as the upper two address bits.
     */
    
    assert((addr & 0xC000) == 0); // 14 bit address
    assert((bankAddr & 0x3FFF) == 0); // multiple of 16 KB
    
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

uint8_t
VIC::memIdleAccess()
{
    /* "As described, the VIC accesses in every first clock phase although there
     *  are some cycles in which no other of the above mentioned accesses is
     *  pending. In this case, the VIC does an idle access; a read access to
     *  video address $3fff (i.e. to $3fff, $7fff, $bfff or $ffff depending on
     *  the VIC bank) of which the result is discarded." [C.B.]
     */
    return memAccess(0x3FFF);
}

void
VIC::cAccess()
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
        
        /* "Nevertheless, the VIC accesses the video matrix, or at least it
         *  tries, because as long as AEC is still high in the second clock
         *  phase, the address and data bus drivers D0-D7 of the VIC are in
         *  tri-state and the VIC reads the value $ff from D0-D7 instead of the
         *  data from the video matrix in the first three cycles. The data lines
         *  D8-D13 of the VIC however don't have tri-state drivers and are
         *  always set to input. But the VIC doesn't get valid Color RAM data
         *  from there either, because as AEC is high, the 6510 is still
         *  considered the bus master and unless it doesn't by chance want to
         *  read the next opcode from the Color RAM, the chip select input of
         *  the Color RAM is not active. [...]
         *  To make a long story short: In the first three cycles after BA went
         *  low, the VIC reads $ff as character pointers and as color
         *  information the lower 4 bits of the opcode after the access to
         *  $d011. Not until then, regular video matrix data is read." [C.B.]
         */
        characterSpace[registerVMLI] = 0xFF;
        colorSpace[registerVMLI] = c64->mem.ram[c64->cpu.getPC()] & 0x0F;
    }
}

void
VIC::gAccess()
{
    uint16_t addr;
    
    assert ((registerVC & 0xFC00) == 0); // 10 bit register
    assert ((registerRC & 0xF8) == 0);   // 3 bit register
    
    if (displayState) {
        
        /* "The address generator for the text/bitmap accesses (c- and
         *  g-accesses) has basically 3 modes for the g-accesses (the c-accesses
         *  always follow the same address scheme). In display state, the BMM
         *  bit selects either character generator accesses (BMM=0) or bitmap
         *  accesses (BMM=1). In idle state, the g-accesses are always done at
         *  video address $3fff. If the ECM bit is set, the address generator
         *  always holds the address lines 9 and 10 low without any other
         *  changes to the addressing scheme (e.g. the g-accesses in idle state
         *  then occur at address $39ff)." [C.B.]
         */
        
        //  BMM=1: |CB13| VC9| VC8|VC7|VC6|VC5|VC4|VC3|VC2|VC1|VC0|RC2|RC1|RC0|
        //  BMM=0: |CB13|CB12|CB11|D7 |D6 |D5 |D4 |D3 |D2 |D1 |D0 |RC2|RC1|RC0|
        
        // Determine value of BMM bit
        uint8_t bmm = GET_BIT(control1.delayed(), 5);
        if (!is856x()) {
            bmm |= GET_BIT(control1.current(), 5);
        }
            
        if (BMMbit()) {
            addr = (CB13() << 10) |
            (registerVC << 3) | registerRC;
        } else {
            addr = (CB13CB12CB11() << 10) |
            (characterSpace[registerVMLI] << 3) | registerRC;
        }
        
        /* "If the ECM bit is set, the address generator always holds the
         *  address lines 9 and 10 low without any other changes to the
         *  addressing scheme (e.g. the g-accesses in idle state then occur at
         *  address $39ff)." [C.B.]
         */
        if (ECMbit())
            addr &= 0xF9FF;
        
        // Prepare graphic sequencer
        p.g_data = memAccess(addr);
        p.g_character = characterSpace[registerVMLI];
        p.g_color = colorSpace[registerVMLI];
        
        // "VC and VMLI are incremented after each g-access in display state."
        registerVC++;
        registerVC &= 0x3FF; // 10 bit overflow
        registerVMLI++;
        registerVMLI &= 0x3F; // 6 bit overflow
        
    } else {
        
        // In idle state, a g-access reads from memory location 0x3FFF
        addr = ECMbit() ? 0x39FF : 0x3FFF;
        
        // Prepare graphic sequencer
        p.g_data = memAccess(addr);
        p.g_character = 0;
        p.g_color = 0;
    }
}

void
VIC::pAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    // |VM13|VM12|VM11|VM10|  1 |  1 |  1 |  1 |  1 |  1 |  1 |  Spr.-Nummer |
    spritePtr[sprite] = memAccess((VM13VM12VM11VM10() << 6) | 0x03F8 | sprite) << 6;
    
}

void
VIC::sFirstAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    
    isFirstDMAcycle = (1 << sprite);
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        assert(BApulledDownForAtLeastThreeCycles());
        // if (BApulledDownForAtLeastThreeCycles())
        data = memAccess(spritePtr[sprite] | mc[sprite]);
        
        mc[sprite]++;
        mc[sprite] &= 0x3F; // 6 bit overflow
    }
    
    pixelEngine.sprite_sr[sprite].chunk1 = data;
}

void
VIC::sSecondAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    bool memAccessed = false;
    
    isFirstDMAcycle = 0;
    isSecondDMAcycle = (1 << sprite);
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        assert(BApulledDownForAtLeastThreeCycles());
        // if (BApulledDownForAtLeastThreeCycles())
        {
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

void
VIC::sThirdAccess(unsigned sprite)
{
    assert(sprite < 8);
    
    uint8_t data = 0x00; // TODO: VICE is doing this: vicii.last_bus_phi2;
    
    if (spriteDmaOnOff & (1 << sprite)) {
        
        assert(BApulledDownForAtLeastThreeCycles());
        // if (BApulledDownForAtLeastThreeCycles())
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


