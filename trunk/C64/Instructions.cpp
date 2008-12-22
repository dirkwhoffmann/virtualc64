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
		
#define FETCH_OPCODE opcode = mem->peek(PC);

// Cycle 0
void 
CPU::fetch() {
	
	// Used for debugging...
	history[historyPtr++] = packState(); 
	//oldPC = PC;
	
	// Check interrupt line
	/* "Ist dieser Eingang auf Low-Pegel, wird eine Interruptbearbeitung ausgel�st, sofern der Interrupt
	 �ber ein Bit im Statusregister freigegeben wurde. Die Unterbrechung erfolgt fr�hestens nach zwei 
	 Taktzyklen beim Erreichen des n�chsten Befehls. Mit diesem Pin kann der VIC einen Interrupt im 
	 Prozessor ausl�sen. Interrupts werden nur erkannt, wenn RDY high ist. */

	// TODO: Not cycle accurate yet!!!!!
	if (nmiLine) {
		if (nmiHistory)
			printf("WARNING: STILL IN NMI INTERRUPT ROUTINE\n");
		nmiHistory = nmiLine;
		next = &CPU::nmi_2;
		return;
	} else if (irqLine && !getI()) {
		if (irqHistory)
			printf("WARNING: STILL IN IRQ INTERRUPT ROUTINE\n");
		irqHistory = irqLine;
		next = &CPU::irq_2;
		return;
	} 

	// Check breakpoint tag
	if (breakpoint[PC] != NO_BREAKPOINT) {
		// Soft breakpoints get deleted when reached
		breakpoint[PC] &= (255 - SOFT_BREAKPOINT);
		setErrorState(BREAKPOINT_REACHED);
		debug("Breakpoint reached\n");
	}

	FETCH_OPCODE;
	PC++;
	next = actionFunc[opcode];
}
	
	


void 
CPU::registerCallback(uint8_t opcode, void (CPU::*func)())
{
	registerCallback(opcode, "???", ADDR_IMPLIED, func);
}

void 
CPU::registerCallback(uint8_t opcode, char *mnc, AddressingMode mode, void (CPU::*func)())
{
	// table is write once!
	if (func != &CPU::defaultCallback) 
		assert(actionFunc[opcode] == &CPU::defaultCallback);
	
	actionFunc[opcode] = func;
	mnemonic[opcode] = mnc;
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
	
	registerCallback(0x8B, "ANE*", ADDR_IMMEDIATE, &CPU::ANE_immediate);

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

	registerCallback(0xA7, "LAX*", ADDR_ZERO_PAGE, &CPU::LAX_zero_page);
	registerCallback(0xB7, "LAX*", ADDR_ZERO_PAGE_Y, &CPU::LAX_zero_page_y);
	registerCallback(0xA3, "LAX*", ADDR_INDIRECT_X, &CPU::LAX_indirect_x);
	registerCallback(0xB3, "LAX*", ADDR_INDIRECT_Y, &CPU::LAX_indirect_y);
	registerCallback(0xAF, "LAX*", ADDR_ABSOLUTE, &CPU::LAX_absolute);
	registerCallback(0xBF, "LAX*", ADDR_ABSOLUTE_Y, &CPU::LAX_absolute_y);

	registerCallback(0xAB, "LXA*", ADDR_IMMEDIATE, &CPU::LXA_immediate);

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

void CPU::defaultCallback()
{
	setErrorState(ILLEGAL_INSTRUCTION);
}


// -------------------------------------------------------------------------------
// IRQ handling
// -------------------------------------------------------------------------------
inline void CPU::irq()
{
	IDLE_READ_IMPLIED;
	next = &CPU::irq_2;
}
inline void CPU::irq_2()
{
	IDLE_READ_IMPLIED;
	next = &CPU::irq_3;
}
inline void CPU::irq_3()
{
	mem->poke(0x100+(SP--), HI_BYTE(PC));
	next = &CPU::irq_4;
}
inline void CPU::irq_4()
{
	mem->poke(0x100+(SP--), LO_BYTE(PC));
	next = &CPU::irq_5;
}
inline void CPU::irq_5()
{
	mem->poke(0x100+(SP--), getPWithClearedB());	
	setI(1);
	next = &CPU::irq_6;
}
inline void CPU::irq_6()
{
	setPCL(mem->peek(0xFFFE));
	next = &CPU::irq_7;
}
inline void CPU::irq_7()
{
	setPCH(mem->peek(0xFFFF));
	DONE;
}


// -------------------------------------------------------------------------------
// NMI handling
// -------------------------------------------------------------------------------
inline void CPU::nmi()
{
	IDLE_READ_IMPLIED;
	next = &CPU::nmi_2;
}
inline void CPU::nmi_2()
{
	// We clear the NMI line. (otherwise, the NMI would be recursively interrupted by itself)
	// This does not happen in a real C64(?). Perhaps, the NMI line is edge triggered (???)
	clearNMILine(0xff);

	IDLE_READ_IMPLIED;
	next = &CPU::nmi_3;
}
inline void CPU::nmi_3()
{
	mem->poke(0x100+(SP--), HI_BYTE(PC));
	next = &CPU::nmi_4;
}
inline void CPU::nmi_4()
{
	mem->poke(0x100+(SP--), LO_BYTE(PC));
	next = &CPU::nmi_5;
}
inline void CPU::nmi_5()
{
	mem->poke(0x100+(SP--), getPWithClearedB());	
	setI(1);
	next = &CPU::nmi_6;
}
inline void CPU::nmi_6()
{
	setPCL(mem->peek(0xFFFA));
	next = &CPU::nmi_7;
}
inline void CPU::nmi_7()
{
	setPCH(mem->peek(0xFFFB));
	DONE;
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

// -------------------------------------------------------------------------------
void CPU::ADC_immediate() 
{
	READ_IMMEDIATE;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ADC_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::ADC_zero_page_2;
}

void CPU::ADC_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ADC_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::ADC_zero_page_x_2;
}
void CPU::ADC_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::ADC_zero_page_x_3;
}
void CPU::ADC_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ADC_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::ADC_absolute_2; 
}
void CPU::ADC_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::ADC_absolute_3; 
}
void CPU::ADC_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	adc(data); 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::ADC_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ADC_absolute_x_2;
}
void CPU::ADC_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::ADC_absolute_x_3;
}
void CPU::ADC_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::ADC_absolute_x_4;
	} else {
		adc(data);
		DONE;
	}
}
void CPU::ADC_absolute_x_4()
{
	READ_FROM_ADDRESS;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ADC_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::ADC_absolute_y_2;
}
void CPU::ADC_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::ADC_absolute_y_3;
}
void CPU::ADC_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::ADC_absolute_y_4;
	} else {
		adc(data);
		DONE;
	}
}
void CPU::ADC_absolute_y_4() 
{
	READ_FROM_ADDRESS;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ADC_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ADC_indirect_x_2;
}
void CPU::ADC_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::ADC_indirect_x_3;
}
void CPU::ADC_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ADC_indirect_x_4;
}
void CPU::ADC_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::ADC_indirect_x_5;
}
void CPU::ADC_indirect_x_5()
{
	READ_FROM_ADDRESS;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ADC_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ADC_indirect_y_2;
}
void CPU::ADC_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ADC_indirect_y_3;
}
void CPU::ADC_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::ADC_indirect_y_4;
}
void CPU::ADC_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::ADC_indirect_y_5;
	} else {
		adc(data);
		DONE;
	}
}
void CPU::ADC_indirect_y_5()
{
	READ_FROM_ADDRESS;
	adc(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: AND
//
// Operation:   A := A AND M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::AND_immediate()
{
	READ_IMMEDIATE;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::AND_absolute_2;
}
void CPU::AND_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::AND_absolute_3;
}
void CPU::AND_absolute_3() {
	READ_FROM_ADDRESS;
	loadA(A & data);	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::AND_zero_page_2;
}
void CPU::AND_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::AND_zero_page_x_2;
}
void CPU::AND_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::AND_zero_page_x_3;
}
void CPU::AND_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::AND_absolute_x_2;
}
void CPU::AND_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::AND_absolute_x_3;
}
void CPU::AND_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::AND_absolute_x_4;
	} else {
		loadA(A & data);
		DONE;
	}
}
void CPU::AND_absolute_x_4()
{
	READ_FROM_ADDRESS;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::AND_absolute_y_2;
}
void CPU::AND_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::AND_absolute_y_3;
}
void CPU::AND_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::AND_absolute_y_4;
	} else {
		loadA(A & data);
		DONE;
	}
}
void CPU::AND_absolute_y_4()
{
	READ_FROM_ADDRESS;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::AND_indirect_x_2;
}
void CPU::AND_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::AND_indirect_x_3;
}
void CPU::AND_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::AND_indirect_x_4;
}
void CPU::AND_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::AND_indirect_x_5;
}
void CPU::AND_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::AND_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::AND_indirect_y_2;
}
void CPU::AND_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::AND_indirect_y_3;
}
void CPU::AND_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::AND_indirect_y_4;
}
void CPU::AND_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::AND_indirect_y_5;
	} else {
		loadA(A & data);
		DONE;
	}
}
void CPU::AND_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadA(A & data);
	DONE;
}


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
void CPU::ASL_accumulator()
{
	IDLE_READ_IMPLIED;
	setC(A & 128); loadA(A << 1);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ASL_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::ASL_zero_page_2;
}
void CPU::ASL_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ASL_zero_page_3;
}
void CPU::ASL_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_ASL;
	next = &CPU::ASL_zero_page_4;
}
void CPU::ASL_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ASL_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::ASL_zero_page_x_2;
}
void CPU::ASL_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::ASL_zero_page_x_3;
}
void CPU::ASL_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ASL_zero_page_x_4;
}
void CPU::ASL_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_ASL;
	next = &CPU::ASL_zero_page_x_5;
}
void CPU::ASL_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ASL_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::ASL_absolute_2;
}
void CPU::ASL_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::ASL_absolute_3;
}
void CPU::ASL_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::ASL_absolute_4;
}
void CPU::ASL_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_ASL;
	next = &CPU::ASL_absolute_5;
}
void CPU::ASL_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ASL_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ASL_absolute_x_2;
}
void CPU::ASL_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::ASL_absolute_x_3;
}
void CPU::ASL_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::ASL_absolute_x_4;
}
void CPU::ASL_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::ASL_absolute_x_5;
}
void CPU::ASL_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_ASL;
	next = &CPU::ASL_absolute_x_6;
}
void CPU::ASL_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ASL_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ASL_indirect_x_2;
}
void CPU::ASL_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::ASL_indirect_x_3;
}
void CPU::ASL_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ASL_indirect_x_4;
}
void CPU::ASL_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::ASL_indirect_x_5;
}
void CPU::ASL_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::ASL_indirect_x_6;
}
void CPU::ASL_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_ASL;
	next = &CPU::ASL_indirect_x_7;
}
void CPU::ASL_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BCC
//
// Operation:   Branch on C = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

inline void CPU::branch(int8_t offset)
{
	PC += offset;
}

// ------------------------------------------------------------------------------
void CPU::BCC_relative()
{	
	READ_IMMEDIATE;
	if (!getC()) { 
		next = &CPU::BCC_relative_2;
	} else {
		DONE;
	}
}
void CPU::BCC_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BCC_relative_3;
	} else {
		DONE;
	}
}
void CPU::BCC_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BCS
//
// Operation:   Branch on C = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BCS_relative()
{	
	READ_IMMEDIATE;
	if (getC()) { 
		next = &CPU::BCS_relative_2;
	} else {
		DONE;
	}
}
void CPU::BCS_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BCS_relative_3;
	} else {
		DONE;
	}
}
void CPU::BCS_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BEQ
//
// Operation:   Branch on Z = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BEQ_relative()
{	
	READ_IMMEDIATE;
	if (getZ()) { 
		next = &CPU::BEQ_relative_2;
	} else {
		DONE;
	}
}
void CPU::BEQ_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BEQ_relative_3;
	} else {
		DONE;
	}
}
void CPU::BEQ_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BIT
//
// Operation:   A AND M, N := M7, V := M6
//
// Flags:       N Z C I D V
//              / / - - - /
// -------------------------------------------------------------------------------

void CPU::BIT_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::BIT_zero_page_2;
}
void CPU::BIT_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	setN(data & 128);
	setV(data & 64);
	setZ((data & A) == 0);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::BIT_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::BIT_absolute_2;
}
void CPU::BIT_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::BIT_absolute_3;
}
void CPU::BIT_absolute_3()
{
	READ_FROM_ADDRESS;
	setN(data & 128);
	setV(data & 64);
	setZ((data & A) == 0);
	DONE;	
}


// -------------------------------------------------------------------------------
// Instruction: BMI
//
// Operation:   Branch on N = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BMI_relative()
{	
	READ_IMMEDIATE;
	if (getN()) { 
		next = &CPU::BMI_relative_2;
	} else {
		DONE;
	}
}
void CPU::BMI_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BMI_relative_3;
	} else {
		DONE;
	}
}
void CPU::BMI_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BNE
//
// Operation:   Branch on Z = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BNE_relative()
{	
	READ_IMMEDIATE;
	if (!getZ()) { 
		next = &CPU::BNE_relative_2;
	} else {
		DONE;
	}
}
void CPU::BNE_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BNE_relative_3;
	} else {
		DONE;
	}
}
void CPU::BNE_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BPL
//
// Operation:   Branch on N = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BPL_relative()
{	
	READ_IMMEDIATE;
	if (!getN()) { 
		next = &CPU::BPL_relative_2;
	} else {
		DONE;
	}
}
void CPU::BPL_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BPL_relative_3;
	} else {
		DONE;
	}
}
void CPU::BPL_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BRK
//
// Operation:   Forced Interrupt (Break)
//
// Flags:       N Z C I D V    B
//              - - - 1 - -    1
// -------------------------------------------------------------------------------

void CPU::BRK()
{
	IDLE_READ_IMMEDIATE;
	next = &CPU::BRK_2;
}
void CPU::BRK_2()
{
	setB(1); 
	PUSH_PCH;
	next = &CPU::BRK_3;
}
void CPU::BRK_3()
{
	PUSH_PCL;
	next = &CPU::BRK_4;
}
void CPU::BRK_4()
{
	PUSH_P;
	next = &CPU::BRK_5;
}
void CPU::BRK_5()
{
	setPCL(mem->peek(0xFFFE));
	next = &CPU::BRK_6;
}
void CPU::BRK_6()
{
	setPCH(mem->peek(0xFFFF));	
	setI(1);
	DONE;
}

// -------------------------------------------------------------------------------
// Instruction: BVC
//
// Operation:   Branch on V = 0
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BVC_relative()
{	
	READ_IMMEDIATE;
	if (!getV()) { 
		next = &CPU::BVC_relative_2;
	} else {
		DONE;
	}
}
void CPU::BVC_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BVC_relative_3;
	} else {
		DONE;
	}
}
void CPU::BVC_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: BVS
//
// Operation:   Branch on V = 1
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::BVS_relative()
{	
	READ_IMMEDIATE;
	if (getV()) { 
		next = &CPU::BVS_relative_2;
	} else {
		DONE;
	}
}
void CPU::BVS_relative_2()
{
	uint8_t pc_hi = HI_BYTE(PC);
	PC += (int8_t)data;
	
	if (pc_hi != HI_BYTE(PC)) {
		next = &CPU::BVS_relative_3;
	} else {
		DONE;
	}
}
void CPU::BVS_relative_3()
{
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: CLC
//
// Operation:   C := 0
//
// Flags:       N Z C I D V
//              - - 0 - - -
// -------------------------------------------------------------------------------

void CPU::CLC()
{
	IDLE_READ_IMPLIED;
	setC(0);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: CLD
//
// Operation:   D := 0
//
// Flags:       N Z C I D V
//              - - - - 0 -
// -------------------------------------------------------------------------------

void CPU::CLD()
{
	IDLE_READ_IMPLIED;
	setD(0);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: CLI
//
// Operation:   I := 0
//
// Flags:       N Z C I D V
//              - - - 0 - -
// -------------------------------------------------------------------------------

void CPU::CLI()
{
	IDLE_READ_IMPLIED;
	setI(0);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: CLV
//
// Operation:   V := 0
//
// Flags:       N Z C I D V
//              - - - - - 0
// -------------------------------------------------------------------------------

void CPU::CLV()
{
	IDLE_READ_IMPLIED;
	setV(0);
	DONE;
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

// -------------------------------------------------------------------------------
void CPU::CMP_immediate()
{
	READ_IMMEDIATE;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::CMP_absolute_2;
}
void CPU::CMP_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::CMP_absolute_3;
}
void CPU::CMP_absolute_3() {
	READ_FROM_ADDRESS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::CMP_zero_page_2;
}
void CPU::CMP_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::CMP_zero_page_x_2;
}
void CPU::CMP_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::CMP_zero_page_x_3;
}
void CPU::CMP_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::CMP_absolute_x_2;
}
void CPU::CMP_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::CMP_absolute_x_3;
}
void CPU::CMP_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::CMP_absolute_x_4;
	} else {
		cmp(A, data);
		DONE;
	}
}
void CPU::CMP_absolute_x_4()
{
	READ_FROM_ADDRESS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::CMP_absolute_y_2;
}
void CPU::CMP_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::CMP_absolute_y_3;
}
void CPU::CMP_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::CMP_absolute_y_4;
	} else {
		cmp(A, data);
		DONE;
	}
}
void CPU::CMP_absolute_y_4()
{
	READ_FROM_ADDRESS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::CMP_indirect_x_2;
}
void CPU::CMP_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::CMP_indirect_x_3;
}
void CPU::CMP_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::CMP_indirect_x_4;
}
void CPU::CMP_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::CMP_indirect_x_5;
}
void CPU::CMP_indirect_x_5()
{
	READ_FROM_ADDRESS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CMP_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::CMP_indirect_y_2;
}
void CPU::CMP_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::CMP_indirect_y_3;
}
void CPU::CMP_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::CMP_indirect_y_4;
}
void CPU::CMP_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::CMP_indirect_y_5;
	} else {
		cmp(A, data);
		DONE;
	}
}
void CPU::CMP_indirect_y_5()
{
	READ_FROM_ADDRESS;
	cmp(A, data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: CPX
//
// Operation:   X-M
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::CPX_immediate()
{
	READ_IMMEDIATE;
	cmp(X, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CPX_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::CPX_zero_page_2;
}
void CPU::CPX_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	cmp(X, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CPX_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::CPX_absolute_2;
}
void CPU::CPX_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::CPX_absolute_3;
}
void CPU::CPX_absolute_3() {
	READ_FROM_ADDRESS;
	cmp(X, data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: CPY
//
// Operation:   Y-M
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::CPY_immediate()
{
	READ_IMMEDIATE;
	cmp(Y, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CPY_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::CPY_zero_page_2;
}
void CPU::CPY_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	cmp(Y, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::CPY_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::CPY_absolute_2;
}
void CPU::CPY_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::CPY_absolute_3;
}
void CPU::CPY_absolute_3() {
	READ_FROM_ADDRESS;
	cmp(Y, data);
	DONE;
}


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
void CPU::DEC_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::DEC_zero_page_2;
}
void CPU::DEC_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::DEC_zero_page_3;
}
void CPU::DEC_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_DEC;
	next = &CPU::DEC_zero_page_4;
}
void CPU::DEC_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DEC_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::DEC_zero_page_x_2;
}
void CPU::DEC_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::DEC_zero_page_x_3;
}
void CPU::DEC_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::DEC_zero_page_x_4;
}
void CPU::DEC_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_DEC;
	next = &CPU::DEC_zero_page_x_5;
}
void CPU::DEC_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DEC_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::DEC_absolute_2;
}
void CPU::DEC_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::DEC_absolute_3;
}
void CPU::DEC_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::DEC_absolute_4;
}
void CPU::DEC_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DEC_absolute_5;
}
void CPU::DEC_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DEC_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::DEC_absolute_x_2;
}
void CPU::DEC_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::DEC_absolute_x_3;
}
void CPU::DEC_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::DEC_absolute_x_4;
}
void CPU::DEC_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::DEC_absolute_x_5;
}
void CPU::DEC_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DEC_absolute_x_6;
}
void CPU::DEC_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DEC_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::DEC_indirect_x_2;
}
void CPU::DEC_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::DEC_indirect_x_3;
}
void CPU::DEC_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::DEC_indirect_x_4;
}
void CPU::DEC_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::DEC_indirect_x_5;
}
void CPU::DEC_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::DEC_indirect_x_6;
}
void CPU::DEC_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DEC_indirect_x_7;
}
void CPU::DEC_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

#if 0
// -------------------------------------------------------------------------------
void CPU::DEC_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::DEC_indirect_y_2;
}
void CPU::DEC_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::DEC_indirect_y_3;
}
void CPU::DEC_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::DEC_indirect_y_4;
}
void CPU::DEC_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::DEC_indirect_y_5;
}
void CPU::DEC_indirect_y_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::DEC_indirect_y_6;
}
void CPU::DEC_indirect_y_6()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DEC_indirect_y_7;
}

void CPU::DEC_indirect_y_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}
#endif

// -------------------------------------------------------------------------------
// Instruction: DEX
//
// Operation:   X := X - 1 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::DEX()
{
	IDLE_READ_IMPLIED;
	loadX(getX()-1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: DEY
//
// Operation:   Y := Y - 1 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::DEY()
{
	IDLE_READ_IMPLIED;
	loadY(getY()-1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: EOR
//
// Operation:   A := A XOR M 
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::EOR_immediate()
{
	READ_IMMEDIATE;
	loadA(A ^ data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::EOR_absolute_2;
}
void CPU::EOR_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::EOR_absolute_3;
}
void CPU::EOR_absolute_3() {
	READ_FROM_ADDRESS;
	loadA(A ^ data);	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::EOR_zero_page_2;
}
void CPU::EOR_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	loadA(A ^ data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::EOR_zero_page_x_2;
}
void CPU::EOR_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::EOR_zero_page_x_3;
}
void CPU::EOR_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	loadA(A ^ data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::EOR_absolute_x_2;
}
void CPU::EOR_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::EOR_absolute_x_3;
}
void CPU::EOR_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::EOR_absolute_x_4;
	} else {
		loadA(A ^ data);
		DONE;
	}
}
void CPU::EOR_absolute_x_4()
{
	READ_FROM_ADDRESS;
	loadA(A ^ data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::EOR_absolute_y_2;
}
void CPU::EOR_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::EOR_absolute_y_3;
}
void CPU::EOR_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::EOR_absolute_y_4;
	} else {
		loadA(A ^ data);
		DONE;
	}
}
void CPU::EOR_absolute_y_4()
{
	READ_FROM_ADDRESS;
	loadA(A ^ data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::EOR_indirect_x_2;
}
void CPU::EOR_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::EOR_indirect_x_3;
}
void CPU::EOR_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::EOR_indirect_x_4;
}
void CPU::EOR_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::EOR_indirect_x_5;
}
void CPU::EOR_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadA(A ^ data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::EOR_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::EOR_indirect_y_2;
}
void CPU::EOR_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::EOR_indirect_y_3;
}
void CPU::EOR_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::EOR_indirect_y_4;
}
void CPU::EOR_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::EOR_indirect_y_5;
	} else {
		loadA(A ^ data);
		DONE;
	}
}
void CPU::EOR_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadA(A ^ data);
	DONE;
}


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
void CPU::INC_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::INC_zero_page_2;
}
void CPU::INC_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::INC_zero_page_3;
}
void CPU::INC_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_INC;
	next = &CPU::INC_zero_page_4;
}
void CPU::INC_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::INC_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::INC_zero_page_x_2;
}
void CPU::INC_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::INC_zero_page_x_3;
}
void CPU::INC_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::INC_zero_page_x_4;
}
void CPU::INC_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_INC;
	next = &CPU::INC_zero_page_x_5;
}
void CPU::INC_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::INC_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::INC_absolute_2;
}
void CPU::INC_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::INC_absolute_3;
}
void CPU::INC_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::INC_absolute_4;
}
void CPU::INC_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::INC_absolute_5;
}
void CPU::INC_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::INC_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::INC_absolute_x_2;
}
void CPU::INC_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::INC_absolute_x_3;
}
void CPU::INC_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::INC_absolute_x_4;
}
void CPU::INC_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::INC_absolute_x_5;
}
void CPU::INC_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::INC_absolute_x_6;
}
void CPU::INC_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::INC_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::INC_indirect_x_2;
}
void CPU::INC_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::INC_indirect_x_3;
}
void CPU::INC_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::INC_indirect_x_4;
}
void CPU::INC_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::INC_indirect_x_5;
}
void CPU::INC_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::INC_indirect_x_6;
}
void CPU::INC_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::INC_indirect_x_7;
}
void CPU::INC_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: INX
//
// Operation:   X := X + 1
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::INX()
{
	IDLE_READ_IMPLIED;
	loadX(getX()+1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: INY
//
// Operation:   Y := Y + 1
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::INY()
{
	IDLE_READ_IMPLIED;
	loadY(getY()+1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: JMP
//
// Operation:   PC := Operand
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::JMP_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::JMP_absolute_2;
}
void CPU::JMP_absolute_2()
{
	FETCH_ADDR_HI;
	setPC(LO_HI(addr_lo, addr_hi));
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::JMP_absolute_indirect()
{
	FETCH_ADDR_LO;
	next = &CPU::JMP_absolute_indirect_2;
}
void CPU::JMP_absolute_indirect_2()
{
	FETCH_ADDR_HI;
	next = &CPU::JMP_absolute_indirect_3;
}
void CPU::JMP_absolute_indirect_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::JMP_absolute_indirect_4;
}
void CPU::JMP_absolute_indirect_4()
{
	setPCL(data);	
	setPCH(mem->peek(addr_lo+1, addr_hi)); 
	DONE;
}

// -------------------------------------------------------------------------------
// Instruction: JSR
//
// Operation:   PC to stack, PC := Operand
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::JSR()
{
	FETCH_ADDR_LO;
	callStack[callStackPointer++] = PC;
	next = &CPU::JSR_2;
}
void CPU::JSR_2()
{
	next = &CPU::JSR_3;
}
void CPU::JSR_3()
{
	PUSH_PCH;
	next = &CPU::JSR_4;
}
void CPU::JSR_4()
{
	PUSH_PCL;
	next = &CPU::JSR_5;
}
void CPU::JSR_5()
{
	FETCH_ADDR_HI;	
	setPC(LO_HI(addr_lo, addr_hi));
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: LDA
//
// Operation:   A := M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
void CPU::LDA_immediate() 
{
	READ_IMMEDIATE;
	loadA(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDA_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::LDA_zero_page_2;
}

void CPU::LDA_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	loadA(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDA_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::LDA_zero_page_x_2;
}
void CPU::LDA_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::LDA_zero_page_x_3;
}
void CPU::LDA_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	loadA(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDA_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::LDA_absolute_2; 
}
void CPU::LDA_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::LDA_absolute_3; 
}
void CPU::LDA_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	loadA(data); 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::LDA_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::LDA_absolute_x_2;
}
void CPU::LDA_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::LDA_absolute_x_3;
}
void CPU::LDA_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDA_absolute_x_4;
	} else {
		loadA(data);
		DONE;
	}
}
void CPU::LDA_absolute_x_4()
{
	READ_FROM_ADDRESS;
	loadA(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDA_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::LDA_absolute_y_2;
}
void CPU::LDA_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::LDA_absolute_y_3;
}
void CPU::LDA_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDA_absolute_y_4;
	} else {
		loadA(data);
		DONE;
	}
}
void CPU::LDA_absolute_y_4() 
{
	READ_FROM_ADDRESS;
	loadA(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDA_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LDA_indirect_x_2;
}
void CPU::LDA_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::LDA_indirect_x_3;
}
void CPU::LDA_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LDA_indirect_x_4;
}
void CPU::LDA_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::LDA_indirect_x_5;
}
void CPU::LDA_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadA(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDA_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LDA_indirect_y_2;
}
void CPU::LDA_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LDA_indirect_y_3;
}
void CPU::LDA_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::LDA_indirect_y_4;
}
void CPU::LDA_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDA_indirect_y_5;
	} else {
		loadA(data);
		DONE;
	}
}
void CPU::LDA_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadA(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: LDX
//
// Operation:   X := M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
void CPU::LDX_immediate() 
{
	READ_IMMEDIATE;
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDX_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::LDX_zero_page_2;
}

void CPU::LDX_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDX_zero_page_y() 
{
	FETCH_ADDR_LO;
	next = &CPU::LDX_zero_page_y_2;
}
void CPU::LDX_zero_page_y_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_Y;
	next = &CPU::LDX_zero_page_y_3;
}
void CPU::LDX_zero_page_y_3() 
{
	READ_FROM_ZERO_PAGE;
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDX_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::LDX_absolute_2; 
}
void CPU::LDX_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::LDX_absolute_3; 
}
void CPU::LDX_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	loadX(data); 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::LDX_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::LDX_absolute_y_2;
}
void CPU::LDX_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::LDX_absolute_y_3;
}
void CPU::LDX_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDX_absolute_y_4;
	} else {
		loadX(data);
		DONE;
	}
}
void CPU::LDX_absolute_y_4() 
{
	READ_FROM_ADDRESS;
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDX_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LDX_indirect_x_2;
}
void CPU::LDX_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::LDX_indirect_x_3;
}
void CPU::LDX_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LDX_indirect_x_4;
}
void CPU::LDX_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::LDX_indirect_x_5;
}
void CPU::LDX_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDX_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LDX_indirect_y_2;
}
void CPU::LDX_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LDX_indirect_y_3;
}
void CPU::LDX_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::LDX_indirect_y_4;
}
void CPU::LDX_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDX_indirect_y_5;
	} else {
		loadX(data);
		DONE;
	}
}
void CPU::LDX_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadX(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: LDY
//
// Operation:   Y := M
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
void CPU::LDY_immediate() 
{
	READ_IMMEDIATE;
	loadY(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDY_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::LDY_zero_page_2;
}

void CPU::LDY_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	loadY(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDY_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::LDY_zero_page_x_2;
}
void CPU::LDY_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::LDY_zero_page_x_3;
}
void CPU::LDY_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	loadY(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDY_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::LDY_absolute_2; 
}
void CPU::LDY_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::LDY_absolute_3; 
}
void CPU::LDY_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	loadY(data); 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::LDY_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::LDY_absolute_x_2;
}
void CPU::LDY_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::LDY_absolute_x_3;
}
void CPU::LDY_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDY_absolute_x_4;
	} else {
		loadY(data);
		DONE;
	}
}
void CPU::LDY_absolute_x_4() 
{
	READ_FROM_ADDRESS;
	loadY(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDY_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LDY_indirect_x_2;
}
void CPU::LDY_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::LDY_indirect_x_3;
}
void CPU::LDY_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LDY_indirect_x_4;
}
void CPU::LDY_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::LDY_indirect_x_5;
}
void CPU::LDY_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadY(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LDY_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LDY_indirect_y_2;
}
void CPU::LDY_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LDY_indirect_y_3;
}
void CPU::LDY_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::LDY_indirect_y_4;
}
void CPU::LDY_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LDY_indirect_y_5;
	} else {
		loadY(data);
		DONE;
	}
}
void CPU::LDY_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadY(data);
	DONE;
}


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
void CPU::LSR_accumulator()
{
	IDLE_READ_IMPLIED;
	setC(A & 1); loadA(A >> 1);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::LSR_zero_page_2;
}
void CPU::LSR_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::LSR_zero_page_3;
}
void CPU::LSR_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_LSR;
	next = &CPU::LSR_zero_page_4;
}
void CPU::LSR_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::LSR_zero_page_x_2;
}
void CPU::LSR_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::LSR_zero_page_x_3;
}
void CPU::LSR_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::LSR_zero_page_x_4;
}
void CPU::LSR_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_LSR;
	next = &CPU::LSR_zero_page_x_5;
}
void CPU::LSR_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::LSR_absolute_2;
}
void CPU::LSR_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::LSR_absolute_3;
}
void CPU::LSR_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::LSR_absolute_4;
}
void CPU::LSR_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_LSR;
	next = &CPU::LSR_absolute_5;
}
void CPU::LSR_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::LSR_absolute_x_2;
}
void CPU::LSR_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::LSR_absolute_x_3;
}
void CPU::LSR_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::LSR_absolute_x_4;
}
void CPU::LSR_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::LSR_absolute_x_5;
}
void CPU::LSR_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_LSR;
	next = &CPU::LSR_absolute_x_6;
}
void CPU::LSR_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::LSR_absolute_y_2;
}
void CPU::LSR_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::LSR_absolute_y_3;
}
void CPU::LSR_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::LSR_absolute_y_4;
}
void CPU::LSR_absolute_y_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::LSR_absolute_y_5;
}
void CPU::LSR_absolute_y_5()
{
	WRITE_TO_ADDRESS;
	DO_LSR;
	next = &CPU::LSR_absolute_y_6;
}
void CPU::LSR_absolute_y_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LSR_indirect_x_2;
}
void CPU::LSR_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::LSR_indirect_x_3;
}
void CPU::LSR_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LSR_indirect_x_4;
}
void CPU::LSR_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::LSR_indirect_x_5;
}
void CPU::LSR_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::LSR_indirect_x_6;
}
void CPU::LSR_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_LSR;
	next = &CPU::LSR_indirect_x_7;
}
void CPU::LSR_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LSR_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LSR_indirect_y_2;
}
void CPU::LSR_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LSR_indirect_y_3;
}
void CPU::LSR_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::LSR_indirect_y_4;
}
void CPU::LSR_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::LSR_indirect_y_5;
}
void CPU::LSR_indirect_y_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::LSR_indirect_y_6;
}
void CPU::LSR_indirect_y_6()
{
	WRITE_TO_ADDRESS;
	DO_LSR;
	next = &CPU::LSR_indirect_y_7;
}

void CPU::LSR_indirect_y_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: NOP
//
// Operation:   No operation
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::NOP()
{
	IDLE_READ_IMPLIED;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::NOP_immediate()
{
	IDLE_READ_IMMEDIATE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::NOP_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::NOP_zero_page_2;
}

void CPU::NOP_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::NOP_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::NOP_zero_page_x_2;
}
void CPU::NOP_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::NOP_zero_page_x_3;
}
void CPU::NOP_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::NOP_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::NOP_absolute_2; 
}
void CPU::NOP_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::NOP_absolute_3; 
}
void CPU::NOP_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::NOP_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::NOP_absolute_x_2;
}
void CPU::NOP_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::NOP_absolute_x_3;
}
void CPU::NOP_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::NOP_absolute_x_4;
	} else {
		DONE;
	}
}
void CPU::NOP_absolute_x_4()
{
	READ_FROM_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: A := A v M
//
// Operation:   No operation
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::ORA_immediate()
{
	READ_IMMEDIATE;
	loadA(A | data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::ORA_absolute_2;
}
void CPU::ORA_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::ORA_absolute_3;
}
void CPU::ORA_absolute_3() {
	READ_FROM_ADDRESS;
	loadA(A | data);	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::ORA_zero_page_2;
}
void CPU::ORA_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	loadA(A | data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ORA_zero_page_x_2;
}
void CPU::ORA_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::ORA_zero_page_x_3;
}
void CPU::ORA_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	loadA(A | data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ORA_absolute_x_2;
}
void CPU::ORA_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::ORA_absolute_x_3;
}
void CPU::ORA_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::ORA_absolute_x_4;
	} else {
		loadA(A | data);
		DONE;
	}
}
void CPU::ORA_absolute_x_4()
{
	READ_FROM_ADDRESS;
	loadA(A | data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::ORA_absolute_y_2;
}
void CPU::ORA_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::ORA_absolute_y_3;
}
void CPU::ORA_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::ORA_absolute_y_4;
	} else {
		loadA(A | data);
		DONE;
	}
}
void CPU::ORA_absolute_y_4()
{
	READ_FROM_ADDRESS;
	loadA(A | data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ORA_indirect_x_2;
}
void CPU::ORA_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::ORA_indirect_x_3;
}
void CPU::ORA_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ORA_indirect_x_4;
}
void CPU::ORA_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::ORA_indirect_x_5;
}
void CPU::ORA_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadA(A | data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ORA_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ORA_indirect_y_2;
}
void CPU::ORA_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ORA_indirect_y_3;
}
void CPU::ORA_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::ORA_indirect_y_4;
}
void CPU::ORA_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::ORA_indirect_y_5;
	} else {
		loadA(A | data);
		DONE;
	}
}
void CPU::ORA_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadA(A | data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: PHA
//
// Operation:   A to stack
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::PHA()
{
	IDLE_READ_IMPLIED;
	next = &CPU::PHA_2;
}
void CPU::PHA_2()
{
	PUSH_A;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: PHA
//
// Operation:   P to stack
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::PHP()
{
	IDLE_READ_IMPLIED;
	next = &CPU::PHP_2;
}
void CPU::PHP_2()
{
	PUSH_P;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: PLA
//
// Operation:   Stack to A
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::PLA()
{
	IDLE_READ_IMPLIED;
	next = &CPU::PLA_2;
}
void CPU::PLA_2()
{
	SP++;
	next = &CPU::PLA_3;
}
void CPU::PLA_3()
{
	PULL_A;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: PLP
//
// Operation:   Stack to p
//
// Flags:       N Z C I D V
//              / / / / / /
// -------------------------------------------------------------------------------

void CPU::PLP()
{
	IDLE_READ_IMPLIED;
	next = &CPU::PLP_2;
}
void CPU::PLP_2()
{
	SP++;
	next = &CPU::PLP_3;
}
void CPU::PLP_3()
{
	PULL_P;
	DONE;
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

#define DO_ROL if (getC()) { setC(data & 128); data = (data << 1) + 1; } else { setC(data & 128); data = (data << 1); }

// -------------------------------------------------------------------------------
void CPU::ROL_accumulator()
{
	IDLE_READ_IMPLIED;
	if (getC()) { 
		setC(A & 128); 
		loadA((A << 1) + 1);
	} else { 
		setC(A & 128); 
		loadA(A << 1); 
	}
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROL_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::ROL_zero_page_2;
}
void CPU::ROL_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ROL_zero_page_3;
}
void CPU::ROL_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_ROL;
	next = &CPU::ROL_zero_page_4;
}
void CPU::ROL_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROL_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::ROL_zero_page_x_2;
}
void CPU::ROL_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::ROL_zero_page_x_3;
}
void CPU::ROL_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ROL_zero_page_x_4;
}
void CPU::ROL_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_ROL;
	next = &CPU::ROL_zero_page_x_5;
}
void CPU::ROL_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROL_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::ROL_absolute_2;
}
void CPU::ROL_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::ROL_absolute_3;
}
void CPU::ROL_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::ROL_absolute_4;
}
void CPU::ROL_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_ROL;
	next = &CPU::ROL_absolute_5;
}
void CPU::ROL_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROL_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ROL_absolute_x_2;
}
void CPU::ROL_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::ROL_absolute_x_3;
}
void CPU::ROL_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::ROL_absolute_x_4;
}
void CPU::ROL_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::ROL_absolute_x_5;
}
void CPU::ROL_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_ROL;
	next = &CPU::ROL_absolute_x_6;
}
void CPU::ROL_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROL_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ROL_indirect_x_2;
}
void CPU::ROL_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::ROL_indirect_x_3;
}
void CPU::ROL_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ROL_indirect_x_4;
}
void CPU::ROL_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::ROL_indirect_x_5;
}
void CPU::ROL_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::ROL_indirect_x_6;
}
void CPU::ROL_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_ROL;
	next = &CPU::ROL_indirect_x_7;
}
void CPU::ROL_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}


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

#define DO_ROR if (getC()) { setC(data & 1); data = (data >> 1) + 128; } else { setC(data & 1); data = (data >> 1); }

// -------------------------------------------------------------------------------
void CPU::ROR_accumulator()
{
	IDLE_READ_IMPLIED;
	if (getC()) { 
		setC(A & 1); 
		loadA((A >> 1) + 128);
	} else { 
		setC(A & 1); 
		loadA(A >> 1); 
	}
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROR_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::ROR_zero_page_2;
}
void CPU::ROR_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ROR_zero_page_3;
}
void CPU::ROR_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_ROR;
	next = &CPU::ROR_zero_page_4;
}
void CPU::ROR_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROR_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::ROR_zero_page_x_2;
}
void CPU::ROR_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::ROR_zero_page_x_3;
}
void CPU::ROR_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ROR_zero_page_x_4;
}
void CPU::ROR_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_ROR;
	next = &CPU::ROR_zero_page_x_5;
}
void CPU::ROR_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROR_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::ROR_absolute_2;
}
void CPU::ROR_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::ROR_absolute_3;
}
void CPU::ROR_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::ROR_absolute_4;
}
void CPU::ROR_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_ROR;
	next = &CPU::ROR_absolute_5;
}
void CPU::ROR_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROR_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ROR_absolute_x_2;
}
void CPU::ROR_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::ROR_absolute_x_3;
}
void CPU::ROR_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::ROR_absolute_x_4;
}
void CPU::ROR_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::ROR_absolute_x_5;
}
void CPU::ROR_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_ROR;
	next = &CPU::ROR_absolute_x_6;
}
void CPU::ROR_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ROR_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ROR_indirect_x_2;
}
void CPU::ROR_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::ROR_indirect_x_3;
}
void CPU::ROR_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ROR_indirect_x_4;
}
void CPU::ROR_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::ROR_indirect_x_5;
}
void CPU::ROR_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::ROR_indirect_x_6;
}
void CPU::ROR_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_ROR;
	next = &CPU::ROR_indirect_x_7;
}
void CPU::ROR_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: RTI
//
// Operation:   P from Stack, PC from Stack
//
// Flags:       N Z C I D V
//              / / / / / /
// -------------------------------------------------------------------------------

void CPU::RTI()
{
	IDLE_READ_IMMEDIATE;
	next = &CPU::RTI_2;
}
void CPU::RTI_2()
{
	nmiHistory = irqHistory = 0;
	SP++;
	next = &CPU::RTI_3;
}
void CPU::RTI_3()
{
	PULL_P;
	SP++;
	next = &CPU::RTI_4;
}
void CPU::RTI_4()
{
	PULL_PCL;
	SP++;
	next = &CPU::RTI_5;
}
void CPU::RTI_5()
{
	PULL_PCH;
	DONE;
}

// -------------------------------------------------------------------------------
// Instruction: RTS
//
// Operation:   PC from Stack
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::RTS()
{
	IDLE_READ_IMMEDIATE;
	next = &CPU::RTS_2;
}
void CPU::RTS_2()
{
	SP++;
	next = &CPU::RTS_3;
}
void CPU::RTS_3()
{
	PULL_PCL;
	SP++;
	next = &CPU::RTS_4;
}
void CPU::RTS_4()
{
	PULL_PCH;
	next = &CPU::RTS_5;
}
void CPU::RTS_5()
{
	PC++;
	callStackPointer--;
	DONE;
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

// -------------------------------------------------------------------------------
void CPU::SBC_immediate() 
{
	READ_IMMEDIATE;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SBC_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::SBC_zero_page_2;
}

void CPU::SBC_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SBC_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::SBC_zero_page_x_2;
}
void CPU::SBC_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::SBC_zero_page_x_3;
}
void CPU::SBC_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SBC_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::SBC_absolute_2; 
}
void CPU::SBC_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::SBC_absolute_3; 
}
void CPU::SBC_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	sbc(data); 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::SBC_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::SBC_absolute_x_2;
}
void CPU::SBC_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::SBC_absolute_x_3;
}
void CPU::SBC_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SBC_absolute_x_4;
	} else {
		sbc(data);
		DONE;
	}
}
void CPU::SBC_absolute_x_4()
{
	READ_FROM_ADDRESS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SBC_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::SBC_absolute_y_2;
}
void CPU::SBC_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::SBC_absolute_y_3;
}
void CPU::SBC_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SBC_absolute_y_4;
	} else {
		sbc(data);
		DONE;
	}
}
void CPU::SBC_absolute_y_4() 
{
	READ_FROM_ADDRESS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SBC_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SBC_indirect_x_2;
}
void CPU::SBC_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::SBC_indirect_x_3;
}
void CPU::SBC_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SBC_indirect_x_4;
}
void CPU::SBC_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::SBC_indirect_x_5;
}
void CPU::SBC_indirect_x_5()
{
	READ_FROM_ADDRESS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SBC_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SBC_indirect_y_2;
}
void CPU::SBC_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SBC_indirect_y_3;
}
void CPU::SBC_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::SBC_indirect_y_4;
}
void CPU::SBC_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SBC_indirect_y_5;
	} else {
		sbc(data);
		DONE;
	}
}
void CPU::SBC_indirect_y_5()
{
	READ_FROM_ADDRESS;
	sbc(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SEC
//
// Operation:   C := 1
//
// Flags:       N Z C I D V
//              - - 1 - - -
// -------------------------------------------------------------------------------

void CPU::SEC()
{
	IDLE_READ_IMPLIED;
	setC(1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SED
//
// Operation:   D := 1
//
// Flags:       N Z C I D V
//              - - - - 1 -
// -------------------------------------------------------------------------------

void CPU::SED()
{
	IDLE_READ_IMPLIED;
	setD(1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SEI
//
// Operation:   D := 1
//
// Flags:       N Z C I D V
//              - - - 1 - -
// -------------------------------------------------------------------------------

void CPU::SEI()
{
	IDLE_READ_IMPLIED;
	setI(1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: STA
//
// Operation:   M := A
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::STA_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::STA_zero_page_2;
}
void CPU::STA_zero_page_2()
{
	data = A;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STA_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::STA_zero_page_x_2;
}
void CPU::STA_zero_page_x_2()
{
	IDLE_READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::STA_zero_page_x_3;
}	
void CPU::STA_zero_page_x_3()
{
	data = A;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STA_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::STA_absolute_2;
}
void CPU::STA_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::STA_absolute_3;
}
void CPU::STA_absolute_3()
{
	data = A;
	WRITE_TO_ADDRESS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STA_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::STA_absolute_x_2;
}
void CPU::STA_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::STA_absolute_x_3;
}
void CPU::STA_absolute_x_3()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::STA_absolute_x_4;
	} else {
		data = A;
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::STA_absolute_x_4()
{
	data = A;
	WRITE_TO_ADDRESS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STA_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::STA_absolute_y_2;
}
void CPU::STA_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::STA_absolute_y_3;
}
void CPU::STA_absolute_y_3()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::STA_absolute_y_4;
	} else {
		data = A;
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::STA_absolute_y_4()
{
	data = A;
	WRITE_TO_ADDRESS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STA_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::STA_indirect_x_2;
}
void CPU::STA_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::STA_indirect_x_3;
}
void CPU::STA_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::STA_indirect_x_4;
}
void CPU::STA_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::STA_indirect_x_5;
}
void CPU::STA_indirect_x_5()
{
	data = A;
	WRITE_TO_ADDRESS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STA_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::STA_indirect_y_2;
}
void CPU::STA_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::STA_indirect_y_3;
}
void CPU::STA_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::STA_indirect_y_4;
}
void CPU::STA_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::STA_indirect_y_5;
	} else {
		data = A;
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::STA_indirect_y_5()
{
	data = A;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: STX
//
// Operation:   M := X
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::STX_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::STX_zero_page_2;
}
void CPU::STX_zero_page_2()
{
	data = X;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STX_zero_page_y()
{
	FETCH_ADDR_LO;
	next = &CPU::STX_zero_page_y_2;
}
void CPU::STX_zero_page_y_2()
{
	IDLE_READ_FROM_ZERO_PAGE;
	ADD_INDEX_Y;
	next = &CPU::STX_zero_page_y_3;
}	
void CPU::STX_zero_page_y_3()
{
	data = X;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STX_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::STX_absolute_2;
}
void CPU::STX_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::STX_absolute_3;
}
void CPU::STX_absolute_3()
{
	data = X;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: STY
//
// Operation:   M := Y
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::STY_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::STY_zero_page_2;
}
void CPU::STY_zero_page_2()
{
	data = Y;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STY_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::STY_zero_page_x_2;
}
void CPU::STY_zero_page_x_2()
{
	IDLE_READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::STY_zero_page_x_3;
}	
void CPU::STY_zero_page_x_3()
{
	data = Y;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::STY_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::STY_absolute_2;
}
void CPU::STY_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::STY_absolute_3;
}
void CPU::STY_absolute_3()
{
	data = Y;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: TAX
//
// Operation:   X := A
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::TAX()
{
	IDLE_READ_IMPLIED;
	loadX(A);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: TAY
//
// Operation:   Y := A
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::TAY()
{
	IDLE_READ_IMPLIED;
	loadY(A);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: TSX
//
// Operation:   X := Stack pointer
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::TSX()
{
	IDLE_READ_IMPLIED;
	loadX(SP);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: TXA
//
// Operation:   A := X
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::TXA()
{
	IDLE_READ_IMPLIED;
	loadA(X);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: TXS
//
// Operation:   Stack pointer := X
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::TXS()
{
	IDLE_READ_IMPLIED;
	SP = X;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: TYA
//
// Operation:   A := Y
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::TYA()
{
	IDLE_READ_IMPLIED;
	loadA(Y);
	DONE;
}


// -------------------------------------------------------------------------------
// Illegal instructions
// -------------------------------------------------------------------------------

// -------------------------------------------------------------------------------
// Instruction: AHX (SHA)
//
// Operation:   Mem := A & X & (M + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::AHX_absolute_y()
{
	data = mem->peek(PC+1);
	FETCH_ADDR_LO;
	next = &CPU::AHX_absolute_y_2;
}
void CPU::AHX_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::AHX_absolute_y_3;
}
void CPU::AHX_absolute_y_3()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::AHX_absolute_y_4;
	} else {
		data = A & X & (data + 1);
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::AHX_absolute_y_4()
{
	data = A & X & (data + 1);
	WRITE_TO_ADDRESS;
	DONE;
}

#if 0
void CPU::AHX_absolute_y()
{
	uint8_t  value = mem->peek(PC+1);
	uint16_t addr  = fetchAddressAbsoluteY();
	mem->poke(addr, A & X & (value + 1));
	DONE;
}
#endif

// -------------------------------------------------------------------------------
void CPU::AHX_indirect_y()
{
	data = mem->peek(mem->peek(PC) + 1);
	FETCH_POINTER_ADDR;
	next = &CPU::AHX_indirect_y_2;
}
void CPU::AHX_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::AHX_indirect_y_3;
}
void CPU::AHX_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::AHX_indirect_y_4;
}
void CPU::AHX_indirect_y_4()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::AHX_indirect_y_5;
	} else {
		data = A & X & (data + 1);
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::AHX_indirect_y_5()
{
	data = A & X & (data + 1);
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: ALR
//
// Operation:   AND, followed by LSR
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::ALR_immediate()
{
	READ_IMMEDIATE;
	A = A & data;
	setC(A & 1);
	loadA(A >> 1);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: ANC
//
// Operation:   A := A & op,   N flag is copied to C
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::ANC_immediate()
{
	READ_IMMEDIATE;
	loadA(A & data);
	setC(getN());
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: ARR
//
// Operation:   AND, followed by ROR
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

void CPU::ARR_immediate()
{
	READ_IMMEDIATE;

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
			A = A & 0xf0 | (A + 6) & 0x0f;
		c_flag = (tmp2 + (tmp2 & 0x10)) & 0x1f0;
		if (c_flag > 0x50) {
			setC(1);
			A += 0x60;
		} else {
			setC(0);
		}
	}
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: AXS
//
// Operation:   X = (A & X) - op
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::AXS_immediate()
{
	READ_IMMEDIATE;
	
	uint8_t op2  = A & X;
	uint8_t tmp = op2 - data; 

	setC(op2 >= data);
	loadX(tmp);
	DONE;
}

// -------------------------------------------------------------------------------
// Instruction: DCP
//
// Operation:   DEC followed by CMP
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::DCP_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::DCP_zero_page_2;
}
void CPU::DCP_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::DCP_zero_page_3;
}
void CPU::DCP_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_DEC;
	next = &CPU::DCP_zero_page_4;
}
void CPU::DCP_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DCP_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::DCP_zero_page_x_2;
}
void CPU::DCP_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::DCP_zero_page_x_3;
}
void CPU::DCP_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::DCP_zero_page_x_4;
}
void CPU::DCP_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_DEC;
	next = &CPU::DCP_zero_page_x_5;
}
void CPU::DCP_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DCP_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::DCP_absolute_2;
}
void CPU::DCP_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::DCP_absolute_3;
}
void CPU::DCP_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::DCP_absolute_4;
}
void CPU::DCP_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DCP_absolute_5;
}
void CPU::DCP_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DCP_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::DCP_absolute_x_2;
}
void CPU::DCP_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::DCP_absolute_x_3;
}
void CPU::DCP_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::DCP_absolute_x_4;
}
void CPU::DCP_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::DCP_absolute_x_5;
}
void CPU::DCP_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DCP_absolute_x_6;
}
void CPU::DCP_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DCP_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::DCP_absolute_y_2;
}
void CPU::DCP_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::DCP_absolute_y_3;
}
void CPU::DCP_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::DCP_absolute_y_4;
}
void CPU::DCP_absolute_y_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::DCP_absolute_y_5;
}
void CPU::DCP_absolute_y_5()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DCP_absolute_y_6;
}
void CPU::DCP_absolute_y_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DCP_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::DCP_indirect_x_2;
}
void CPU::DCP_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::DCP_indirect_x_3;
}
void CPU::DCP_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::DCP_indirect_x_4;
}
void CPU::DCP_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::DCP_indirect_x_5;
}
void CPU::DCP_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::DCP_indirect_x_6;
}
void CPU::DCP_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DCP_indirect_x_7;
}
void CPU::DCP_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::DCP_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::DCP_indirect_y_2;
}
void CPU::DCP_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::DCP_indirect_y_3;
}
void CPU::DCP_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::DCP_indirect_y_4;
}
void CPU::DCP_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::DCP_indirect_y_5;
}
void CPU::DCP_indirect_y_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::DCP_indirect_y_6;
}
void CPU::DCP_indirect_y_6()
{
	WRITE_TO_ADDRESS;
	DO_DEC;
	next = &CPU::DCP_indirect_y_7;
}

void CPU::DCP_indirect_y_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	cmp(A, data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: ISC
//
// Operation:   INC followed by SBC
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

void CPU::ISC_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::ISC_zero_page_2;
}
void CPU::ISC_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ISC_zero_page_3;
}
void CPU::ISC_zero_page_3()
{
	WRITE_TO_ZERO_PAGE;
	DO_INC;
	next = &CPU::ISC_zero_page_4;
}
void CPU::ISC_zero_page_4()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ISC_zero_page_x() 
{
	FETCH_ADDR_LO;
	next = &CPU::ISC_zero_page_x_2;
}
void CPU::ISC_zero_page_x_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::ISC_zero_page_x_3;
}
void CPU::ISC_zero_page_x_3() 
{
	READ_FROM_ZERO_PAGE;
	next = &CPU::ISC_zero_page_x_4;
}
void CPU::ISC_zero_page_x_4()
{
	WRITE_TO_ZERO_PAGE;
	DO_INC;
	next = &CPU::ISC_zero_page_x_5;
}
void CPU::ISC_zero_page_x_5()
{
	WRITE_TO_ZERO_PAGE_AND_SET_FLAGS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ISC_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::ISC_absolute_2;
}
void CPU::ISC_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::ISC_absolute_3;
}
void CPU::ISC_absolute_3()
{
	READ_FROM_ADDRESS;
	next = &CPU::ISC_absolute_4;
}
void CPU::ISC_absolute_4()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::ISC_absolute_5;
}
void CPU::ISC_absolute_5()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ISC_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::ISC_absolute_x_2;
}
void CPU::ISC_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::ISC_absolute_x_3;
}
void CPU::ISC_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::ISC_absolute_x_4;
}
void CPU::ISC_absolute_x_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::ISC_absolute_x_5;
}
void CPU::ISC_absolute_x_5()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::ISC_absolute_x_6;
}
void CPU::ISC_absolute_x_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ISC_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::ISC_absolute_y_2;
}
void CPU::ISC_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::ISC_absolute_y_3;
}
void CPU::ISC_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::ISC_absolute_y_4;
}
void CPU::ISC_absolute_y_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::ISC_absolute_y_5;
}
void CPU::ISC_absolute_y_5()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::ISC_absolute_y_6;
}
void CPU::ISC_absolute_y_6()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ISC_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ISC_indirect_x_2;
}
void CPU::ISC_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::ISC_indirect_x_3;
}
void CPU::ISC_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ISC_indirect_x_4;
}
void CPU::ISC_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::ISC_indirect_x_5;
}
void CPU::ISC_indirect_x_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::ISC_indirect_x_6;
}
void CPU::ISC_indirect_x_6()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::ISC_indirect_x_7;
}
void CPU::ISC_indirect_x_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	sbc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::ISC_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::ISC_indirect_y_2;
}
void CPU::ISC_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::ISC_indirect_y_3;
}
void CPU::ISC_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::ISC_indirect_y_4;
}
void CPU::ISC_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::ISC_indirect_y_5;
}
void CPU::ISC_indirect_y_5()
{
	READ_FROM_ADDRESS;
	next = &CPU::ISC_indirect_y_6;
}
void CPU::ISC_indirect_y_6()
{
	WRITE_TO_ADDRESS;
	DO_INC;
	next = &CPU::ISC_indirect_y_7;
}

void CPU::ISC_indirect_y_7()
{
	WRITE_TO_ADDRESS_AND_SET_FLAGS;
	sbc(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: LAS
//
// Operation:   SP,X,A = op & SP
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::LAS_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::LAS_absolute_y_2;
}
void CPU::LAS_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::LAS_absolute_y_3;
}
void CPU::LAS_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
	}
	next = &CPU::LAS_absolute_y_4;
}
void CPU::LAS_absolute_y_4()
{
	READ_FROM_ADDRESS;
	next = &CPU::LAS_absolute_y_5;
}
void CPU::LAS_absolute_y_5()
{
	WRITE_TO_ADDRESS;
	data &= SP;
	next = &CPU::LAS_absolute_y_6;
}
void CPU::LAS_absolute_y_6()
{
	SP = data;
	X = data;
	loadA(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: LAX
//
// Operation:   LDA, followed by LDX
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

#if 0
// -------------------------------------------------------------------------------
void CPU::LAX_immediate() 
{
	READ_IMMEDIATE;
	loadA(data);
	loadX(data);
	DONE;
}
#endif

// -------------------------------------------------------------------------------
void CPU::LAX_zero_page() 
{
	FETCH_ADDR_LO;
	next = &CPU::LAX_zero_page_2;
}

void CPU::LAX_zero_page_2() 
{
	READ_FROM_ZERO_PAGE;
	loadA(data);
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LAX_zero_page_y() 
{
	FETCH_ADDR_LO;
	next = &CPU::LAX_zero_page_y_2;
}
void CPU::LAX_zero_page_y_2() 
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_Y;
	next = &CPU::LAX_zero_page_y_3;
}
void CPU::LAX_zero_page_y_3() 
{
	READ_FROM_ZERO_PAGE;
	loadA(data);
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LAX_absolute() 
{ 
	FETCH_ADDR_LO; 
	next = &CPU::LAX_absolute_2; 
}
void CPU::LAX_absolute_2() 
{ 
	FETCH_ADDR_HI; 
	next = &CPU::LAX_absolute_3; 
}
void CPU::LAX_absolute_3() 
{ 
	READ_FROM_ADDRESS; 
	loadA(data);
	loadX(data); 
	DONE; 
}

// -------------------------------------------------------------------------------
void CPU::LAX_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::LAX_absolute_y_2;
}
void CPU::LAX_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::LAX_absolute_y_3;
}
void CPU::LAX_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LAX_absolute_y_4;
	} else {
		loadA(data);
		loadX(data);
		DONE;
	}
}
void CPU::LAX_absolute_y_4() 
{
	READ_FROM_ADDRESS;
	loadA(data);
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LAX_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LAX_indirect_x_2;
}
void CPU::LAX_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::LAX_indirect_x_3;
}
void CPU::LAX_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LAX_indirect_x_4;
}
void CPU::LAX_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::LAX_indirect_x_5;
}
void CPU::LAX_indirect_x_5()
{
	READ_FROM_ADDRESS;
	loadA(data);
	loadX(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::LAX_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::LAX_indirect_y_2;
}
void CPU::LAX_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::LAX_indirect_y_3;
}
void CPU::LAX_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::LAX_indirect_y_4;
}
void CPU::LAX_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::LAX_indirect_y_5;
	} else {
		loadA(data);
		loadX(data);
		DONE;
	}
}
void CPU::LAX_indirect_y_5()
{
	READ_FROM_ADDRESS;
	loadA(data);
	loadX(data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: RLA
//
// Operation:   ROL, followed by AND
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

void CPU::RLA_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::RLA_absolute_2;
}
void CPU::RLA_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::RLA_absolute_3;
}
void CPU::RLA_absolute_3() {
	READ_FROM_ADDRESS;
	DO_ROL;
	WRITE_TO_ADDRESS;
	loadA(A & data);	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RLA_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::RLA_zero_page_2;
}
void CPU::RLA_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	DO_ROL;
	WRITE_TO_ZERO_PAGE;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RLA_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::RLA_zero_page_x_2;
}
void CPU::RLA_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::RLA_zero_page_x_3;
}
void CPU::RLA_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	DO_ROL;
	WRITE_TO_ZERO_PAGE;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RLA_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::RLA_absolute_x_2;
}
void CPU::RLA_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::RLA_absolute_x_3;
}
void CPU::RLA_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::RLA_absolute_x_4;
	} else {
		DO_ROL;
		WRITE_TO_ADDRESS;
		loadA(A & data);
		DONE;
	}
}
void CPU::RLA_absolute_x_4()
{
	READ_FROM_ADDRESS;
	DO_ROL;
	WRITE_TO_ADDRESS;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RLA_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::RLA_absolute_y_2;
}
void CPU::RLA_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::RLA_absolute_y_3;
}
void CPU::RLA_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::RLA_absolute_y_4;
	} else {
		DO_ROL;
		WRITE_TO_ADDRESS;
		loadA(A & data);
		DONE;
	}
}
void CPU::RLA_absolute_y_4()
{
	READ_FROM_ADDRESS;
	DO_ROL;
	WRITE_TO_ADDRESS;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RLA_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::RLA_indirect_x_2;
}
void CPU::RLA_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::RLA_indirect_x_3;
}
void CPU::RLA_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::RLA_indirect_x_4;
}
void CPU::RLA_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::RLA_indirect_x_5;
}
void CPU::RLA_indirect_x_5()
{
	READ_FROM_ADDRESS;
	DO_ROL;
	WRITE_TO_ADDRESS;
	loadA(A & data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RLA_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::RLA_indirect_y_2;
}
void CPU::RLA_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::RLA_indirect_y_3;
}
void CPU::RLA_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::RLA_indirect_y_4;
}
void CPU::RLA_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::RLA_indirect_y_5;
	} else {
		DO_ROL;
		WRITE_TO_ADDRESS;
		loadA(A & data);
		DONE;
	}
}
void CPU::RLA_indirect_y_5()
{
	READ_FROM_ADDRESS;
	DO_ROL;
	WRITE_TO_ADDRESS;
	loadA(A & data);
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: RRA
//
// Operation:   ROR, followed by ADC
//
// Flags:       N Z C I D V
//              / / / - - /
// -------------------------------------------------------------------------------

void CPU::RRA_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::RRA_absolute_2;
}
void CPU::RRA_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::RRA_absolute_3;
}
void CPU::RRA_absolute_3() {
	READ_FROM_ADDRESS;
	DO_ROR;
	WRITE_TO_ADDRESS;
	adc(data);	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::RRA_zero_page_2;
}
void CPU::RRA_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	DO_ROR;
	WRITE_TO_ZERO_PAGE;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::RRA_zero_page_x_2;
}
void CPU::RRA_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::RRA_zero_page_x_3;
}
void CPU::RRA_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	DO_ROR;
	WRITE_TO_ZERO_PAGE;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::RRA_absolute_x_2;
}
void CPU::RRA_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::RRA_absolute_x_3;
}
void CPU::RRA_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::RRA_absolute_x_4;
	} else {
		DO_ROR;
		WRITE_TO_ADDRESS;
		adc(data);
		DONE;
	}
}
void CPU::RRA_absolute_x_4()
{
	READ_FROM_ADDRESS;
	DO_ROR;
	WRITE_TO_ADDRESS;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::RRA_absolute_y_2;
}
void CPU::RRA_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::RRA_absolute_y_3;
}
void CPU::RRA_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::RRA_absolute_y_4;
	} else {
		DO_ROR;
		WRITE_TO_ADDRESS;
		adc(data);
		DONE;
	}
}
void CPU::RRA_absolute_y_4()
{
	READ_FROM_ADDRESS;
	DO_ROR;
	WRITE_TO_ADDRESS;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::RRA_indirect_x_2;
}
void CPU::RRA_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::RRA_indirect_x_3;
}
void CPU::RRA_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::RRA_indirect_x_4;
}
void CPU::RRA_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::RRA_indirect_x_5;
}
void CPU::RRA_indirect_x_5()
{
	READ_FROM_ADDRESS;
	DO_ROR;
	WRITE_TO_ADDRESS;
	adc(data);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::RRA_indirect_y_2;
}
void CPU::RRA_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::RRA_indirect_y_3;
}
void CPU::RRA_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::RRA_indirect_y_4;
}
void CPU::RRA_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::RRA_indirect_y_5;
	} else {
		DO_ROR;
		WRITE_TO_ADDRESS;
		adc(data);
		DONE;
	}
}
void CPU::RRA_indirect_y_5()
{
	READ_FROM_ADDRESS;
	DO_ROR;
	WRITE_TO_ADDRESS;
	adc(data);
	DONE;
}

#if 0
void CPU::RRA_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}
void CPU::RRA_absolute_2()
{
}
void CPU::RRA_absolute_3()
{
}

// -------------------------------------------------------------------------------
void CPU::RRA_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::RRA_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t value = ror(mem->peek(addr));
	loadM(addr, value);
	adc(value);
	DONE;
}
#endif

// -------------------------------------------------------------------------------
// Instruction: SAX
//
// Operation:   Mem := A & X
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::SAX_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::SAX_zero_page_2;
}
void CPU::SAX_zero_page_2()
{
	data = A & X;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SAX_zero_page_y()
{
	FETCH_ADDR_LO;
	next = &CPU::SAX_zero_page_y_2;
}
void CPU::SAX_zero_page_y_2()
{
	IDLE_READ_FROM_ZERO_PAGE;
	ADD_INDEX_Y;
	next = &CPU::SAX_zero_page_y_3;
}	
void CPU::SAX_zero_page_y_3()
{
	data = A & X;
	WRITE_TO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SAX_absolute()
{
	FETCH_ADDR_LO;
	next = &CPU::SAX_absolute_2;
}
void CPU::SAX_absolute_2()
{
	FETCH_ADDR_HI;
	next = &CPU::SAX_absolute_3;
}
void CPU::SAX_absolute_3()
{
	data = A & X;
	WRITE_TO_ADDRESS;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SAX_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SAX_indirect_x_2;
}
void CPU::SAX_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::SAX_indirect_x_3;
}
void CPU::SAX_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SAX_indirect_x_4;
}
void CPU::SAX_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::SAX_indirect_x_5;
}
void CPU::SAX_indirect_x_5()
{
	data = A & X;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SHX
//
// Operation:   Mem := X & (HI_BYTE(op) + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::SHX_absolute_y()
{
	data = mem->peek(PC+1) + 1;
	FETCH_ADDR_LO;
	next = &CPU::SHX_absolute_y_2;
}
void CPU::SHX_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::SHX_absolute_y_3;
}
void CPU::SHX_absolute_y_3()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SHX_absolute_y_4;
	} else {
		data &= X;
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::SHX_absolute_y_4()
{
	data &= X;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SHY
//
// Operation:   Mem := Y & (HI_BYTE(op) + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
// -------------------------------------------------------------------------------

void CPU::SHY_absolute_x()
{
	data = mem->peek(PC+1) + 1;
	FETCH_ADDR_LO;
	next = &CPU::SHY_absolute_x_2;
}
void CPU::SHY_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::SHY_absolute_x_3;
}
void CPU::SHY_absolute_x_3()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SHY_absolute_x_4;
	} else {
		data &= Y;
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::SHY_absolute_x_4()
{
	data &= Y;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SLO
//
// Operation:   ASL memory location, followed by OR on accumulator
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

#define DO_SLO_ZERO_PAGE setC(data & 128); data <<= 1; loadM(addr_lo, data); loadA(A | data);
#define DO_SLO setC(data & 128); data <<= 1; loadM((addr_hi << 8) | addr_lo, data); loadA(A | data);

void CPU::SLO_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::SLO_absolute_2;
}
void CPU::SLO_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::SLO_absolute_3;
}
void CPU::SLO_absolute_3() {
	READ_FROM_ADDRESS;
	DO_SLO;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SLO_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::SLO_zero_page_2;
}
void CPU::SLO_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	DO_SLO_ZERO_PAGE;	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SLO_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::SLO_zero_page_x_2;
}
void CPU::SLO_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::SLO_zero_page_x_3;
}
void CPU::SLO_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	DO_SLO_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SLO_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::SLO_absolute_x_2;
}
void CPU::SLO_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::SLO_absolute_x_3;
}
void CPU::SLO_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SLO_absolute_x_4;
	} else {
		DO_SLO;
		DONE;
	}
}
void CPU::SLO_absolute_x_4()
{
	READ_FROM_ADDRESS;
	DO_SLO;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SLO_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::SLO_absolute_y_2;
}
void CPU::SLO_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::SLO_absolute_y_3;
}
void CPU::SLO_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SLO_absolute_y_4;
	} else {
		DO_SLO;
		DONE;
	}
}
void CPU::SLO_absolute_y_4()
{
	READ_FROM_ADDRESS;
	DO_SLO;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SLO_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SLO_indirect_x_2;
}
void CPU::SLO_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::SLO_indirect_x_3;
}
void CPU::SLO_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SLO_indirect_x_4;
}
void CPU::SLO_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::SLO_indirect_x_5;
}
void CPU::SLO_indirect_x_5()
{
	READ_FROM_ADDRESS;
	DO_SLO;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SLO_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SLO_indirect_y_2;
}
void CPU::SLO_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SLO_indirect_y_3;
}
void CPU::SLO_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::SLO_indirect_y_4;
}
void CPU::SLO_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SLO_indirect_y_5;
	} else {
		DO_SLO;
		DONE;
	}
}
void CPU::SLO_indirect_y_5()
{
	READ_FROM_ADDRESS;
	DO_SLO;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: SRE
//
// Operation:   LSR, followed by EOR
//
// Flags:       N Z C I D V
//              / / / - - -
// -------------------------------------------------------------------------------

#define DO_SRE_ZERO_PAGE setC(data & 1); data >>= 1; loadM(addr_lo, data); loadA(A ^ data);
#define DO_SRE setC(data & 1); data >>= 1; loadM((addr_hi << 8) | addr_lo, data); loadA(A ^ data);

void CPU::SRE_absolute() {
	FETCH_ADDR_LO;
	next = &CPU::SRE_absolute_2;
}
void CPU::SRE_absolute_2() {
	FETCH_ADDR_HI;
	next = &CPU::SRE_absolute_3;
}
void CPU::SRE_absolute_3() {
	READ_FROM_ADDRESS;
	DO_SRE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_zero_page()
{
	FETCH_ADDR_LO;
	next = &CPU::SRE_zero_page_2;
}
void CPU::SRE_zero_page_2()
{
	READ_FROM_ZERO_PAGE;
	DO_SRE_ZERO_PAGE;	
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_zero_page_x()
{
	FETCH_ADDR_LO;
	next = &CPU::SRE_zero_page_x_2;
}
void CPU::SRE_zero_page_x_2()
{
	READ_FROM_ZERO_PAGE;
	ADD_INDEX_X;
	next = &CPU::SRE_zero_page_x_3;
}
void CPU::SRE_zero_page_x_3()
{
	READ_FROM_ZERO_PAGE;
	DO_SRE_ZERO_PAGE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_absolute_x()
{
	FETCH_ADDR_LO;
	next = &CPU::SRE_absolute_x_2;
}
void CPU::SRE_absolute_x_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_X;
	next = &CPU::SRE_absolute_x_3;
}
void CPU::SRE_absolute_x_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SRE_absolute_x_4;
	} else {
		DO_SRE;
		DONE;
	}
}
void CPU::SRE_absolute_x_4()
{
	READ_FROM_ADDRESS;
	DO_SRE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_absolute_y()
{
	FETCH_ADDR_LO;
	next = &CPU::SRE_absolute_y_2;
}
void CPU::SRE_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::SRE_absolute_y_3;
}
void CPU::SRE_absolute_y_3()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SRE_absolute_y_4;
	} else {
		DO_SRE;
		DONE;
	}
}
void CPU::SRE_absolute_y_4()
{
	READ_FROM_ADDRESS;
	DO_SRE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_indirect_x()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SRE_indirect_x_2;
}
void CPU::SRE_indirect_x_2()
{
	IDLE_READ_FROM_ADDRESS_INDIRECT;
	ADD_INDEX_X_INDIRECT;
	next = &CPU::SRE_indirect_x_3;
}
void CPU::SRE_indirect_x_3()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SRE_indirect_x_4;
}
void CPU::SRE_indirect_x_4()
{
	FETCH_ADDR_HI_INDIRECT;
	next = &CPU::SRE_indirect_x_5;
}
void CPU::SRE_indirect_x_5()
{
	READ_FROM_ADDRESS;
	DO_SRE;
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_indirect_y()
{
	FETCH_POINTER_ADDR;
	next = &CPU::SRE_indirect_y_2;
}
void CPU::SRE_indirect_y_2()
{
	FETCH_ADDR_LO_INDIRECT;
	next = &CPU::SRE_indirect_y_3;
}
void CPU::SRE_indirect_y_3()
{
	FETCH_ADDR_HI_INDIRECT;
	ADD_INDEX_Y;
	next = &CPU::SRE_indirect_y_4;
}
void CPU::SRE_indirect_y_4()
{
	READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::SRE_indirect_y_5;
	} else {
		DO_SRE;
		DONE;
	}
}
void CPU::SRE_indirect_y_5()
{
	READ_FROM_ADDRESS;
	DO_SRE;
	DONE;
}



#if 0
void CPU::SRE_zero_page()
{
	uint16_t addr = fetchAddressZeroPage();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_zero_page_x()
{
	uint16_t addr = fetchAddressZeroPageX();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_absolute()
{
	uint16_t addr = fetchAddressAbsolute();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}
void CPU::SRE_absolute_2()
{
}
void CPU::SRE_absolute_3()
{
}

// -------------------------------------------------------------------------------
void CPU::SRE_absolute_x()
{
	uint16_t addr = fetchAddressAbsoluteX();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_absolute_y()
{
	uint16_t addr = fetchAddressAbsoluteY();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_indirect_x()
{
	uint16_t addr = fetchAddressIndirectX();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}

// -------------------------------------------------------------------------------
void CPU::SRE_indirect_y()
{
	uint16_t addr = fetchAddressIndirectY();
	uint8_t op = mem->peek(addr);
	setC(op & 1);
	op >>= 1;
	loadM(addr, op);
	loadA(A ^ op);
	DONE;
}
#endif

// -------------------------------------------------------------------------------
// Instruction: TAS
//
// Operation:   SP := A & X,  Mem := SP & (HI_BYTE(op) + 1)
//
// Flags:       N Z C I D V
//              - - - - - -
//
// -------------------------------------------------------------------------------

void CPU::TAS_absolute_y()
{
	data = mem->peek(PC + 1) + 1;
	FETCH_ADDR_LO;
	next = &CPU::TAS_absolute_y_2;
}
void CPU::TAS_absolute_y_2()
{
	FETCH_ADDR_HI;
	ADD_INDEX_Y;
	next = &CPU::TAS_absolute_y_3;
}
void CPU::TAS_absolute_y_3()
{
	IDLE_READ_FROM_ADDRESS;
	if (PAGE_BOUNDARY_CROSSED) {
		FIX_ADDR_HI;
		next = &CPU::TAS_absolute_y_4;
	} else {
		SP = A & X;
		data &= SP;
		WRITE_TO_ADDRESS;
		DONE;
	}
}
void CPU::TAS_absolute_y_4()
{
	SP = A & X;
	data &= SP;
	WRITE_TO_ADDRESS;
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: ANE
//
// Operation:   A = X & op & (A | 0xEE) (taken from Frodo)
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::ANE_immediate()
{
	READ_IMMEDIATE;
	loadA(X & data & (A | 0xEE));
	DONE;
}


// -------------------------------------------------------------------------------
// Instruction: LXA
//
// Operation:   A = X = op & (A | 0xEE) (taken from Frodo)
//
// Flags:       N Z C I D V
//              / / - - - -
// -------------------------------------------------------------------------------

void CPU::LXA_immediate()
{
	READ_IMMEDIATE;
	X = data & (A | 0xEE);
	loadA(X); 
	DONE;
}
