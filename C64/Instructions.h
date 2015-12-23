/*
 * (C) 2006-2009 Dirk W. Hoffmann. All rights reserved.
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

// Atomic CPU tasks
#define FETCH_OPCODE if (rdyLine) opcode = mem->peek(PC++); else return;
#define FETCH_ADDR_LO if (rdyLine) addr_lo = mem->peek(PC++); else return;
#define FETCH_ADDR_HI if (rdyLine) addr_hi = mem->peek(PC++); else return;
#define FETCH_POINTER_ADDR if (rdyLine) ptr = mem->peek(PC++); else return;
#define FETCH_ADDR_LO_INDIRECT if (rdyLine) addr_lo = mem->peek((uint16_t)ptr++); else return;
#define FETCH_ADDR_HI_INDIRECT if (rdyLine) addr_hi = mem->peek((uint16_t)ptr++); else return;

#define READ_RELATIVE if (rdyLine) data = mem->peek(PC); else return;
#define READ_IMMEDIATE if (rdyLine) data = mem->peek(PC++); else return;
#define READ_FROM_ADDRESS if (rdyLine) data = mem->peek((addr_hi << 8) | addr_lo); else return;
#define READ_FROM_ZERO_PAGE if (rdyLine) data = mem->peek((uint16_t)addr_lo); else return;
#define READ_FROM_ADDRESS_INDIRECT if (rdyLine) data = mem->peek((uint16_t)ptr); else return;
#define IDLE_READ_FROM(x) if (rdyLine) (void)mem->peek(x); else return;
#define IDLE_READ_IMPLIED if (rdyLine) (void)mem->peek(PC); else return;
#define IDLE_READ_IMMEDIATE if (rdyLine) (void)mem->peek(PC++); else return;
#define IDLE_READ_IMMEDIATE_SP if (rdyLine) (void)mem->peek(0x100 | SP++); else return;
#define IDLE_READ_FROM_ADDRESS if (rdyLine) (void)(mem->peek((addr_hi << 8) | addr_lo)); else return;
#define IDLE_READ_FROM_ZERO_PAGE if (rdyLine) (void)mem->peek((uint16_t)addr_lo); else return;
#define IDLE_READ_FROM_ADDRESS_INDIRECT if (rdyLine) (void)mem->peek((uint16_t)ptr); else return;

#define WRITE_TO_ADDRESS mem->poke((addr_hi << 8) | addr_lo, data);
#define WRITE_TO_ADDRESS_AND_SET_FLAGS loadM((addr_hi << 8) | addr_lo, data);
#define WRITE_TO_ZERO_PAGE mem->poke((uint16_t)addr_lo, data);
#define WRITE_TO_ZERO_PAGE_AND_SET_FLAGS loadM((uint16_t)addr_lo, data);

#define ADD_INDEX_X overflow = ((int)addr_lo + (int)X >= 0x100); addr_lo += X; 
#define ADD_INDEX_Y overflow = ((int)addr_lo + (int)Y >= 0x100); addr_lo += Y; 
#define ADD_INDEX_X_INDIRECT ptr += X;
#define ADD_INDEX_Y_INDIRECT ptr += Y;

#define PUSH_PCL mem->poke(0x100+(SP--), LO_BYTE(PC));
#define PUSH_PCH mem->poke(0x100+(SP--), HI_BYTE(PC));
#define PUSH_P mem->poke(0x100+(SP--), getP());
#define PUSH_P_WITH_B_SET mem->poke(0x100+(SP--), getP() | B_FLAG);
#define PUSH_A mem->poke(0x100+(SP--), A); 
#define PULL_PCL if (rdyLine) setPCL(mem->peek(0x100 | SP)); else return;
#define PULL_PCH if (rdyLine) setPCH(mem->peek(0x100 | SP)); else return;
#define PULL_P if (rdyLine) setPWithoutB(mem->peek(0x100 | SP)); else return;
#define PULL_A if (rdyLine) loadA(mem->peek(0x100 | SP)); else return;

#define PAGE_BOUNDARY_CROSSED overflow
#define FIX_ADDR_HI addr_hi++;

#define DONE next = &CPU::fetch;

//! Mnemonic strings (used by the source level debugger only)
const char *mnemonic[256];

//! Adressing mode (used by the source level debugger only)
AddressingMode addressingMode[256];

//! Static array containing all callback function
static void (CPU::*callbacks[])(void);

//! Register callback function for a single opcode
void registerCallback(uint8_t opcode, void (CPU::*func)(void));

//! Register callback function for a single opcode
void registerCallback(uint8_t opcode, const char *mnemonic, AddressingMode mode, void (CPU::*func)(void));

//! Register illegal instructions
void registerIllegalInstructions();

//! Register complete instruction set
void registerInstructions();

// Helper functions
void adc(uint8_t op);
void adc_binary(uint8_t op);
void adc_bcd(uint8_t op);
void sbc(uint8_t op);
void sbc_binary(uint8_t op);
void sbc_bcd(uint8_t op);
void branch(int8_t offset); // returns number of penalty cycles
void cmp(uint8_t op1, uint8_t op2);
uint8_t ror(uint8_t op);
uint8_t rol(uint8_t op);

// Execution functions
public: 

void fetch();

void JAM(); void JAM_2(); // Default action for illegal instructions

void irq(); void irq_2(); void irq_3(); void irq_4(); void irq_5(); void irq_6(); void irq_7();
void nmi(); void nmi_2(); void nmi_3(); void nmi_4(); void nmi_5(); void nmi_6(); void nmi_7();

void ADC_immediate();
void ADC_zero_page(); void ADC_zero_page_2();
void ADC_zero_page_x(); void ADC_zero_page_x_2(); void ADC_zero_page_x_3();
void ADC_absolute(); void ADC_absolute_2(); void ADC_absolute_3();
void ADC_absolute_x(); void ADC_absolute_x_2(); void ADC_absolute_x_3(); void ADC_absolute_x_4();
void ADC_absolute_y(); void ADC_absolute_y_2(); void ADC_absolute_y_3(); void ADC_absolute_y_4();
void ADC_indirect_x(); void ADC_indirect_x_2(); void ADC_indirect_x_3(); void ADC_indirect_x_4(); void ADC_indirect_x_5();
void ADC_indirect_y(); void ADC_indirect_y_2(); void ADC_indirect_y_3(); void ADC_indirect_y_4(); void ADC_indirect_y_5();

void AND_immediate();
void AND_zero_page(); void AND_zero_page_2();
void AND_zero_page_x(); void AND_zero_page_x_2(); void AND_zero_page_x_3();
void AND_absolute(); void AND_absolute_2(); void AND_absolute_3();
void AND_absolute_x(); void AND_absolute_x_2(); void AND_absolute_x_3(); void AND_absolute_x_4();
void AND_absolute_y(); void AND_absolute_y_2(); void AND_absolute_y_3(); void AND_absolute_y_4();
void AND_indirect_x(); void AND_indirect_x_2(); void AND_indirect_x_3(); void AND_indirect_x_4(); void AND_indirect_x_5();
void AND_indirect_y(); void AND_indirect_y_2(); void AND_indirect_y_3(); void AND_indirect_y_4(); void AND_indirect_y_5();

void ASL_accumulator();
void ASL_zero_page(); void ASL_zero_page_2(); void ASL_zero_page_3(); void ASL_zero_page_4(); 
void ASL_zero_page_x(); void ASL_zero_page_x_2(); void ASL_zero_page_x_3(); void ASL_zero_page_x_4(); void ASL_zero_page_x_5();
void ASL_absolute(); void ASL_absolute_2(); void ASL_absolute_3(); void ASL_absolute_4(); void ASL_absolute_5();
void ASL_absolute_x(); void ASL_absolute_x_2(); void ASL_absolute_x_3(); void ASL_absolute_x_4(); void ASL_absolute_x_5(); void ASL_absolute_x_6();
void ASL_indirect_x(); void ASL_indirect_x_2(); void ASL_indirect_x_3(); void ASL_indirect_x_4(); void ASL_indirect_x_5(); void ASL_indirect_x_6(); void ASL_indirect_x_7();     

void branch_3_underflow(); void branch_3_overflow();
void BCC_relative(); void BCC_relative_2(); 
void BCS_relative(); void BCS_relative_2(); 
void BEQ_relative(); void BEQ_relative_2(); 

void BIT_zero_page(); void BIT_zero_page_2();
void BIT_absolute(); void BIT_absolute_2(); void BIT_absolute_3();

void BMI_relative(); void BMI_relative_2(); 
void BNE_relative(); void BNE_relative_2(); 
void BPL_relative(); void BPL_relative_2(); 

void BRK(); void BRK_2(); void BRK_3(); void BRK_4(); void BRK_5(); void BRK_6();
void BRK_nmi_4(); void BRK_nmi_5(); void BRK_nmi_6(); 

void BVC_relative(); void BVC_relative_2(); 
void BVS_relative(); void BVS_relative_2(); 
void CLC();
void CLD();
void CLI();
void CLV();

void CMP_immediate();
void CMP_zero_page(); void CMP_zero_page_2();
void CMP_zero_page_x(); void CMP_zero_page_x_2(); void CMP_zero_page_x_3();
void CMP_absolute(); void CMP_absolute_2(); void CMP_absolute_3();
void CMP_absolute_x(); void CMP_absolute_x_2(); void CMP_absolute_x_3(); void CMP_absolute_x_4();
void CMP_absolute_y(); void CMP_absolute_y_2(); void CMP_absolute_y_3(); void CMP_absolute_y_4();
void CMP_indirect_x(); void CMP_indirect_x_2(); void CMP_indirect_x_3(); void CMP_indirect_x_4(); void CMP_indirect_x_5();
void CMP_indirect_y(); void CMP_indirect_y_2(); void CMP_indirect_y_3(); void CMP_indirect_y_4(); void CMP_indirect_y_5();

void CPX_immediate();
void CPX_zero_page(); void CPX_zero_page_2(); 
void CPX_absolute(); void CPX_absolute_2(); void CPX_absolute_3();

void CPY_immediate();
void CPY_zero_page(); void CPY_zero_page_2();
void CPY_absolute(); void CPY_absolute_2(); void CPY_absolute_3();

void DEC_zero_page(); void DEC_zero_page_2(); void DEC_zero_page_3(); void DEC_zero_page_4();
void DEC_zero_page_x(); void DEC_zero_page_x_2(); void DEC_zero_page_x_3(); void DEC_zero_page_x_4(); void DEC_zero_page_x_5();
void DEC_absolute(); void DEC_absolute_2(); void DEC_absolute_3(); void DEC_absolute_4(); void DEC_absolute_5();
void DEC_absolute_x(); void DEC_absolute_x_2(); void DEC_absolute_x_3(); void DEC_absolute_x_4(); void DEC_absolute_x_5(); void DEC_absolute_x_6();
void DEC_indirect_x(); void DEC_indirect_x_2(); void DEC_indirect_x_3(); void DEC_indirect_x_4(); void DEC_indirect_x_5(); void DEC_indirect_x_6(); void DEC_indirect_x_7();     

void DEX();
void DEY();

void EOR_immediate();
void EOR_zero_page(); void EOR_zero_page_2();
void EOR_zero_page_x(); void EOR_zero_page_x_2(); void EOR_zero_page_x_3();
void EOR_absolute(); void EOR_absolute_2(); void EOR_absolute_3();
void EOR_absolute_x(); void EOR_absolute_x_2(); void EOR_absolute_x_3(); void EOR_absolute_x_4();
void EOR_absolute_y(); void EOR_absolute_y_2(); void EOR_absolute_y_3(); void EOR_absolute_y_4();
void EOR_indirect_x(); void EOR_indirect_x_2(); void EOR_indirect_x_3(); void EOR_indirect_x_4(); void EOR_indirect_x_5();
void EOR_indirect_y(); void EOR_indirect_y_2(); void EOR_indirect_y_3(); void EOR_indirect_y_4(); void EOR_indirect_y_5();

void INC_zero_page(); void INC_zero_page_2(); void INC_zero_page_3(); void INC_zero_page_4();
void INC_zero_page_x(); void INC_zero_page_x_2(); void INC_zero_page_x_3(); void INC_zero_page_x_4(); void INC_zero_page_x_5();
void INC_absolute(); void INC_absolute_2(); void INC_absolute_3(); void INC_absolute_4(); void INC_absolute_5();
void INC_absolute_x(); void INC_absolute_x_2(); void INC_absolute_x_3(); void INC_absolute_x_4(); void INC_absolute_x_5(); void INC_absolute_x_6();
void INC_indirect_x(); void INC_indirect_x_2(); void INC_indirect_x_3(); void INC_indirect_x_4(); void INC_indirect_x_5(); void INC_indirect_x_6(); void INC_indirect_x_7();     

void INX();
void INY();

void JMP_absolute(); void JMP_absolute_2();
void JMP_absolute_indirect(); void JMP_absolute_indirect_2(); void JMP_absolute_indirect_3(); void JMP_absolute_indirect_4();

void JSR(); void JSR_2(); void JSR_3(); void JSR_4(); void JSR_5();

void LDA_immediate();
void LDA_zero_page(); void LDA_zero_page_2();
void LDA_zero_page_x(); void LDA_zero_page_x_2(); void LDA_zero_page_x_3();
void LDA_absolute(); void LDA_absolute_2(); void LDA_absolute_3();
void LDA_absolute_x(); void LDA_absolute_x_2(); void LDA_absolute_x_3(); void LDA_absolute_x_4();
void LDA_absolute_y(); void LDA_absolute_y_2(); void LDA_absolute_y_3(); void LDA_absolute_y_4();
void LDA_indirect_x(); void LDA_indirect_x_2(); void LDA_indirect_x_3(); void LDA_indirect_x_4(); void LDA_indirect_x_5();
void LDA_indirect_y(); void LDA_indirect_y_2(); void LDA_indirect_y_3(); void LDA_indirect_y_4(); void LDA_indirect_y_5();

void LDX_immediate();
void LDX_zero_page(); void LDX_zero_page_2();
void LDX_zero_page_y(); void LDX_zero_page_y_2(); void LDX_zero_page_y_3();
void LDX_absolute(); void LDX_absolute_2(); void LDX_absolute_3();
void LDX_absolute_y(); void LDX_absolute_y_2(); void LDX_absolute_y_3(); void LDX_absolute_y_4();
void LDX_indirect_x(); void LDX_indirect_x_2(); void LDX_indirect_x_3(); void LDX_indirect_x_4(); void LDX_indirect_x_5();
void LDX_indirect_y(); void LDX_indirect_y_2(); void LDX_indirect_y_3(); void LDX_indirect_y_4(); void LDX_indirect_y_5();

void LDY_immediate();
void LDY_zero_page(); void LDY_zero_page_2();
void LDY_zero_page_x(); void LDY_zero_page_x_2(); void LDY_zero_page_x_3();
void LDY_absolute(); void LDY_absolute_2(); void LDY_absolute_3();
void LDY_absolute_x(); void LDY_absolute_x_2(); void LDY_absolute_x_3(); void LDY_absolute_x_4();
void LDY_indirect_x(); void LDY_indirect_x_2(); void LDY_indirect_x_3(); void LDY_indirect_x_4(); void LDY_indirect_x_5();
void LDY_indirect_y(); void LDY_indirect_y_2(); void LDY_indirect_y_3(); void LDY_indirect_y_4(); void LDY_indirect_y_5();

void LSR_accumulator();
void LSR_zero_page(); void LSR_zero_page_2(); void LSR_zero_page_3(); void LSR_zero_page_4();
void LSR_zero_page_x(); void LSR_zero_page_x_2(); void LSR_zero_page_x_3(); void LSR_zero_page_x_4(); void LSR_zero_page_x_5();
void LSR_absolute(); void LSR_absolute_2(); void LSR_absolute_3(); void LSR_absolute_4(); void LSR_absolute_5();
void LSR_absolute_x(); void LSR_absolute_x_2(); void LSR_absolute_x_3(); void LSR_absolute_x_4(); void LSR_absolute_x_5(); void LSR_absolute_x_6();
void LSR_absolute_y(); void LSR_absolute_y_2(); void LSR_absolute_y_3(); void LSR_absolute_y_4(); void LSR_absolute_y_5(); void LSR_absolute_y_6(); 
void LSR_indirect_x(); void LSR_indirect_x_2(); void LSR_indirect_x_3(); void LSR_indirect_x_4(); void LSR_indirect_x_5(); void LSR_indirect_x_6(); void LSR_indirect_x_7();     
void LSR_indirect_y(); void LSR_indirect_y_2(); void LSR_indirect_y_3(); void LSR_indirect_y_4(); void LSR_indirect_y_5(); void LSR_indirect_y_6(); void LSR_indirect_y_7();

void NOP();
void NOP_immediate();
void NOP_zero_page(); void NOP_zero_page_2();
void NOP_zero_page_x(); void NOP_zero_page_x_2(); void NOP_zero_page_x_3();
void NOP_absolute(); void NOP_absolute_2(); void NOP_absolute_3();
void NOP_absolute_x(); void NOP_absolute_x_2(); void NOP_absolute_x_3(); void NOP_absolute_x_4();

void ORA_immediate();
void ORA_zero_page(); void ORA_zero_page_2();
void ORA_zero_page_x(); void ORA_zero_page_x_2(); void ORA_zero_page_x_3();
void ORA_absolute(); void ORA_absolute_2(); void ORA_absolute_3();
void ORA_absolute_x(); void ORA_absolute_x_2(); void ORA_absolute_x_3(); void ORA_absolute_x_4();
void ORA_absolute_y(); void ORA_absolute_y_2(); void ORA_absolute_y_3(); void ORA_absolute_y_4();
void ORA_indirect_x(); void ORA_indirect_x_2(); void ORA_indirect_x_3(); void ORA_indirect_x_4(); void ORA_indirect_x_5();
void ORA_indirect_y(); void ORA_indirect_y_2(); void ORA_indirect_y_3(); void ORA_indirect_y_4(); void ORA_indirect_y_5();

void PHA(); void PHA_2();
void PHP(); void PHP_2();
void PLA(); void PLA_2(); void PLA_3();
void PLP(); void PLP_2(); void PLP_3();

void ROL_accumulator();
void ROL_zero_page(); void ROL_zero_page_2(); void ROL_zero_page_3(); void ROL_zero_page_4();
void ROL_zero_page_x(); void ROL_zero_page_x_2(); void ROL_zero_page_x_3(); void ROL_zero_page_x_4(); void ROL_zero_page_x_5();
void ROL_absolute(); void ROL_absolute_2(); void ROL_absolute_3(); void ROL_absolute_4(); void ROL_absolute_5();
void ROL_absolute_x(); void ROL_absolute_x_2(); void ROL_absolute_x_3(); void ROL_absolute_x_4(); void ROL_absolute_x_5(); void ROL_absolute_x_6();
void ROL_indirect_x(); void ROL_indirect_x_2(); void ROL_indirect_x_3(); void ROL_indirect_x_4(); void ROL_indirect_x_5(); void ROL_indirect_x_6(); void ROL_indirect_x_7();     

void ROR_accumulator();
void ROR_zero_page(); void ROR_zero_page_2(); void ROR_zero_page_3(); void ROR_zero_page_4();
void ROR_zero_page_x(); void ROR_zero_page_x_2(); void ROR_zero_page_x_3(); void ROR_zero_page_x_4(); void ROR_zero_page_x_5();
void ROR_absolute(); void ROR_absolute_2(); void ROR_absolute_3(); void ROR_absolute_4(); void ROR_absolute_5();
void ROR_absolute_x(); void ROR_absolute_x_2(); void ROR_absolute_x_3(); void ROR_absolute_x_4(); void ROR_absolute_x_5(); void ROR_absolute_x_6();
void ROR_indirect_x(); void ROR_indirect_x_2(); void ROR_indirect_x_3(); void ROR_indirect_x_4(); void ROR_indirect_x_5(); void ROR_indirect_x_6(); void ROR_indirect_x_7();     

void RTI(); void RTI_2(); void RTI_3(); void RTI_4(); void RTI_5();
void RTS(); void RTS_2(); void RTS_3(); void RTS_4(); void RTS_5();

void SBC_immediate();
void SBC_zero_page(); void SBC_zero_page_2();
void SBC_zero_page_x(); void SBC_zero_page_x_2(); void SBC_zero_page_x_3();
void SBC_absolute(); void SBC_absolute_2(); void SBC_absolute_3();
void SBC_absolute_x(); void SBC_absolute_x_2(); void SBC_absolute_x_3(); void SBC_absolute_x_4();
void SBC_absolute_y(); void SBC_absolute_y_2(); void SBC_absolute_y_3(); void SBC_absolute_y_4();
void SBC_indirect_x(); void SBC_indirect_x_2(); void SBC_indirect_x_3(); void SBC_indirect_x_4(); void SBC_indirect_x_5();
void SBC_indirect_y(); void SBC_indirect_y_2(); void SBC_indirect_y_3(); void SBC_indirect_y_4(); void SBC_indirect_y_5();

void SEC();
void SED();
void SEI();

void STA_zero_page(); void STA_zero_page_2();
void STA_zero_page_x(); void STA_zero_page_x_2(); void STA_zero_page_x_3();
void STA_absolute(); void STA_absolute_2(); void STA_absolute_3();
void STA_absolute_x(); void STA_absolute_x_2(); void STA_absolute_x_3(); void STA_absolute_x_4();
void STA_absolute_y(); void STA_absolute_y_2(); void STA_absolute_y_3(); void STA_absolute_y_4();
void STA_indirect_x(); void STA_indirect_x_2(); void STA_indirect_x_3(); void STA_indirect_x_4(); void STA_indirect_x_5();
void STA_indirect_y(); void STA_indirect_y_2(); void STA_indirect_y_3(); void STA_indirect_y_4(); void STA_indirect_y_5();

void STX_zero_page(); void STX_zero_page_2();
void STX_zero_page_y(); void STX_zero_page_y_2(); void STX_zero_page_y_3();
void STX_absolute(); void STX_absolute_2(); void STX_absolute_3();

void STY_zero_page(); void STY_zero_page_2();
void STY_zero_page_x(); void STY_zero_page_x_2(); void STY_zero_page_x_3();
void STY_absolute(); void STY_absolute_2(); void STY_absolute_3();

void TAX();
void TAY();
void TSX();
void TXA();
void TXS();
void TYA();

// Illegal instructions

void ALR_immediate();   
void ANC_immediate();   
void ANE_immediate();
void ARR_immediate();   
void AXS_immediate();   

void DCP_zero_page(); void DCP_zero_page_2(); void DCP_zero_page_3(); void DCP_zero_page_4();
void DCP_zero_page_x(); void DCP_zero_page_x_2(); void DCP_zero_page_x_3(); void DCP_zero_page_x_4(); void DCP_zero_page_x_5();
void DCP_absolute(); void DCP_absolute_2(); void DCP_absolute_3(); void DCP_absolute_4(); void DCP_absolute_5();
void DCP_absolute_x(); void DCP_absolute_x_2(); void DCP_absolute_x_3(); void DCP_absolute_x_4(); void DCP_absolute_x_5(); void DCP_absolute_x_6();
void DCP_absolute_y(); void DCP_absolute_y_2(); void DCP_absolute_y_3(); void DCP_absolute_y_4(); void DCP_absolute_y_5(); void DCP_absolute_y_6(); 
void DCP_indirect_x(); void DCP_indirect_x_2(); void DCP_indirect_x_3(); void DCP_indirect_x_4(); void DCP_indirect_x_5(); void DCP_indirect_x_6(); void DCP_indirect_x_7();     
void DCP_indirect_y(); void DCP_indirect_y_2(); void DCP_indirect_y_3(); void DCP_indirect_y_4(); void DCP_indirect_y_5(); void DCP_indirect_y_6(); void DCP_indirect_y_7();

void ISC_zero_page(); void ISC_zero_page_2(); void ISC_zero_page_3(); void ISC_zero_page_4();
void ISC_zero_page_x(); void ISC_zero_page_x_2(); void ISC_zero_page_x_3(); void ISC_zero_page_x_4(); void ISC_zero_page_x_5();
void ISC_absolute(); void ISC_absolute_2(); void ISC_absolute_3(); void ISC_absolute_4(); void ISC_absolute_5();
void ISC_absolute_x(); void ISC_absolute_x_2(); void ISC_absolute_x_3(); void ISC_absolute_x_4(); void ISC_absolute_x_5(); void ISC_absolute_x_6();
void ISC_absolute_y(); void ISC_absolute_y_2(); void ISC_absolute_y_3(); void ISC_absolute_y_4(); void ISC_absolute_y_5(); void ISC_absolute_y_6(); 
void ISC_indirect_x(); void ISC_indirect_x_2(); void ISC_indirect_x_3(); void ISC_indirect_x_4(); void ISC_indirect_x_5(); void ISC_indirect_x_6(); void ISC_indirect_x_7();     
void ISC_indirect_y(); void ISC_indirect_y_2(); void ISC_indirect_y_3(); void ISC_indirect_y_4(); void ISC_indirect_y_5(); void ISC_indirect_y_6(); void ISC_indirect_y_7();

void LAS_absolute_y(); void LAS_absolute_y_2(); void LAS_absolute_y_3(); void LAS_absolute_y_4();

void LAX_zero_page(); void LAX_zero_page_2();
void LAX_zero_page_y(); void LAX_zero_page_y_2(); void LAX_zero_page_y_3();
void LAX_absolute(); void LAX_absolute_2(); void LAX_absolute_3();
void LAX_absolute_y(); void LAX_absolute_y_2(); void LAX_absolute_y_3(); void LAX_absolute_y_4();
void LAX_indirect_x(); void LAX_indirect_x_2(); void LAX_indirect_x_3(); void LAX_indirect_x_4(); void LAX_indirect_x_5();
void LAX_indirect_y(); void LAX_indirect_y_2(); void LAX_indirect_y_3(); void LAX_indirect_y_4(); void LAX_indirect_y_5();

void LXA_immediate();

void RLA_zero_page(); void RLA_zero_page_2(); void RLA_zero_page_3(); void RLA_zero_page_4();
void RLA_zero_page_x(); void RLA_zero_page_x_2(); void RLA_zero_page_x_3(); void RLA_zero_page_x_4(); void RLA_zero_page_x_5();
void RLA_absolute(); void RLA_absolute_2(); void RLA_absolute_3(); void RLA_absolute_4(); void RLA_absolute_5();
void RLA_absolute_x(); void RLA_absolute_x_2(); void RLA_absolute_x_3(); void RLA_absolute_x_4(); void RLA_absolute_x_5(); void RLA_absolute_x_6();
void RLA_absolute_y(); void RLA_absolute_y_2(); void RLA_absolute_y_3(); void RLA_absolute_y_4(); void RLA_absolute_y_5(); void RLA_absolute_y_6();
void RLA_indirect_x(); void RLA_indirect_x_2(); void RLA_indirect_x_3(); void RLA_indirect_x_4(); void RLA_indirect_x_5(); void RLA_indirect_x_6(); void RLA_indirect_x_7();
void RLA_indirect_y(); void RLA_indirect_y_2(); void RLA_indirect_y_3(); void RLA_indirect_y_4(); void RLA_indirect_y_5(); void RLA_indirect_y_6(); void RLA_indirect_y_7();

void RRA_zero_page(); void RRA_zero_page_2(); void RRA_zero_page_3(); void RRA_zero_page_4();
void RRA_zero_page_x(); void RRA_zero_page_x_2(); void RRA_zero_page_x_3(); void RRA_zero_page_x_4(); void RRA_zero_page_x_5();
void RRA_absolute(); void RRA_absolute_2(); void RRA_absolute_3(); void RRA_absolute_4(); void RRA_absolute_5();
void RRA_absolute_x(); void RRA_absolute_x_2(); void RRA_absolute_x_3(); void RRA_absolute_x_4(); void RRA_absolute_x_5(); void RRA_absolute_x_6();
void RRA_absolute_y(); void RRA_absolute_y_2(); void RRA_absolute_y_3(); void RRA_absolute_y_4(); void RRA_absolute_y_5(); void RRA_absolute_y_6();
void RRA_indirect_x(); void RRA_indirect_x_2(); void RRA_indirect_x_3(); void RRA_indirect_x_4(); void RRA_indirect_x_5(); void RRA_indirect_x_6(); void RRA_indirect_x_7();
void RRA_indirect_y(); void RRA_indirect_y_2(); void RRA_indirect_y_3(); void RRA_indirect_y_4(); void RRA_indirect_y_5(); void RRA_indirect_y_6(); void RRA_indirect_y_7();

void SAX_zero_page(); void SAX_zero_page_2();
void SAX_zero_page_y(); void SAX_zero_page_y_2(); void SAX_zero_page_y_3();
void SAX_absolute(); void SAX_absolute_2(); void SAX_absolute_3();
void SAX_indirect_x(); void SAX_indirect_x_2(); void SAX_indirect_x_3(); void SAX_indirect_x_4(); void SAX_indirect_x_5();

void SHA_indirect_y(); void SHA_indirect_y_2(); void SHA_indirect_y_3(); void SHA_indirect_y_4(); void SHA_indirect_y_5();
void SHA_absolute_y(); void SHA_absolute_y_2(); void SHA_absolute_y_3(); void SHA_absolute_y_4();

void SHX_absolute_y(); void SHX_absolute_y_2(); void SHX_absolute_y_3(); void SHX_absolute_y_4();
void SHY_absolute_x(); void SHY_absolute_x_2(); void SHY_absolute_x_3(); void SHY_absolute_x_4();

void SLO_zero_page(); void SLO_zero_page_2(); void SLO_zero_page_3(); void SLO_zero_page_4();
void SLO_zero_page_x(); void SLO_zero_page_x_2(); void SLO_zero_page_x_3(); void SLO_zero_page_x_4(); void SLO_zero_page_x_5();
void SLO_absolute(); void SLO_absolute_2(); void SLO_absolute_3(); void SLO_absolute_4(); void SLO_absolute_5();
void SLO_absolute_x(); void SLO_absolute_x_2(); void SLO_absolute_x_3(); void SLO_absolute_x_4(); void SLO_absolute_x_5(); void SLO_absolute_x_6();
void SLO_absolute_y(); void SLO_absolute_y_2(); void SLO_absolute_y_3(); void SLO_absolute_y_4(); void SLO_absolute_y_5(); void SLO_absolute_y_6();
void SLO_indirect_x(); void SLO_indirect_x_2(); void SLO_indirect_x_3(); void SLO_indirect_x_4(); void SLO_indirect_x_5(); void SLO_indirect_x_6(); void SLO_indirect_x_7();
void SLO_indirect_y(); void SLO_indirect_y_2(); void SLO_indirect_y_3(); void SLO_indirect_y_4(); void SLO_indirect_y_5(); void SLO_indirect_y_6(); void SLO_indirect_y_7();

void SRE_zero_page(); void SRE_zero_page_2(); void SRE_zero_page_3(); void SRE_zero_page_4();
void SRE_zero_page_x(); void SRE_zero_page_x_2(); void SRE_zero_page_x_3(); void SRE_zero_page_x_4(); void SRE_zero_page_x_5();
void SRE_absolute(); void SRE_absolute_2(); void SRE_absolute_3(); void SRE_absolute_4(); void SRE_absolute_5();
void SRE_absolute_x(); void SRE_absolute_x_2(); void SRE_absolute_x_3(); void SRE_absolute_x_4(); void SRE_absolute_x_5(); void SRE_absolute_x_6();
void SRE_absolute_y(); void SRE_absolute_y_2(); void SRE_absolute_y_3(); void SRE_absolute_y_4(); void SRE_absolute_y_5(); void SRE_absolute_y_6();
void SRE_indirect_x(); void SRE_indirect_x_2(); void SRE_indirect_x_3(); void SRE_indirect_x_4(); void SRE_indirect_x_5(); void SRE_indirect_x_6(); void SRE_indirect_x_7();
void SRE_indirect_y(); void SRE_indirect_y_2(); void SRE_indirect_y_3(); void SRE_indirect_y_4(); void SRE_indirect_y_5(); void SRE_indirect_y_6(); void SRE_indirect_y_7();

void TAS_absolute_y(); void TAS_absolute_y_2(); void TAS_absolute_y_3(); void TAS_absolute_y_4(); 



