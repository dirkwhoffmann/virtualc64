/*
 * (C) 2006 - 2018 Dirk W. Hoffmann. All rights reserved.
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

void
CPU::adc(uint8_t op)
{
    if (getD())
        adc_bcd(op);
    else
        adc_binary(op);
}

void
CPU::adc_binary(uint8_t op)
{
    uint16_t sum = A + op + (getC() ? 1 : 0);
    
    setC(sum > 255);
    setV(!((A ^ op) & 0x80) && ((A ^ sum) & 0x80));
    loadA((uint8_t)sum);
}

void
CPU::adc_bcd(uint8_t op)
{
    uint16_t sum       = A + op + (getC() ? 1 : 0);
    uint8_t  highDigit = (A >> 4) + (op >> 4);
    uint8_t  lowDigit  = (A & 0x0F) + (op & 0x0F) + (getC() ? 1 : 0);
    
    // Check for overflow conditions
    // If an overflow occurs on a BCD digit, it needs to be fixed by adding the pseudo-tetrade 0110 (=6)
    if (lowDigit > 9) {
        lowDigit = lowDigit + 6;
    }
    if (lowDigit > 0x0F) {
        highDigit++;
    }
    
    setZ((sum & 0xFF) == 0);
    setN(highDigit & 0x08);
    setV((((highDigit << 4) ^ A) & 0x80) && !((A ^ op) & 0x80));
    
    if (highDigit > 9) {
        highDigit = (highDigit + 6);
    }
    if (highDigit > 0x0F) {
        setC(1);
    } else {
        setC(0);
    }
    
    lowDigit &= 0x0F;
    A = (uint8_t)((highDigit << 4) | lowDigit);
}

void
CPU::cmp(uint8_t op1, uint8_t op2)
{
    uint8_t tmp = op1 - op2;
    
    setC(op1 >= op2);
    setN(tmp & 128);
    setZ(tmp == 0);
}

void
CPU::sbc(uint8_t op)
{
    if (getD())
        sbc_bcd(op);
    else
        sbc_binary(op);
}

void
CPU::sbc_binary(uint8_t op)
{
    uint16_t sum = A - op - (getC() ? 0 : 1);
    
    setC(sum <= 255);
    setV(((A ^ sum) & 0x80) && ((A ^ op) & 0x80));
    loadA((uint8_t)sum);
}

void
CPU::sbc_bcd(uint8_t op)
{
    uint16_t sum       = A - op - (getC() ? 0 : 1);
    uint8_t  highDigit = (A >> 4) - (op >> 4);
    uint8_t  lowDigit  = (A & 0x0F) - (op & 0x0F) - (getC() ? 0 : 1);
    
    // Check for underflow conditions
    // If an overflow occurs on a BCD digit, it needs to be fixed by subtracting the pseudo-tetrade 0110 (=6)
    if (lowDigit & 0x10) {
        lowDigit = lowDigit - 6;
        highDigit--;
    }
    if (highDigit & 0x10) {
        highDigit = highDigit - 6;
    }
    
    setC(sum < 0x100);
    setV(((A ^ sum) & 0x80) && ((A ^ op) & 0x80));
    setZ((sum & 0xFF) == 0);
    setN(sum & 0x80);
    
    A = (uint8_t)((highDigit << 4) | (lowDigit & 0x0f));
}

void 
CPU::registerCallback(uint8_t opcode, MicroInstruction mInstr)
{
	registerCallback(opcode, "???", ADDR_IMPLIED, mInstr);
}

void 
CPU::registerCallback(uint8_t opcode, const char *mnc,
                      AddressingMode mode, MicroInstruction mInstr)
{
	// table is write once!
	if (mInstr != JAM)
		assert(actionFunc[opcode] == JAM);
	
    mnemonic[opcode] = mnc;
    addressingMode[opcode] = mode;
	actionFunc[opcode] = mInstr;
}

void 
CPU::registerIllegalInstructions()
{
	registerCallback(0x93, "SHA*", ADDR_INDIRECT_Y, SHA_ind_y);
	registerCallback(0x9F, "SHA*", ADDR_ABSOLUTE_Y, SHA_abs_y);

	registerCallback(0x4B, "ALR*", ADDR_IMMEDIATE, ALR_imm);

	registerCallback(0x0B, "ANC*", ADDR_IMMEDIATE, ANC_imm);
	registerCallback(0x2B, "ANC*", ADDR_IMMEDIATE, ANC_imm);
	
	registerCallback(0x8B, "ANE*", ADDR_IMMEDIATE, ANE_imm);

	registerCallback(0x6B, "ARR*", ADDR_IMMEDIATE, ARR_imm);
	registerCallback(0xCB, "AXS*", ADDR_IMMEDIATE, AXS_imm);

	registerCallback(0xC7, "DCP*", ADDR_ZERO_PAGE, DCP_zpg);
	registerCallback(0xD7, "DCP*", ADDR_ZERO_PAGE_X, DCP_zpg_x);
	registerCallback(0xC3, "DCP*", ADDR_INDIRECT_X, DCP_ind_x);
	registerCallback(0xD3, "DCP*", ADDR_INDIRECT_Y, DCP_ind_y);
	registerCallback(0xCF, "DCP*", ADDR_ABSOLUTE, DCP_abs);
	registerCallback(0xDF, "DCP*", ADDR_ABSOLUTE_X, DCP_abs_x);
	registerCallback(0xDB, "DCP*", ADDR_ABSOLUTE_Y, DCP_abs_y);

	registerCallback(0xE7, "ISC*", ADDR_ZERO_PAGE, ISC_zpg);
	registerCallback(0xF7, "ISC*", ADDR_ZERO_PAGE_X, ISC_zpg_x);
	registerCallback(0xE3, "ISC*", ADDR_INDIRECT_X, ISC_ind_x);
	registerCallback(0xF3, "ISC*", ADDR_INDIRECT_Y, ISC_ind_y);
	registerCallback(0xEF, "ISC*", ADDR_ABSOLUTE, ISC_abs);
	registerCallback(0xFF, "ISC*", ADDR_ABSOLUTE_X, ISC_abs_x);
	registerCallback(0xFB, "ISC*", ADDR_ABSOLUTE_Y, ISC_abs_y);

	registerCallback(0xBB, "LAS*", ADDR_ABSOLUTE_Y, LAS_abs_y);

	registerCallback(0xA7, "LAX*", ADDR_ZERO_PAGE, LAX_zpg);
	registerCallback(0xB7, "LAX*", ADDR_ZERO_PAGE_Y, LAX_zpg_y);
	registerCallback(0xA3, "LAX*", ADDR_INDIRECT_X, LAX_ind_x);
	registerCallback(0xB3, "LAX*", ADDR_INDIRECT_Y, LAX_ind_y);
	registerCallback(0xAF, "LAX*", ADDR_ABSOLUTE, LAX_abs);
	registerCallback(0xBF, "LAX*", ADDR_ABSOLUTE_Y, LAX_abs_y);

	registerCallback(0xAB, "LXA*", ADDR_IMMEDIATE, LXA_imm);

	registerCallback(0x80, "NOP*", ADDR_IMMEDIATE, NOP_imm);
	registerCallback(0x82, "NOP*", ADDR_IMMEDIATE, NOP_imm);
	registerCallback(0x89, "NOP*", ADDR_IMMEDIATE, NOP_imm);
	registerCallback(0xC2, "NOP*", ADDR_IMMEDIATE, NOP_imm);
	registerCallback(0xE2, "NOP*", ADDR_IMMEDIATE, NOP_imm);
	registerCallback(0x1A, "NOP*", ADDR_IMPLIED, NOP);
	registerCallback(0x3A, "NOP*", ADDR_IMPLIED, NOP);
	registerCallback(0x5A, "NOP*", ADDR_IMPLIED, NOP);
	registerCallback(0x7A, "NOP*", ADDR_IMPLIED, NOP);
	registerCallback(0xDA, "NOP*", ADDR_IMPLIED, NOP);
	registerCallback(0xFA, "NOP*", ADDR_IMPLIED, NOP);
	registerCallback(0x04, "NOP*", ADDR_ZERO_PAGE, NOP_zpg);
	registerCallback(0x44, "NOP*", ADDR_ZERO_PAGE, NOP_zpg);
	registerCallback(0x64, "NOP*", ADDR_ZERO_PAGE, NOP_zpg);
	registerCallback(0x0C, "NOP*", ADDR_ABSOLUTE, NOP_abs);
	registerCallback(0x14, "NOP*", ADDR_ZERO_PAGE_X, NOP_zpg_x);
	registerCallback(0x34, "NOP*", ADDR_ZERO_PAGE_X, NOP_zpg_x);
	registerCallback(0x54, "NOP*", ADDR_ZERO_PAGE_X, NOP_zpg_x);
	registerCallback(0x74, "NOP*", ADDR_ZERO_PAGE_X, NOP_zpg_x);
	registerCallback(0xD4, "NOP*", ADDR_ZERO_PAGE_X, NOP_zpg_x);
	registerCallback(0xF4, "NOP*", ADDR_ZERO_PAGE_X, NOP_zpg_x);
	registerCallback(0x1C, "NOP*", ADDR_ABSOLUTE_X, NOP_abs_x);
	registerCallback(0x3C, "NOP*", ADDR_ABSOLUTE_X, NOP_abs_x);
	registerCallback(0x5C, "NOP*", ADDR_ABSOLUTE_X, NOP_abs_x);
	registerCallback(0x7C, "NOP*", ADDR_ABSOLUTE_X, NOP_abs_x);
	registerCallback(0xDC, "NOP*", ADDR_ABSOLUTE_X, NOP_abs_x);
	registerCallback(0xFC, "NOP*", ADDR_ABSOLUTE_X, NOP_abs_x);

	registerCallback(0x27, "RLA*", ADDR_ZERO_PAGE, RLA_zpg);
	registerCallback(0x37, "RLA*", ADDR_ZERO_PAGE_X, RLA_zpg_x);
	registerCallback(0x23, "RLA*", ADDR_INDIRECT_X, RLA_ind_x);
	registerCallback(0x33, "RLA*", ADDR_INDIRECT_Y, RLA_ind_y);
	registerCallback(0x2F, "RLA*", ADDR_ABSOLUTE, RLA_abs);
	registerCallback(0x3F, "RLA*", ADDR_ABSOLUTE_X, RLA_abs_x);
	registerCallback(0x3B, "RLA*", ADDR_ABSOLUTE_Y, RLA_abs_y);

	registerCallback(0x67, "RRA*", ADDR_ZERO_PAGE, RRA_zpg);
	registerCallback(0x77, "RRA*", ADDR_ZERO_PAGE_X, RRA_zpg_x);
	registerCallback(0x63, "RRA*", ADDR_INDIRECT_X, RRA_ind_x);
	registerCallback(0x73, "RRA*", ADDR_INDIRECT_Y, RRA_ind_y);
	registerCallback(0x6F, "RRA*", ADDR_ABSOLUTE, RRA_abs);
	registerCallback(0x7F, "RRA*", ADDR_ABSOLUTE_X, RRA_abs_x);
	registerCallback(0x7B, "RRA*", ADDR_ABSOLUTE_Y, RRA_abs_y);

	registerCallback(0x87, "SAX*", ADDR_ZERO_PAGE, SAX_zpg);
	registerCallback(0x97, "SAX*", ADDR_ZERO_PAGE_Y, SAX_zpg_y);
	registerCallback(0x83, "SAX*", ADDR_INDIRECT_X, SAX_ind_x);
	registerCallback(0x8F, "SAX*", ADDR_ABSOLUTE, SAX_abs);

	registerCallback(0xEB, "SBC*", ADDR_IMMEDIATE, SBC_imm);

	registerCallback(0x9E, "SHX*", ADDR_ABSOLUTE_Y, SHX_abs_y);
	registerCallback(0x9C, "SHY*", ADDR_ABSOLUTE_X, SHY_abs_x);

	registerCallback(0x07, "SLO*", ADDR_ZERO_PAGE, SLO_zpg);
	registerCallback(0x17, "SLO*", ADDR_ZERO_PAGE_X, SLO_zpg_x);
	registerCallback(0x03, "SLO*", ADDR_INDIRECT_X, SLO_ind_x);
	registerCallback(0x13, "SLO*", ADDR_INDIRECT_Y, SLO_ind_y);
	registerCallback(0x0F, "SLO*", ADDR_ABSOLUTE, SLO_abs);
	registerCallback(0x1F, "SLO*", ADDR_ABSOLUTE_X, SLO_abs_x);
	registerCallback(0x1B, "SLO*", ADDR_ABSOLUTE_Y, SLO_abs_y);

	registerCallback(0x47, "SRE*", ADDR_ZERO_PAGE, SRE_zpg);
	registerCallback(0x57, "SRE*", ADDR_ZERO_PAGE_X, SRE_zpg_x);
	registerCallback(0x43, "SRE*", ADDR_INDIRECT_X, SRE_ind_x);
	registerCallback(0x53, "SRE*", ADDR_INDIRECT_Y, SRE_ind_y);
	registerCallback(0x4F, "SRE*", ADDR_ABSOLUTE, SRE_abs);
	registerCallback(0x5F, "SRE*", ADDR_ABSOLUTE_X, SRE_abs_x);
	registerCallback(0x5B, "SRE*", ADDR_ABSOLUTE_Y, SRE_abs_y);
	
	registerCallback(0x9B, "TAS*", ADDR_ABSOLUTE_Y, TAS_abs_y);
}

	
void CPU::registerInstructions()
{
	for (int i=0; i<256; i++)
		registerCallback(i, JAM);

	registerCallback(0x69, "ADC", ADDR_IMMEDIATE, ADC_imm);
	registerCallback(0x65, "ADC", ADDR_ZERO_PAGE, ADC_zpg);
	registerCallback(0x75, "ADC", ADDR_ZERO_PAGE_X, ADC_zpg_x);
	registerCallback(0x6D, "ADC", ADDR_ABSOLUTE, ADC_abs);
	registerCallback(0x7D, "ADC", ADDR_ABSOLUTE_X, ADC_abs_x);
	registerCallback(0x79, "ADC", ADDR_ABSOLUTE_Y, ADC_abs_y);
	registerCallback(0x61, "ADC", ADDR_INDIRECT_X, ADC_ind_x);
	registerCallback(0x71, "ADC", ADDR_INDIRECT_Y, ADC_ind_y);

	registerCallback(0x29, "AND", ADDR_IMMEDIATE, AND_imm);
	registerCallback(0x25, "AND", ADDR_ZERO_PAGE, AND_zpg);
	registerCallback(0x35, "AND", ADDR_ZERO_PAGE_X, AND_zpg_x);
	registerCallback(0x2D, "AND", ADDR_ABSOLUTE, AND_abs);
	registerCallback(0x3D, "AND", ADDR_ABSOLUTE_X, AND_abs_x);
	registerCallback(0x39, "AND", ADDR_ABSOLUTE_Y, AND_abs_y);
	registerCallback(0x21, "AND", ADDR_INDIRECT_X, AND_ind_x);
	registerCallback(0x31, "AND", ADDR_INDIRECT_Y, AND_ind_y);
	
	registerCallback(0x0A, "ASL", ADDR_ACCUMULATOR, ASL_acc);
	registerCallback(0x06, "ASL", ADDR_ZERO_PAGE, ASL_zpg);
	registerCallback(0x16, "ASL", ADDR_ZERO_PAGE_X, ASL_zpg_x);
	registerCallback(0x0E, "ASL", ADDR_ABSOLUTE, ASL_abs);
	registerCallback(0x1E, "ASL", ADDR_ABSOLUTE_X, ASL_abs_x);
	
	registerCallback(0x90, "BCC", ADDR_RELATIVE, BCC_rel);
	registerCallback(0xB0, "BCS", ADDR_RELATIVE, BCS_rel);
	registerCallback(0xF0, "BEQ", ADDR_RELATIVE, BEQ_rel);

	registerCallback(0x24, "BIT", ADDR_ZERO_PAGE, BIT_zpg);
	registerCallback(0x2C, "BIT", ADDR_ABSOLUTE, BIT_abs);
	
	registerCallback(0x30, "BMI", ADDR_RELATIVE, BMI_rel);
	registerCallback(0xD0, "BNE", ADDR_RELATIVE, BNE_rel);
	registerCallback(0x10, "BPL", ADDR_RELATIVE, BPL_rel);
	registerCallback(0x00, "BRK", ADDR_IMPLIED, BRK);
	registerCallback(0x50, "BVC", ADDR_RELATIVE, BVC_rel);
	registerCallback(0x70, "BVS", ADDR_RELATIVE, BVS_rel);

	registerCallback(0x18, "CLC", ADDR_IMPLIED, CLC);
	registerCallback(0xD8, "CLD", ADDR_IMPLIED, CLD);
	registerCallback(0x58, "CLI", ADDR_IMPLIED, CLI);
	registerCallback(0xB8, "CLV", ADDR_IMPLIED, CLV);

	registerCallback(0xC9, "CMP", ADDR_IMMEDIATE, CMP_imm);
	registerCallback(0xC5, "CMP", ADDR_ZERO_PAGE, CMP_zpg);
	registerCallback(0xD5, "CMP", ADDR_ZERO_PAGE_X, CMP_zpg_x);
	registerCallback(0xCD, "CMP", ADDR_ABSOLUTE, CMP_abs);
	registerCallback(0xDD, "CMP", ADDR_ABSOLUTE_X, CMP_abs_x);
	registerCallback(0xD9, "CMP", ADDR_ABSOLUTE_Y, CMP_abs_y);
	registerCallback(0xC1, "CMP", ADDR_INDIRECT_X, CMP_ind_x);
	registerCallback(0xD1, "CMP", ADDR_INDIRECT_Y, CMP_ind_y);

	registerCallback(0xE0, "CPX", ADDR_IMMEDIATE, CPX_imm);
	registerCallback(0xE4, "CPX", ADDR_ZERO_PAGE, CPX_zpg);
	registerCallback(0xEC, "CPX", ADDR_ABSOLUTE, CPX_abs);

	registerCallback(0xC0, "CPY", ADDR_IMMEDIATE, CPY_imm);
	registerCallback(0xC4, "CPY", ADDR_ZERO_PAGE, CPY_zpg);
	registerCallback(0xCC, "CPY", ADDR_ABSOLUTE, CPY_abs);

	registerCallback(0xC6, "DEC", ADDR_ZERO_PAGE, DEC_zpg);
	registerCallback(0xD6, "DEC", ADDR_ZERO_PAGE_X, DEC_zpg_x);
	registerCallback(0xCE, "DEC", ADDR_ABSOLUTE, DEC_abs);
	registerCallback(0xDE, "DEC", ADDR_ABSOLUTE_X, DEC_abs_x);

	registerCallback(0xCA, "DEX", ADDR_IMPLIED, DEX);
	registerCallback(0x88, "DEY", ADDR_IMPLIED, DEY);
	
	registerCallback(0x49, "EOR", ADDR_IMMEDIATE, EOR_imm);
	registerCallback(0x45, "EOR", ADDR_ZERO_PAGE, EOR_zpg);
	registerCallback(0x55, "EOR", ADDR_ZERO_PAGE_X, EOR_zpg_x);
	registerCallback(0x4D, "EOR", ADDR_ABSOLUTE, EOR_abs);
	registerCallback(0x5D, "EOR", ADDR_ABSOLUTE_X, EOR_abs_x);
	registerCallback(0x59, "EOR", ADDR_ABSOLUTE_Y, EOR_abs_y);
	registerCallback(0x41, "EOR", ADDR_INDIRECT_X, EOR_ind_x);
	registerCallback(0x51, "EOR", ADDR_INDIRECT_Y, EOR_ind_y);

	registerCallback(0xE6, "INC", ADDR_ZERO_PAGE, INC_zpg);
	registerCallback(0xF6, "INC", ADDR_ZERO_PAGE_X, INC_zpg_x);
	registerCallback(0xEE, "INC", ADDR_ABSOLUTE, INC_abs);
	registerCallback(0xFE, "INC", ADDR_ABSOLUTE_X, INC_abs_x);
	
	registerCallback(0xE8, "INX", ADDR_IMPLIED, INX);
	registerCallback(0xC8, "INY", ADDR_IMPLIED, INY);

	registerCallback(0x4C, "JMP", ADDR_DIRECT, JMP_abs);
	registerCallback(0x6C, "JMP", ADDR_INDIRECT, JMP_abs_indirect);

	registerCallback(0x20, "JSR", ADDR_DIRECT, JSR);

	registerCallback(0xA9, "LDA", ADDR_IMMEDIATE, LDA_imm);
	registerCallback(0xA5, "LDA", ADDR_ZERO_PAGE, LDA_zpg);
	registerCallback(0xB5, "LDA", ADDR_ZERO_PAGE_X, LDA_zpg_x);
	registerCallback(0xAD, "LDA", ADDR_ABSOLUTE, LDA_abs);
	registerCallback(0xBD, "LDA", ADDR_ABSOLUTE_X, LDA_abs_x);
	registerCallback(0xB9, "LDA", ADDR_ABSOLUTE_Y, LDA_abs_y);
	registerCallback(0xA1, "LDA", ADDR_INDIRECT_X, LDA_ind_x);
	registerCallback(0xB1, "LDA", ADDR_INDIRECT_Y, LDA_ind_y);

	registerCallback(0xA2, "LDX", ADDR_IMMEDIATE, LDX_imm);
	registerCallback(0xA6, "LDX", ADDR_ZERO_PAGE, LDX_zpg);
	registerCallback(0xB6, "LDX", ADDR_ZERO_PAGE_Y,LDX_zpg_y);
	registerCallback(0xAE, "LDX", ADDR_ABSOLUTE, LDX_abs);
	registerCallback(0xBE, "LDX", ADDR_ABSOLUTE_Y, LDX_abs_y);

	registerCallback(0xA0, "LDY", ADDR_IMMEDIATE, LDY_imm);
	registerCallback(0xA4, "LDY", ADDR_ZERO_PAGE, LDY_zpg);
	registerCallback(0xB4, "LDY", ADDR_ZERO_PAGE_X, LDY_zpg_x);
	registerCallback(0xAC, "LDY", ADDR_ABSOLUTE, LDY_abs);
	registerCallback(0xBC, "LDY", ADDR_ABSOLUTE_X, LDY_abs_x);
	
	registerCallback(0x4A, "LSR", ADDR_ACCUMULATOR, LSR_acc);
	registerCallback(0x46, "LSR", ADDR_ZERO_PAGE, LSR_zpg);
	registerCallback(0x56, "LSR", ADDR_ZERO_PAGE_X, LSR_zpg_x);
	registerCallback(0x4E, "LSR", ADDR_ABSOLUTE, LSR_abs);
	registerCallback(0x5E, "LSR", ADDR_ABSOLUTE_X, LSR_abs_x);

	registerCallback(0xEA, "NOP", ADDR_IMPLIED, NOP);
	
	registerCallback(0x09, "ORA", ADDR_IMMEDIATE, ORA_imm);
	registerCallback(0x05, "ORA", ADDR_ZERO_PAGE, ORA_zpg);
	registerCallback(0x15, "ORA", ADDR_ZERO_PAGE_X, ORA_zpg_x);
	registerCallback(0x0D, "ORA", ADDR_ABSOLUTE, ORA_abs);
	registerCallback(0x1D, "ORA", ADDR_ABSOLUTE_X, ORA_abs_x);
	registerCallback(0x19, "ORA", ADDR_ABSOLUTE_Y, ORA_abs_y);
	registerCallback(0x01, "ORA", ADDR_INDIRECT_X, ORA_ind_x);
	registerCallback(0x11, "ORA", ADDR_INDIRECT_Y, ORA_ind_y);

	registerCallback(0x48, "PHA", ADDR_IMPLIED, PHA);
	registerCallback(0x08, "PHP", ADDR_IMPLIED, PHP);
	registerCallback(0x68, "PLA", ADDR_IMPLIED, PLA);
	registerCallback(0x28, "PLP", ADDR_IMPLIED, PLP);

	registerCallback(0x2A, "ROL", ADDR_ACCUMULATOR, ROL_acc);
	registerCallback(0x26, "ROL", ADDR_ZERO_PAGE, ROL_zpg);
	registerCallback(0x36, "ROL", ADDR_ZERO_PAGE_X, ROL_zpg_x);
	registerCallback(0x2E, "ROL", ADDR_ABSOLUTE, ROL_abs);
	registerCallback(0x3E, "ROL", ADDR_ABSOLUTE_X, ROL_abs_x);

	registerCallback(0x6A, "ROR", ADDR_ACCUMULATOR, ROR_acc);
	registerCallback(0x66, "ROR", ADDR_ZERO_PAGE, ROR_zpg);
	registerCallback(0x76, "ROR", ADDR_ZERO_PAGE_X, ROR_zpg_x);
	registerCallback(0x6E, "ROR", ADDR_ABSOLUTE, ROR_abs);
	registerCallback(0x7E, "ROR", ADDR_ABSOLUTE_X, ROR_abs_x);
	
	registerCallback(0x40, "RTI", ADDR_IMPLIED, RTI);
	registerCallback(0x60, "RTS", ADDR_IMPLIED, RTS);

	registerCallback(0xE9, "SBC", ADDR_IMMEDIATE, SBC_imm);
	registerCallback(0xE5, "SBC", ADDR_ZERO_PAGE, SBC_zpg);
	registerCallback(0xF5, "SBC", ADDR_ZERO_PAGE_X, SBC_zpg_x);
	registerCallback(0xED, "SBC", ADDR_ABSOLUTE, SBC_abs);
	registerCallback(0xFD, "SBC", ADDR_ABSOLUTE_X, SBC_abs_x);
	registerCallback(0xF9, "SBC", ADDR_ABSOLUTE_Y, SBC_abs_y);
	registerCallback(0xE1, "SBC", ADDR_INDIRECT_X, SBC_ind_x);
	registerCallback(0xF1, "SBC", ADDR_INDIRECT_Y, SBC_ind_y);

	registerCallback(0x38, "SEC", ADDR_IMPLIED, SEC);
	registerCallback(0xF8, "SED", ADDR_IMPLIED, SED);
	registerCallback(0x78, "SEI", ADDR_IMPLIED, SEI);

	registerCallback(0x85, "STA", ADDR_ZERO_PAGE, STA_zpg);
	registerCallback(0x95, "STA", ADDR_ZERO_PAGE_X, STA_zpg_x);
	registerCallback(0x8D, "STA", ADDR_ABSOLUTE, STA_abs);
	registerCallback(0x9D, "STA", ADDR_ABSOLUTE_X, STA_abs_x);
	registerCallback(0x99, "STA", ADDR_ABSOLUTE_Y, STA_abs_y);
	registerCallback(0x81, "STA", ADDR_INDIRECT_X, STA_ind_x);
	registerCallback(0x91, "STA", ADDR_INDIRECT_Y, STA_ind_y);

	registerCallback(0x86, "STX", ADDR_ZERO_PAGE, STX_zpg);
	registerCallback(0x96, "STX", ADDR_ZERO_PAGE_Y, STX_zpg_y);
	registerCallback(0x8E, "STX", ADDR_ABSOLUTE, STX_abs);

	registerCallback(0x84, "STY", ADDR_ZERO_PAGE, STY_zpg);
	registerCallback(0x94, "STY", ADDR_ZERO_PAGE_X, STY_zpg_x);
	registerCallback(0x8C, "STY", ADDR_ABSOLUTE, STY_abs);

	registerCallback(0xAA, "TAX", ADDR_IMPLIED, TAX);
	registerCallback(0xA8, "TAY", ADDR_IMPLIED, TAY);
	registerCallback(0xBA, "TSX", ADDR_IMPLIED, TSX);
	registerCallback(0x8A, "TXA", ADDR_IMPLIED, TXA);
	registerCallback(0x9A, "TXS", ADDR_IMPLIED, TXS);
	registerCallback(0x98, "TYA", ADDR_IMPLIED, TYA);

	// Register illegal instructions
	registerIllegalInstructions();	
}

void
CPU::executeMicroInstruction()
{
    switch (next) {
            
        case fetch:
            
            /* DEBUG
             if (PC == 0x0861) {
                 startTracing(100);
             }
            */
            
            PC_at_cycle_0 = PC;
            
            // Check interrupt lines
            if (doNmi) {
                
                if (tracingEnabled()) trace("NMI (source = %02X)\n", nmiLine);
                clear8_delayed(edgeDetector);
                next = nmi_2;
                doNmi = false;
                doIrq = false; // NMI wins
                return;
                
            } else if (doIrq) {
                
                if (tracingEnabled()) trace("IRQ (source = %02X)\n", irqLine);
                next = irq_2;
                doIrq = false;
                return;
            }
            
            // Execute fetch phase
            FETCH_OPCODE
            next = actionFunc[opcode];
            
            // Disassemble command if requested
            if (tracingEnabled()) {
                DisassembledInstruction instr = disassemble(true /* hex output */);
                trace("%s: %s %s %s   %s %s %s %s %s %s\n",
                        instr.pc,
                        instr.byte[0], instr.byte[1], instr.byte[2],
                        instr.A, instr.X, instr.Y, instr.SP,
                        instr.flags,
                        instr.command);
            }
            
            // Check breakpoint tag
            if (breakpoint[PC_at_cycle_0] != NO_BREAKPOINT) {
                if (breakpoint[PC_at_cycle_0] & SOFT_BREAKPOINT) {
                    breakpoint[PC_at_cycle_0] &= ~SOFT_BREAKPOINT; // Soft breakpoints get deleted when reached
                    setErrorState(CPU_SOFT_BREAKPOINT_REACHED);
                } else {
                    setErrorState(CPU_HARD_BREAKPOINT_REACHED);
                }
                debug(1, "Breakpoint reached\n");
            }
            return;
            
        // -------------------------------------------------------------------------------
        // Illegal instructions
        // -------------------------------------------------------------------------------
            
        case JAM:
            
            setErrorState(CPU_ILLEGAL_INSTRUCTION);
            CONTINUE

        case JAM_2:
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // IRQ handling
        // -------------------------------------------------------------------------------

        case irq:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case irq_2:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case irq_3:
            
            mem->poke(0x100+(SP--), HI_BYTE(PC));
            CONTINUE
            
        case irq_4:
            
            mem->poke(0x100+(SP--), LO_BYTE(PC));
            
            // Check for interrupt hijacking
            
            // If there is a positive edge on the NMI line ...
            if (edgeDetector.value) {
                
                // ... the processor will jump to the NMI vector instead of the IRQ vector
                clear8_delayed(edgeDetector);
                next = nmi_5;
                return;
            }
            CONTINUE
            
        case irq_5:
            
            mem->poke(0x100+(SP--), getPWithClearedB());
            setI(1);
            CONTINUE
            
        case irq_6:
            
            data = mem->peek(0xFFFE);
            CONTINUE
            
        case irq_7:
            
            setPCL(data);
            setPCH(mem->peek(0xFFFF));
            DONE
            
        // -------------------------------------------------------------------------------
        // NMI handling
        // -------------------------------------------------------------------------------
            
        case nmi:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case nmi_2:

            IDLE_READ_IMPLIED
            CONTINUE
            
        case nmi_3:
            
            mem->poke(0x100+(SP--), HI_BYTE(PC));
            CONTINUE
            
        case nmi_4:
            
            mem->poke(0x100+(SP--), LO_BYTE(PC));
            CONTINUE
            
        case nmi_5:
            
            mem->poke(0x100+(SP--), getPWithClearedB());
            setI(1);
            CONTINUE
            
        case nmi_6:
            
            data = mem->peek(0xFFFA);
            CONTINUE
            
        case nmi_7:
            
            setPCL(data);
            setPCH(mem->peek(0xFFFB));
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ADC
        //
        // Operation:   A,C := A+M+C
        //
        // Flags:       N Z C I D V
        //              / / / - - /
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case ADC_imm:

            READ_IMMEDIATE
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ADC_zpg_2:
            
            READ_FROM_ZERO_PAGE
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ADC_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case ADC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ADC_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case ADC_abs_3:
            
            READ_FROM_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ADC_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ADC_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                adc(data);
                POLL_INT
                DONE
            }
            
        case ADC_abs_x_4:
            
            READ_FROM_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ADC_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case ADC_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                adc(data);
                POLL_INT
                DONE
            }
            
        case ADC_abs_y_4:
            
            READ_FROM_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ADC_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ADC_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ADC_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ADC_ind_x_5:
            
            READ_FROM_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ADC_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ADC_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ADC_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y;
            CONTINUE
            
        case ADC_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                adc(data);
                POLL_INT
                DONE
            }
            
        case ADC_ind_y_5:
            
            READ_FROM_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: AND
        //
        // Operation:   A := A AND M
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case AND_imm:
            
            READ_IMMEDIATE
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case AND_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case AND_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case AND_abs_3:
            READ_FROM_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case AND_zpg:

            FETCH_ADDR_LO
            CONTINUE

        case AND_zpg_2:

            READ_FROM_ZERO_PAGE
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case AND_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case AND_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case AND_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case AND_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case AND_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case AND_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A & data);
                POLL_INT
                DONE
            }
            
        case AND_abs_x_4:
            
            READ_FROM_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case AND_abs_y:
            
            FETCH_ADDR_LO;
            CONTINUE
            
        case AND_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case AND_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A & data);
                POLL_INT
                DONE
            }
            
        case AND_abs_y_4:
        
            READ_FROM_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE
        
        // -------------------------------------------------------------------------------
        case AND_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case AND_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case AND_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case AND_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case AND_ind_x_5:
            
            READ_FROM_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        case AND_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case AND_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case AND_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case AND_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A & data);
                POLL_INT
                DONE
            }
            
        case AND_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        // Instruction: ASL
        //
        // Operation:   C <- (A|M << 1) <- 0
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------
    
        #define DO_ASL setC(data & 128); data = data << 1;

        // -------------------------------------------------------------------------------
        case ASL_acc:
            
            IDLE_READ_IMPLIED
            setC(A & 128); loadA(A << 1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ASL_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ASL_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ASL_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_ASL
            CONTINUE
            
        case ASL_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ASL_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ASL_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case ASL_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ASL_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ASL
            CONTINUE
            
        case ASL_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        case ASL_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ASL_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case ASL_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ASL_abs_4:
            
            WRITE_TO_ADDRESS
            DO_ASL
            CONTINUE
            
        case ASL_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ASL_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ASL_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ASL_abs_x_3:
            
            READ_FROM_ADDRESS;
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case ASL_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ASL_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_ASL
            CONTINUE
            
        case ASL_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ASL_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ASL_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ASL_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ASL_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ASL_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ASL_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ASL
            CONTINUE
            
        case ASL_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        // Instruction: BCC
        //
        // Operation:   Branch on C = 0
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        // void CPU::branch(int8_t offset) { PC += offset; }
            
        case branch_3_underflow:
            
            IDLE_READ_FROM(PC + 0x100)
            POLL_INT_AGAIN
            DONE
            
        case branch_3_overflow:
            
            IDLE_READ_FROM(PC - 0x100)
            POLL_INT_AGAIN
            DONE

        // ------------------------------------------------------------------------------
        case BCC_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (!getC()) {
                CONTINUE
            } else {
                DONE
            }
            
        case BCC_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }
            
        // -------------------------------------------------------------------------------
        // Instruction: BCS
        //
        // Operation:   Branch on C = 1
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case BCS_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getC()) {
                CONTINUE
            } else {
                DONE
            }
            
        case BCS_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }
            
        // -------------------------------------------------------------------------------
        // Instruction: BEQ
        //
        // Operation:   Branch on Z = 1
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------
            
        case BEQ_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getZ()) {
                CONTINUE
            } else {
                DONE
            }
            
        case BEQ_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }
            
        // -------------------------------------------------------------------------------
        // Instruction: BIT
        //
        // Operation:   A AND M, N := M7, V := M6
        //
        // Flags:       N Z C I D V
        //              / / - - - /
        // -------------------------------------------------------------------------------

        case BIT_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case BIT_zpg_2:
            
            READ_FROM_ZERO_PAGE
            setN(data & 128);
            setV(data & 64);
            setZ((data & A) == 0);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case BIT_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case BIT_abs_2:
            
            FETCH_ADDR_HI;
            CONTINUE
            
        case BIT_abs_3:
            
            READ_FROM_ADDRESS
            setN(data & 128);
            setV(data & 64);
            setZ((data & A) == 0);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: BMI
        //
        // Operation:   Branch on N = 1
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case BMI_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getN()) {
                CONTINUE
            } else {
                DONE
            }
            
        case BMI_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }
            
        // -------------------------------------------------------------------------------
        // Instruction: BNE
        //
        // Operation:   Branch on Z = 0
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------
            
        case BNE_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (!getZ()) {
                CONTINUE
            } else {
                DONE
            }
            
        case BNE_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }

        // -------------------------------------------------------------------------------
        // Instruction: BPL
        //
        // Operation:   Branch on N = 0
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case BPL_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (!getN()) {
                CONTINUE
            } else {
                DONE
            }
            
        case BPL_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }

        // -------------------------------------------------------------------------------
        // Instruction: BRK
        //
        // Operation:   Forced Interrupt (Break)
        //
        // Flags:       N Z C I D V    B
        //              - - - 1 - -    1
        // -------------------------------------------------------------------------------
            
        case BRK:
            
            IDLE_READ_IMMEDIATE
            CONTINUE
            
        case BRK_2:
            
            setB(1);
            PUSH_PCH
            CONTINUE
            
        case BRK_3:
        
            PUSH_PCL
            
            // Check for interrupt hijacking
            
            // If there is a positive edge on the NMI line ...
            if (edgeDetector.value) {
            
                // ... the processor will jump to the NMI vector instead of the IRQ vector
                clear8_delayed(edgeDetector);
                next = BRK_nmi_4;
                return;
                
            } else {
                CONTINUE
            }
            
        case BRK_4:
            
            PUSH_P
            CONTINUE
            
        case BRK_5:
            
            data = mem->peek(0xFFFE);
            CONTINUE
            
        case BRK_6:
            
            setPCL(data);
            setPCH(mem->peek(0xFFFF));
            setI(1);
            
            POLL_INT
            doNmi = false; // Only the level detector is polled here. This is the reason why
                           // only IRQs can be triggered right after a BRK command, but not NMIs.
            DONE
            
        case BRK_nmi_4:
            
            PUSH_P
            CONTINUE
            
        case BRK_nmi_5:
            
            data = mem->peek(0xFFFA);
            CONTINUE
            
        case BRK_nmi_6:
            
            setPCL(data);
            setPCH(mem->peek(0xFFFB));
            setI(1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: BVC
        //
        // Operation:   Branch on V = 0
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case BVC_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (chipModel == MOS_6502 /* Drive CPU */ && !c64->floppy.getBitAccuracy()) {
                
                // Special handling for the VC1541 CPU. Taken from Frodo
                if (!((c64->floppy.via2.io[12] & 0x0E) == 0x0E || getV())) {
                    CONTINUE
                } else {
                    DONE
                }
                
            } else {
                
                // Standard CPU behavior
                if (!getV()) {
                    CONTINUE
                } else {
                    DONE
                }
            }
            
        case BVC_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }

        // -------------------------------------------------------------------------------
        // Instruction: BVS
        //
        // Operation:   Branch on V = 1
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case BVS_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (chipModel == MOS_6502 /* Drive CPU */ && !c64->floppy.getBitAccuracy()) {
                
                // Special handling for the VC1541 CPU. Taken from Frodo
                if ((c64->floppy.via2.io[12] & 0x0E) == 0x0E || getV()) {
                    CONTINUE
                } else {
                    DONE
                }
                
            } else {
                
                // Standard CPU behavior
                if (getV()) {
                    CONTINUE
                } else {
                    DONE
                }
            }
            
        case BVS_rel_2:
        {
            IDLE_READ_IMPLIED
            uint8_t pc_hi = HI_BYTE(PC);
            PC += (int8_t)data;
            
            if (pc_hi != HI_BYTE(PC)) {
                next = (data & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }

        // -------------------------------------------------------------------------------
        // Instruction: CLC
        //
        // Operation:   C := 0
        //
        // Flags:       N Z C I D V
        //              - - 0 - - -
        // -------------------------------------------------------------------------------

        case CLC:
            
            IDLE_READ_IMPLIED
            setC(0);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: CLD
        //
        // Operation:   D := 0
        //
        // Flags:       N Z C I D V
        //              - - - - 0 -
        // -------------------------------------------------------------------------------

        case CLD:
            
            IDLE_READ_IMPLIED
            setD(0);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: CLI
        //
        // Operation:   I := 0
        //
        // Flags:       N Z C I D V
        //              - - - 0 - -
        // -------------------------------------------------------------------------------

        case CLI:
            
            IDLE_READ_IMPLIED
            POLL_INT
            setI(0);
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: CLV
        //
        // Operation:   V := 0
        //
        // Flags:       N Z C I D V
        //              - - - - - 0
        // -------------------------------------------------------------------------------

        case CLV:
            
            IDLE_READ_IMPLIED
            setV(0);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: CMP
        //
        // Operation:   A-M
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case CMP_imm:
            
            READ_IMMEDIATE
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CMP_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case CMP_abs_3:
            
            READ_FROM_ADDRESS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CMP_zpg_2:
            
            READ_FROM_ZERO_PAGE
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CMP_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case CMP_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CMP_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case CMP_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                cmp(A, data);
                POLL_INT
                DONE
            }
            
        case CMP_abs_x_4:
            
            READ_FROM_ADDRESS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CMP_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case CMP_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                cmp(A, data);
                POLL_INT
                DONE
            }
            
        case CMP_abs_y_4:
            
            READ_FROM_ADDRESS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case CMP_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case CMP_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case CMP_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case CMP_ind_x_5:
            
            READ_FROM_ADDRESS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CMP_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case CMP_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case CMP_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case CMP_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                cmp(A, data);
                POLL_INT
                DONE
            }
            
        case CMP_ind_y_5:
            
            READ_FROM_ADDRESS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: CPX
        //
        // Operation:   X-M
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        case CPX_imm:
            
            READ_IMMEDIATE
            cmp(X, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CPX_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CPX_zpg_2:
            
            READ_FROM_ZERO_PAGE
            cmp(X, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CPX_abs:
            FETCH_ADDR_LO
            CONTINUE
            
        case CPX_abs_2:
            FETCH_ADDR_HI
            CONTINUE
            
        case CPX_abs_3:
            READ_FROM_ADDRESS
            cmp(X, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: CPY
        //
        // Operation:   Y-M
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        case CPY_imm:
            
            READ_IMMEDIATE
            cmp(Y, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CPY_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CPY_zpg_2:
            
            READ_FROM_ZERO_PAGE
            cmp(Y, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case CPY_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case CPY_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case CPY_abs_3:
            
            READ_FROM_ADDRESS
            cmp(Y, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: DEC
        //
        // Operation:   M := : M - 1
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        #define DO_DEC data--;

        // -------------------------------------------------------------------------------
        case DEC_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DEC_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case DEC_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_DEC
            CONTINUE
            
        case DEC_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DEC_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DEC_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case DEC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case DEC_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_DEC
            CONTINUE
            
        case DEC_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DEC_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DEC_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case DEC_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DEC_abs_4:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DEC_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DEC_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DEC_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case DEC_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case DEC_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DEC_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DEC_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DEC_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case DEC_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case DEC_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case DEC_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case DEC_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DEC_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DEC_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: DEX
        //
        // Operation:   X := X - 1
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case DEX:
            
            IDLE_READ_IMPLIED
            loadX(getX()-1);
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        // Instruction: DEY
        //
        // Operation:   Y := Y - 1
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case DEY:
            
            IDLE_READ_IMPLIED
            loadY(getY()-1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: EOR
        //
        // Operation:   A := A XOR M
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case EOR_imm:
            
            READ_IMMEDIATE
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case EOR_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case EOR_abs_3:
            
            READ_FROM_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case EOR_zpg_2:
            
            READ_FROM_ZERO_PAGE
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case EOR_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case EOR_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case EOR_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case EOR_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                CONTINUE
            } else {
                loadA(A ^ data);
                POLL_INT
                DONE
            }
            
        case EOR_abs_x_4:
            
            READ_FROM_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case EOR_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case EOR_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A ^ data);
                POLL_INT
                DONE
            }
            
        case EOR_abs_y_4:
            
            READ_FROM_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case EOR_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case EOR_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case EOR_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case EOR_ind_x_5:
            
            READ_FROM_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case EOR_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case EOR_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case EOR_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case EOR_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A ^ data);
                POLL_INT
                DONE
            }
            
        case EOR_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: INC
        //
        // Operation:   M := M + 1
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        #define DO_INC data++;

        // -------------------------------------------------------------------------------
        case INC_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case INC_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case INC_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_INC
            CONTINUE
            
        case INC_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case INC_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case INC_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case INC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case INC_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_INC
            CONTINUE
            
        case INC_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case INC_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case INC_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case INC_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case INC_abs_4:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case INC_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case INC_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case INC_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case INC_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case INC_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case INC_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case INC_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case INC_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case INC_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case INC_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case INC_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case INC_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case INC_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case INC_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: INX
        //
        // Operation:   X := X + 1
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case INX:
            
            IDLE_READ_IMPLIED
            loadX(getX()+1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: INY
        //
        // Operation:   Y := Y + 1
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case INY:
            
            IDLE_READ_IMPLIED
            loadY(getY()+1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: JMP
        //
        // Operation:   PC := Operand
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case JMP_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case JMP_abs_2:
            
            FETCH_ADDR_HI
            setPC(LO_HI(addr_lo, addr_hi));
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case JMP_abs_indirect:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case JMP_abs_ind_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case JMP_abs_ind_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case JMP_abs_ind_4:
            
            setPCL(data);
            setPCH(mem->peek(addr_lo+1, addr_hi));
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: JSR
        //
        // Operation:   PC to stack, PC := Operand
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------
            
        case JSR:
            
            FETCH_ADDR_LO
            callStack[callStackPointer++] = PC;
            CONTINUE
            
        case JSR_2:
            
            CONTINUE
            
        case JSR_3:
            
            PUSH_PCH
            CONTINUE
            
        case JSR_4:
            
            PUSH_PCL
            CONTINUE
            
        case JSR_5:
            
            FETCH_ADDR_HI
            setPC(LO_HI(addr_lo, addr_hi));
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LDA
        //
        // Operation:   A := M
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case LDA_imm:
            
            READ_IMMEDIATE
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDA_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDA_zpg_2:
            
            READ_FROM_ZERO_PAGE
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDA_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDA_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case LDA_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDA_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDA_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case LDA_abs_3:
            
            READ_FROM_ADDRESS
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDA_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDA_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case LDA_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(data);
                POLL_INT
                DONE
            }
            
        case LDA_abs_x_4:
            
            READ_FROM_ADDRESS
            loadA(data);
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        case LDA_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDA_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case LDA_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(data);
                POLL_INT
                DONE
            }
            
        case LDA_abs_y_4:
            
            READ_FROM_ADDRESS
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDA_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LDA_ind_x_2:

            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case LDA_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LDA_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case LDA_ind_x_5:
            
            READ_FROM_ADDRESS
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDA_ind_y:

            FETCH_POINTER_ADDR
            CONTINUE
            
        case LDA_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LDA_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case LDA_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(data);
                POLL_INT
                DONE
            }
            
        case LDA_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LDX
        //
        // Operation:   X := M
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case LDX_imm:
            
            READ_IMMEDIATE
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDX_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDX_zpg_2:
            
            READ_FROM_ZERO_PAGE
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDX_zpg_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDX_zpg_y_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_Y
            CONTINUE
            
        case LDX_zpg_y_3:
            
            READ_FROM_ZERO_PAGE
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDX_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDX_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case LDX_abs_3:
            
            READ_FROM_ADDRESS
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDX_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDX_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case LDX_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadX(data);
                POLL_INT
                DONE
            }
            
        case LDX_abs_y_4:
            
            READ_FROM_ADDRESS
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDX_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LDX_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case LDX_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LDX_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case LDX_ind_x_5:
            
            READ_FROM_ADDRESS
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDX_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LDX_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LDX_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case LDX_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadX(data);
                POLL_INT
                DONE
            }
            
        case LDX_ind_y_5:
            
            READ_FROM_ADDRESS
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LDY
        //
        // Operation:   Y := M
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case LDY_imm:
            
            READ_IMMEDIATE
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDY_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDY_zpg_2:
            
            READ_FROM_ZERO_PAGE
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDY_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDY_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case LDY_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDY_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LDY_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case LDY_abs_3:
            
            READ_FROM_ADDRESS;
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDY_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE

        case LDY_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case LDY_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadY(data);
                POLL_INT
                DONE
            }
            
        case LDY_abs_x_4:
            
            READ_FROM_ADDRESS
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDY_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LDY_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case LDY_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LDY_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case LDY_ind_x_5:
            
            READ_FROM_ADDRESS
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LDY_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LDY_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LDY_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case LDY_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadY(data);
                POLL_INT
                DONE
            }
            
        case LDY_ind_y_5:
            
            READ_FROM_ADDRESS
            loadY(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LSR
        //
        // Operation:   0 -> (A|M >> 1) -> C
        //
        // Flags:       N Z C I D V
        //              0 / / - - -
        // -------------------------------------------------------------------------------

        #define DO_LSR setC(data & 1); data = data >> 1;

        // -------------------------------------------------------------------------------
        case LSR_acc:
            
            IDLE_READ_IMPLIED
            setC(A & 1); loadA(A >> 1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LSR_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LSR_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case LSR_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_LSR
            CONTINUE
            
        case LSR_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        case LSR_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LSR_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case LSR_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case LSR_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_LSR
            CONTINUE
            
        case LSR_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        case LSR_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LSR_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case LSR_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case LSR_abs_4:
            
            WRITE_TO_ADDRESS
            DO_LSR
            CONTINUE
            
        case LSR_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE
            
        // -------------------------------------------------------------------------------
        case LSR_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LSR_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case LSR_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
            }
            CONTINUE
            
        case LSR_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case LSR_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_LSR
            CONTINUE
            
        case LSR_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LSR_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LSR_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case LSR_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
            }
            CONTINUE
            
        case LSR_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case LSR_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_LSR
            CONTINUE
            
        case LSR_abs_y_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

            // -------------------------------------------------------------------------------
        case LSR_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LSR_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case LSR_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LSR_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case LSR_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case LSR_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_LSR
            CONTINUE
            
        case LSR_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LSR_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LSR_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LSR_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case LSR_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
            }
            CONTINUE
            
        case LSR_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case LSR_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_LSR
            CONTINUE
            
        case LSR_ind_y_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: NOP
        //
        // Operation:   No operation
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case NOP:
            
            IDLE_READ_IMPLIED
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case NOP_imm:
            
            IDLE_READ_IMMEDIATE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case NOP_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case NOP_zpg_2:
            
            READ_FROM_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case NOP_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case NOP_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case NOP_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case NOP_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case NOP_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case NOP_abs_3:
            
            READ_FROM_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case NOP_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case NOP_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case NOP_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                POLL_INT
                DONE
            }
            
        case NOP_abs_x_4:
            
            READ_FROM_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ORA
        //
        // Operation:   A := A v M
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case ORA_imm:
            
            READ_IMMEDIATE
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ORA_abs_2:
            FETCH_ADDR_HI
            CONTINUE
            
        case ORA_abs_3:
            READ_FROM_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ORA_zpg_2:
            
            READ_FROM_ZERO_PAGE
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ORA_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case ORA_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ORA_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ORA_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A | data);
                POLL_INT
                DONE
            }
            
        case ORA_abs_x_4:
            
            READ_FROM_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ORA_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case ORA_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A | data);
                POLL_INT
                DONE
            }
            
        case ORA_abs_y_4:
            
            READ_FROM_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ORA_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ORA_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE;
            
        case ORA_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ORA_ind_x_5:
            
            READ_FROM_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ORA_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ORA_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ORA_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case ORA_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(A | data);
                POLL_INT
                DONE
            }
            
        case ORA_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: PHA
        //
        // Operation:   A to stack
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case PHA:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case PHA_2:
            
            PUSH_A
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: PHA
        //
        // Operation:   P to stack
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------
            
        case PHP:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case PHP_2:
            
            PUSH_P
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: PLA
        //
        // Operation:   Stack to A
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case PLA:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case PLA_2:
            
            SP++;
            CONTINUE
            
        case PLA_3:
            
            PULL_A
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: PLP
        //
        // Operation:   Stack to p
        //
        // Flags:       N Z C I D V
        //              / / / / / /
        // -------------------------------------------------------------------------------

        case PLP:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case PLP_2:
            
            SP++;
            CONTINUE
            
        case PLP_3:
            
            POLL_INT // Interrupts are polled before P is pulled
            PULL_P
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ROL
        //
        //              -----------------------
        //              |                     |
        // Operation:   ---(A|M << 1) <- C <---
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

            /*
inline uint8_t CPU::rol(uint8_t op)
{
	uint8_t bit8    = (op & 128);
	uint8_t shifted = (op << 1) + (getC() ? 1 : 0); 
	setC(bit8);
	return shifted;
}	
*/
            
        #define DO_ROL if (getC()) { setC(data & 128); data = (data << 1) + 1; } else { setC(data & 128); data = (data << 1); }

        // -------------------------------------------------------------------------------
        case ROL_acc:
            
            IDLE_READ_IMPLIED
            if (getC()) {
                setC(A & 128);
                loadA((A << 1) + 1);
            } else {
                setC(A & 128);
                loadA(A << 1);
            }
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROL_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROL_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ROL_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_ROL
            CONTINUE
            
        case ROL_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROL_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROL_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case ROL_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ROL_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROL
            CONTINUE
            
        case ROL_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROL_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROL_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case ROL_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ROL_abs_4:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case ROL_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROL_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROL_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ROL_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case ROL_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ROL_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case ROL_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROL_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ROL_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ROL_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ROL_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ROL_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ROL_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case ROL_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ROR
        //
        //              -----------------------
        //              |                     |
        // Operation:   --->(A|M >> 1) -> C ---
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

    /*
inline uint8_t CPU::ror(uint8_t op)
{
	uint8_t bit1    = (op & 1);
	uint8_t shifted = (op >> 1) + (getC() ? 128 : 0); 
	setC(bit1);
	return shifted;
}	
 */
    
        #define DO_ROR if (getC()) { setC(data & 1); data = (data >> 1) + 128; } else { setC(data & 1); data = (data >> 1); }

        // -------------------------------------------------------------------------------
        case ROR_acc:
            
            IDLE_READ_IMPLIED
            if (getC()) {
                setC(A & 1);
                loadA((A >> 1) + 128);
            } else {
                setC(A & 1);
                loadA(A >> 1);
            }
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROR_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROR_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ROR_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_ROR
            CONTINUE
            
        case ROR_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROR_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROR_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case ROR_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ROR_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROR
            CONTINUE
            
        case ROR_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROR_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROR_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case ROR_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ROR_abs_4:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case ROR_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROR_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ROR_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ROR_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
            }
            CONTINUE
            
        case ROR_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ROR_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case ROR_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ROR_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ROR_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ROR_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ROR_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ROR_ind_x_5:
            
            READ_FROM_ADDRESS;
            CONTINUE
            
        case ROR_ind_x_6:
            
            WRITE_TO_ADDRESS;
            DO_ROR;
            CONTINUE
            
        case ROR_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: RTI
        //
        // Operation:   P from Stack, PC from Stack
        //
        // Flags:       N Z C I D V
        //              / / / / / /
        // -------------------------------------------------------------------------------

        case RTI:
            
            IDLE_READ_IMMEDIATE;
            CONTINUE
            
        case RTI_2:
            
            SP++;
            CONTINUE
            
        case RTI_3:
            
            PULL_P
            SP++;
            CONTINUE
            
        case RTI_4:
            
            PULL_PCL
            SP++;
            CONTINUE
            
        case RTI_5:
            
            PULL_PCH
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: RTS
        //
        // Operation:   PC from Stack
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------
            
        case RTS:
            
            IDLE_READ_IMMEDIATE
            CONTINUE
            
        case RTS_2:
            
            IDLE_READ_IMMEDIATE_SP
            CONTINUE
            
        case RTS_3:
            
            PULL_PCL
            SP++;
            CONTINUE
            
        case RTS_4:
            
            PULL_PCH
            CONTINUE
            
        case RTS_5:
            
            IDLE_READ_IMMEDIATE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SBC
        //
        // Operation:   A := A - M - (~C)
        //
        // Flags:       N Z C I D V
        //              / / / - - /
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case SBC_imm:
            
            READ_IMMEDIATE
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SBC_zpg_2:
            
            READ_FROM_ZERO_PAGE
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SBC_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case SBC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SBC_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case SBC_abs_3:
            
            READ_FROM_ADDRESS;
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SBC_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case SBC_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                sbc(data);
                POLL_INT
                DONE
            }
            
        case SBC_abs_x_4:
            
            READ_FROM_ADDRESS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SBC_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case SBC_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                sbc(data);
                POLL_INT
                DONE
            }
            
        case SBC_abs_y_4:
            
            READ_FROM_ADDRESS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SBC_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case SBC_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SBC_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case SBC_ind_x_5:
            
            READ_FROM_ADDRESS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SBC_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SBC_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SBC_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case SBC_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                sbc(data);
                POLL_INT
                DONE
            }
            
        case SBC_ind_y_5:
            
            READ_FROM_ADDRESS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SEC
        //
        // Operation:   C := 1
        //
        // Flags:       N Z C I D V
        //              - - 1 - - -
        // -------------------------------------------------------------------------------

        case SEC:
            
            IDLE_READ_IMPLIED
            setC(1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SED
        //
        // Operation:   D := 1
        //
        // Flags:       N Z C I D V
        //              - - - - 1 -
        // -------------------------------------------------------------------------------

        case SED:
            
            IDLE_READ_IMPLIED
            setD(1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SEI
        //
        // Operation:   I := 1
        //
        // Flags:       N Z C I D V
        //              - - - 1 - -
        // -------------------------------------------------------------------------------

        case SEI:
            
            IDLE_READ_IMPLIED
            POLL_INT
            setI(1);
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: STA
        //
        // Operation:   M := A
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case STA_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STA_zpg_2:
            
            data = A;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STA_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STA_zpg_x_2:
            
            IDLE_READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case STA_zpg_x_3:
            
            data = A;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STA_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STA_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case STA_abs_3:
            
            data = A;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STA_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STA_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case STA_abs_x_3:
            
            IDLE_READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case STA_abs_x_4:
            
            data = A;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STA_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STA_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case STA_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED)
                FIX_ADDR_HI
                CONTINUE
                
                case STA_abs_y_4:
                
                data = A;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STA_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case STA_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case STA_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case STA_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case STA_ind_x_5:
            
            data = A;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STA_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case STA_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case STA_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case STA_ind_y_4:
            
            IDLE_READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case STA_ind_y_5:
            
            data = A;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: STX
        //
        // Operation:   M := X
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case STX_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STX_zpg_2:
            
            data = X;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STX_zpg_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STX_zpg_y_2:
            
            IDLE_READ_FROM_ZERO_PAGE
            ADD_INDEX_Y
            CONTINUE
            
        case STX_zpg_y_3:
            
            data = X;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STX_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STX_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case STX_abs_3:
            
            data = X;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: STY
        //
        // Operation:   M := Y
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case STY_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STY_zpg_2:
            
            data = Y;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STY_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STY_zpg_x_2:
            
            IDLE_READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case STY_zpg_x_3:
            
            data = Y;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case STY_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case STY_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case STY_abs_3:
            
            data = Y;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TAX
        //
        // Operation:   X := A
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case TAX:
            
            IDLE_READ_IMPLIED
            loadX(A);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TAY
        //
        // Operation:   Y := A
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case TAY:
            
            IDLE_READ_IMPLIED
            loadY(A);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TSX
        //
        // Operation:   X := Stack pointer
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case TSX:
            
            IDLE_READ_IMPLIED
            loadX(SP);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TXA
        //
        // Operation:   A := X
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case TXA:
            
            IDLE_READ_IMPLIED
            loadA(X);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TXS
        //
        // Operation:   Stack pointer := X
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case TXS:
            
            IDLE_READ_IMPLIED
            SP = X;
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TYA
        //
        // Operation:   A := Y
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case TYA:
            
            IDLE_READ_IMPLIED
            loadA(Y);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Illegal instructions
        // -------------------------------------------------------------------------------
        

        // -------------------------------------------------------------------------------
        // Instruction: ALR
        //
        // Operation:   AND, followed by LSR
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        case ALR_imm:
            
            READ_IMMEDIATE
            A = A & data;
            setC(A & 1);
            loadA(A >> 1);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ANC
        //
        // Operation:   A := A & op,   N flag is copied to C
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        case ANC_imm:
            
            READ_IMMEDIATE
            loadA(A & data);
            setC(getN());
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ARR
        //
        // Operation:   AND, followed by ROR
        //
        // Flags:       N Z C I D V
        //              / / / - - /
        // -------------------------------------------------------------------------------

        case ARR_imm:
        {
            READ_IMMEDIATE
            
            uint8_t tmp2 = A & data;
            
            // Taken from Frodo...
            A = (getC() ? (tmp2 >> 1) | 0x80 : tmp2 >> 1);
            if (!getD()) {
                setN(A & 0x80);
                setZ(A == 0);
                setC(A & 0x40);
                setV((A & 0x40) ^ ((A & 0x20) << 1));
            } else {
                int c_flag;
                
                setN(getC());
                setZ(A == 0);
                setV((tmp2 ^ A) & 0x40);
                if ((tmp2 & 0x0f) + (tmp2 & 0x01) > 5)
                    A = (A & 0xf0) | ((A + 6) & 0x0f);
                c_flag = (tmp2 + (tmp2 & 0x10)) & 0x1f0;
                if (c_flag > 0x50) {
                    setC(1);
                    A += 0x60;
                } else {
                    setC(0);
                }
            }
            POLL_INT
            DONE
        }

        // -------------------------------------------------------------------------------
        // Instruction: AXS
        //
        // Operation:   X = (A & X) - op
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        case AXS_imm:
        {
            READ_IMMEDIATE
            
            uint8_t op2  = A & X;
            uint8_t tmp = op2 - data;
            
            setC(op2 >= data);
            loadX(tmp);
            POLL_INT
            DONE
        }

        // -------------------------------------------------------------------------------
        // Instruction: DCP
        //
        // Operation:   DEC followed by CMP
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------
            
        case DCP_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DCP_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case DCP_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_DEC
            CONTINUE
            
        case DCP_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DCP_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DCP_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case DCP_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case DCP_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_DEC
            CONTINUE
            
        case DCP_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DCP_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DCP_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case DCP_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DCP_abs_4:
            
            WRITE_TO_ADDRESS
            DO_DEC;
            CONTINUE
            
        case DCP_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DCP_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DCP_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case DCP_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case DCP_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DCP_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DCP_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DCP_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case DCP_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case DCP_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case DCP_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DCP_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DCP_abs_y_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DCP_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case DCP_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case DCP_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case DCP_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case DCP_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DCP_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DCP_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case DCP_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case DCP_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case DCP_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case DCP_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case DCP_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case DCP_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DCP_ind_y_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            cmp(A, data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ISC
        //
        // Operation:   INC followed by SBC
        //
        // Flags:       N Z C I D V
        //              / / / - - /
        // -------------------------------------------------------------------------------

        case ISC_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ISC_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ISC_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_INC
            CONTINUE
            
        case ISC_zpg_4:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ISC_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ISC_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case ISC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case ISC_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_INC
            CONTINUE
            
        case ISC_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ISC_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ISC_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case ISC_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ISC_abs_4:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case ISC_abs_5:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ISC_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ISC_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ISC_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case ISC_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ISC_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case ISC_abs_x_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ISC_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ISC_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case ISC_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case ISC_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ISC_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case ISC_abs_y_6:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ISC_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ISC_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ISC_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ISC_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ISC_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ISC_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case ISC_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case ISC_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ISC_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ISC_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case ISC_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case ISC_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case ISC_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case ISC_ind_y_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            sbc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LAS
        //
        // Operation:   SP,X,A = op & SP
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case LAS_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LAS_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case LAS_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                data &= SP;
                SP = data;
                X = data;
                loadA(data);
                POLL_INT
                DONE
            }
            
        case LAS_abs_y_4:
            
            READ_FROM_ADDRESS
            data &= SP;
            SP = data;
            X = data;
            loadA(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LAX
        //
        // Operation:   LDA, followed by LDX
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case LAX_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LAX_zpg_2:
            
            READ_FROM_ZERO_PAGE
            loadA(data);
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LAX_zpg_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LAX_zpg_y_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_Y
            CONTINUE
            
        case LAX_zpg_y_3:
            
            READ_FROM_ZERO_PAGE
            loadA(data);
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LAX_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LAX_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case LAX_abs_3:
            
            READ_FROM_ADDRESS;
            loadA(data);
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LAX_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case LAX_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case LAX_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(data);
                loadX(data);
                POLL_INT
                DONE
            }
            
        case LAX_abs_y_4:
            
            READ_FROM_ADDRESS
            loadA(data);
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LAX_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LAX_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case LAX_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LAX_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case LAX_ind_x_5:
            
            READ_FROM_ADDRESS
            loadA(data);
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case LAX_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case LAX_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case LAX_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case LAX_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(data);
                loadX(data);
                POLL_INT
                DONE
            }
            
        case LAX_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(data);
            loadX(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: RLA
        //
        // Operation:   ROL, followed by AND
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case RLA_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RLA_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case RLA_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_ROL
            CONTINUE
            
        case RLA_zpg_4:
            
            WRITE_TO_ZERO_PAGE
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RLA_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RLA_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case RLA_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case RLA_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROL
            CONTINUE
            
        case RLA_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RLA_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RLA_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case RLA_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RLA_abs_4:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case RLA_abs_5:
            
            WRITE_TO_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RLA_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RLA_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case RLA_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case RLA_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RLA_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case RLA_abs_x_6:
            
            WRITE_TO_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RLA_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RLA_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case RLA_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case RLA_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RLA_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case RLA_abs_y_6:
            
            WRITE_TO_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RLA_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case RLA_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case RLA_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case RLA_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case RLA_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RLA_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case RLA_ind_x_7:
            
            WRITE_TO_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RLA_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case RLA_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case RLA_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case RLA_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case RLA_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RLA_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case RLA_ind_y_7:
            
            WRITE_TO_ADDRESS
            loadA(A & data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: RRA
        //
        // Operation:   ROR, followed by ADC
        //
        // Flags:       N Z C I D V
        //              / / / - - /
        // -------------------------------------------------------------------------------

        // -------------------------------------------------------------------------------
        case RRA_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RRA_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case RRA_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_ROR
            CONTINUE
            
        case RRA_zpg_4:
            
            WRITE_TO_ZERO_PAGE
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RRA_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RRA_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case RRA_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case RRA_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROR
            CONTINUE
            
        case RRA_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RRA_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RRA_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case RRA_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RRA_abs_4:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case RRA_abs_5:
            
            WRITE_TO_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RRA_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RRA_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case RRA_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case RRA_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RRA_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case RRA_abs_x_6:
            
            WRITE_TO_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RRA_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case RRA_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case RRA_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case RRA_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RRA_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case RRA_abs_y_6:
            
            WRITE_TO_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RRA_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case RRA_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case RRA_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case RRA_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case RRA_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RRA_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case RRA_ind_x_7:
            
            WRITE_TO_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case RRA_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case RRA_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case RRA_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case RRA_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case RRA_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case RRA_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case RRA_ind_y_7:
            
            WRITE_TO_ADDRESS
            adc(data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SAX
        //
        // Operation:   Mem := A & X
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case SAX_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SAX_zpg_2:
            
            data = A & X;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SAX_zpg_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SAX_zpg_y_2:
            
            IDLE_READ_FROM_ZERO_PAGE
            ADD_INDEX_Y
            CONTINUE
            
        case SAX_zpg_y_3:
            
            data = A & X;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SAX_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SAX_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case SAX_abs_3:
            
            data = A & X;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SAX_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SAX_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case SAX_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SAX_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case SAX_ind_x_5:
            
            data = A & X;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SHA
        //
        // Operation:   Mem := A & X & (M + 1)
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case SHA_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SHA_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case SHA_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            
            /* "There are two unstable conditions, the first is when a DMA is going on while
             *  the instruction executes (the CPU is halted by the VIC-II) then the & M+1 part
             *  drops off and the instruction becomes addr = A & X. The other unstable condition
             *  is when the addressing/indexing causes a page boundary crossing, in that case
             *  the highbyte of the target address may become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                data = A & X & addr_hi;
                addr_hi = X & addr_hi;
            } else {
                data = A & X & (addr_hi + 1);
            }
            
            if (rdyLineUp == c64->cycle) {
                data = A & X;
            }
        
            CONTINUE
            
        case SHA_abs_y_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SHA_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SHA_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SHA_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case SHA_ind_y_4:
            
            IDLE_READ_FROM_ADDRESS
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                data = A & X & addr_hi;
                addr_hi = X & addr_hi;
            } else {
                data = A & X & (addr_hi + 1);
            }
            
            if (rdyLineUp == c64->cycle) {
                data = A & X;
            }
            
            CONTINUE
            
        case SHA_ind_y_5:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SHX
        //
        // Operation:   Mem := X & (HI_BYTE(op) + 1)
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case SHX_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SHX_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
         
        case SHX_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                data = X & addr_hi;
                addr_hi = X & addr_hi;
            } else {
                data = X & (addr_hi + 1);
            }
            
            if (rdyLineUp == c64->cycle) {
                data = X;
            }
            
            CONTINUE
           
        case SHX_abs_y_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SHY
        //
        // Operation:   Mem := Y & (HI_BYTE(op) + 1)
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        // -------------------------------------------------------------------------------

        case SHY_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SHY_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case SHY_abs_x_3:
            
            IDLE_READ_FROM_ADDRESS
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                data = Y & addr_hi;
                addr_hi = Y & addr_hi;
            } else {
                data = Y & (addr_hi + 1);
            }
            
            if (rdyLineUp == c64->cycle) {
                data = Y;
            }
            
            CONTINUE
            
        case SHY_abs_x_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SLO (ASO)
        //
        // Operation:   ASL memory location, followed by OR on accumulator
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        #define DO_SLO setC(data & 128); data <<= 1;

        // -------------------------------------------------------------------------------
        case SLO_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SLO_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case SLO_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_SLO
            CONTINUE
            
        case SLO_zpg_4:
            
            WRITE_TO_ZERO_PAGE
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SLO_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SLO_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case SLO_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case SLO_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_SLO
            CONTINUE
            
        case SLO_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SLO_abs:
            FETCH_ADDR_LO
            CONTINUE
            
        case SLO_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case SLO_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SLO_abs_4:
            
            WRITE_TO_ADDRESS
            DO_SLO
            CONTINUE
            
        case SLO_abs_5:
            
            WRITE_TO_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SLO_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SLO_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case SLO_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case SLO_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SLO_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_SLO
            CONTINUE
            
        case SLO_abs_x_6:
            
            WRITE_TO_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SLO_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SLO_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case SLO_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case SLO_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SLO_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_SLO
            CONTINUE
            
        case SLO_abs_y_6:
            
            WRITE_TO_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SLO_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SLO_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case SLO_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SLO_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case SLO_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SLO_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_SLO
            CONTINUE
            
        case SLO_ind_x_7:
            
            WRITE_TO_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SLO_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SLO_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SLO_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case SLO_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case SLO_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SLO_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_SLO
            CONTINUE
            
        case SLO_ind_y_7:
            WRITE_TO_ADDRESS
            loadA(A | data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: SRE (LSE)
        //
        // Operation:   LSR, followed by EOR
        //
        // Flags:       N Z C I D V
        //              / / / - - -
        // -------------------------------------------------------------------------------

        #define DO_SRE setC(data & 1); data >>= 1;

        // -------------------------------------------------------------------------------
        case SRE_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SRE_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case SRE_zpg_3:
            
            WRITE_TO_ZERO_PAGE
            DO_SRE
            CONTINUE
            
        case SRE_zpg_4:
            
            WRITE_TO_ZERO_PAGE
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SRE_zpg_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SRE_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE
            
        case SRE_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
        case SRE_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_SRE
            CONTINUE
            
        case SRE_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SRE_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SRE_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case SRE_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SRE_abs_4:
            
            WRITE_TO_ADDRESS
            DO_SRE
            CONTINUE
            
        case SRE_abs_5:
            
            WRITE_TO_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SRE_abs_x:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SRE_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case SRE_abs_x_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case SRE_abs_x_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SRE_abs_x_5:
            
            WRITE_TO_ADDRESS
            DO_SRE
            CONTINUE
            
        case SRE_abs_x_6:
            
            WRITE_TO_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SRE_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case SRE_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case SRE_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case SRE_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SRE_abs_y_5:
            
            WRITE_TO_ADDRESS
            DO_SRE
            CONTINUE
            
        case SRE_abs_y_6:
            
            WRITE_TO_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SRE_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SRE_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case SRE_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SRE_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case SRE_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SRE_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_SRE
            CONTINUE
            
        case SRE_ind_x_7:
            
            WRITE_TO_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        case SRE_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case SRE_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case SRE_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE
            
        case SRE_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case SRE_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
        case SRE_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_SRE
            CONTINUE
            
        case SRE_ind_y_7:
            
            WRITE_TO_ADDRESS
            loadA(A ^ data);
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: TAS (SHS)
        //
        // Operation:   SP := A & X,  Mem := SP & (HI_BYTE(op) + 1)
        //
        // Flags:       N Z C I D V
        //              - - - - - -
        //
        // -------------------------------------------------------------------------------

        case TAS_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case TAS_abs_y_2:
            
            FETCH_ADDR_HI;
            ADD_INDEX_Y;
            CONTINUE
            
        case TAS_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            
            SP = A & X;
            
            /* "There are two unstable conditions, the first is when a DMA is going on while
             *  the instruction executes (the CPU is halted by the VIC-II) then the & M+1 part
             *  drops off and the instruction becomes SP = A & X, addr = SP.
             *  The other unstable condition is when the addressing/indexing causes a
             *  page boundary crossing, in that case the highbyte of the target address may
             *  become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                data = SP & addr_hi;
                addr_hi = SP & addr_hi;
            } else {
                data = SP & (addr_hi + 1);
            }
            
            if (rdyLineUp == c64->cycle) {
                data = SP;
            }
            
            CONTINUE
            
        case TAS_abs_y_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: ANE
        //
        // Operation:   A = X & op & (A | 0xEE) (taken from Frodo)
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case ANE_imm:
            
            READ_IMMEDIATE
            loadA(X & data & (A | 0xEE));
            POLL_INT
            DONE

        // -------------------------------------------------------------------------------
        // Instruction: LXA
        //
        // Operation:   A = X = op & (A | 0xEE) (taken from Frodo)
        //
        // Flags:       N Z C I D V
        //              / / - - - -
        // -------------------------------------------------------------------------------

        case LXA_imm:
            
            READ_IMMEDIATE
            X = data & (A | 0xEE);
            loadA(X);
            POLL_INT
            DONE

        default:
            debug("ERROR: UNIMPLEMENTED OPCODE: %d (%02X)\n", next, next);
    }
}

