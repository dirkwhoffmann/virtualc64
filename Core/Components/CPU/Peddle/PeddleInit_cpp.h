// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

void
Peddle::registerCallback(u8 opcode, const char *mnemonic, AddrMode mode, MicroInstruction mInstr)
{
    // Table is write-once
    assert(mInstr == JAM || actionFunc[opcode] == JAM);

    this->actionFunc[opcode] = mInstr;
    this->mnemonic[opcode] = mnemonic;
    this->addressingMode[opcode] = mode;
}

void
Peddle::registerInstructions()
{
    if (mnemonic[0] == nullptr) {

        for (isize i = 0; i < 256; i++) {
            registerCallback((u8)i, "???", AddrMode::ADDR_IMPLIED, JAM);
        }
        registerLegalInstructions();
        registerIllegalInstructions();
    }
}

void
Peddle::registerLegalInstructions()
{
    registerCallback(0x69, "ADC", AddrMode::ADDR_IMMEDIATE, ADC_imm);
    registerCallback(0x65, "ADC", AddrMode::ADDR_ZERO_PAGE, ADC_zpg);
    registerCallback(0x75, "ADC", AddrMode::ADDR_ZERO_PAGE_X, ADC_zpg_x);
    registerCallback(0x6D, "ADC", AddrMode::ADDR_ABSOLUTE, ADC_abs);
    registerCallback(0x7D, "ADC", AddrMode::ADDR_ABSOLUTE_X, ADC_abs_x);
    registerCallback(0x79, "ADC", AddrMode::ADDR_ABSOLUTE_Y, ADC_abs_y);
    registerCallback(0x61, "ADC", AddrMode::ADDR_INDIRECT_X, ADC_ind_x);
    registerCallback(0x71, "ADC", AddrMode::ADDR_INDIRECT_Y, ADC_ind_y);

    registerCallback(0x29, "AND", AddrMode::ADDR_IMMEDIATE, AND_imm);
    registerCallback(0x25, "AND", AddrMode::ADDR_ZERO_PAGE, AND_zpg);
    registerCallback(0x35, "AND", AddrMode::ADDR_ZERO_PAGE_X, AND_zpg_x);
    registerCallback(0x2D, "AND", AddrMode::ADDR_ABSOLUTE, AND_abs);
    registerCallback(0x3D, "AND", AddrMode::ADDR_ABSOLUTE_X, AND_abs_x);
    registerCallback(0x39, "AND", AddrMode::ADDR_ABSOLUTE_Y, AND_abs_y);
    registerCallback(0x21, "AND", AddrMode::ADDR_INDIRECT_X, AND_ind_x);
    registerCallback(0x31, "AND", AddrMode::ADDR_INDIRECT_Y, AND_ind_y);

    registerCallback(0x0A, "ASL", AddrMode::ADDR_ACCUMULATOR, ASL_acc);
    registerCallback(0x06, "ASL", AddrMode::ADDR_ZERO_PAGE, ASL_zpg);
    registerCallback(0x16, "ASL", AddrMode::ADDR_ZERO_PAGE_X, ASL_zpg_x);
    registerCallback(0x0E, "ASL", AddrMode::ADDR_ABSOLUTE, ASL_abs);
    registerCallback(0x1E, "ASL", AddrMode::ADDR_ABSOLUTE_X, ASL_abs_x);

    registerCallback(0x90, "BCC", AddrMode::ADDR_RELATIVE, BCC_rel);
    registerCallback(0xB0, "BCS", AddrMode::ADDR_RELATIVE, BCS_rel);
    registerCallback(0xF0, "BEQ", AddrMode::ADDR_RELATIVE, BEQ_rel);

    registerCallback(0x24, "BIT", AddrMode::ADDR_ZERO_PAGE, BIT_zpg);
    registerCallback(0x2C, "BIT", AddrMode::ADDR_ABSOLUTE, BIT_abs);

    registerCallback(0x30, "BMI", AddrMode::ADDR_RELATIVE, BMI_rel);
    registerCallback(0xD0, "BNE", AddrMode::ADDR_RELATIVE, BNE_rel);
    registerCallback(0x10, "BPL", AddrMode::ADDR_RELATIVE, BPL_rel);
    registerCallback(0x00, "BRK", AddrMode::ADDR_IMPLIED, BRK);
    registerCallback(0x50, "BVC", AddrMode::ADDR_RELATIVE, BVC_rel);
    registerCallback(0x70, "BVS", AddrMode::ADDR_RELATIVE, BVS_rel);

    registerCallback(0x18, "CLC", AddrMode::ADDR_IMPLIED, CLC);
    registerCallback(0xD8, "CLD", AddrMode::ADDR_IMPLIED, CLD);
    registerCallback(0x58, "CLI", AddrMode::ADDR_IMPLIED, CLI);
    registerCallback(0xB8, "CLV", AddrMode::ADDR_IMPLIED, CLV);

    registerCallback(0xC9, "CMP", AddrMode::ADDR_IMMEDIATE, CMP_imm);
    registerCallback(0xC5, "CMP", AddrMode::ADDR_ZERO_PAGE, CMP_zpg);
    registerCallback(0xD5, "CMP", AddrMode::ADDR_ZERO_PAGE_X, CMP_zpg_x);
    registerCallback(0xCD, "CMP", AddrMode::ADDR_ABSOLUTE, CMP_abs);
    registerCallback(0xDD, "CMP", AddrMode::ADDR_ABSOLUTE_X, CMP_abs_x);
    registerCallback(0xD9, "CMP", AddrMode::ADDR_ABSOLUTE_Y, CMP_abs_y);
    registerCallback(0xC1, "CMP", AddrMode::ADDR_INDIRECT_X, CMP_ind_x);
    registerCallback(0xD1, "CMP", AddrMode::ADDR_INDIRECT_Y, CMP_ind_y);

    registerCallback(0xE0, "CPX", AddrMode::ADDR_IMMEDIATE, CPX_imm);
    registerCallback(0xE4, "CPX", AddrMode::ADDR_ZERO_PAGE, CPX_zpg);
    registerCallback(0xEC, "CPX", AddrMode::ADDR_ABSOLUTE, CPX_abs);

    registerCallback(0xC0, "CPY", AddrMode::ADDR_IMMEDIATE, CPY_imm);
    registerCallback(0xC4, "CPY", AddrMode::ADDR_ZERO_PAGE, CPY_zpg);
    registerCallback(0xCC, "CPY", AddrMode::ADDR_ABSOLUTE, CPY_abs);

    registerCallback(0xC6, "DEC", AddrMode::ADDR_ZERO_PAGE, DEC_zpg);
    registerCallback(0xD6, "DEC", AddrMode::ADDR_ZERO_PAGE_X, DEC_zpg_x);
    registerCallback(0xCE, "DEC", AddrMode::ADDR_ABSOLUTE, DEC_abs);
    registerCallback(0xDE, "DEC", AddrMode::ADDR_ABSOLUTE_X, DEC_abs_x);

    registerCallback(0xCA, "DEX", AddrMode::ADDR_IMPLIED, DEX);
    registerCallback(0x88, "DEY", AddrMode::ADDR_IMPLIED, DEY);

    registerCallback(0x49, "EOR", AddrMode::ADDR_IMMEDIATE, EOR_imm);
    registerCallback(0x45, "EOR", AddrMode::ADDR_ZERO_PAGE, EOR_zpg);
    registerCallback(0x55, "EOR", AddrMode::ADDR_ZERO_PAGE_X, EOR_zpg_x);
    registerCallback(0x4D, "EOR", AddrMode::ADDR_ABSOLUTE, EOR_abs);
    registerCallback(0x5D, "EOR", AddrMode::ADDR_ABSOLUTE_X, EOR_abs_x);
    registerCallback(0x59, "EOR", AddrMode::ADDR_ABSOLUTE_Y, EOR_abs_y);
    registerCallback(0x41, "EOR", AddrMode::ADDR_INDIRECT_X, EOR_ind_x);
    registerCallback(0x51, "EOR", AddrMode::ADDR_INDIRECT_Y, EOR_ind_y);

    registerCallback(0xE6, "INC", AddrMode::ADDR_ZERO_PAGE, INC_zpg);
    registerCallback(0xF6, "INC", AddrMode::ADDR_ZERO_PAGE_X, INC_zpg_x);
    registerCallback(0xEE, "INC", AddrMode::ADDR_ABSOLUTE, INC_abs);
    registerCallback(0xFE, "INC", AddrMode::ADDR_ABSOLUTE_X, INC_abs_x);

    registerCallback(0xE8, "INX", AddrMode::ADDR_IMPLIED, INX);
    registerCallback(0xC8, "INY", AddrMode::ADDR_IMPLIED, INY);

    registerCallback(0x4C, "JMP", AddrMode::ADDR_DIRECT, JMP_abs);
    registerCallback(0x6C, "JMP", AddrMode::ADDR_INDIRECT, JMP_abs_ind);

    registerCallback(0x20, "JSR", AddrMode::ADDR_DIRECT, JSR);

    registerCallback(0xA9, "LDA", AddrMode::ADDR_IMMEDIATE, LDA_imm);
    registerCallback(0xA5, "LDA", AddrMode::ADDR_ZERO_PAGE, LDA_zpg);
    registerCallback(0xB5, "LDA", AddrMode::ADDR_ZERO_PAGE_X, LDA_zpg_x);
    registerCallback(0xAD, "LDA", AddrMode::ADDR_ABSOLUTE, LDA_abs);
    registerCallback(0xBD, "LDA", AddrMode::ADDR_ABSOLUTE_X, LDA_abs_x);
    registerCallback(0xB9, "LDA", AddrMode::ADDR_ABSOLUTE_Y, LDA_abs_y);
    registerCallback(0xA1, "LDA", AddrMode::ADDR_INDIRECT_X, LDA_ind_x);
    registerCallback(0xB1, "LDA", AddrMode::ADDR_INDIRECT_Y, LDA_ind_y);

    registerCallback(0xA2, "LDX", AddrMode::ADDR_IMMEDIATE, LDX_imm);
    registerCallback(0xA6, "LDX", AddrMode::ADDR_ZERO_PAGE, LDX_zpg);
    registerCallback(0xB6, "LDX", AddrMode::ADDR_ZERO_PAGE_Y,LDX_zpg_y);
    registerCallback(0xAE, "LDX", AddrMode::ADDR_ABSOLUTE, LDX_abs);
    registerCallback(0xBE, "LDX", AddrMode::ADDR_ABSOLUTE_Y, LDX_abs_y);

    registerCallback(0xA0, "LDY", AddrMode::ADDR_IMMEDIATE, LDY_imm);
    registerCallback(0xA4, "LDY", AddrMode::ADDR_ZERO_PAGE, LDY_zpg);
    registerCallback(0xB4, "LDY", AddrMode::ADDR_ZERO_PAGE_X, LDY_zpg_x);
    registerCallback(0xAC, "LDY", AddrMode::ADDR_ABSOLUTE, LDY_abs);
    registerCallback(0xBC, "LDY", AddrMode::ADDR_ABSOLUTE_X, LDY_abs_x);

    registerCallback(0x4A, "LSR", AddrMode::ADDR_ACCUMULATOR, LSR_acc);
    registerCallback(0x46, "LSR", AddrMode::ADDR_ZERO_PAGE, LSR_zpg);
    registerCallback(0x56, "LSR", AddrMode::ADDR_ZERO_PAGE_X, LSR_zpg_x);
    registerCallback(0x4E, "LSR", AddrMode::ADDR_ABSOLUTE, LSR_abs);
    registerCallback(0x5E, "LSR", AddrMode::ADDR_ABSOLUTE_X, LSR_abs_x);

    registerCallback(0xEA, "NOP", AddrMode::ADDR_IMPLIED, NOP);

    registerCallback(0x09, "ORA", AddrMode::ADDR_IMMEDIATE, ORA_imm);
    registerCallback(0x05, "ORA", AddrMode::ADDR_ZERO_PAGE, ORA_zpg);
    registerCallback(0x15, "ORA", AddrMode::ADDR_ZERO_PAGE_X, ORA_zpg_x);
    registerCallback(0x0D, "ORA", AddrMode::ADDR_ABSOLUTE, ORA_abs);
    registerCallback(0x1D, "ORA", AddrMode::ADDR_ABSOLUTE_X, ORA_abs_x);
    registerCallback(0x19, "ORA", AddrMode::ADDR_ABSOLUTE_Y, ORA_abs_y);
    registerCallback(0x01, "ORA", AddrMode::ADDR_INDIRECT_X, ORA_ind_x);
    registerCallback(0x11, "ORA", AddrMode::ADDR_INDIRECT_Y, ORA_ind_y);

    registerCallback(0x48, "PHA", AddrMode::ADDR_IMPLIED, PHA);
    registerCallback(0x08, "PHP", AddrMode::ADDR_IMPLIED, PHP);
    registerCallback(0x68, "PLA", AddrMode::ADDR_IMPLIED, PLA);
    registerCallback(0x28, "PLP", AddrMode::ADDR_IMPLIED, PLP);

    registerCallback(0x2A, "ROL", AddrMode::ADDR_ACCUMULATOR, ROL_acc);
    registerCallback(0x26, "ROL", AddrMode::ADDR_ZERO_PAGE, ROL_zpg);
    registerCallback(0x36, "ROL", AddrMode::ADDR_ZERO_PAGE_X, ROL_zpg_x);
    registerCallback(0x2E, "ROL", AddrMode::ADDR_ABSOLUTE, ROL_abs);
    registerCallback(0x3E, "ROL", AddrMode::ADDR_ABSOLUTE_X, ROL_abs_x);

    registerCallback(0x6A, "ROR", AddrMode::ADDR_ACCUMULATOR, ROR_acc);
    registerCallback(0x66, "ROR", AddrMode::ADDR_ZERO_PAGE, ROR_zpg);
    registerCallback(0x76, "ROR", AddrMode::ADDR_ZERO_PAGE_X, ROR_zpg_x);
    registerCallback(0x6E, "ROR", AddrMode::ADDR_ABSOLUTE, ROR_abs);
    registerCallback(0x7E, "ROR", AddrMode::ADDR_ABSOLUTE_X, ROR_abs_x);

    registerCallback(0x40, "RTI", AddrMode::ADDR_IMPLIED, RTI);
    registerCallback(0x60, "RTS", AddrMode::ADDR_IMPLIED, RTS);

    registerCallback(0xE9, "SBC", AddrMode::ADDR_IMMEDIATE, SBC_imm);
    registerCallback(0xE5, "SBC", AddrMode::ADDR_ZERO_PAGE, SBC_zpg);
    registerCallback(0xF5, "SBC", AddrMode::ADDR_ZERO_PAGE_X, SBC_zpg_x);
    registerCallback(0xED, "SBC", AddrMode::ADDR_ABSOLUTE, SBC_abs);
    registerCallback(0xFD, "SBC", AddrMode::ADDR_ABSOLUTE_X, SBC_abs_x);
    registerCallback(0xF9, "SBC", AddrMode::ADDR_ABSOLUTE_Y, SBC_abs_y);
    registerCallback(0xE1, "SBC", AddrMode::ADDR_INDIRECT_X, SBC_ind_x);
    registerCallback(0xF1, "SBC", AddrMode::ADDR_INDIRECT_Y, SBC_ind_y);

    registerCallback(0x38, "SEC", AddrMode::ADDR_IMPLIED, SEC);
    registerCallback(0xF8, "SED", AddrMode::ADDR_IMPLIED, SED);
    registerCallback(0x78, "SEI", AddrMode::ADDR_IMPLIED, SEI);

    registerCallback(0x85, "STA", AddrMode::ADDR_ZERO_PAGE, STA_zpg);
    registerCallback(0x95, "STA", AddrMode::ADDR_ZERO_PAGE_X, STA_zpg_x);
    registerCallback(0x8D, "STA", AddrMode::ADDR_ABSOLUTE, STA_abs);
    registerCallback(0x9D, "STA", AddrMode::ADDR_ABSOLUTE_X, STA_abs_x);
    registerCallback(0x99, "STA", AddrMode::ADDR_ABSOLUTE_Y, STA_abs_y);
    registerCallback(0x81, "STA", AddrMode::ADDR_INDIRECT_X, STA_ind_x);
    registerCallback(0x91, "STA", AddrMode::ADDR_INDIRECT_Y, STA_ind_y);

    registerCallback(0x86, "STX", AddrMode::ADDR_ZERO_PAGE, STX_zpg);
    registerCallback(0x96, "STX", AddrMode::ADDR_ZERO_PAGE_Y, STX_zpg_y);
    registerCallback(0x8E, "STX", AddrMode::ADDR_ABSOLUTE, STX_abs);

    registerCallback(0x84, "STY", AddrMode::ADDR_ZERO_PAGE, STY_zpg);
    registerCallback(0x94, "STY", AddrMode::ADDR_ZERO_PAGE_X, STY_zpg_x);
    registerCallback(0x8C, "STY", AddrMode::ADDR_ABSOLUTE, STY_abs);

    registerCallback(0xAA, "TAX", AddrMode::ADDR_IMPLIED, TAX);
    registerCallback(0xA8, "TAY", AddrMode::ADDR_IMPLIED, TAY);
    registerCallback(0xBA, "TSX", AddrMode::ADDR_IMPLIED, TSX);
    registerCallback(0x8A, "TXA", AddrMode::ADDR_IMPLIED, TXA);
    registerCallback(0x9A, "TXS", AddrMode::ADDR_IMPLIED, TXS);
    registerCallback(0x98, "TYA", AddrMode::ADDR_IMPLIED, TYA);
}

void
Peddle::registerIllegalInstructions()
{
    registerCallback(0x93, "SHA*", AddrMode::ADDR_INDIRECT_Y, SHA_ind_y);
    registerCallback(0x9F, "SHA*", AddrMode::ADDR_ABSOLUTE_Y, SHA_abs_y);

    registerCallback(0x4B, "ALR*", AddrMode::ADDR_IMMEDIATE, ALR_imm);

    registerCallback(0x0B, "ANC*", AddrMode::ADDR_IMMEDIATE, ANC_imm);
    registerCallback(0x2B, "ANC*", AddrMode::ADDR_IMMEDIATE, ANC_imm);

    registerCallback(0x8B, "ANE*", AddrMode::ADDR_IMMEDIATE, ANE_imm);

    registerCallback(0x6B, "ARR*", AddrMode::ADDR_IMMEDIATE, ARR_imm);
    registerCallback(0xCB, "AXS*", AddrMode::ADDR_IMMEDIATE, AXS_imm);

    registerCallback(0xC7, "DCP*", AddrMode::ADDR_ZERO_PAGE, DCP_zpg);
    registerCallback(0xD7, "DCP*", AddrMode::ADDR_ZERO_PAGE_X, DCP_zpg_x);
    registerCallback(0xC3, "DCP*", AddrMode::ADDR_INDIRECT_X, DCP_ind_x);
    registerCallback(0xD3, "DCP*", AddrMode::ADDR_INDIRECT_Y, DCP_ind_y);
    registerCallback(0xCF, "DCP*", AddrMode::ADDR_ABSOLUTE, DCP_abs);
    registerCallback(0xDF, "DCP*", AddrMode::ADDR_ABSOLUTE_X, DCP_abs_x);
    registerCallback(0xDB, "DCP*", AddrMode::ADDR_ABSOLUTE_Y, DCP_abs_y);

    registerCallback(0xE7, "ISC*", AddrMode::ADDR_ZERO_PAGE, ISC_zpg);
    registerCallback(0xF7, "ISC*", AddrMode::ADDR_ZERO_PAGE_X, ISC_zpg_x);
    registerCallback(0xE3, "ISC*", AddrMode::ADDR_INDIRECT_X, ISC_ind_x);
    registerCallback(0xF3, "ISC*", AddrMode::ADDR_INDIRECT_Y, ISC_ind_y);
    registerCallback(0xEF, "ISC*", AddrMode::ADDR_ABSOLUTE, ISC_abs);
    registerCallback(0xFF, "ISC*", AddrMode::ADDR_ABSOLUTE_X, ISC_abs_x);
    registerCallback(0xFB, "ISC*", AddrMode::ADDR_ABSOLUTE_Y, ISC_abs_y);

    registerCallback(0xBB, "LAS*", AddrMode::ADDR_ABSOLUTE_Y, LAS_abs_y);

    registerCallback(0xA7, "LAX*", AddrMode::ADDR_ZERO_PAGE, LAX_zpg);
    registerCallback(0xB7, "LAX*", AddrMode::ADDR_ZERO_PAGE_Y, LAX_zpg_y);
    registerCallback(0xA3, "LAX*", AddrMode::ADDR_INDIRECT_X, LAX_ind_x);
    registerCallback(0xB3, "LAX*", AddrMode::ADDR_INDIRECT_Y, LAX_ind_y);
    registerCallback(0xAF, "LAX*", AddrMode::ADDR_ABSOLUTE, LAX_abs);
    registerCallback(0xBF, "LAX*", AddrMode::ADDR_ABSOLUTE_Y, LAX_abs_y);

    registerCallback(0xAB, "LXA*", AddrMode::ADDR_IMMEDIATE, LXA_imm);

    registerCallback(0x80, "NOP*", AddrMode::ADDR_IMMEDIATE, NOP_imm);
    registerCallback(0x82, "NOP*", AddrMode::ADDR_IMMEDIATE, NOP_imm);
    registerCallback(0x89, "NOP*", AddrMode::ADDR_IMMEDIATE, NOP_imm);
    registerCallback(0xC2, "NOP*", AddrMode::ADDR_IMMEDIATE, NOP_imm);
    registerCallback(0xE2, "NOP*", AddrMode::ADDR_IMMEDIATE, NOP_imm);
    registerCallback(0x1A, "NOP*", AddrMode::ADDR_IMPLIED, NOP);
    registerCallback(0x3A, "NOP*", AddrMode::ADDR_IMPLIED, NOP);
    registerCallback(0x5A, "NOP*", AddrMode::ADDR_IMPLIED, NOP);
    registerCallback(0x7A, "NOP*", AddrMode::ADDR_IMPLIED, NOP);
    registerCallback(0xDA, "NOP*", AddrMode::ADDR_IMPLIED, NOP);
    registerCallback(0xFA, "NOP*", AddrMode::ADDR_IMPLIED, NOP);
    registerCallback(0x04, "NOP*", AddrMode::ADDR_ZERO_PAGE, NOP_zpg);
    registerCallback(0x44, "NOP*", AddrMode::ADDR_ZERO_PAGE, NOP_zpg);
    registerCallback(0x64, "NOP*", AddrMode::ADDR_ZERO_PAGE, NOP_zpg);
    registerCallback(0x0C, "NOP*", AddrMode::ADDR_ABSOLUTE, NOP_abs);
    registerCallback(0x14, "NOP*", AddrMode::ADDR_ZERO_PAGE_X, NOP_zpg_x);
    registerCallback(0x34, "NOP*", AddrMode::ADDR_ZERO_PAGE_X, NOP_zpg_x);
    registerCallback(0x54, "NOP*", AddrMode::ADDR_ZERO_PAGE_X, NOP_zpg_x);
    registerCallback(0x74, "NOP*", AddrMode::ADDR_ZERO_PAGE_X, NOP_zpg_x);
    registerCallback(0xD4, "NOP*", AddrMode::ADDR_ZERO_PAGE_X, NOP_zpg_x);
    registerCallback(0xF4, "NOP*", AddrMode::ADDR_ZERO_PAGE_X, NOP_zpg_x);
    registerCallback(0x1C, "NOP*", AddrMode::ADDR_ABSOLUTE_X, NOP_abs_x);
    registerCallback(0x3C, "NOP*", AddrMode::ADDR_ABSOLUTE_X, NOP_abs_x);
    registerCallback(0x5C, "NOP*", AddrMode::ADDR_ABSOLUTE_X, NOP_abs_x);
    registerCallback(0x7C, "NOP*", AddrMode::ADDR_ABSOLUTE_X, NOP_abs_x);
    registerCallback(0xDC, "NOP*", AddrMode::ADDR_ABSOLUTE_X, NOP_abs_x);
    registerCallback(0xFC, "NOP*", AddrMode::ADDR_ABSOLUTE_X, NOP_abs_x);

    registerCallback(0x27, "RLA*", AddrMode::ADDR_ZERO_PAGE, RLA_zpg);
    registerCallback(0x37, "RLA*", AddrMode::ADDR_ZERO_PAGE_X, RLA_zpg_x);
    registerCallback(0x23, "RLA*", AddrMode::ADDR_INDIRECT_X, RLA_ind_x);
    registerCallback(0x33, "RLA*", AddrMode::ADDR_INDIRECT_Y, RLA_ind_y);
    registerCallback(0x2F, "RLA*", AddrMode::ADDR_ABSOLUTE, RLA_abs);
    registerCallback(0x3F, "RLA*", AddrMode::ADDR_ABSOLUTE_X, RLA_abs_x);
    registerCallback(0x3B, "RLA*", AddrMode::ADDR_ABSOLUTE_Y, RLA_abs_y);

    registerCallback(0x67, "RRA*", AddrMode::ADDR_ZERO_PAGE, RRA_zpg);
    registerCallback(0x77, "RRA*", AddrMode::ADDR_ZERO_PAGE_X, RRA_zpg_x);
    registerCallback(0x63, "RRA*", AddrMode::ADDR_INDIRECT_X, RRA_ind_x);
    registerCallback(0x73, "RRA*", AddrMode::ADDR_INDIRECT_Y, RRA_ind_y);
    registerCallback(0x6F, "RRA*", AddrMode::ADDR_ABSOLUTE, RRA_abs);
    registerCallback(0x7F, "RRA*", AddrMode::ADDR_ABSOLUTE_X, RRA_abs_x);
    registerCallback(0x7B, "RRA*", AddrMode::ADDR_ABSOLUTE_Y, RRA_abs_y);

    registerCallback(0x87, "SAX*", AddrMode::ADDR_ZERO_PAGE, SAX_zpg);
    registerCallback(0x97, "SAX*", AddrMode::ADDR_ZERO_PAGE_Y, SAX_zpg_y);
    registerCallback(0x83, "SAX*", AddrMode::ADDR_INDIRECT_X, SAX_ind_x);
    registerCallback(0x8F, "SAX*", AddrMode::ADDR_ABSOLUTE, SAX_abs);

    registerCallback(0xEB, "SBC*", AddrMode::ADDR_IMMEDIATE, SBC_imm);

    registerCallback(0x9E, "SHX*", AddrMode::ADDR_ABSOLUTE_Y, SHX_abs_y);
    registerCallback(0x9C, "SHY*", AddrMode::ADDR_ABSOLUTE_X, SHY_abs_x);

    registerCallback(0x07, "SLO*", AddrMode::ADDR_ZERO_PAGE, SLO_zpg);
    registerCallback(0x17, "SLO*", AddrMode::ADDR_ZERO_PAGE_X, SLO_zpg_x);
    registerCallback(0x03, "SLO*", AddrMode::ADDR_INDIRECT_X, SLO_ind_x);
    registerCallback(0x13, "SLO*", AddrMode::ADDR_INDIRECT_Y, SLO_ind_y);
    registerCallback(0x0F, "SLO*", AddrMode::ADDR_ABSOLUTE, SLO_abs);
    registerCallback(0x1F, "SLO*", AddrMode::ADDR_ABSOLUTE_X, SLO_abs_x);
    registerCallback(0x1B, "SLO*", AddrMode::ADDR_ABSOLUTE_Y, SLO_abs_y);

    registerCallback(0x47, "SRE*", AddrMode::ADDR_ZERO_PAGE, SRE_zpg);
    registerCallback(0x57, "SRE*", AddrMode::ADDR_ZERO_PAGE_X, SRE_zpg_x);
    registerCallback(0x43, "SRE*", AddrMode::ADDR_INDIRECT_X, SRE_ind_x);
    registerCallback(0x53, "SRE*", AddrMode::ADDR_INDIRECT_Y, SRE_ind_y);
    registerCallback(0x4F, "SRE*", AddrMode::ADDR_ABSOLUTE, SRE_abs);
    registerCallback(0x5F, "SRE*", AddrMode::ADDR_ABSOLUTE_X, SRE_abs_x);
    registerCallback(0x5B, "SRE*", AddrMode::ADDR_ABSOLUTE_Y, SRE_abs_y);

    registerCallback(0x9B, "TAS*", AddrMode::ADDR_ABSOLUTE_Y, TAS_abs_y);
}
