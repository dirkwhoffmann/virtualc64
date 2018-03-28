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

// Microinstructions
typedef enum {
    
    fetch,
    
    JAM, JAM_2,
    
    irq, irq_2, irq_3, irq_4, irq_5, irq_6, irq_7,
    nmi, nmi_2, nmi_3, nmi_4, nmi_5, nmi_6, nmi_7,
    
    ADC_imm,
    ADC_zpg,   ADC_zpg_2,
    ADC_zpg_x, ADC_zpg_x_2, ADC_zpg_x_3,
    ADC_abs,   ADC_abs_2,   ADC_abs_3,
    ADC_abs_x, ADC_abs_x_2, ADC_abs_x_3, ADC_abs_x_4,
    ADC_abs_y, ADC_abs_y_2, ADC_abs_y_3, ADC_abs_y_4,
    ADC_ind_x, ADC_ind_x_2, ADC_ind_x_3, ADC_ind_x_4, ADC_ind_x_5,
    ADC_ind_y, ADC_ind_y_2, ADC_ind_y_3, ADC_ind_y_4, ADC_ind_y_5,
    
    AND_imm,
    AND_zpg,   AND_zpg_2,
    AND_zpg_x, AND_zpg_x_2, AND_zpg_x_3,
    AND_abs,   AND_abs_2,   AND_abs_3,
    AND_abs_x, AND_abs_x_2, AND_abs_x_3, AND_abs_x_4,
    AND_abs_y, AND_abs_y_2, AND_abs_y_3, AND_abs_y_4,
    AND_ind_x, AND_ind_x_2, AND_ind_x_3, AND_ind_x_4, AND_ind_x_5,
    AND_ind_y, AND_ind_y_2, AND_ind_y_3, AND_ind_y_4, AND_ind_y_5,
    
    ASL_acc,
    ASL_zpg,   ASL_zpg_2,   ASL_zpg_3,   ASL_zpg_4,
    ASL_zpg_x, ASL_zpg_x_2, ASL_zpg_x_3, ASL_zpg_x_4, ASL_zpg_x_5,
    ASL_abs,   ASL_abs_2,   ASL_abs_3,   ASL_abs_4,   ASL_abs_5,
    ASL_abs_x, ASL_abs_x_2, ASL_abs_x_3, ASL_abs_x_4, ASL_abs_x_5, ASL_abs_x_6,
    ASL_ind_x, ASL_ind_x_2, ASL_ind_x_3, ASL_ind_x_4, ASL_ind_x_5, ASL_ind_x_6, ASL_ind_x_7,
    
    branch_3_underflow, branch_3_overflow,
    BCC_rel, BCC_rel_2,
    BCS_rel, BCS_rel_2,
    BEQ_rel, BEQ_rel_2,
    
    BIT_zpg, BIT_zpg_2,
    BIT_abs, BIT_abs_2, BIT_abs_3,
    
    BMI_rel, BMI_rel_2,
    BNE_rel, BNE_rel_2,
    BPL_rel, BPL_rel_2,
    
    BRK, BRK_2, BRK_3, BRK_4, BRK_5, BRK_6,
    BRK_nmi_4, BRK_nmi_5, BRK_nmi_6,
    
    BVC_rel, BVC_rel_2,
    BVS_rel, BVS_rel_2,
    CLC,
    CLD,
    CLI,
    CLV,
    
    CMP_imm,
    CMP_zpg,   CMP_zpg_2,
    CMP_zpg_x, CMP_zpg_x_2, CMP_zpg_x_3,
    CMP_abs,   CMP_abs_2,   CMP_abs_3,
    CMP_abs_x, CMP_abs_x_2, CMP_abs_x_3, CMP_abs_x_4,
    CMP_abs_y, CMP_abs_y_2, CMP_abs_y_3, CMP_abs_y_4,
    CMP_ind_x, CMP_ind_x_2, CMP_ind_x_3, CMP_ind_x_4, CMP_ind_x_5,
    CMP_ind_y, CMP_ind_y_2, CMP_ind_y_3, CMP_ind_y_4, CMP_ind_y_5,
    
    CPX_imm,
    CPX_zpg, CPX_zpg_2,
    CPX_abs, CPX_abs_2, CPX_abs_3,
    
    CPY_imm,
    CPY_zpg, CPY_zpg_2,
    CPY_abs, CPY_abs_2, CPY_abs_3,
    
    DEC_zpg,   DEC_zpg_2,   DEC_zpg_3,   DEC_zpg_4,
    DEC_zpg_x, DEC_zpg_x_2, DEC_zpg_x_3, DEC_zpg_x_4, DEC_zpg_x_5,
    DEC_abs,   DEC_abs_2,   DEC_abs_3,   DEC_abs_4,   DEC_abs_5,
    DEC_abs_x, DEC_abs_x_2, DEC_abs_x_3, DEC_abs_x_4, DEC_abs_x_5, DEC_abs_x_6,
    DEC_ind_x, DEC_ind_x_2, DEC_ind_x_3, DEC_ind_x_4, DEC_ind_x_5, DEC_ind_x_6, DEC_ind_x_7,
    
    DEX,
    DEY,
    
    EOR_imm,
    EOR_zpg,   EOR_zpg_2,
    EOR_zpg_x, EOR_zpg_x_2, EOR_zpg_x_3,
    EOR_abs,   EOR_abs_2,   EOR_abs_3,
    EOR_abs_x, EOR_abs_x_2, EOR_abs_x_3, EOR_abs_x_4,
    EOR_abs_y, EOR_abs_y_2, EOR_abs_y_3, EOR_abs_y_4,
    EOR_ind_x, EOR_ind_x_2, EOR_ind_x_3, EOR_ind_x_4, EOR_ind_x_5,
    EOR_ind_y, EOR_ind_y_2, EOR_ind_y_3, EOR_ind_y_4, EOR_ind_y_5,
    
    INC_zpg,   INC_zpg_2,   INC_zpg_3,   INC_zpg_4,
    INC_zpg_x, INC_zpg_x_2, INC_zpg_x_3, INC_zpg_x_4, INC_zpg_x_5,
    INC_abs,   INC_abs_2,   INC_abs_3,   INC_abs_4,   INC_abs_5,
    INC_abs_x, INC_abs_x_2, INC_abs_x_3, INC_abs_x_4, INC_abs_x_5, INC_abs_x_6,
    INC_ind_x, INC_ind_x_2, INC_ind_x_3, INC_ind_x_4, INC_ind_x_5, INC_ind_x_6, INC_ind_x_7,
    
    INX,
    INY,
    
    JMP_abs, JMP_abs_2,
    JMP_abs_indirect, JMP_abs_ind_2, JMP_abs_ind_3, JMP_abs_ind_4,
    
    JSR, JSR_2, JSR_3, JSR_4, JSR_5,
    
    LDA_imm,
    LDA_zpg,   LDA_zpg_2,
    LDA_zpg_x, LDA_zpg_x_2, LDA_zpg_x_3,
    LDA_abs,   LDA_abs_2,   LDA_abs_3,
    LDA_abs_x, LDA_abs_x_2, LDA_abs_x_3, LDA_abs_x_4,
    LDA_abs_y, LDA_abs_y_2, LDA_abs_y_3, LDA_abs_y_4,
    LDA_ind_x, LDA_ind_x_2, LDA_ind_x_3, LDA_ind_x_4, LDA_ind_x_5,
    LDA_ind_y, LDA_ind_y_2, LDA_ind_y_3, LDA_ind_y_4, LDA_ind_y_5,
    
    LDX_imm,
    LDX_zpg,   LDX_zpg_2,
    LDX_zpg_y, LDX_zpg_y_2, LDX_zpg_y_3,
    LDX_abs,   LDX_abs_2,   LDX_abs_3,
    LDX_abs_y, LDX_abs_y_2, LDX_abs_y_3, LDX_abs_y_4,
    LDX_ind_x, LDX_ind_x_2, LDX_ind_x_3, LDX_ind_x_4, LDX_ind_x_5,
    LDX_ind_y, LDX_ind_y_2, LDX_ind_y_3, LDX_ind_y_4, LDX_ind_y_5,
    
    LDY_imm,
    LDY_zpg,   LDY_zpg_2,
    LDY_zpg_x, LDY_zpg_x_2, LDY_zpg_x_3,
    LDY_abs,   LDY_abs_2,   LDY_abs_3,
    LDY_abs_x, LDY_abs_x_2, LDY_abs_x_3, LDY_abs_x_4,
    LDY_ind_x, LDY_ind_x_2, LDY_ind_x_3, LDY_ind_x_4, LDY_ind_x_5,
    LDY_ind_y, LDY_ind_y_2, LDY_ind_y_3, LDY_ind_y_4, LDY_ind_y_5,
    
    LSR_acc,
    LSR_zpg,   LSR_zpg_2,   LSR_zpg_3,   LSR_zpg_4,
    LSR_zpg_x, LSR_zpg_x_2, LSR_zpg_x_3, LSR_zpg_x_4, LSR_zpg_x_5,
    LSR_abs,   LSR_abs_2,   LSR_abs_3,   LSR_abs_4,   LSR_abs_5,
    LSR_abs_x, LSR_abs_x_2, LSR_abs_x_3, LSR_abs_x_4, LSR_abs_x_5, LSR_abs_x_6,
    LSR_abs_y, LSR_abs_y_2, LSR_abs_y_3, LSR_abs_y_4, LSR_abs_y_5, LSR_abs_y_6,
    LSR_ind_x, LSR_ind_x_2, LSR_ind_x_3, LSR_ind_x_4, LSR_ind_x_5, LSR_ind_x_6, LSR_ind_x_7,
    LSR_ind_y, LSR_ind_y_2, LSR_ind_y_3, LSR_ind_y_4, LSR_ind_y_5, LSR_ind_y_6, LSR_ind_y_7,
    
    NOP,
    NOP_imm,
    NOP_zpg,   NOP_zpg_2,
    NOP_zpg_x, NOP_zpg_x_2, NOP_zpg_x_3,
    NOP_abs,   NOP_abs_2,   NOP_abs_3,
    NOP_abs_x, NOP_abs_x_2, NOP_abs_x_3, NOP_abs_x_4,
    
    ORA_imm,
    ORA_zpg,   ORA_zpg_2,
    ORA_zpg_x, ORA_zpg_x_2, ORA_zpg_x_3,
    ORA_abs,   ORA_abs_2,   ORA_abs_3,
    ORA_abs_x, ORA_abs_x_2, ORA_abs_x_3, ORA_abs_x_4,
    ORA_abs_y, ORA_abs_y_2, ORA_abs_y_3, ORA_abs_y_4,
    ORA_ind_x, ORA_ind_x_2, ORA_ind_x_3, ORA_ind_x_4, ORA_ind_x_5,
    ORA_ind_y, ORA_ind_y_2, ORA_ind_y_3, ORA_ind_y_4, ORA_ind_y_5,
    
    PHA, PHA_2,
    PHP, PHP_2,
    PLA, PLA_2, PLA_3,
    PLP, PLP_2, PLP_3,
    
    ROL_acc,
    ROL_zpg,   ROL_zpg_2,   ROL_zpg_3,   ROL_zpg_4,
    ROL_zpg_x, ROL_zpg_x_2, ROL_zpg_x_3, ROL_zpg_x_4, ROL_zpg_x_5,
    ROL_abs,   ROL_abs_2,   ROL_abs_3,   ROL_abs_4,   ROL_abs_5,
    ROL_abs_x, ROL_abs_x_2, ROL_abs_x_3, ROL_abs_x_4, ROL_abs_x_5, ROL_abs_x_6,
    ROL_ind_x, ROL_ind_x_2, ROL_ind_x_3, ROL_ind_x_4, ROL_ind_x_5, ROL_ind_x_6, ROL_ind_x_7,
    
    ROR_acc,
    ROR_zpg,   ROR_zpg_2,   ROR_zpg_3,   ROR_zpg_4,
    ROR_zpg_x, ROR_zpg_x_2, ROR_zpg_x_3, ROR_zpg_x_4, ROR_zpg_x_5,
    ROR_abs,   ROR_abs_2,   ROR_abs_3,   ROR_abs_4,   ROR_abs_5,
    ROR_abs_x, ROR_abs_x_2, ROR_abs_x_3, ROR_abs_x_4, ROR_abs_x_5, ROR_abs_x_6,
    ROR_ind_x, ROR_ind_x_2, ROR_ind_x_3, ROR_ind_x_4, ROR_ind_x_5, ROR_ind_x_6, ROR_ind_x_7,
    
    RTI, RTI_2, RTI_3, RTI_4, RTI_5,
    RTS, RTS_2, RTS_3, RTS_4, RTS_5,
    
    SBC_imm,
    SBC_zpg,   SBC_zpg_2,
    SBC_zpg_x, SBC_zpg_x_2, SBC_zpg_x_3,
    SBC_abs,   SBC_abs_2,   SBC_abs_3,
    SBC_abs_x, SBC_abs_x_2, SBC_abs_x_3, SBC_abs_x_4,
    SBC_abs_y, SBC_abs_y_2, SBC_abs_y_3, SBC_abs_y_4,
    SBC_ind_x, SBC_ind_x_2, SBC_ind_x_3, SBC_ind_x_4, SBC_ind_x_5,
    SBC_ind_y, SBC_ind_y_2, SBC_ind_y_3, SBC_ind_y_4, SBC_ind_y_5,
    
    SEC,
    SED,
    SEI,
    
    STA_zpg,   STA_zpg_2,
    STA_zpg_x, STA_zpg_x_2, STA_zpg_x_3,
    STA_abs,   STA_abs_2,   STA_abs_3,
    STA_abs_x, STA_abs_x_2, STA_abs_x_3, STA_abs_x_4,
    STA_abs_y, STA_abs_y_2, STA_abs_y_3, STA_abs_y_4,
    STA_ind_x, STA_ind_x_2, STA_ind_x_3, STA_ind_x_4, STA_ind_x_5,
    STA_ind_y, STA_ind_y_2, STA_ind_y_3, STA_ind_y_4, STA_ind_y_5,
    
    STX_zpg,   STX_zpg_2,
    STX_zpg_y, STX_zpg_y_2, STX_zpg_y_3,
    STX_abs,   STX_abs_2,   STX_abs_3,
    
    STY_zpg,   STY_zpg_2,
    STY_zpg_x, STY_zpg_x_2, STY_zpg_x_3,
    STY_abs,   STY_abs_2,   STY_abs_3,
    
    TAX,
    TAY,
    TSX,
    TXA,
    TXS,
    TYA,
    
    // Illegal instructions
    
    ALR_imm,
    ANC_imm,
    ANE_imm,
    ARR_imm,
    AXS_imm,
    
    DCP_zpg,   DCP_zpg_2,   DCP_zpg_3,   DCP_zpg_4,
    DCP_zpg_x, DCP_zpg_x_2, DCP_zpg_x_3, DCP_zpg_x_4, DCP_zpg_x_5,
    DCP_abs,   DCP_abs_2,   DCP_abs_3,   DCP_abs_4,   DCP_abs_5,
    DCP_abs_x, DCP_abs_x_2, DCP_abs_x_3, DCP_abs_x_4, DCP_abs_x_5, DCP_abs_x_6,
    DCP_abs_y, DCP_abs_y_2, DCP_abs_y_3, DCP_abs_y_4, DCP_abs_y_5, DCP_abs_y_6,
    DCP_ind_x, DCP_ind_x_2, DCP_ind_x_3, DCP_ind_x_4, DCP_ind_x_5, DCP_ind_x_6, DCP_ind_x_7,
    DCP_ind_y, DCP_ind_y_2, DCP_ind_y_3, DCP_ind_y_4, DCP_ind_y_5, DCP_ind_y_6, DCP_ind_y_7,
    
    ISC_zpg,   ISC_zpg_2,   ISC_zpg_3,   ISC_zpg_4,
    ISC_zpg_x, ISC_zpg_x_2, ISC_zpg_x_3, ISC_zpg_x_4, ISC_zpg_x_5,
    ISC_abs,   ISC_abs_2,   ISC_abs_3,   ISC_abs_4,   ISC_abs_5,
    ISC_abs_x, ISC_abs_x_2, ISC_abs_x_3, ISC_abs_x_4, ISC_abs_x_5, ISC_abs_x_6,
    ISC_abs_y, ISC_abs_y_2, ISC_abs_y_3, ISC_abs_y_4, ISC_abs_y_5, ISC_abs_y_6,
    ISC_ind_x, ISC_ind_x_2, ISC_ind_x_3, ISC_ind_x_4, ISC_ind_x_5, ISC_ind_x_6, ISC_ind_x_7,
    ISC_ind_y, ISC_ind_y_2, ISC_ind_y_3, ISC_ind_y_4, ISC_ind_y_5, ISC_ind_y_6, ISC_ind_y_7,
    
    LAS_abs_y, LAS_abs_y_2, LAS_abs_y_3, LAS_abs_y_4,
    
    LAX_zpg,   LAX_zpg_2,
    LAX_zpg_y, LAX_zpg_y_2, LAX_zpg_y_3,
    LAX_abs,   LAX_abs_2,   LAX_abs_3,
    LAX_abs_y, LAX_abs_y_2, LAX_abs_y_3, LAX_abs_y_4,
    LAX_ind_x, LAX_ind_x_2, LAX_ind_x_3, LAX_ind_x_4, LAX_ind_x_5,
    LAX_ind_y, LAX_ind_y_2, LAX_ind_y_3, LAX_ind_y_4, LAX_ind_y_5,
    
    LXA_imm,
    
    RLA_zpg,   RLA_zpg_2,   RLA_zpg_3,   RLA_zpg_4,
    RLA_zpg_x, RLA_zpg_x_2, RLA_zpg_x_3, RLA_zpg_x_4, RLA_zpg_x_5,
    RLA_abs,   RLA_abs_2,   RLA_abs_3,   RLA_abs_4,   RLA_abs_5,
    RLA_abs_x, RLA_abs_x_2, RLA_abs_x_3, RLA_abs_x_4, RLA_abs_x_5, RLA_abs_x_6,
    RLA_abs_y, RLA_abs_y_2, RLA_abs_y_3, RLA_abs_y_4, RLA_abs_y_5, RLA_abs_y_6,
    RLA_ind_x, RLA_ind_x_2, RLA_ind_x_3, RLA_ind_x_4, RLA_ind_x_5, RLA_ind_x_6, RLA_ind_x_7,
    RLA_ind_y, RLA_ind_y_2, RLA_ind_y_3, RLA_ind_y_4, RLA_ind_y_5, RLA_ind_y_6, RLA_ind_y_7,
    
    RRA_zpg,   RRA_zpg_2,   RRA_zpg_3,   RRA_zpg_4,
    RRA_zpg_x, RRA_zpg_x_2, RRA_zpg_x_3, RRA_zpg_x_4, RRA_zpg_x_5,
    RRA_abs,   RRA_abs_2,   RRA_abs_3,   RRA_abs_4,   RRA_abs_5,
    RRA_abs_x, RRA_abs_x_2, RRA_abs_x_3, RRA_abs_x_4, RRA_abs_x_5, RRA_abs_x_6,
    RRA_abs_y, RRA_abs_y_2, RRA_abs_y_3, RRA_abs_y_4, RRA_abs_y_5, RRA_abs_y_6,
    RRA_ind_x, RRA_ind_x_2, RRA_ind_x_3, RRA_ind_x_4, RRA_ind_x_5, RRA_ind_x_6, RRA_ind_x_7,
    RRA_ind_y, RRA_ind_y_2, RRA_ind_y_3, RRA_ind_y_4, RRA_ind_y_5, RRA_ind_y_6, RRA_ind_y_7,
    
    SAX_zpg,   SAX_zpg_2,
    SAX_zpg_y, SAX_zpg_y_2, SAX_zpg_y_3,
    SAX_abs,   SAX_abs_2,   SAX_abs_3,
    SAX_ind_x, SAX_ind_x_2, SAX_ind_x_3, SAX_ind_x_4, SAX_ind_x_5,
    
    SHA_ind_y, SHA_ind_y_2, SHA_ind_y_3, SHA_ind_y_4, SHA_ind_y_5,
    SHA_abs_y, SHA_abs_y_2, SHA_abs_y_3, SHA_abs_y_4,
    
    SHX_abs_y, SHX_abs_y_2, SHX_abs_y_3, SHX_abs_y_4,
    SHY_abs_x, SHY_abs_x_2, SHY_abs_x_3, SHY_abs_x_4,
    
    SLO_zpg,   SLO_zpg_2,   SLO_zpg_3,   SLO_zpg_4,
    SLO_zpg_x, SLO_zpg_x_2, SLO_zpg_x_3, SLO_zpg_x_4, SLO_zpg_x_5,
    SLO_abs,   SLO_abs_2,   SLO_abs_3,   SLO_abs_4,   SLO_abs_5,
    SLO_abs_x, SLO_abs_x_2, SLO_abs_x_3, SLO_abs_x_4, SLO_abs_x_5, SLO_abs_x_6,
    SLO_abs_y, SLO_abs_y_2, SLO_abs_y_3, SLO_abs_y_4, SLO_abs_y_5, SLO_abs_y_6,
    SLO_ind_x, SLO_ind_x_2, SLO_ind_x_3, SLO_ind_x_4, SLO_ind_x_5, SLO_ind_x_6, SLO_ind_x_7,
    SLO_ind_y, SLO_ind_y_2, SLO_ind_y_3, SLO_ind_y_4, SLO_ind_y_5, SLO_ind_y_6, SLO_ind_y_7,
    
    SRE_zpg,   SRE_zpg_2,   SRE_zpg_3,   SRE_zpg_4,
    SRE_zpg_x, SRE_zpg_x_2, SRE_zpg_x_3, SRE_zpg_x_4, SRE_zpg_x_5,
    SRE_abs,   SRE_abs_2,   SRE_abs_3,   SRE_abs_4,   SRE_abs_5,
    SRE_abs_x, SRE_abs_x_2, SRE_abs_x_3, SRE_abs_x_4, SRE_abs_x_5, SRE_abs_x_6,
    SRE_abs_y, SRE_abs_y_2, SRE_abs_y_3, SRE_abs_y_4, SRE_abs_y_5, SRE_abs_y_6,
    SRE_ind_x, SRE_ind_x_2, SRE_ind_x_3, SRE_ind_x_4, SRE_ind_x_5, SRE_ind_x_6, SRE_ind_x_7,
    SRE_ind_y, SRE_ind_y_2, SRE_ind_y_3, SRE_ind_y_4, SRE_ind_y_5, SRE_ind_y_6, SRE_ind_y_7,
    
    TAS_abs_y, TAS_abs_y_2, TAS_abs_y_3, TAS_abs_y_4
    
} MicroInstruction;

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

#define POLL_IRQ doIrq = (read8_delayed(levelDetector) && I == 0);
#define POLL_IRQ_AND_NMI doIrq = (read8_delayed(levelDetector) && I == 0); doNmi = read8_delayed(edgeDetector);
#define POLL_IRQ_AND_NMI_AGAIN doIrq |= read8_delayed(levelDetector) && I == 0; doNmi |= read8_delayed(edgeDetector);

#define CONTINUE next = (MicroInstruction)((int)next+1); return;
#define DONE next = fetch; doIrq = (read8_delayed(levelDetector) && I == 0); doNmi = read8_delayed(edgeDetector); return;
#define DONE_NO_POLL next = fetch; return;


//! Next microinstruction to be executed
MicroInstruction next;

//! Callback function array pointing to the execution function of each instruction
MicroInstruction actionFunc[256];

//! Mnemonic strings (used by the source level debugger only)
const char *mnemonic[256];

//! Adressing mode (used by the source level debugger only)
AddressingMode addressingMode[256];

//! Static array containing all callback function
static void (CPU::*callbacks[])(void);

//! Register callback function for a single opcode
void registerCallback(uint8_t opcode, MicroInstruction mInstr);

//! Register callback function for a single opcode
void registerCallback(uint8_t opcode, const char *mnemonic,
                      AddressingMode mode, MicroInstruction mInstr);

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

