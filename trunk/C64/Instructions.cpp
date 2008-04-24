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
																						
void 
CPU::registerCallback(uint8_t opcode, int (CPU::*func)())
{
	registerCallback(opcode, "???", ADDR_IMPLIED, func);
}

void 
CPU::registerCallback(uint8_t opcode, char *mnc, AddressingMode mode, int (CPU::*func)())
{
	// table is write once only!
	if (func != &CPU::defaultCallback) 
		assert(actionFunc[opcode] == &CPU::defaultCallback);
	
	actionFunc[opcode]     = func;
	mnemonic[opcode]       = mnc;
	addressingMode[opcode] = mode;
}

void 
CPU::registerIllegalInstructions()
{
	registerCallback(0x93, "AHX*", ADDR_INDIRECT_Y, &CPU::AHX_indirect_y);
	registerCallback(0x9F, "AHX*", ADDR_ABSOLUTE_Y, &CPU::AHX_absolute_y);

	registerCallback(0x4B, "ALR*", ADDR_IMMEDIATE, &CPU::ALR_immediate);

	registerCallback(0x0B, "ANC*", ADDR_IMMEDIATE, &CPU::ANC_immediate);
	registerCallback(0x2B, "ANC*", ADDR_IMMEDIATE, &CPU::ANC_immediate);

	registerCallback(0x6B, "ARR*", ADDR_IMMEDIATE, &CPU::ARR_immediate);
	registerCallback(0xCB, "AXS*", ADDR_IMMEDIATE, &CPU::AXS_immediate);

	registerCallback(0xC7, "DCP*", ADDR_ZERO_PAGE, &CPU::DCP_zero_page);
	registerCallback(0xD7, "DCP*", ADDR_ZERO_PAGE_X, &CPU::DCP_zero_page_x);
	registerCallback(0xC3, "DCP*", ADDR_INDIRECT_X, &CPU::DCP_indirect_x);
	registerCallback(0xD3, "DCP*", ADDR_INDIRECT_Y, &CPU::DCP_indirect_y);
	registerCallback(0xCF, "DCP*", ADDR_ABSOLUTE, &CPU::DCP_absolute);
	registerCallback(0xDF, "DCP*", ADDR_ABSOLUTE_X, &CPU::DCP_absolute_x);
	registerCallback(0xDB, "DCP*", ADDR_ABSOLUTE_Y, &CPU::DCP_absolute_y);

	registerCallback(0xE7, "ISC*", ADDR_ZERO_PAGE, &CPU::ISC_zero_page);
	registerCallback(0xF7, "ISC*", ADDR_ZERO_PAGE_X, &CPU::ISC_zero_page_x);
	registerCallback(0xE3, "ISC*", ADDR_INDIRECT_X, &CPU::ISC_indirect_x);
	registerCallback(0xF3, "ISC*", ADDR_INDIRECT_Y, &CPU::ISC_indirect_y);
	registerCallback(0xEF, "ISC*", ADDR_ABSOLUTE, &CPU::ISC_absolute);
	registerCallback(0xFF, "ISC*", ADDR_ABSOLUTE_X, &CPU::ISC_absolute_x);
	registerCallback(0xFB, "ISC*", ADDR_ABSOLUTE_Y, &CPU::ISC_absolute_y);

	registerCallback(0xBB, "LAS*", ADDR_ABSOLUTE_Y, &CPU::LAS_absolute_y);

	registerCallback(0xAB, "LAX*", ADDR_IMMEDIATE, &CPU::LAX_immediate);
	registerCallback(0xA7, "LAX*", ADDR_ZERO_PAGE, &CPU::LAX_zero_page);
	registerCallback(0xB7, "LAX*", ADDR_ZERO_PAGE_Y, &CPU::LAX_zero_page_y);
	registerCallback(0xA3, "LAX*", ADDR_INDIRECT_X, &CPU::LAX_indirect_x);
	registerCallback(0xB3, "LAX*", ADDR_INDIRECT_Y, &CPU::LAX_indirect_y);
	registerCallback(0xAF, "LAX*", ADDR_ABSOLUTE, &CPU::LAX_absolute);
	registerCallback(0xBF, "LAX*", ADDR_ABSOLUTE_Y, &CPU::LAX_absolute_y);

	registerCallback(0x80, "NOP*", ADDR_IMMEDIATE, &CPU::NOP_immediate);
	registerCallback(0x82, "NOP*", ADDR_IMMEDIATE, &CPU::NOP_immediate);
	registerCallback(0x89, "NOP*", ADDR_IMMEDIATE, &CPU::NOP_immediate);
	registerCallback(0xC2, "NOP*", ADDR_IMMEDIATE, &CPU::NOP_immediate);	
	registerCallback(0xE2, "NOP*", ADDR_IMMEDIATE, &CPU::NOP_immediate);
	registerCallback(0x1A, "NOP*", ADDR_IMPLIED, &CPU::NOP);
	registerCallback(0x3A, "NOP*", ADDR_IMPLIED, &CPU::NOP);
	registerCallback(0x5A, "NOP*", ADDR_IMPLIED, &CPU::NOP);
	registerCallback(0x7A, "NOP*", ADDR_IMPLIED, &CPU::NOP);
	registerCallback(0xDA, "NOP*", ADDR_IMPLIED, &CPU::NOP);
	registerCallback(0xFA, "NOP*", ADDR_IMPLIED, &CPU::NOP);
	registerCallback(0x04, "NOP*", ADDR_ZERO_PAGE, &CPU::NOP_zero_page);
	registerCallback(0x44, "NOP*", ADDR_ZERO_PAGE, &CPU::NOP_zero_page);
	registerCallback(0x64, "NOP*", ADDR_ZERO_PAGE, &CPU::NOP_zero_page);
	registerCallback(0x0C, "NOP*", ADDR_ABSOLUTE, &CPU::NOP_absolute);
	registerCallback(0x14, "NOP*", ADDR_ZERO_PAGE_X, &CPU::NOP_zero_page_x);
	registerCallback(0x34, "NOP*", ADDR_ZERO_PAGE_X, &CPU::NOP_zero_page_x);
	registerCallback(0x54, "NOP*", ADDR_ZERO_PAGE_X, &CPU::NOP_zero_page_x);
	registerCallback(0x74, "NOP*", ADDR_ZERO_PAGE_X, &CPU::NOP_zero_page_x);
	registerCallback(0xD4, "NOP*", ADDR_ZERO_PAGE_X, &CPU::NOP_zero_page_x);
	registerCallback(0xF4, "NOP*", ADDR_ZERO_PAGE_X, &CPU::NOP_zero_page_x);
	registerCallback(0x1C, "NOP*", ADDR_ABSOLUTE_X, &CPU::NOP_absolute_x);
	registerCallback(0x3C, "NOP*", ADDR_ABSOLUTE_X, &CPU::NOP_absolute_x);
	registerCallback(0x5C, "NOP*", ADDR_ABSOLUTE_X, &CPU::NOP_absolute_x);
	registerCallback(0x7C, "NOP*", ADDR_ABSOLUTE_X, &CPU::NOP_absolute_x);
	registerCallback(0xDC, "NOP*", ADDR_ABSOLUTE_X, &CPU::NOP_absolute_x);
	registerCallback(0xFC, "NOP*", ADDR_ABSOLUTE_X, &CPU::NOP_absolute_x);

	registerCallback(0x27, "RLA*", ADDR_ZERO_PAGE, &CPU::RLA_zero_page);
	registerCallback(0x37, "RLA*", ADDR_ZERO_PAGE_X, &CPU::RLA_zero_page_x);
	registerCallback(0x23, "RLA*", ADDR_INDIRECT_X, &CPU::RLA_indirect_x);
	registerCallback(0x33, "RLA*", ADDR_INDIRECT_Y, &CPU::RLA_indirect_y);
	registerCallback(0x2F, "RLA*", ADDR_ABSOLUTE, &CPU::RLA_absolute);
	registerCallback(0x3F, "RLA*", ADDR_ABSOLUTE_X, &CPU::RLA_absolute_x);
	registerCallback(0x3B, "RLA*", ADDR_ABSOLUTE_Y, &CPU::RLA_absolute_y);

	registerCallback(0x67, "RRA*", ADDR_ZERO_PAGE, &CPU::RRA_zero_page);
	registerCallback(0x77, "RRA*", ADDR_ZERO_PAGE_X, &CPU::RRA_zero_page_x);
	registerCallback(0x63, "RRA*", ADDR_INDIRECT_X, &CPU::RRA_indirect_x);
	registerCallback(0x73, "RRA*", ADDR_INDIRECT_Y, &CPU::RRA_indirect_y);
	registerCallback(0x6F, "RRA*", ADDR_ABSOLUTE, &CPU::RRA_absolute);
	registerCallback(0x7F, "RRA*", ADDR_ABSOLUTE_X, &CPU::RRA_absolute_x);
	registerCallback(0x7B, "RRA*", ADDR_ABSOLUTE_Y, &CPU::RRA_absolute_y);

	registerCallback(0x87, "SAX*", ADDR_ZERO_PAGE, &CPU::SAX_zero_page);
	registerCallback(0x97, "SAX*", ADDR_ZERO_PAGE_Y, &CPU::SAX_zero_page_y);
	registerCallback(0x83, "SAX*", ADDR_INDIRECT_X, &CPU::SAX_indirect_x);
	registerCallback(0x8F, "SAX*", ADDR_ABSOLUTE, &CPU::SAX_absolute);

	registerCallback(0xEB, "SBC*", ADDR_IMMEDIATE, &CPU::SBC_immediate);

	registerCallback(0x9E, "SHX*", ADDR_ABSOLUTE_Y, &CPU::SHX_absolute_y);
	registerCallback(0x9C, "SHY*", ADDR_ABSOLUTE_X, &CPU::SHY_absolute_x);

	registerCallback(0x07, "SLO*", ADDR_ZERO_PAGE, &CPU::SLO_zero_page);
	registerCallback(0x17, "SLO*", ADDR_ZERO_PAGE_X, &CPU::SLO_zero_page_x);
	registerCallback(0x03, "SLO*", ADDR_INDIRECT_X, &CPU::SLO_indirect_x);
	registerCallback(0x13, "SLO*", ADDR_INDIRECT_Y, &CPU::SLO_indirect_y);
	registerCallback(0x0F, "SLO*", ADDR_ABSOLUTE, &CPU::SLO_absolute);
	registerCallback(0x1F, "SLO*", ADDR_ABSOLUTE_X, &CPU::SLO_absolute_x);
	registerCallback(0x1B, "SLO*", ADDR_ABSOLUTE_Y, &CPU::SLO_absolute_y);

	registerCallback(0x47, "SRE*", ADDR_ZERO_PAGE, &CPU::SRE_zero_page);
	registerCallback(0x57, "SRE*", ADDR_ZERO_PAGE_X, &CPU::SRE_zero_page_x);
	registerCallback(0x43, "SRE*", ADDR_INDIRECT_X, &CPU::SRE_indirect_x);
	registerCallback(0x53, "SRE*", ADDR_INDIRECT_Y, &CPU::SRE_indirect_y);
	registerCallback(0x4F, "SRE*", ADDR_ABSOLUTE, &CPU::SRE_absolute);
	registerCallback(0x5F, "SRE*", ADDR_ABSOLUTE_X, &CPU::SRE_absolute_x);
	registerCallback(0x5B, "SRE*", ADDR_ABSOLUTE_Y, &CPU::SRE_absolute_y);
	
	registerCallback(0x9B, "TAS*", ADDR_ABSOLUTE_Y, &CPU::TAS_absolute_y);
	registerCallback(0x8B, "XAA*", ADDR_IMMEDIATE, &CPU::XAA_immediate);
}

	
void CPU::registerInstructions()
{
	for (int i=0; i<256; i++)
		registerCallback(i, &CPU::defaultCallback);

	registerCallback(0x69, "ADC", ADDR_IMMEDIATE, &CPU::ADC_immediate);
	registerCallback(0x65, "ADC", ADDR_ZERO_PAGE, &CPU::ADC_zero_page);
	registerCallback(0x75, "ADC", ADDR_ZERO_PAGE_X, &CPU::ADC_zero_page_x);
	registerCallback(0x6D, "ADC", ADDR_ABSOLUTE, &CPU::ADC_absolute);
	registerCallback(0x7D, "ADC", ADDR_ABSOLUTE_X, &CPU::ADC_absolute_x);
	registerCallback(0x79, "ADC", ADDR_ABSOLUTE_Y, &CPU::ADC_absolute_y);
	registerCallback(0x61, "ADC", ADDR_INDIRECT_X, &CPU::ADC_indirect_x);
	registerCallback(0x71, "ADC", ADDR_INDIRECT_Y, &CPU::ADC_indirect_y);

	registerCallback(0x29, "AND", ADDR_IMMEDIATE, &CPU::AND_immediate);
	registerCallback(0x25, "AND", ADDR_ZERO_PAGE, &CPU::AND_zero_page);
	registerCallback(0x35, "AND", ADDR_ZERO_PAGE_X, &CPU::AND_zero_page_x);
	registerCallback(0x2D, "AND", ADDR_ABSOLUTE, &CPU::AND_absolute);
	registerCallback(0x3D, "AND", ADDR_ABSOLUTE_X, &CPU::AND_absolute_x);
	registerCallback(0x39, "AND", ADDR_ABSOLUTE_Y, &CPU::AND_absolute_y);
	registerCallback(0x21, "AND", ADDR_INDIRECT_X, &CPU::AND_indirect_x);
	registerCallback(0x31, "AND", ADDR_INDIRECT_Y, &CPU::AND_indirect_y);
	
	registerCallback(0x0A, "ASL", ADDR_ACCUMULATOR, &CPU::ASL_accumulator);
	registerCallback(0x06, "ASL", ADDR_ZERO_PAGE, &CPU::ASL_zero_page);
	registerCallback(0x16, "ASL", ADDR_ZERO_PAGE_X, &CPU::ASL_zero_page_x);
	registerCallback(0x0E, "ASL", ADDR_ABSOLUTE, &CPU::ASL_absolute);
	registerCallback(0x1E, "ASL", ADDR_ABSOLUTE_X, &CPU::ASL_absolute_x);
	
	registerCallback(0x90, "BCC", ADDR_RELATIVE, &CPU::BCC_relative);
	registerCallback(0xB0, "BCS", ADDR_RELATIVE, &CPU::BCS_relative);
	registerCallback(0xF0, "BEQ", ADDR_RELATIVE, &CPU::BEQ_relative);

	registerCallback(0x24, "BIT", ADDR_ZERO_PAGE, &CPU::BIT_zero_page);
	registerCallback(0x2C, "BIT", ADDR_ABSOLUTE, &CPU::BIT_absolute);
	
	registerCallback(0x30, "BMI", ADDR_RELATIVE, &CPU::BMI_relative);
	registerCallback(0xD0, "BNE", ADDR_RELATIVE, &CPU::BNE_relative);
	registerCallback(0x10, "BPL", ADDR_RELATIVE, &CPU::BPL_relative);
	registerCallback(0x00, "BRK", ADDR_IMPLIED, &CPU::BRK);
	registerCallback(0x50, "BVC", ADDR_RELATIVE, &CPU::BVC_relative);
	registerCallback(0x70, "BVS", ADDR_RELATIVE, &CPU::BVS_relative);

	registerCallback(0x18, "CLC", ADDR_IMPLIED, &CPU::CLC);
	registerCallback(0xD8, "CLD", ADDR_IMPLIED, &CPU::CLD);
	registerCallback(0x58, "CLI", ADDR_IMPLIED, &CPU::CLI);
	registerCallback(0xB8, "CLV", ADDR_IMPLIED, &CPU::CLV);

	registerCallback(0xC9, "CMP", ADDR_IMMEDIATE, &CPU::CMP_immediate);
	registerCallback(0xC5, "CMP", ADDR_ZERO_PAGE, &CPU::CMP_zero_page);
	registerCallback(0xD5, "CMP", ADDR_ZERO_PAGE_X, &CPU::CMP_zero_page_x);
	registerCallback(0xCD, "CMP", ADDR_ABSOLUTE, &CPU::CMP_absolute);
	registerCallback(0xDD, "CMP", ADDR_ABSOLUTE_X, &CPU::CMP_absolute_x);
	registerCallback(0xD9, "CMP", ADDR_ABSOLUTE_Y, &CPU::CMP_absolute_y);
	registerCallback(0xC1, "CMP", ADDR_INDIRECT_X, &CPU::CMP_indirect_x);
	registerCallback(0xD1, "CMP", ADDR_INDIRECT_Y, &CPU::CMP_indirect_y);

	registerCallback(0xE0, "CPX", ADDR_IMMEDIATE, &CPU::CPX_immediate);
	registerCallback(0xE4, "CPX", ADDR_ZERO_PAGE, &CPU::CPX_zero_page);
	registerCallback(0xEC, "CPX", ADDR_ABSOLUTE, &CPU::CPX_absolute);

	registerCallback(0xC0, "CPY", ADDR_IMMEDIATE, &CPU::CPY_immediate);
	registerCallback(0xC4, "CPY", ADDR_ZERO_PAGE, &CPU::CPY_zero_page);
	registerCallback(0xCC, "CPY", ADDR_ABSOLUTE, &CPU::CPY_absolute);

	registerCallback(0xC6, "DEC", ADDR_ZERO_PAGE, &CPU::DEC_zero_page);
	registerCallback(0xD6, "DEC", ADDR_ZERO_PAGE_X, &CPU::DEC_zero_page_x);
	registerCallback(0xCE, "DEC", ADDR_ABSOLUTE, &CPU::DEC_absolute);
	registerCallback(0xDE, "DEC", ADDR_ABSOLUTE_X, &CPU::DEC_absolute_x);

	registerCallback(0xCA, "DEX", ADDR_IMPLIED, &CPU::DEX);
	registerCallback(0x88, "DEY", ADDR_IMPLIED, &CPU::DEY);
	
	registerCallback(0x49, "EOR", ADDR_IMMEDIATE, &CPU::EOR_immediate);
	registerCallback(0x45, "EOR", ADDR_ZERO_PAGE, &CPU::EOR_zero_page);
	registerCallback(0x55, "EOR", ADDR_ZERO_PAGE_X, &CPU::EOR_zero_page_x);
	registerCallback(0x4D, "EOR", ADDR_ABSOLUTE, &CPU::EOR_absolute);
	registerCallback(0x5D, "EOR", ADDR_ABSOLUTE_X, &CPU::EOR_absolute_x);
	registerCallback(0x59, "EOR", ADDR_ABSOLUTE_Y, &CPU::EOR_absolute_y);
	registerCallback(0x41, "EOR", ADDR_INDIRECT_X, &CPU::EOR_indirect_x);
	registerCallback(0x51, "EOR", ADDR_INDIRECT_Y, &CPU::EOR_indirect_y);

	registerCallback(0xE6, "INC", ADDR_ZERO_PAGE, &CPU::INC_zero_page);
	registerCallback(0xF6, "INC", ADDR_ZERO_PAGE_X, &CPU::INC_zero_page_x);
	registerCallback(0xEE, "INC", ADDR_ABSOLUTE, &CPU::INC_absolute);
	registerCallback(0xFE, "INC", ADDR_ABSOLUTE_X, &CPU::INC_absolute_x);
	
	registerCallback(0xE8, "INX", ADDR_IMPLIED, &CPU::INX);
	registerCallback(0xC8, "INY", ADDR_IMPLIED, &CPU::INY);

	registerCallback(0x4C, "JMP", ADDR_DIRECT, &CPU::JMP_absolute);
	registerCallback(0x6C, "JMP", ADDR_INDIRECT, &CPU::JMP_absolute_indirect);

	registerCallback(0x20, "JSR", ADDR_DIRECT, &CPU::JSR);

	registerCallback(0xA9, "LDA", ADDR_IMMEDIATE, &CPU::LDA_immediate);
	registerCallback(0xA5, "LDA", ADDR_ZERO_PAGE, &CPU::LDA_zero_page);
	registerCallback(0xB5, "LDA", ADDR_ZERO_PAGE_X, &CPU::LDA_zero_page_x);
	registerCallback(0xAD, "LDA", ADDR_ABSOLUTE, &CPU::LDA_absolute);
	registerCallback(0xBD, "LDA", ADDR_ABSOLUTE_X, &CPU::LDA_absolute_x);
	registerCallback(0xB9, "LDA", ADDR_ABSOLUTE_Y, &CPU::LDA_absolute_y);
	registerCallback(0xA1, "LDA", ADDR_INDIRECT_X, &CPU::LDA_indirect_x);
	registerCallback(0xB1, "LDA", ADDR_INDIRECT_Y, &CPU::LDA_indirect_y);

	registerCallback(0xA2, "LDX", ADDR_IMMEDIATE, &CPU::LDX_immediate);
	registerCallback(0xA6, "LDX", ADDR_ZERO_PAGE, &CPU::LDX_zero_page);
	registerCallback(0xB6, "LDX", ADDR_ZERO_PAGE_Y,&CPU::LDX_zero_page_y);
	registerCallback(0xAE, "LDX", ADDR_ABSOLUTE, &CPU::LDX_absolute);
	registerCallback(0xBE, "LDX", ADDR_ABSOLUTE_Y, &CPU::LDX_absolute_y);

	registerCallback(0xA0, "LDY", ADDR_IMMEDIATE, &CPU::LDY_immediate);
	registerCallback(0xA4, "LDY", ADDR_ZERO_PAGE, &CPU::LDY_zero_page);
	registerCallback(0xB4, "LDY", ADDR_ZERO_PAGE_X, &CPU::LDY_zero_page_x);
	registerCallback(0xAC, "LDY", ADDR_ABSOLUTE, &CPU::LDY_absolute);
	registerCallback(0xBC, "LDY", ADDR_ABSOLUTE_X, &CPU::LDY_absolute_x);
	
	registerCallback(0x4A, "LSR", ADDR_ACCUMULATOR, &CPU::LSR_accumulator);
	registerCallback(0x46, "LSR", ADDR_ZERO_PAGE, &CPU::LSR_zero_page);
	registerCallback(0x56, "LSR", ADDR_ZERO_PAGE_X, &CPU::LSR_zero_page_x);
	registerCallback(0x4E, "LSR", ADDR_ABSOLUTE, &CPU::LSR_absolute);
	registerCallback(0x5E, "LSR", ADDR_ABSOLUTE_X, &CPU::LSR_absolute_x);

	registerCallback(0xEA, "NOP", ADDR_IMPLIED, &CPU::NOP);
	
	registerCallback(0x09, "ORA", ADDR_IMMEDIATE, &CPU::ORA_immediate);
	registerCallback(0x05, "ORA", ADDR_ZERO_PAGE, &CPU::ORA_zero_page);
	registerCallback(0x15, "ORA", ADDR_ZERO_PAGE_X, &CPU::ORA_zero_page_x);
	registerCallback(0x0D, "ORA", ADDR_ABSOLUTE, &CPU::ORA_absolute);
	registerCallback(0x1D, "ORA", ADDR_ABSOLUTE_X, &CPU::ORA_absolute_x);
	registerCallback(0x19, "ORA", ADDR_ABSOLUTE_Y, &CPU::ORA_absolute_y);
	registerCallback(0x01, "ORA", ADDR_INDIRECT_X, &CPU::ORA_indirect_x);
	registerCallback(0x11, "ORA", ADDR_INDIRECT_Y, &CPU::ORA_indirect_y);

	registerCallback(0x48, "PHA", ADDR_IMPLIED, &CPU::PHA);
	registerCallback(0x08, "PHP", ADDR_IMPLIED, &CPU::PHP);
	registerCallback(0x68, "PLA", ADDR_IMPLIED, &CPU::PLA);
	registerCallback(0x28, "PLP", ADDR_IMPLIED, &CPU::PLP);

	registerCallback(0x2A, "ROL", ADDR_ACCUMULATOR, &CPU::ROL_accumulator);
	registerCallback(0x26, "ROL", ADDR_ZERO_PAGE, &CPU::ROL_zero_page);
	registerCallback(0x36, "ROL", ADDR_ZERO_PAGE_X, &CPU::ROL_zero_page_x);
	registerCallback(0x2E, "ROL", ADDR_ABSOLUTE, &CPU::ROL_absolute);
	registerCallback(0x3E, "ROL", ADDR_ABSOLUTE_X, &CPU::ROL_absolute_x);

	registerCallback(0x6A, "ROR", ADDR_ACCUMULATOR, &CPU::ROR_accumulator);
	registerCallback(0x66, "ROR", ADDR_ZERO_PAGE, &CPU::ROR_zero_page);
	registerCallback(0x76, "ROR", ADDR_ZERO_PAGE_X, &CPU::ROR_zero_page_x);
	registerCallback(0x6E, "ROR", ADDR_ABSOLUTE, &CPU::ROR_absolute);
	registerCallback(0x7E, "ROR", ADDR_ABSOLUTE_X, &CPU::ROR_absolute_x);
	
	registerCallback(0x40, "RTI", ADDR_IMPLIED, &CPU::RTI);
	registerCallback(0x60, "RTS", ADDR_IMPLIED, &CPU::RTS);

	registerCallback(0xE9, "SBC", ADDR_IMMEDIATE, &CPU::SBC_immediate);
	registerCallback(0xE5, "SBC", ADDR_ZERO_PAGE, &CPU::SBC_zero_page);
	registerCallback(0xF5, "SBC", ADDR_ZERO_PAGE_X, &CPU::SBC_zero_page_x);
	registerCallback(0xED, "SBC", ADDR_ABSOLUTE, &CPU::SBC_absolute);
	registerCallback(0xFD, "SBC", ADDR_ABSOLUTE_X, &CPU::SBC_absolute_x);
	registerCallback(0xF9, "SBC", ADDR_ABSOLUTE_Y, &CPU::SBC_absolute_y);
	registerCallback(0xE1, "SBC", ADDR_INDIRECT_X, &CPU::SBC_indirect_x);
	registerCallback(0xF1, "SBC", ADDR_INDIRECT_Y, &CPU::SBC_indirect_y);	

	registerCallback(0x38, "SEC", ADDR_IMPLIED, &CPU::SEC);	
	registerCallback(0xF8, "SED", ADDR_IMPLIED, &CPU::SED);	
	registerCallback(0x78, "SEI", ADDR_IMPLIED, &CPU::SEI);	

	registerCallback(0x85, "STA", ADDR_ZERO_PAGE, &CPU::STA_zero_page);
	registerCallback(0x95, "STA", ADDR_ZERO_PAGE_X, &CPU::STA_zero_page_x);
	registerCallback(0x8D, "STA", ADDR_ABSOLUTE, &CPU::STA_absolute);
	registerCallback(0x9D, "STA", ADDR_ABSOLUTE_X, &CPU::STA_absolute_x);
	registerCallback(0x99, "STA", ADDR_ABSOLUTE_Y, &CPU::STA_absolute_y);
	registerCallback(0x81, "STA", ADDR_INDIRECT_X, &CPU::STA_indirect_x);
	registerCallback(0x91, "STA", ADDR_INDIRECT_Y, &CPU::STA_indirect_y);

	registerCallback(0x86, "STX", ADDR_ZERO_PAGE, &CPU::STX_zero_page);
	registerCallback(0x96, "STX", ADDR_ZERO_PAGE_Y, &CPU::STX_zero_page_y);
	registerCallback(0x8E, "STX", ADDR_ABSOLUTE, &CPU::STX_absolute);

	registerCallback(0x84, "STY", ADDR_ZERO_PAGE, &CPU::STY_zero_page);
	registerCallback(0x94, "STY", ADDR_ZERO_PAGE_X, &CPU::STY_zero_page_x);
	registerCallback(0x8C, "STY", ADDR_ABSOLUTE, &CPU::STY_absolute);

	registerCallback(0xAA, "TAX", ADDR_IMPLIED, &CPU::TAX);	
	registerCallback(0xA8, "TAY", ADDR_IMPLIED, &CPU::TAY);	
	registerCallback(0xBA, "TSX", ADDR_IMPLIED, &CPU::TSX);	
	registerCallback(0x8A, "TXA", ADDR_IMPLIED, &CPU::TXA);	
	registerCallback(0x9A, "TXS", ADDR_IMPLIED, &CPU::TXS);	
	registerCallback(0x98, "TYA", ADDR_IMPLIED, &CPU::TYA);	

	// Register illegal instructions
	registerIllegalInstructions();	
}

int CPU::defaultCallback()
{
	// printf("Illegal Opcode at Address %04x (Opcode = %02x)\n", getPC(), mem->peek(getPC()));
	setErrorState(ILLEGAL_INSTRUCTION);
	return 1;
}

// Fetch Operands


inline void CPU::fetchOperandImplied()
{
	//  #  address R/W description
	// --- ------- --- -----------------------------------------------
	//  1    PC     R  fetch opcode, increment PC
	PC++;
	//  2    PC     R  read next instruction byte (and throw it away)
}

inline void CPU::fetchOperandAccumulator()
{
	//  #  address R/W description
	// --- ------- --- -----------------------------------------------
	//  1    PC     R  fetch opcode, increment PC
	PC++;
	//  2    PC     R  read next instruction byte (and throw it away)
}

inline uint8_t CPU::fetchOperandImmediate()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	//  1    PC     R  fetch opcode, increment PC
	PC++;
	//  2    PC     R  fetch value, increment PC
	return mem->peek(PC++);
}

inline uint16_t CPU::fetchAddressZeroPage()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	//  1    PC     R  fetch opcode, increment PC
	PC++;
	//  2    PC     R  fetch address, increment PC	
	uint8_t lo = mem->peek(PC++);
	return (uint16_t)lo;
}
inline uint8_t CPU::fetchOperandZeroPage()
{
	return mem->peek(fetchAddressZeroPage());
}

inline uint16_t CPU::fetchAddressZeroPageX()
{
	//  #   address  R/W description
	// --- --------- --- ------------------------------------------
	//  1     PC      R  fetch opcode, increment PC
	PC++;
	//  2     PC      R  fetch address, increment PC
	uint8_t lo = mem->peek(PC++);
	//  3   address   R  read from address, add index register to it
	lo += X;
	//      Note: The high byte of the effective address is always zero,
	//            i.e. page boundary crossings are not handled.
	return (uint16_t)lo; 
}
inline uint8_t CPU::fetchOperandZeroPageX()
{
	return mem->peek(fetchAddressZeroPageX());
}


inline uint16_t CPU::fetchAddressZeroPageY()
{
	//  #   address  R/W description
	// --- --------- --- ------------------------------------------
	//  1     PC      R  fetch opcode, increment PC
	PC++;
	//  2     PC      R  fetch address, increment PC
	uint8_t lo = mem->peek(PC++);
	//  3   address   R  read from address, add index register to it
	lo += Y;
	//      Note: The high byte of the effective address is always zero,
	//            i.e. page boundary crossings are not handled.
	return (uint16_t)lo;
}
inline uint8_t CPU::fetchOperandZeroPageY()
{
	return mem->peek(fetchAddressZeroPageY());
}


inline uint16_t CPU::fetchAddressAbsolute()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	//	1    PC     R  fetch opcode, increment PC
	PC++;
	//	2    PC     R  fetch low byte of address, increment PC
	//	3    PC     R  fetch high byte of address, increment PC
	uint16_t addr = mem->peekWord(PC); PC += 2;
	return addr;
}
inline uint8_t CPU::fetchOperandAbsolute()
{
	return mem->peek(fetchAddressAbsolute());
}

inline uint16_t CPU::fetchAddressAbsoluteX()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	// 1     PC      R  fetch opcode, increment PC
	PC++;
	// 2     PC      R  fetch low byte of address, increment PC
	// 3     PC      R  fetch high byte of address, add index register to low address byte, increment PC
	uint16_t addr = mem->peekWord(PC) + X; PC += 2;
	return addr;
}
inline uint8_t CPU::fetchOperandAbsoluteX()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	uint16_t addr = fetchAddressAbsoluteX();
	// 4  address+X* R  read from effective address, fix the high byte of effective address	
	// 5+ address+X  R  re-read from effective address
	//
	// * The high byte of the effective address may be invalid at this time, 
	//   i.e. it may be smaller by $100.
	//
	// + This cycle will be executed only if the effective address was invalid 
	//   during cycle #4, i.e. page boundary was crossed.
	if (HI_BYTE(PC) != HI_BYTE(addr)) // Page boundary crossed
		cycles++; 
	return mem->peek(addr);
}

inline uint16_t CPU::fetchAddressAbsoluteY()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	// 1     PC      R  fetch opcode, increment PC
	PC++;
	// 2     PC      R  fetch low byte of address, increment PC
	// 3     PC      R  fetch high byte of address, add index register to low address byte, increment PC
	uint16_t addr = mem->peekWord(PC) + Y; PC += 2;
	return addr;
}
inline uint8_t CPU::fetchOperandAbsoluteY()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	// 4  address+X* R  read from effective address, fix the high byte of effective address	
	// 5+ address+X  R  re-read from effective address
	uint16_t addr = fetchAddressAbsoluteY();
	//
	// * The high byte of the effective address may be invalid at this time, 
	//   i.e. it may be smaller by $100.
	//
	// + This cycle will be executed only if the effective address was invalid 
	//   during cycle #4, i.e. page boundary was crossed.
	if (HI_BYTE(PC) != HI_BYTE(addr)) // Page boundary crossed
		cycles++; 
	return mem->peek(addr);
}

inline uint16_t CPU::fetchAddressIndirectX()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	//	1      PC       R  fetch opcode, increment PC
	PC++;
	//	2      PC       R  fetch pointer address, increment PC
	uint8_t ptr = mem->peek(PC++);
	//	3    pointer    R  read from the address, add X to it
	ptr += X;
	//	4   pointer+X   R  fetch effective address low
	uint8_t lo = mem->peek(ptr, 0);
	//	5  pointer+X+1  R  fetch effective address high
	ptr++;
	uint8_t hi = mem->peek(ptr, 0);
	//  Note: The effective address is always fetched from zero page,
	//	i.e. the zero page boundary crossing is not handled.	
	return (hi << 8) + lo;
}
inline uint8_t CPU::fetchOperandIndirectX()
{
	return mem->peek(fetchAddressIndirectX());
}

inline uint16_t CPU::fetchAddressIndirectY()
{
	//  #  address R/W description
	// --- ------- --- ------------------------------------------
	//  1      PC       R  fetch opcode, increment PC
	PC++;
	//  2      PC       R  fetch pointer address, increment PC
	uint8_t ptr = mem->peek(PC++);
	//  3    pointer    R  fetch effective address low
	uint8_t lo = mem->peek(ptr, 0);
	//  4   pointer+1   R  fetch effective address high, add Y to low byte of effective address
	ptr++;
	uint8_t hi = mem->peek(ptr, 0);
	// Notes: The effective address is always fetched from zero page,
	//        i.e. the zero page boundary crossing is not handled.
	return (hi << 8) + lo + Y;
}

inline uint8_t CPU::fetchOperandIndirectY()
{
	return mem->peek(fetchAddressIndirectY());
}

inline uint8_t CPU::fetchOperandRelative()
{
	// 1     PC      R  fetch opcode, increment PC
	PC++;
	// 2     PC      R  fetch operand, increment PC
	return mem->peek(PC++);
}



// -------------------------------------------------------------------------------
// Instruction: ADC
//
// Operation:   A,C := A+M+C
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

inline void CPU::adc(uint8_t op) 
{
	if (getD()) 
		adc_bcd(op);
	else
		adc_binary(op);
}

inline void CPU::adc_binary(uint8_t op) 
{
	uint16_t sum = A + op + (getC() ? 1 : 0);

	setC(sum > 255);
	setV(!((A ^ op) & 0x80) && ((A ^ sum) & 0x80));
	loadA((uint8_t)sum);
}

inline void CPU::adc_bcd(uint8_t op) 
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
		
int CPU::ADC_immediate()
{
	uint8_t op = fetchOperandImmediate();
	adc(op);
	return 2;
}

int CPU::ADC_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	adc(op);
	return 3;
}

int CPU::ADC_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	adc(op);
	return 4;
}

int CPU::ADC_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	adc(op);
	return 4;
}

int CPU::ADC_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	adc(op);
	return 5;
}

int CPU::ADC_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	adc(op);
	return 5;
}

int CPU::ADC_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	adc(op);
	return 6;
}


int CPU::ADC_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	adc(op);
	return 5;
}

// -------------------------------------------------------------------------------
// Instruction: AND
//
// Operation:   A := A AND M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::AND_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadA(A & op);
	return 2;
}

int CPU::AND_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadA(A & op);
	
	return 4;
}

int CPU::AND_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadA(A & op);
	return 3;
}

int CPU::AND_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	loadA(A & op);
	return 4;
}

int CPU::AND_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	loadA(A & op);
	return 5;
}

int CPU::AND_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	loadA(A & op);
	return 5;
}

int CPU::AND_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	loadA(A & op);
	return 6;
}

int CPU::AND_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	loadA(A & op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: ASL
//
// Operation:   C <- (A|M << 1) <- 0
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::ASL_accumulator()
{
	fetchOperandAccumulator();
	setC(A & 128);
	loadA(A << 1);
	return 2;
}

int CPU::ASL_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
	return 5;
}

int CPU::ASL_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
	return 6;
}

int CPU::ASL_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
	return 6;
}

int CPU::ASL_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
	return 7;
}

void CPU::ASL_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
}

void CPU::ASL_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
}

void CPU::ASL_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	loadM(addr, op << 1);
}


// -------------------------------------------------------------------------------
// Instruction: BCC
//
// Operation:   Branch on C = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

inline int CPU::branch(int8_t offset)
{
	int cycles = 1;
	
	uint8_t hi = HI_BYTE(PC);
	PC += offset;
	if(HI_BYTE(PC) != hi) {
		// page boundary crossed, we add one penalty cycle
		cycles++; 
	}

	return cycles;
}

int CPU::BCC_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (!getC()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BCS
//
// Operation:   Branch on C = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BCS_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (getC()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BEQ
//
// Operation:   Branch on Z = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BEQ_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (getZ()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BIT
//
// Operation:   A AND M, N := M7, V := M6
//
// Flags:       N Z C I D V
//              / / - - - /
// -------------------------------------------------------------------------------

int CPU::BIT_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	setN(op & 128);
	setV(op & 64);
	setZ((op & A) == 0);
	return 3;
}

int CPU::BIT_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	setN(op & 128);
	setV(op & 64);
	setZ((op & A) == 0);
	return 4;
}


// -------------------------------------------------------------------------------
// Instruction: BMI
//
// Operation:   Branch on N = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BMI_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (getN()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BNE
//
// Operation:   Branch on Z = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BNE_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (!getZ()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BPL
//
// Operation:   Branch on N = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BPL_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (!getN()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BRK
//
// Operation:   Forced Interrupt (Break)
//
// Flags:       N Z C I D V    B
//              - - - 1 - -    1
// -------------------------------------------------------------------------------

int CPU::BRK()
{
	uint16_t pc_addr;

	fetchOperandImplied();
	// 3  $0100,S  W  push PCH on stack (with B flag set), decrement S
	setB(1); 
	pc_addr = PC+1;
	mem->poke(0x100+(SP--), HI_BYTE(pc_addr));
	// 4  $0100,S  W  push PCL on stack, decrement S
	mem->poke(0x100+(SP--), LO_BYTE(pc_addr));
	// 5  $0100,S  W  push P on stack, decrement S
	mem->poke(0x100+(SP--), getP());
	// 6   $FFFE   R  fetch PCL	
	// 7   $FFFF   R  fetch PCH
	setPCL(mem->peek(0xFFFE));
	setPCH(mem->peek(0xFFFF));	
	setI(1);

	return 7;
}


// -------------------------------------------------------------------------------
// Instruction: BVC
//
// Operation:   Branch on V = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BVC_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (!getV()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: BVS
//
// Operation:   Branch on V = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::BVS_relative()
{
	int cycles = 2;
	
	uint8_t op = fetchOperandRelative();
	if (getV()) cycles += branch(op);
	return cycles;
}


// -------------------------------------------------------------------------------
// Instruction: CLC
//
// Operation:   C := 0
//
// Flags:       N Z C I D V
//              - - 0 - - -
// -------------------------------------------------------------------------------

int CPU::CLC()
{
	fetchOperandImplied();
	setC(0);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: CLD
//
// Operation:   D := 0
//
// Flags:       N Z C I D V
//              - - - - 0 -
// -------------------------------------------------------------------------------

int CPU::CLD()
{
	fetchOperandImplied();
	setD(0);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: CLI
//
// Operation:   I := 0
//
// Flags:       N Z C I D V
//              - - - 0 - -
// -------------------------------------------------------------------------------

int CPU::CLI()
{
	fetchOperandImplied();
	setI(0);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: CLV
//
// Operation:   V := 0
//
// Flags:       N Z C I D V
//              - - - - - 0
// -------------------------------------------------------------------------------

int CPU::CLV()
{
	fetchOperandImplied();
	setV(0);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: CMP
//
// Operation:   A-M
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

inline void CPU::cmp(uint8_t op1, uint8_t op2) 
{
	uint8_t tmp = op1 - op2;
	
	setC(op1 >= op2);
	setN(tmp & 128);
	setZ(tmp == 0);
}

int CPU::CMP_immediate()
{
	uint8_t op = fetchOperandImmediate();
	cmp(A, op);
	return 2;
}

int CPU::CMP_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	cmp(A, op);
	return 3;
}

int CPU::CMP_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	cmp(A, op);
	return 4;
}

int CPU::CMP_absolute()
{ 
	uint8_t op = fetchOperandAbsolute();
	cmp(A, op);
	return 4;
}

int CPU::CMP_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	cmp(A, op);
	return 5;
}

int CPU::CMP_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	cmp(A, op);
	return 5;
}

int CPU::CMP_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	cmp(A, op);
	return 6;
}

int CPU::CMP_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	cmp(A, op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: CPX
//
// Operation:   X-M
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::CPX_immediate()
{
	uint8_t op = fetchOperandImmediate();
	cmp(X, op);
	return 2;
}

int CPU::CPX_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	cmp(X, op);
	return 3;
}

int CPU::CPX_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	cmp(X, op);
	return 4;
}


// -------------------------------------------------------------------------------
// Instruction: CPY
//
// Operation:   Y-M
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::CPY_immediate()
{
	uint8_t op = fetchOperandImmediate();
	cmp(Y, op);
	return 2;
}

int CPU::CPY_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	cmp(Y, op);
	return 4;
}

int CPU::CPY_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	cmp(Y, op);
	return 3;
}

// -------------------------------------------------------------------------------
// Instruction: DEC
//
// Operation:   M := : M - 1 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::DEC_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	return 5;
}

int CPU::DEC_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	return 6;
}

int CPU::DEC_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	return 6;
}

int CPU::DEC_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	return 7;
}

void CPU::DEC_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
}
	
void CPU::DEC_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
}

void CPU::DEC_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
}


// -------------------------------------------------------------------------------
// Instruction: DEX
//
// Operation:   X := X - 1 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::DEX()
{
	fetchOperandImplied();
	uint8_t op = getX()-1;
	loadX(op);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: DEY
//
// Operation:   Y := Y - 1 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::DEY()
{
	fetchOperandImplied();
	uint8_t op = getY()-1;
	loadY(op);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: EOR
//
// Operation:   A := A XOR M 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::EOR_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadA(A ^ op);
	return 2;
}

int CPU::EOR_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadA(A ^ op);
	return 3;
}

int CPU::EOR_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	loadA(A ^ op);
	return 4;
}

int CPU::EOR_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadA(A ^ op);	
	return 4;
}

int CPU::EOR_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	loadA(A ^ op);
	return 5;
}

int CPU::EOR_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	loadA(A ^ op);
	return 5;
}

int CPU::EOR_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	loadA(A ^ op);
	return 6;
}

int CPU::EOR_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	loadA(A ^ op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: INC
//
// Operation:   M := M + 1
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::INC_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	return 5;
}

int CPU::INC_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	return 6;
}

int CPU::INC_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	return 6;
}

int CPU::INC_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	return 7;
}

void CPU::INC_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
}

void CPU::INC_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
}

void CPU::INC_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
}


// -------------------------------------------------------------------------------
// Instruction: INX
//
// Operation:   X := X + 1
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::INX()
{
	fetchOperandImplied();
	uint8_t op = X+1;
	loadX(op);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: INY
//
// Operation:   Y := Y + 1
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::INY()
{
	fetchOperandImplied();
	uint8_t op = Y+1;
	loadY(op);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: JMP
//
// Operation:   PC := Operand
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------


int CPU::JMP_absolute()
{
	//	1    PC     R  fetch opcode, increment PC
	PC++;
	//	2    PC     R  fetch low address byte, increment PC
	uint8_t lo = mem->peek(PC++);
	//	3    PC     R  copy low address byte to PCL, fetch high address byte to PCH
	setPCH(mem->peek(PC)); setPCL(lo);
	return 3;
}

int CPU::JMP_absolute_indirect()
{
	//  1     PC      R  fetch opcode, increment PC
	PC++;
	//  2     PC      R  fetch pointer address low, increment PC
	uint8_t pc_lo = mem->peek(PC++);
	//  3     PC      R  fetch pointer address high, increment PC
	uint8_t pc_hi = mem->peek(PC++);
	//  4   pointer   R  fetch low address to latch
	uint8_t latch = mem->peek(pc_lo, pc_hi);
	//  5  pointer+1* R  fetch PCH, copy latch to PCL
	// Note: * The PCH will always be fetched from the same page
	// than PCL, i.e. page boundary crossing is not handled.
	setPCH(mem->peek(pc_lo+1, pc_hi)); setPCL(latch);	
	
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: JSR
//
// Operation:   PC to stack, PC := Operand
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::JSR()
{
	callStack[callStackPointer++] = PC;
	
	//  1    PC     R  fetch opcode, increment PC
	PC++;
	//  2    PC     R  fetch low address byte, increment PC
	uint8_t lo = mem->peek(PC++);
	//  3  $0100,S  R  internal operation (predecrement S?)
	//  4  $0100,S  W  push PCH on stack, decrement S
	mem->poke(0x100+SP--, HI_BYTE(PC));
	//  5  $0100,S  W  push PCL on stack, decrement S
	mem->poke(0x100+SP--, LO_BYTE(PC));
	//  6    PC     R  copy low address byte to PCL, fetch high address byte to PCH
	setPCH(mem->peek(PC)); setPCL(lo);

	return 6;
}


// -------------------------------------------------------------------------------
// Instruction: LDA
//
// Operation:   A := M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::LDA_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadA(op);
	return 2;
}

int CPU::LDA_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadA(op);
	return 3;
}

int CPU::LDA_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	loadA(op);
	return 4;
}

void CPU::LDA_zero_page_y()
{
	uint8_t op = fetchOperandZeroPageY();
	loadA(op);
}

int CPU::LDA_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadA(op);
	
	return 4;
}

int CPU::LDA_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	loadA(op);
	return 5;
}

int CPU::LDA_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	loadA(op);
	return 5;
}

int CPU::LDA_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	loadA(op);
	return 6;
}

int CPU::LDA_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	loadA(op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: LDX
//
// Operation:   X := M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::LDX_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadX(op);
	return 2;
}

int CPU::LDX_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadX(op);
	return 3;
}

int CPU::LDX_zero_page_y()
{
	uint8_t op = fetchOperandZeroPageY();
	loadX(op);
	return 4;
}

int CPU::LDX_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadX(op);
	
	return 4;
}

int CPU::LDX_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	loadX(op);
	return 5;
}

void CPU::LDX_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	loadX(op);
}

void CPU::LDX_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	loadX(op);
}


// -------------------------------------------------------------------------------
// Instruction: LDY
//
// Operation:   Y := M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::LDY_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadY(op);
	return 2;
}

int CPU::LDY_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadY(op);
	return 3;
}

int CPU::LDY_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	loadY(op);
	return 4;
}

int CPU::LDY_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadY(op);
	
	return 4;
}

int CPU::LDY_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	loadY(op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: LSR
//
// Operation:   0 -> (A|M >> 1) -> C
//
// Flags:       N Z C I D V
//              0 / / - - -
// -------------------------------------------------------------------------------

int CPU::LSR_accumulator()
{
	fetchOperandAccumulator();
	setC(A & 1);
	loadA(A >> 1);
	return 2;
}

int CPU::LSR_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);
	return 5;
}

int CPU::LSR_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);
	return 6;
}

int CPU::LSR_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);	
	return 6;
}

int CPU::LSR_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);
	return 7;
}

void CPU::LSR_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);
}

void CPU::LSR_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);
}

void CPU::LSR_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op   = mem->peek(addr);
	setC(op & 1);
	loadM(addr, op >> 1);
}


// -------------------------------------------------------------------------------
// Instruction: NOP
//
// Operation:   No operation
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::NOP()
{
	fetchOperandImplied();
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: A := A v M
//
// Operation:   No operation
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::ORA_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadA(A | op);
	return 2;
}

int CPU::ORA_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadA(A | op);
	return 3;
}

int CPU::ORA_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	loadA(A | op);
	return 4;
}

int CPU::ORA_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadA(A | op);
	return 4;
}

int CPU::ORA_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	loadA(A | op);
	return 5;
}

int CPU::ORA_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	loadA(A | op);
	return 5;
}

int CPU::ORA_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	loadA(A | op);
	return 6;
}

int CPU::ORA_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	loadA(A | op);
	return 5;
}

// -------------------------------------------------------------------------------
// Instruction: PHA
//
// Operation:   A to stack
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::PHA()
{
	fetchOperandImplied();
	//  3  $0100,S  W  push register on stack, decrement S
	mem->poke(0x100+SP, A); 	
	SP--;
	return 3;
}


// -------------------------------------------------------------------------------
// Instruction: PHA
//
// Operation:   P to stack
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::PHP()
{
	fetchOperandImplied();	
	//  3  $0100,S  W  push register on stack, decrement S
	mem->poke(0x100+SP, getP());
	SP--;
	return 3;
}


// -------------------------------------------------------------------------------
// Instruction: PLA
//
// Operation:   Stack to A
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::PLA()
{
	fetchOperandImplied();
	//  4  $0100,S  R  pull register from stack
	SP++;
	//A = mem->peek(0x100+SP);
	loadA(mem->peek(0x100+SP));
	return 4;
}

// -------------------------------------------------------------------------------
// Instruction: PLP
//
// Operation:   Stack to p
//
// Flags:       N Z C I D V
//              / / / / / /
// -------------------------------------------------------------------------------

int CPU::PLP()
{
	fetchOperandImplied();
	//  4  $0100,S  R  pull register from stack
	SP++;
	setPWithoutB(mem->peek(0x100+SP));
	return 4;
}


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

inline uint8_t CPU::rol(uint8_t op)
{
	uint8_t bit8    = (op & 128);
	uint8_t shifted = (op << 1) + (getC() ? 1 : 0); 
	setC(bit8);
	return shifted;
}	

int CPU::ROL_accumulator()
{
	fetchOperandAccumulator();
	loadA(rol(A));
	return 2;
}

int CPU::ROL_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	return 5;
}

int CPU::ROL_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	return 5;
}

int CPU::ROL_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	return 6;
}

int CPU::ROL_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	return 7;
}

#if 0
void CPU::ROL_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
}

void CPU::ROL_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
}

void CPU::ROL_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
}
#endif

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

inline uint8_t CPU::ror(uint8_t op)
{
	uint8_t bit1    = (op & 1);
	uint8_t shifted = (op >> 1) + (getC() ? 128 : 0); 
	setC(bit1);
	return shifted;
}	

int CPU::ROR_accumulator()
{
	fetchOperandAccumulator();
	loadA(ror(A));
	return 2;
}

int CPU::ROR_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	return 5;
}

int CPU::ROR_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	return 6;
}

int CPU::ROR_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	return 6;
}

int CPU::ROR_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	return 7;
}

#if 0
void CPU::ROR_absolute_y()
{
	uint16_t addr   = fetchAddressAbsoluteY();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
}

void CPU::ROR_indirect_x()
{
	uint16_t addr   = fetchAddressIndirectX();
	uint8_t op      = mem->peek(addr);
	uint8_t bit1    = (op & 1);
	uint8_t shifted = (op >> 1) + (getC() ? 128 : 0); 
	setC(bit1);
	loadM(addr, shifted);
}

void CPU::ROR_indirect_y()
{
	uint16_t addr   = fetchAddressIndirectY();
	uint8_t op      = mem->peek(addr);
	uint8_t bit1    = (op & 1);
	uint8_t shifted = (op >> 1) + (getC() ? 128 : 0); 
	setC(bit1);
	loadM(addr, shifted);
}
#endif

// -------------------------------------------------------------------------------
// Instruction: RTI
//
// Operation:   P from Stack, PC from Stack
//
// Flags:       N Z C I D V
//              / / / / / /
// -------------------------------------------------------------------------------

int CPU::RTI() {
	
	fetchOperandImplied();
	//	3  $0100,SP  R  increment SP
	SP++;
	//	4  $0100,SP  R  pull P from stack, increment SP
	setPWithoutB(mem->peek(0x100+SP++));
	//	5  $0100,SP  R  pull PCL from stack, increment SP
	setPCL(mem->peek(0x100+SP++));
	//	6    PC      R  pull PCH from stack
	setPCH(mem->peek(0x100+SP));
		
	return 6;
}


// -------------------------------------------------------------------------------
// Instruction: RTS
//
// Operation:   PC from Stack
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::RTS() {
	
	fetchOperandImplied();
	//	3  $0100,S  R  increment S
	SP++;
	//	4  $0100,S  R  pull PCL from stack, increment S
	setPCL(mem->peek(0x100+SP++));
	//	5  $0100,S  R  pull PCH from stack
	setPCH(mem->peek(0x100+SP));
	//	6    PC     R  increment PC
	PC++;

	callStackPointer--;
	return 6;
}


// -------------------------------------------------------------------------------
// Instruction: SBC
//
// Operation:   A := A - M - (~C)
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

inline void CPU::sbc(uint8_t op)
{
	if (getD()) 
		sbc_bcd(op);
	else
		sbc_binary(op);
}

inline void CPU::sbc_binary(uint8_t op) 
{
	uint16_t sum = A - op - (getC() ? 0 : 1);

	setC(sum <= 255);
	setV(((A ^ sum) & 0x80) && ((A ^ op) & 0x80));
	loadA((uint8_t)sum);
}

inline void CPU::sbc_bcd(uint8_t op) 
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

int CPU::SBC_immediate()
{
	uint8_t op = fetchOperandImmediate();
	sbc(op);
	return 2;
}

int CPU::SBC_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	sbc(op);
	return 3;
}

int CPU::SBC_zero_page_x()
{
	uint8_t op = fetchOperandZeroPageX();
	sbc(op);
	return 4;
}

int CPU::SBC_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	sbc(op);
	return 4;
}

int CPU::SBC_absolute_x()
{
	uint8_t op = fetchOperandAbsoluteX();
	sbc(op);
	return 5;
}

int CPU::SBC_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	sbc(op);
	return 5;
}

int CPU::SBC_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	sbc(op);
	return 6;
}

int CPU::SBC_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	sbc(op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: SEC
//
// Operation:   C := 1
//
// Flags:       N Z C I D V
//              - - 1 - - -
// -------------------------------------------------------------------------------

int CPU::SEC()
{
	fetchOperandImplied();
	setC(1);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: SED
//
// Operation:   D := 1
//
// Flags:       N Z C I D V
//              - - - - 1 -
// -------------------------------------------------------------------------------

int CPU::SED()
{
	fetchOperandImplied();
	setD(1);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: SEI
//
// Operation:   D := 1
//
// Flags:       N Z C I D V
//              - - - 1 - -
// -------------------------------------------------------------------------------

int CPU::SEI()
{
	fetchOperandImplied();
	setI(1);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: STA
//
// Operation:   M := A
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::STA_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	mem->poke(addr, A);
	return 4;
}

int CPU::STA_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	mem->poke(addr, A);
	return 3;
}

int CPU::STA_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	mem->poke(addr, A);
	return 4;
}

int CPU::STA_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	mem->poke(addr, A);
	return 5;
}

int CPU::STA_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	mem->poke(addr, A);
	return 5;
}

int CPU::STA_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	mem->poke(addr, A);
	return 6;
}

int CPU::STA_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	mem->poke(addr, A);
	return 6;
}


// -------------------------------------------------------------------------------
// Instruction: STX
//
// Operation:   M := X
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::STX_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	mem->poke(addr, X);
	return 4;
}

int CPU::STX_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	mem->poke(addr, X);
	return 3;
}

int CPU::STX_zero_page_y()
{
	uint16_t addr = fetchAddressZeroPageY();
	mem->poke(addr, X);
	return 4;
}


// -------------------------------------------------------------------------------
// Instruction: STY
//
// Operation:   M := Y
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::STY_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	mem->poke(addr, Y);
	return 4;
}

int CPU::STY_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	mem->poke(addr, Y);
	return 3;
}

int CPU::STY_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	mem->poke(addr, Y);
	return 4;
}


// -------------------------------------------------------------------------------
// Instruction: TAX
//
// Operation:   X := A
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::TAX()
{
	fetchOperandImplied();
	loadX(A);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: TAY
//
// Operation:   Y := A
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::TAY()
{
	fetchOperandImplied();
	loadY(A);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: TSX
//
// Operation:   X := Stack pointer
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::TSX()
{
	fetchOperandImplied();
	loadX(SP);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: TXA
//
// Operation:   A := X
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::TXA()
{
	fetchOperandImplied();
	loadA(X);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: TXS
//
// Operation:   Stack pointer := X
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::TXS()
{
	fetchOperandImplied();
	SP = X;
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: TYA
//
// Operation:   A := Y
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::TYA()
{
	fetchOperandImplied();
	loadA(Y);
	return 2;
}


// -------------------------------------------------------------------------------
// Illegal instructions
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
// Instruction: AHX
//
// Operation:   Mem := A & X & (M + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::AHX_indirect_y()
{
	uint8_t  value = mem->peek(mem->peek(PC+1) + 1);
	uint16_t addr  = fetchAddressIndirectY();
	mem->poke(addr, A & X & (value + 1));
	return 6;
}

int CPU::AHX_absolute_y()
{
	uint8_t  value = mem->peek(PC+2);
	uint16_t addr  = fetchAddressAbsoluteY();
	mem->poke(addr, A & X & (value + 1));
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: ALR
//
// Operation:   AND, followed by LSR
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::ALR_immediate()
{
	uint8_t op = fetchOperandImmediate();
	A = A & op;
	setC(A & 1);
	loadA(A >> 1);
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: ANC
//
// Operation:   A := A & op,   N flag is copied to C
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::ANC_immediate()
{
	AND_immediate();
	setC(getN());
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: ARR
//
// Operation:   AND, followed by ROR
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

int CPU::ARR_immediate()
{
	uint8_t op = fetchOperandImmediate();
	uint8_t tmp2 = A & op;

#if 0
	uint8_t result = ror(anded);
	loadA(result);
#endif
	
	
	// Adapted from Frodo
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
			A = A & 0xf0 | (A + 6) & 0x0f;
		c_flag = (tmp2 + (tmp2 & 0x10)) & 0x1f0;
		if (c_flag > 0x50) {
			setC(1);
			A += 0x60;
		} else {
			setC(0);
		}
	}
	return 2;
}


// -------------------------------------------------------------------------------
// Instruction: AXS
//
// Operation:   X = (A & X) - op
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::AXS_immediate()
{
	uint8_t op1  = A & X;
	uint8_t op2  = fetchOperandImmediate();
	uint8_t tmp = op1 - op2; 

	setC(op1 >= op2);
	loadX(tmp);
	return 2;
}

// -------------------------------------------------------------------------------
// Instruction: DCP
//
// Operation:   DEC followed by CMP
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::DCP_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 5;
}

int CPU::DCP_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 6;
}

int CPU::DCP_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 6;
}

int CPU::DCP_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 7;
}

int CPU::DCP_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 7;
}

int CPU::DCP_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 8;
}

int CPU::DCP_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr) - 1;
	loadM(addr, op);
	cmp(A, op);
	return 8;
}


// -------------------------------------------------------------------------------
// Instruction: ISC
//
// Operation:   INC followed by SBC
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

int CPU::ISC_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 5;
}

int CPU::ISC_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 6;
}

int CPU::ISC_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 6;
}

int CPU::ISC_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 7;
}

int CPU::ISC_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 7;
}

int CPU::ISC_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 8;
}

int CPU::ISC_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr) + 1;
	loadM(addr, op);
	sbc(op);
	return 8;
}

// -------------------------------------------------------------------------------
// Instruction: LAS
//
// Operation:   SP,X,A = op & SP
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::LAS_absolute_y()
{
	uint8_t value = fetchOperandAbsoluteY() & SP;
	SP = value;
	X  = value;
	loadA(value);
	return 4;
}


// -------------------------------------------------------------------------------
// Instruction: LAX
//
// Operation:   LDA, followed by LDX
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::LAX_immediate()
{
	uint8_t op = (A | 0xEE) & fetchOperandImmediate();
	loadA(op);
	loadX(op);
	return 2;	
}

int CPU::LAX_zero_page()
{
	uint8_t op = fetchOperandZeroPage();
	loadA(op);
	loadX(op);
	return 3;
}

int CPU::LAX_zero_page_y()
{
	uint8_t op = fetchOperandZeroPageY();
	loadA(op);
	loadX(op);
	return 4;
}

int CPU::LAX_absolute()
{
	uint8_t op = fetchOperandAbsolute();
	loadA(op);
	loadX(op);
	return 4;
}

int CPU::LAX_absolute_y()
{
	uint8_t op = fetchOperandAbsoluteY();
	loadA(op);
	loadX(op);
	return 4;
}

int CPU::LAX_indirect_x()
{
	uint8_t op = fetchOperandIndirectX();
	loadA(op);
	loadX(op);
	return 6;
}

int CPU::LAX_indirect_y()
{
	uint8_t op = fetchOperandIndirectY();
	loadA(op);
	loadX(op);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: NOP
//
// Operation:   Consume CPU cycles by fetching and discarding an operand
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::NOP_immediate()
{
	(void)fetchOperandImmediate();
	return 2;
}

int CPU::NOP_zero_page()
{
	(void)fetchOperandZeroPage();
	return 3;
}

int CPU::NOP_zero_page_x()
{
	(void)fetchOperandZeroPageX();
	return 4;
}

int CPU::NOP_absolute()
{
	(void)fetchOperandAbsolute();
	return 4;
}

int CPU::NOP_absolute_x()
{
	(void)fetchOperandAbsoluteX();
	return 4;
}


// -------------------------------------------------------------------------------
// Instruction: RLA
//
// Operation:   ROL, followed by AND
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::RLA_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 5;
}

int CPU::RLA_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 6;
}

int CPU::RLA_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 6;
}

int CPU::RLA_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 7;
}

int CPU::RLA_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 7;
}

int CPU::RLA_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 8;
}

int CPU::RLA_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t value = rol(mem->peek(addr));
	loadM(addr, value);
	loadA(A & value);
	return 8;
}


// -------------------------------------------------------------------------------
// Instruction: RRA
//
// Operation:   ROR, followed by ADC
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

int CPU::RRA_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 6;
}

int CPU::RRA_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 7;
}

int CPU::RRA_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 7;
}

int CPU::RRA_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 5;
}

int CPU::RRA_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 6;
}

int CPU::RRA_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 8;
}

int CPU::RRA_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	return 8;
}


// -------------------------------------------------------------------------------
// Instruction: SAX
//
// Operation:   Mem := A & X
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::SAX_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	mem->poke(addr, A & X);
	return 3;	
}

int CPU::SAX_zero_page_y()
{
	uint16_t addr = fetchAddressZeroPageY();
	mem->poke(addr, A & X);
	return 4;	
}

int CPU::SAX_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	mem->poke(addr, A & X);
	return 4;	
}

int CPU::SAX_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	mem->poke(addr, A & X);
	return 6;	
}


// -------------------------------------------------------------------------------
// Instruction: SHX
//
// Operation:   Mem := X & (HI_BYTE(op) + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::SHX_absolute_y()
{
	uint8_t  value = mem->peek(PC+2) + 1;
	uint16_t addr  = fetchAddressAbsoluteY();
	mem->poke(addr, X & value);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: SHY
//
// Operation:   Mem := Y & (HI_BYTE(op) + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

int CPU::SHY_absolute_x()
{
	uint8_t  value = mem->peek(PC+2) + 1;
	uint16_t addr  = fetchAddressAbsoluteX();
	mem->poke(addr, Y & value);
	return 5;
}


// -------------------------------------------------------------------------------
// Instruction: SLO
//
// Operation:   ASL memory location, followed by OR on accumulator
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::SLO_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 6;
}

int CPU::SLO_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 5;
}

int CPU::SLO_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 6;
}

int CPU::SLO_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 7;
}

int CPU::SLO_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 7;
}

int CPU::SLO_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 8;
}

int CPU::SLO_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr);
	setC(op & 128);
	op <<= 1;
	loadM(addr, op);
	loadA(A | op);
	return 8;
}


// -------------------------------------------------------------------------------
// Instruction: SRE
//
// Operation:   LSR, followed by EOR
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

int CPU::SRE_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 5;
}

int CPU::SRE_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 6;
}

int CPU::SRE_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 6;
}

int CPU::SRE_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 7;
}

int CPU::SRE_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 7;
}

int CPU::SRE_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 8;
}

int CPU::SRE_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	return 8;
}


// -------------------------------------------------------------------------------
// Instruction: TAS
//
// Operation:   SP := A & X,  Mem := SP & (HI_BYTE(op) + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
//
// -------------------------------------------------------------------------------

int CPU::TAS_absolute_y()
{
	uint8_t  value = mem->peek(PC + 2) + 1;
	uint16_t addr  = fetchAddressAbsoluteY();
	SP = A & X;
	mem->poke(addr, SP & value);
	return 5;

}


// -------------------------------------------------------------------------------
// Instruction: XAA
//
// Operation:   A = X & op
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

int CPU::XAA_immediate()
{
	uint8_t op = fetchOperandImmediate();
	loadA(X & op & (A | 0xEE));
	return 2;
}
