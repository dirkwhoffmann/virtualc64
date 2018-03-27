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
    
    ADC_immediate,
    ADC_zero_page, ADC_zero_page_2,
    ADC_zero_page_x, ADC_zero_page_x_2, ADC_zero_page_x_3,
    ADC_absolute, ADC_absolute_2, ADC_absolute_3,
    ADC_absolute_x, ADC_absolute_x_2, ADC_absolute_x_3, ADC_absolute_x_4,
    ADC_absolute_y, ADC_absolute_y_2, ADC_absolute_y_3, ADC_absolute_y_4,
    ADC_indirect_x, ADC_indirect_x_2, ADC_indirect_x_3, ADC_indirect_x_4, ADC_indirect_x_5,
    ADC_indirect_y, ADC_indirect_y_2, ADC_indirect_y_3, ADC_indirect_y_4, ADC_indirect_y_5,
    
    AND_immediate,
    AND_zero_page, AND_zero_page_2,
    AND_zero_page_x, AND_zero_page_x_2, AND_zero_page_x_3,
    AND_absolute, AND_absolute_2, AND_absolute_3,
    AND_absolute_x, AND_absolute_x_2, AND_absolute_x_3, AND_absolute_x_4,
    AND_absolute_y, AND_absolute_y_2, AND_absolute_y_3, AND_absolute_y_4,
    AND_indirect_x, AND_indirect_x_2, AND_indirect_x_3, AND_indirect_x_4, AND_indirect_x_5,
    AND_indirect_y, AND_indirect_y_2, AND_indirect_y_3, AND_indirect_y_4, AND_indirect_y_5,
    
    ASL_accumulator,
    ASL_zero_page, ASL_zero_page_2, ASL_zero_page_3, ASL_zero_page_4,
    ASL_zero_page_x, ASL_zero_page_x_2, ASL_zero_page_x_3, ASL_zero_page_x_4, ASL_zero_page_x_5,
    ASL_absolute, ASL_absolute_2, ASL_absolute_3, ASL_absolute_4, ASL_absolute_5,
    ASL_absolute_x, ASL_absolute_x_2, ASL_absolute_x_3, ASL_absolute_x_4, ASL_absolute_x_5, ASL_absolute_x_6,
    ASL_indirect_x, ASL_indirect_x_2, ASL_indirect_x_3, ASL_indirect_x_4, ASL_indirect_x_5, ASL_indirect_x_6, ASL_indirect_x_7,
    
    branch_3_underflow, branch_3_overflow,
    BCC_relative, BCC_relative_2,
    BCS_relative, BCS_relative_2,
    BEQ_relative, BEQ_relative_2,
    
    BIT_zero_page, BIT_zero_page_2,
    BIT_absolute, BIT_absolute_2, BIT_absolute_3,
    
    BMI_relative, BMI_relative_2,
    BNE_relative, BNE_relative_2,
    BPL_relative, BPL_relative_2,
    
    BRK, BRK_2, BRK_3, BRK_4, BRK_5, BRK_6,
    BRK_nmi_4, BRK_nmi_5, BRK_nmi_6,
    
    BVC_relative, BVC_relative_2,
    BVS_relative, BVS_relative_2,
    CLC,
    CLD,
    CLI,
    CLV,
    
    CMP_immediate,
    CMP_zero_page, CMP_zero_page_2,
    CMP_zero_page_x, CMP_zero_page_x_2, CMP_zero_page_x_3,
    CMP_absolute, CMP_absolute_2, CMP_absolute_3,
    CMP_absolute_x, CMP_absolute_x_2, CMP_absolute_x_3, CMP_absolute_x_4,
    CMP_absolute_y, CMP_absolute_y_2, CMP_absolute_y_3, CMP_absolute_y_4,
    CMP_indirect_x, CMP_indirect_x_2, CMP_indirect_x_3, CMP_indirect_x_4, CMP_indirect_x_5,
    CMP_indirect_y, CMP_indirect_y_2, CMP_indirect_y_3, CMP_indirect_y_4, CMP_indirect_y_5,
    
    CPX_immediate,
    CPX_zero_page, CPX_zero_page_2,
    CPX_absolute, CPX_absolute_2, CPX_absolute_3,
    
    CPY_immediate,
    CPY_zero_page, CPY_zero_page_2,
    CPY_absolute, CPY_absolute_2, CPY_absolute_3,
    
    DEC_zero_page, DEC_zero_page_2, DEC_zero_page_3, DEC_zero_page_4,
    DEC_zero_page_x, DEC_zero_page_x_2, DEC_zero_page_x_3, DEC_zero_page_x_4, DEC_zero_page_x_5,
    DEC_absolute, DEC_absolute_2, DEC_absolute_3, DEC_absolute_4, DEC_absolute_5,
    DEC_absolute_x, DEC_absolute_x_2, DEC_absolute_x_3, DEC_absolute_x_4, DEC_absolute_x_5, DEC_absolute_x_6,
    DEC_indirect_x, DEC_indirect_x_2, DEC_indirect_x_3, DEC_indirect_x_4, DEC_indirect_x_5, DEC_indirect_x_6, DEC_indirect_x_7,
    
    DEX,
    DEY,
    
    EOR_immediate,
    EOR_zero_page, EOR_zero_page_2,
    EOR_zero_page_x, EOR_zero_page_x_2, EOR_zero_page_x_3,
    EOR_absolute, EOR_absolute_2, EOR_absolute_3,
    EOR_absolute_x, EOR_absolute_x_2, EOR_absolute_x_3, EOR_absolute_x_4,
    EOR_absolute_y, EOR_absolute_y_2, EOR_absolute_y_3, EOR_absolute_y_4,
    EOR_indirect_x, EOR_indirect_x_2, EOR_indirect_x_3, EOR_indirect_x_4, EOR_indirect_x_5,
    EOR_indirect_y, EOR_indirect_y_2, EOR_indirect_y_3, EOR_indirect_y_4, EOR_indirect_y_5,
    
    INC_zero_page, INC_zero_page_2, INC_zero_page_3, INC_zero_page_4,
    INC_zero_page_x, INC_zero_page_x_2, INC_zero_page_x_3, INC_zero_page_x_4, INC_zero_page_x_5,
    INC_absolute, INC_absolute_2, INC_absolute_3, INC_absolute_4, INC_absolute_5,
    INC_absolute_x, INC_absolute_x_2, INC_absolute_x_3, INC_absolute_x_4, INC_absolute_x_5, INC_absolute_x_6,
    INC_indirect_x, INC_indirect_x_2, INC_indirect_x_3, INC_indirect_x_4, INC_indirect_x_5, INC_indirect_x_6, INC_indirect_x_7,
    
    INX,
    INY,
    
    JMP_absolute, JMP_absolute_2,
    JMP_absolute_indirect, JMP_absolute_indirect_2, JMP_absolute_indirect_3, JMP_absolute_indirect_4,
    
    JSR, JSR_2, JSR_3, JSR_4, JSR_5,
    
    LDA_immediate,
    LDA_zero_page, LDA_zero_page_2,
    LDA_zero_page_x, LDA_zero_page_x_2, LDA_zero_page_x_3,
    LDA_absolute, LDA_absolute_2, LDA_absolute_3,
    LDA_absolute_x, LDA_absolute_x_2, LDA_absolute_x_3, LDA_absolute_x_4,
    LDA_absolute_y, LDA_absolute_y_2, LDA_absolute_y_3, LDA_absolute_y_4,
    LDA_indirect_x, LDA_indirect_x_2, LDA_indirect_x_3, LDA_indirect_x_4, LDA_indirect_x_5,
    LDA_indirect_y, LDA_indirect_y_2, LDA_indirect_y_3, LDA_indirect_y_4, LDA_indirect_y_5,
    
    LDX_immediate,
    LDX_zero_page, LDX_zero_page_2,
    LDX_zero_page_y, LDX_zero_page_y_2, LDX_zero_page_y_3,
    LDX_absolute, LDX_absolute_2, LDX_absolute_3,
    LDX_absolute_y, LDX_absolute_y_2, LDX_absolute_y_3, LDX_absolute_y_4,
    LDX_indirect_x, LDX_indirect_x_2, LDX_indirect_x_3, LDX_indirect_x_4, LDX_indirect_x_5,
    LDX_indirect_y, LDX_indirect_y_2, LDX_indirect_y_3, LDX_indirect_y_4, LDX_indirect_y_5,
    
    LDY_immediate,
    LDY_zero_page, LDY_zero_page_2,
    LDY_zero_page_x, LDY_zero_page_x_2, LDY_zero_page_x_3,
    LDY_absolute, LDY_absolute_2, LDY_absolute_3,
    LDY_absolute_x, LDY_absolute_x_2, LDY_absolute_x_3, LDY_absolute_x_4,
    LDY_indirect_x, LDY_indirect_x_2, LDY_indirect_x_3, LDY_indirect_x_4, LDY_indirect_x_5,
    LDY_indirect_y, LDY_indirect_y_2, LDY_indirect_y_3, LDY_indirect_y_4, LDY_indirect_y_5,
    
    LSR_accumulator,
    LSR_zero_page, LSR_zero_page_2, LSR_zero_page_3, LSR_zero_page_4,
    LSR_zero_page_x, LSR_zero_page_x_2, LSR_zero_page_x_3, LSR_zero_page_x_4, LSR_zero_page_x_5,
    LSR_absolute, LSR_absolute_2, LSR_absolute_3, LSR_absolute_4, LSR_absolute_5,
    LSR_absolute_x, LSR_absolute_x_2, LSR_absolute_x_3, LSR_absolute_x_4, LSR_absolute_x_5, LSR_absolute_x_6,
    LSR_absolute_y, LSR_absolute_y_2, LSR_absolute_y_3, LSR_absolute_y_4, LSR_absolute_y_5, LSR_absolute_y_6,
    LSR_indirect_x, LSR_indirect_x_2, LSR_indirect_x_3, LSR_indirect_x_4, LSR_indirect_x_5, LSR_indirect_x_6, LSR_indirect_x_7,
    LSR_indirect_y, LSR_indirect_y_2, LSR_indirect_y_3, LSR_indirect_y_4, LSR_indirect_y_5, LSR_indirect_y_6, LSR_indirect_y_7,
    
    NOP,
    NOP_immediate,
    NOP_zero_page, NOP_zero_page_2,
    NOP_zero_page_x, NOP_zero_page_x_2, NOP_zero_page_x_3,
    NOP_absolute, NOP_absolute_2, NOP_absolute_3,
    NOP_absolute_x, NOP_absolute_x_2, NOP_absolute_x_3, NOP_absolute_x_4,
    
    ORA_immediate,
    ORA_zero_page, ORA_zero_page_2,
    ORA_zero_page_x, ORA_zero_page_x_2, ORA_zero_page_x_3,
    ORA_absolute, ORA_absolute_2, ORA_absolute_3,
    ORA_absolute_x, ORA_absolute_x_2, ORA_absolute_x_3, ORA_absolute_x_4,
    ORA_absolute_y, ORA_absolute_y_2, ORA_absolute_y_3, ORA_absolute_y_4,
    ORA_indirect_x, ORA_indirect_x_2, ORA_indirect_x_3, ORA_indirect_x_4, ORA_indirect_x_5,
    ORA_indirect_y, ORA_indirect_y_2, ORA_indirect_y_3, ORA_indirect_y_4, ORA_indirect_y_5,
    
    PHA, PHA_2,
    PHP, PHP_2,
    PLA, PLA_2, PLA_3,
    PLP, PLP_2, PLP_3,
    
    ROL_accumulator,
    ROL_zero_page, ROL_zero_page_2, ROL_zero_page_3, ROL_zero_page_4,
    ROL_zero_page_x, ROL_zero_page_x_2, ROL_zero_page_x_3, ROL_zero_page_x_4, ROL_zero_page_x_5,
    ROL_absolute, ROL_absolute_2, ROL_absolute_3, ROL_absolute_4, ROL_absolute_5,
    ROL_absolute_x, ROL_absolute_x_2, ROL_absolute_x_3, ROL_absolute_x_4, ROL_absolute_x_5, ROL_absolute_x_6,
    ROL_indirect_x, ROL_indirect_x_2, ROL_indirect_x_3, ROL_indirect_x_4, ROL_indirect_x_5, ROL_indirect_x_6, ROL_indirect_x_7,
    
    ROR_accumulator,
    ROR_zero_page, ROR_zero_page_2, ROR_zero_page_3, ROR_zero_page_4,
    ROR_zero_page_x, ROR_zero_page_x_2, ROR_zero_page_x_3, ROR_zero_page_x_4, ROR_zero_page_x_5,
    ROR_absolute, ROR_absolute_2, ROR_absolute_3, ROR_absolute_4, ROR_absolute_5,
    ROR_absolute_x, ROR_absolute_x_2, ROR_absolute_x_3, ROR_absolute_x_4, ROR_absolute_x_5, ROR_absolute_x_6,
    ROR_indirect_x, ROR_indirect_x_2, ROR_indirect_x_3, ROR_indirect_x_4, ROR_indirect_x_5, ROR_indirect_x_6, ROR_indirect_x_7,
    
    RTI, RTI_2, RTI_3, RTI_4, RTI_5,
    RTS, RTS_2, RTS_3, RTS_4, RTS_5,
    
    SBC_immediate,
    SBC_zero_page, SBC_zero_page_2,
    SBC_zero_page_x, SBC_zero_page_x_2, SBC_zero_page_x_3,
    SBC_absolute, SBC_absolute_2, SBC_absolute_3,
    SBC_absolute_x, SBC_absolute_x_2, SBC_absolute_x_3, SBC_absolute_x_4,
    SBC_absolute_y, SBC_absolute_y_2, SBC_absolute_y_3, SBC_absolute_y_4,
    SBC_indirect_x, SBC_indirect_x_2, SBC_indirect_x_3, SBC_indirect_x_4, SBC_indirect_x_5,
    SBC_indirect_y, SBC_indirect_y_2, SBC_indirect_y_3, SBC_indirect_y_4, SBC_indirect_y_5,
    
    SEC,
    SED,
    SEI,
    
    STA_zero_page, STA_zero_page_2,
    STA_zero_page_x, STA_zero_page_x_2, STA_zero_page_x_3,
    STA_absolute, STA_absolute_2, STA_absolute_3,
    STA_absolute_x, STA_absolute_x_2, STA_absolute_x_3, STA_absolute_x_4,
    STA_absolute_y, STA_absolute_y_2, STA_absolute_y_3, STA_absolute_y_4,
    STA_indirect_x, STA_indirect_x_2, STA_indirect_x_3, STA_indirect_x_4, STA_indirect_x_5,
    STA_indirect_y, STA_indirect_y_2, STA_indirect_y_3, STA_indirect_y_4, STA_indirect_y_5,
    
    STX_zero_page, STX_zero_page_2,
    STX_zero_page_y, STX_zero_page_y_2, STX_zero_page_y_3,
    STX_absolute, STX_absolute_2, STX_absolute_3,
    
    STY_zero_page, STY_zero_page_2,
    STY_zero_page_x, STY_zero_page_x_2, STY_zero_page_x_3,
    STY_absolute, STY_absolute_2, STY_absolute_3,
    
    TAX,
    TAY,
    TSX,
    TXA,
    TXS,
    TYA,
    
    // Illegal instructions
    
    ALR_immediate,
    ANC_immediate,
    ANE_immediate,
    ARR_immediate,
    AXS_immediate,
    
    DCP_zero_page, DCP_zero_page_2, DCP_zero_page_3, DCP_zero_page_4,
    DCP_zero_page_x, DCP_zero_page_x_2, DCP_zero_page_x_3, DCP_zero_page_x_4, DCP_zero_page_x_5,
    DCP_absolute, DCP_absolute_2, DCP_absolute_3, DCP_absolute_4, DCP_absolute_5,
    DCP_absolute_x, DCP_absolute_x_2, DCP_absolute_x_3, DCP_absolute_x_4, DCP_absolute_x_5, DCP_absolute_x_6,
    DCP_absolute_y, DCP_absolute_y_2, DCP_absolute_y_3, DCP_absolute_y_4, DCP_absolute_y_5, DCP_absolute_y_6,
    DCP_indirect_x, DCP_indirect_x_2, DCP_indirect_x_3, DCP_indirect_x_4, DCP_indirect_x_5, DCP_indirect_x_6, DCP_indirect_x_7,
    DCP_indirect_y, DCP_indirect_y_2, DCP_indirect_y_3, DCP_indirect_y_4, DCP_indirect_y_5, DCP_indirect_y_6, DCP_indirect_y_7,
    
    ISC_zero_page, ISC_zero_page_2, ISC_zero_page_3, ISC_zero_page_4,
    ISC_zero_page_x, ISC_zero_page_x_2, ISC_zero_page_x_3, ISC_zero_page_x_4, ISC_zero_page_x_5,
    ISC_absolute, ISC_absolute_2, ISC_absolute_3, ISC_absolute_4, ISC_absolute_5,
    ISC_absolute_x, ISC_absolute_x_2, ISC_absolute_x_3, ISC_absolute_x_4, ISC_absolute_x_5, ISC_absolute_x_6,
    ISC_absolute_y, ISC_absolute_y_2, ISC_absolute_y_3, ISC_absolute_y_4, ISC_absolute_y_5, ISC_absolute_y_6,
    ISC_indirect_x, ISC_indirect_x_2, ISC_indirect_x_3, ISC_indirect_x_4, ISC_indirect_x_5, ISC_indirect_x_6, ISC_indirect_x_7,
    ISC_indirect_y, ISC_indirect_y_2, ISC_indirect_y_3, ISC_indirect_y_4, ISC_indirect_y_5, ISC_indirect_y_6, ISC_indirect_y_7,
    
    LAS_absolute_y, LAS_absolute_y_2, LAS_absolute_y_3, LAS_absolute_y_4,
    
    LAX_zero_page, LAX_zero_page_2,
    LAX_zero_page_y, LAX_zero_page_y_2, LAX_zero_page_y_3,
    LAX_absolute, LAX_absolute_2, LAX_absolute_3,
    LAX_absolute_y, LAX_absolute_y_2, LAX_absolute_y_3, LAX_absolute_y_4,
    LAX_indirect_x, LAX_indirect_x_2, LAX_indirect_x_3, LAX_indirect_x_4, LAX_indirect_x_5,
    LAX_indirect_y, LAX_indirect_y_2, LAX_indirect_y_3, LAX_indirect_y_4, LAX_indirect_y_5,
    
    LXA_immediate,
    
    RLA_zero_page, RLA_zero_page_2, RLA_zero_page_3, RLA_zero_page_4,
    RLA_zero_page_x, RLA_zero_page_x_2, RLA_zero_page_x_3, RLA_zero_page_x_4, RLA_zero_page_x_5,
    RLA_absolute, RLA_absolute_2, RLA_absolute_3, RLA_absolute_4, RLA_absolute_5,
    RLA_absolute_x, RLA_absolute_x_2, RLA_absolute_x_3, RLA_absolute_x_4, RLA_absolute_x_5, RLA_absolute_x_6,
    RLA_absolute_y, RLA_absolute_y_2, RLA_absolute_y_3, RLA_absolute_y_4, RLA_absolute_y_5, RLA_absolute_y_6,
    RLA_indirect_x, RLA_indirect_x_2, RLA_indirect_x_3, RLA_indirect_x_4, RLA_indirect_x_5, RLA_indirect_x_6, RLA_indirect_x_7,
    RLA_indirect_y, RLA_indirect_y_2, RLA_indirect_y_3, RLA_indirect_y_4, RLA_indirect_y_5, RLA_indirect_y_6, RLA_indirect_y_7,
    
    RRA_zero_page, RRA_zero_page_2, RRA_zero_page_3, RRA_zero_page_4,
    RRA_zero_page_x, RRA_zero_page_x_2, RRA_zero_page_x_3, RRA_zero_page_x_4, RRA_zero_page_x_5,
    RRA_absolute, RRA_absolute_2, RRA_absolute_3, RRA_absolute_4, RRA_absolute_5,
    RRA_absolute_x, RRA_absolute_x_2, RRA_absolute_x_3, RRA_absolute_x_4, RRA_absolute_x_5, RRA_absolute_x_6,
    RRA_absolute_y, RRA_absolute_y_2, RRA_absolute_y_3, RRA_absolute_y_4, RRA_absolute_y_5, RRA_absolute_y_6,
    RRA_indirect_x, RRA_indirect_x_2, RRA_indirect_x_3, RRA_indirect_x_4, RRA_indirect_x_5, RRA_indirect_x_6, RRA_indirect_x_7,
    RRA_indirect_y, RRA_indirect_y_2, RRA_indirect_y_3, RRA_indirect_y_4, RRA_indirect_y_5, RRA_indirect_y_6, RRA_indirect_y_7,
    
    SAX_zero_page, SAX_zero_page_2,
    SAX_zero_page_y, SAX_zero_page_y_2, SAX_zero_page_y_3,
    SAX_absolute, SAX_absolute_2, SAX_absolute_3,
    SAX_indirect_x, SAX_indirect_x_2, SAX_indirect_x_3, SAX_indirect_x_4, SAX_indirect_x_5,
    
    SHA_indirect_y, SHA_indirect_y_2, SHA_indirect_y_3, SHA_indirect_y_4, SHA_indirect_y_5,
    SHA_absolute_y, SHA_absolute_y_2, SHA_absolute_y_3, SHA_absolute_y_4,
    
    SHX_absolute_y, SHX_absolute_y_2, SHX_absolute_y_3, SHX_absolute_y_4,
    SHY_absolute_x, SHY_absolute_x_2, SHY_absolute_x_3, SHY_absolute_x_4,
    
    SLO_zero_page, SLO_zero_page_2, SLO_zero_page_3, SLO_zero_page_4,
    SLO_zero_page_x, SLO_zero_page_x_2, SLO_zero_page_x_3, SLO_zero_page_x_4, SLO_zero_page_x_5,
    SLO_absolute, SLO_absolute_2, SLO_absolute_3, SLO_absolute_4, SLO_absolute_5,
    SLO_absolute_x, SLO_absolute_x_2, SLO_absolute_x_3, SLO_absolute_x_4, SLO_absolute_x_5, SLO_absolute_x_6,
    SLO_absolute_y, SLO_absolute_y_2, SLO_absolute_y_3, SLO_absolute_y_4, SLO_absolute_y_5, SLO_absolute_y_6,
    SLO_indirect_x, SLO_indirect_x_2, SLO_indirect_x_3, SLO_indirect_x_4, SLO_indirect_x_5, SLO_indirect_x_6, SLO_indirect_x_7,
    SLO_indirect_y, SLO_indirect_y_2, SLO_indirect_y_3, SLO_indirect_y_4, SLO_indirect_y_5, SLO_indirect_y_6, SLO_indirect_y_7,
    
    SRE_zero_page, SRE_zero_page_2, SRE_zero_page_3, SRE_zero_page_4,
    SRE_zero_page_x, SRE_zero_page_x_2, SRE_zero_page_x_3, SRE_zero_page_x_4, SRE_zero_page_x_5,
    SRE_absolute, SRE_absolute_2, SRE_absolute_3, SRE_absolute_4, SRE_absolute_5,
    SRE_absolute_x, SRE_absolute_x_2, SRE_absolute_x_3, SRE_absolute_x_4, SRE_absolute_x_5, SRE_absolute_x_6,
    SRE_absolute_y, SRE_absolute_y_2, SRE_absolute_y_3, SRE_absolute_y_4, SRE_absolute_y_5, SRE_absolute_y_6,
    SRE_indirect_x, SRE_indirect_x_2, SRE_indirect_x_3, SRE_indirect_x_4, SRE_indirect_x_5, SRE_indirect_x_6, SRE_indirect_x_7,
    SRE_indirect_y, SRE_indirect_y_2, SRE_indirect_y_3, SRE_indirect_y_4, SRE_indirect_y_5, SRE_indirect_y_6, SRE_indirect_y_7,
    
    TAS_absolute_y, TAS_absolute_y_2, TAS_absolute_y_3, TAS_absolute_y_4
    
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

#define POLL_IRQ doIrq = ((oldIrqLine != 0) && (I == 0));
#define POLL_NMI doNmi = oldNmiEdge;
#define POLL_IRQ_AND_NMI doIrq = ((oldIrqLine != 0) && (I == 0)); doNmi = oldNmiEdge;
#define POLL_IRQ_AND_NMI_AGAIN doIrq |= (oldIrqLine != 0) && (I == 0); doNmi |= oldNmiEdge;

#define CONTINUE next = (MicroInstruction)((int)next+1); return;
#define DONE next = fetch; doIrq = ((oldIrqLine != 0) && (I == 0)); doNmi = oldNmiEdge; return;
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

