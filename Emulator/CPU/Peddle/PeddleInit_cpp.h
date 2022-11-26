// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

void
Peddle::registerCallback(u8 opcode, const char *mnemonic,
                         AddressingMode mode, MicroInstruction mInstr)
{
    // Table is write-once
    assert(mInstr == JAM || actionFunc[opcode] == JAM);

    actionFunc[opcode] = mInstr;
    debugger.registerInstruction(opcode, mnemonic, mode);
}

void
Peddle::registerInstructions()
{
    for (isize i = 0; i < 256; i++) {
        registerCallback((u8)i, "???", ADDR_IMPLIED, JAM);
    }
    registerLegalInstructions();
    registerIllegalInstructions();
}

void
Peddle::registerLegalInstructions()
{
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
    registerCallback(0x6C, "JMP", ADDR_INDIRECT, JMP_abs_ind);

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
}

void
Peddle::registerIllegalInstructions()
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
