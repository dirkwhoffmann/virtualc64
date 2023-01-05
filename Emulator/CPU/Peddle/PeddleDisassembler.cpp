// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#include "PeddleConfig.h"
#include "Peddle.h"
#include <iostream>
#include <iomanip>
#include <fstream>


//
// Printing
//

namespace vc64::peddle {

static void
sprint8d(char *s, u8 value)
{
    for (int i = 2; i >= 0; i--) {

        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s[3] = 0;
}

static void
sprint8x(char *s, u8 value)
{
    for (int i = 1; i >= 0; i--) {

        u8 digit = value % 16;
        s[i] = (digit <= 9) ? ('0' + digit) : ('A' + digit - 10);
        value /= 16;
    }
    s[2] = 0;
}

static void
sprint16d(char *s, u16 value)
{
    for (int i = 4; i >= 0; i--) {

        u8 digit = value % 10;
        s[i] = '0' + digit;
        value /= 10;
    }
    s[5] = 0;
}

static void
sprint16x(char *s, u16 value)
{
    for (int i = 3; i >= 0; i--) {

        u8 digit = value % 16;
        s[i] = (digit <= 9) ? ('0' + digit) : ('A' + digit - 10);
        value /= 16;
    }
    s[4] = 0;
}

void
Disassembler::setNumberFormat(DasmNumberFormat value)
{
    if (value.prefix == nullptr) {
        throw std::runtime_error("prefix must not be NULL");
    }
    if (value.radix != 10 && value.radix != 16) {
        throw std::runtime_error("Invalid radix: " + std::to_string(value.radix));
    }

    style.numberFormat = value;
}

void
Disassembler::setIndentation(int value)
{
    style.tab = value;
}

isize
Disassembler::disassemble(u16 addr, char *str) const
{
    return disassemble(cpu.getPC0(),
                       cpu.readDasm(addr),
                       cpu.readDasm(addr + 1),
                       cpu.readDasm(addr + 2), str);
}

isize
Disassembler::disassemble(u16 pc, u8 byte1, u8 byte2, u8 byte3, char *str) const
{
    StrWriter writer(str, style);

    // Write mnemonic
    writer << Ins { byte1 };

    // Write operand
    switch (cpu.addressingMode[byte1]) {

        case ADDR_IMMEDIATE:    writer << Tab{} << Imm  { byte2 }; break;
        case ADDR_ZERO_PAGE:    writer << Tab{} << Zp   { byte2 }; break;
        case ADDR_ZERO_PAGE_X:  writer << Tab{} << Zpx  { byte2 }; break;
        case ADDR_ZERO_PAGE_Y:  writer << Tab{} << Zpy  { byte2 }; break;
        case ADDR_ABSOLUTE:     writer << Tab{} << Abs  { LO_HI(byte2, byte3) }; break;
        case ADDR_ABSOLUTE_X:   writer << Tab{} << Absx { LO_HI(byte2, byte3) }; break;
        case ADDR_ABSOLUTE_Y:   writer << Tab{} << Absy { LO_HI(byte2, byte3) }; break;
        case ADDR_DIRECT:       writer << Tab{} << Dir  { LO_HI(byte2, byte3) }; break;
        case ADDR_INDIRECT:     writer << Tab{} << Ind  { LO_HI(byte2, byte3) }; break;
        case ADDR_INDIRECT_X:   writer << Tab{} << Indx { byte2 }; break;
        case ADDR_INDIRECT_Y:   writer << Tab{} << Indy { byte2 }; break;
        case ADDR_RELATIVE:     writer << Tab{} << Rel  { (u16)(pc + 2 + (i8)byte2) }; break;

        default:
            break;
    }

    // Terminate the string
    writer << Fin{};

    return cpu.getLengthOfInstruction(byte1);
}

void
Disassembler::disassembleByte(u8 byte, char *str) const
{
    StrWriter writer(str, style);
    writer << byte;
}

void
Disassembler::disassembleBytes(u8 values[], isize cnt, char *str) const
{
    StrWriter writer(str, style);

    for (isize i = 0; i < cnt; i++) {

        if (i) writer << " ";
        writer << values[i];
    }
}

isize
Disassembler::disassembleInstrBytes(u16 addr, char *str) const
{
    u8 bytes[] = {cpu.readDasm(addr), cpu.readDasm(addr + 1), cpu.readDasm(addr + 2) };
    isize len = cpu.getLengthOfInstruction(bytes[0]);

    disassembleBytes(bytes, len, str);
    return len;
}

void
Disassembler::disassembleWord(u16 word, char *str) const
{
    StrWriter writer(str, style);
    writer << word;
}

void
Disassembler::disassembleMemory(u16 addr, isize cnt, char *str) const
{
    StrWriter writer(str, style);

    for (isize i = 0; i < cnt; i++) {

        if (i) writer << " ";
        writer << cpu.readDasm(U16_ADD(addr, i));
    }
}

void
Disassembler::disassembleFlags(u8 sr, char *str) const
{
    assert(str);

    str[0] = (sr & N_FLAG) ? 'N' : 'n';
    str[1] = (sr & V_FLAG) ? 'V' : 'v';
    str[2] = '-';
    str[3] = (sr & B_FLAG) ? 'B' : 'b';
    str[4] = (sr & D_FLAG) ? 'D' : 'd';
    str[5] = (sr & I_FLAG) ? 'I' : 'i';
    str[6] = (sr & Z_FLAG) ? 'Z' : 'z';
    str[7] = (sr & C_FLAG) ? 'C' : 'c';
    str[8] = 0;
}

void
Disassembler::disassembleFlags(char *str) const
{
    disassembleFlags(cpu.getP(), str);
}


//
// OLD API
//

const char *
Disassembler::disassembleRecordedInstr(isize i, long *len) const
{
    return disassembleInstr(cpu.debugger.logEntryAbs(i), len);
}

const char *
Disassembler::disassembleRecordedBytes(isize i) const
{
    return disassembleBytes(cpu.debugger.logEntryAbs(i));
}

const char *
Disassembler::disassembleRecordedFlags(isize i) const
{
    return disassembleRecordedFlags(cpu.debugger.logEntryAbs(i));
}

const char *
Disassembler::disassembleRecordedPC(isize i) const
{
    return disassembleAddr(cpu.debugger.logEntryAbs(i).pc);
}

const char *
Disassembler::disassembleInstr(u16 addr, long *len) const
{
    RecordedInstruction instr;

    instr.pc = addr;
    instr.byte1 = cpu.readDasm(addr);
    instr.byte2 = cpu.readDasm(addr + 1);
    instr.byte3 = cpu.readDasm(addr + 2);

    return disassembleInstr(instr, len);
}

const char *
Disassembler::disassembleBytes(u16 addr) const
{
    RecordedInstruction instr;

    instr.byte1 = cpu.readDasm(addr);
    instr.byte2 = cpu.readDasm(addr + 1);
    instr.byte3 = cpu.readDasm(addr + 2);

    return disassembleBytes(instr);
}

const char *
Disassembler::disassembleAddr(u16 addr) const
{
    static char result[6];

    style.numberFormat.radix == 16 ? sprint16x(result, addr) : sprint16d(result, addr);
    return result;
}

const char *
Disassembler::disassembleInstr(long *len) const
{
    return disassembleInstr(cpu.getPC0(), len);
}

const char *
Disassembler::disassembleBytes() const
{
    return disassembleBytes(cpu.getPC0());
}

const char *
Disassembler::disassemblePC() const
{
    return disassembleAddr(cpu.getPC0());
}

void
Disassembler::disassembleRange(std::ostream& os, u16 addr, isize count)
{
    disassembleRange(os, std::pair<u16, u16>(addr, UINT16_MAX), count);
}

void
Disassembler::disassembleRange(std::ostream& os, std::pair<u16, u16> range, isize max)
{
    u16 addr = range.first;
    isize numBytes = 0;
    auto pc = cpu.getPC0();

    for (isize i = 0; i < max && addr <= range.second; i++) {

        auto instr = disassembleInstr(addr, &numBytes);
        auto data = disassembleBytes(addr);

        os << std::setfill(' ');

        os << (addr == pc ? "->" : "  ");

        /*
         if (breakpoints.isDisabledAt(addr)) {
         os << "b";
         } else if (breakpoints.isSetAt(addr)) {
         os << "B";
         } else {
         os << " ";
         }
         */
        os << " ";

        os << std::right << std::setw(4) << disassembleAddr(addr);
        os << "   ";
        os << std::left << std::setw(9) << data;
        os << "   ";
        os << instr;
        os << std::endl;

        U16_INC(addr, numBytes);
    }
}

const char *
Disassembler::disassembleInstr(RecordedInstruction instr, long *len) const
{
    static char result[32];
    static char result2[32];

    StrWriter writer(result, style);

    u8 opcode = instr.byte1;
    if (len) *len = cpu.getLengthOfInstruction(opcode);

    writer << Ins { opcode };

    switch (cpu.addressingMode[opcode]) {

        case ADDR_IMMEDIATE:

            writer << Tab{} << Imm { instr.byte2 };
            break;

        case ADDR_ZERO_PAGE:

            writer << Tab{} << Zp { instr.byte2 };
            break;

        case ADDR_ZERO_PAGE_X:

            writer << Tab{} << Zpx { instr.byte2 };
            break;

        case ADDR_ZERO_PAGE_Y:

            writer << Tab{} << Zpy { instr.byte2 };
            break;

        case ADDR_ABSOLUTE:

            writer << Tab{} << Abs { LO_HI(instr.byte2, instr.byte3) };
            break;

        case ADDR_ABSOLUTE_X:

            writer << Tab{} << Absx { LO_HI(instr.byte2, instr.byte3) };
            break;

        case ADDR_ABSOLUTE_Y:

            writer << Tab{} << Absy { LO_HI(instr.byte2, instr.byte3) };
            break;

        case ADDR_DIRECT:

            writer << Tab{} << Dir  { LO_HI(instr.byte2, instr.byte3) };
            break;

        case ADDR_INDIRECT:

            writer << Tab{} << Ind  { LO_HI(instr.byte2, instr.byte3) };
            break;

        case ADDR_INDIRECT_X:

            writer << Tab{} << Indx { instr.byte2 };
            break;

        case ADDR_INDIRECT_Y:

            writer << Tab{} << Indy { instr.byte2 };
            break;

        case ADDR_RELATIVE:

            writer << Tab{} << Rel { (u16)(instr.pc + 2 + (i8)instr.byte2) };
            break;

        default:
            break;
    }

    writer << Fin{};

    // Compare with new code
    auto cnt = disassemble(instr.pc, instr.byte1, instr.byte2, instr.byte3, result2);
    assert(cnt == cpu.getLengthOfInstruction(opcode));
    assert(strcmp(result, result2) == 0);

    return result;
}

const char *
Disassembler::disassembleBytes(const RecordedInstruction &instr) const
{
    static char result[13]; char *ptr = result;

    isize len = cpu.getLengthOfInstruction(instr.byte1);

    if (style.numberFormat.radix == 16) {

        if (len >= 1) { sprint8x(ptr, instr.byte1); ptr[2] = ' '; ptr += 3; }
        if (len >= 2) { sprint8x(ptr, instr.byte2); ptr[2] = ' '; ptr += 3; }
        if (len >= 3) { sprint8x(ptr, instr.byte3); ptr[2] = ' '; ptr += 3; }

    } else {

        if (len >= 1) { sprint8d(ptr, instr.byte1); ptr[3] = ' '; ptr += 4; }
        if (len >= 2) { sprint8d(ptr, instr.byte2); ptr[3] = ' '; ptr += 4; }
        if (len >= 3) { sprint8d(ptr, instr.byte3); ptr[3] = ' '; ptr += 4; }
    }
    ptr[0] = 0;

    return result;
}

const char *
Disassembler::disassembleRecordedFlags(const RecordedInstruction &instr) const
{
    static char result[9];

    result[0] = (instr.flags & N_FLAG) ? 'N' : 'n';
    result[1] = (instr.flags & V_FLAG) ? 'V' : 'v';
    result[2] = '-';
    result[3] = (instr.flags & B_FLAG) ? 'B' : 'b';
    result[4] = (instr.flags & D_FLAG) ? 'D' : 'd';
    result[5] = (instr.flags & I_FLAG) ? 'I' : 'i';
    result[6] = (instr.flags & Z_FLAG) ? 'Z' : 'z';
    result[7] = (instr.flags & C_FLAG) ? 'C' : 'c';
    result[8] = 0;

    return result;
}

}
