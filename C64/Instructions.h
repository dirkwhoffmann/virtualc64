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

// Instruction set
char *mnemonic[256];
AddressingMode addressingMode[256];


//! Register callback function for a single opcode
void registerCallback(uint8_t opcode, int (CPU::*func)());

//! Register callback function for a single opcode
void registerCallback(uint8_t opcode, char *mnemonic, AddressingMode mode, int (CPU::*func)());

//! Register instruction set
void registerInstructions();


int  defaultCallback();

// Operand handling
void fetchOperandImplied();
void fetchOperandAccumulator();
uint8_t fetchOperandImmediate();
uint16_t fetchAddressZeroPage();
uint8_t fetchOperandZeroPage();
uint16_t fetchAddressZeroPageX();
uint8_t fetchOperandZeroPageX();
uint16_t fetchAddressZeroPageY();
uint8_t fetchOperandZeroPageY();
uint16_t fetchAddressAbsolute();
uint8_t fetchOperandAbsolute();
uint16_t fetchAddressAbsoluteX();
uint8_t fetchOperandAbsoluteX();
uint16_t fetchAddressAbsoluteY();
uint8_t fetchOperandAbsoluteY();
uint16_t fetchAddressIndirectX();
uint8_t fetchOperandIndirectX();
uint16_t fetchAddressIndirectY();
uint8_t fetchOperandIndirectY();
uint8_t fetchOperandRelative();

// Helper functions
void CPU::adc(uint8_t op);
void CPU::adc_binary(uint8_t op);
void CPU::adc_bcd(uint8_t op);
void CPU::sbc(uint8_t op);
void CPU::sbc_binary(uint8_t op);
void CPU::sbc_bcd(uint8_t op);
int  CPU::branch(int8_t offset); // returns number of penalty cycles
void CPU::cmp(uint8_t op1, uint8_t op2);
uint8_t CPU::ror(uint8_t op);
uint8_t CPU::rol(uint8_t op);

// Callback functions
int ADC_immediate();
int ADC_zero_page();
int ADC_zero_page_x();
int ADC_absolute();
int ADC_absolute_x();
int ADC_absolute_y();
int ADC_indirect_x();
int ADC_indirect_y();

int AND_immediate();
int AND_zero_page();
int AND_zero_page_x();
int AND_absolute();
int AND_absolute_x();
int AND_absolute_y();
int AND_indirect_x();
int AND_indirect_y();

int ASL_accumulator();
int ASL_zero_page();
int ASL_zero_page_x();
int ASL_absolute();
int ASL_absolute_x();
void ASL_absolute_y(); // Used by illegal instructions, only
void ASL_indirect_x(); // Used by illegal instructions, only
void ASL_indirect_y(); // Used by illegal instructions, only

int BCC_relative();
int BCS_relative();
int BEQ_relative();

int BIT_zero_page();
int BIT_absolute();

int BMI_relative();
int BNE_relative();
int BPL_relative();

int BRK();

int BVC_relative();
int BVS_relative();
int CLC();
int CLD();
int CLI();
int CLV();

int CMP_immediate();
int CMP_zero_page();
int CMP_zero_page_x();
int CMP_absolute();
int CMP_absolute_x();
int CMP_absolute_y();
int CMP_indirect_x();
int CMP_indirect_y();

int CPX_immediate();
int CPX_zero_page();
int CPX_absolute();

int CPY_immediate();
int CPY_zero_page();
int CPY_absolute();

int DEC_zero_page();
int DEC_zero_page_x();
int DEC_absolute();
int DEC_absolute_x();
void DEC_absolute_y(); // Used by illegal instructions, only
void DEC_indirect_x(); // Used by illegal instructions, only
void DEC_indirect_y(); // Used by illegal instructions, only

int DEX();
int DEY();

int EOR_immediate();
int EOR_zero_page();
int EOR_zero_page_x();
int EOR_absolute();
int EOR_absolute_x();
int EOR_absolute_y();
int EOR_indirect_x();
int EOR_indirect_y();

int INC_zero_page();
int INC_zero_page_x();
int INC_absolute();
int INC_absolute_x();
void INC_absolute_y(); // Used by illegal instructions, only
void INC_indirect_x(); // Used by illegal instructions, only
void INC_indirect_y(); // Used by illegal instructions, only


int INX();
int INY();

int JMP_absolute();
int JMP_absolute_indirect();

int JSR();

int LDA_immediate();
int LDA_zero_page();
int LDA_zero_page_x();
void LDA_zero_page_y(); // Used by illegal instructions, only
int LDA_absolute();
int LDA_absolute_x();
int LDA_absolute_y();
int LDA_indirect_x();
int LDA_indirect_y();

int LDX_immediate(); 
int LDX_zero_page();
int LDX_zero_page_y();
int LDX_absolute();
int LDX_absolute_y();
void LDX_indirect_x(); // Used by illegal instructions, only
void LDX_indirect_y(); // Used by illegal instructions, only

int LDY_immediate(); 
int LDY_zero_page(); 
int LDY_zero_page_x();
int LDY_absolute();
int LDY_absolute_x();

int LSR_accumulator();
int LSR_zero_page();
int LSR_zero_page_x();
int LSR_absolute();
int LSR_absolute_x();
void LSR_absolute_y(); // Used by illegal instructions, only
void LSR_indirect_x(); // Used by illegal instructions, only
void LSR_indirect_y(); // Used by illegal instructions, only

int NOP();

int ORA_immediate();
int ORA_zero_page();
int ORA_zero_page_x();
int ORA_absolute();
int ORA_absolute_x();
int ORA_absolute_y();
int ORA_indirect_x();
int ORA_indirect_y();

int PHA();
int PHP();
int PLA();
int PLP();

int ROL_accumulator(); 
int ROL_zero_page(); 
int ROL_zero_page_x();
int ROL_absolute();
int ROL_absolute_x();
void ROL_absolute_y(); // Used by illegal instructions, only
void ROL_indirect_x(); // Used by illegal instructions, only
void ROL_indirect_y(); // Used by illegal instructions, only

int ROR_accumulator(); 
int ROR_zero_page(); 
int ROR_zero_page_x();
int ROR_absolute();
int ROR_absolute_x();
void ROR_absolute_y(); // Used by illegal instructions, only
void ROR_indirect_x(); // Used by illegal instructions, only
void ROR_indirect_y(); // Used by illegal instructions, only

int RTI();
int RTS();

int SBC_immediate();
int SBC_zero_page();
int SBC_zero_page_x();
int SBC_absolute();
int SBC_absolute_x();
int SBC_absolute_y();
int SBC_indirect_x();
int SBC_indirect_y();

int SEC();
int SED();

int SEI();

int STA_zero_page();
int STA_zero_page_x();
int STA_absolute();
int STA_absolute_x();
int STA_absolute_y();
int STA_indirect_x();
int STA_indirect_y();

int STX_zero_page();
int STX_zero_page_y();
int STX_absolute();

int STY_zero_page();
int STY_zero_page_x();
int STY_absolute();

int TAX();
int TAY();
int TSX();
int TXA();
int TXS();
int TYA();

// Illegal instructions

int AHX_indirect_y();  // OK
int AHX_absolute_y();

int ALR_immediate();   // OK
int ANC_immediate();   // OK // Note: Has two opcodes (0x0B and 0x2B)
int ARR_immediate();   // OK
int AXS_immediate();   // OK

int DCP_zero_page();   // OK
int DCP_zero_page_x();
int DCP_absolute();
int DCP_absolute_x();
int DCP_absolute_y();
int DCP_indirect_x();
int DCP_indirect_y();

int ISC_zero_page();   // OK
int ISC_zero_page_x();
int ISC_absolute();
int ISC_absolute_x();
int ISC_absolute_y();
int ISC_indirect_x();
int ISC_indirect_y();

int LAS_absolute_y();

int LAX_immediate();   // OK // Highly unstable instruction
int LAX_zero_page();
int LAX_zero_page_y();
int LAX_absolute();
int LAX_absolute_y();
int LAX_indirect_x();
int LAX_indirect_y();

int NOP_immediate();
int NOP_zero_page();
int NOP_zero_page_x();
int NOP_absolute();
int NOP_absolute_x();

int RLA_zero_page();    // OK
int RLA_zero_page_x();
int RLA_absolute();
int RLA_absolute_x();
int RLA_absolute_y();
int RLA_indirect_x();
int RLA_indirect_y();

int RRA_zero_page();   // OK
int RRA_zero_page_x();
int RRA_absolute();
int RRA_absolute_x();
int RRA_absolute_y();
int RRA_indirect_x();
int RRA_indirect_y();

int SAX_zero_page();  // OK
int SAX_zero_page_y();
int SAX_absolute();
int SAX_indirect_x();

int SHX_absolute_y(); // OK
int SHY_absolute_x(); // OK

int SLO_zero_page();  // OK
int SLO_zero_page_x();
int SLO_absolute();
int SLO_absolute_x();
int SLO_absolute_y();
int SLO_indirect_x();
int SLO_indirect_y();

int SRE_zero_page();  // OK
int SRE_zero_page_x();
int SRE_absolute();
int SRE_absolute_x();
int SRE_absolute_y();
int SRE_indirect_x();
int SRE_indirect_y();

int TAS_absolute_y();

int XAA_immediate();



