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


namespace vc64::peddle {

Disassembler::Disassembler(Peddle& ref) : cpu(ref)
{
    instrStyle = dataStyle = DasmStyle {

        .numberFormat = { .prefix = "", .radix = 16, .upperCase = true, .fill = '0' },
        .tab          = 4
    };
}

void
Disassembler::setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat)
{
    auto validPrefix = [&](DasmNumberFormat fmt) { return fmt.prefix != nullptr; };
    auto validRadix = [&](DasmNumberFormat fmt) { return fmt.radix == 10 || fmt.radix == 16; };

    if (!validPrefix(instrFormat) || !validPrefix(dataFormat)) {
        throw std::runtime_error("prefix must not be NULL");
    }
    if (!validRadix(instrFormat) || !validRadix(dataFormat)) {
        throw std::runtime_error("radix must be 10 or 16");
    }

    instrStyle.numberFormat = instrFormat;
    dataStyle.numberFormat = dataFormat;
}

void
Disassembler::setIndentation(int value)
{
    instrStyle.tab = value;
    dataStyle.tab = value;
}

isize
Disassembler::disass(char *dst, const char *fmt, u16 addr) const
{
    auto instr = RecordedInstruction {

        .byte1 = cpu.readDasm(addr),
        .byte2 = cpu.readDasm(addr + 1),
        .byte3 = cpu.readDasm(addr + 2),
        .pc = addr
    };

    return disass(dst, fmt, instr);
}

isize
Disassembler::disass(char *dst, const char *fmt, const RecordedInstruction &instr) const
{
    bool ctrl = false;
    isize tab = 0;

    for (char c = fmt[0]; c != 0; c = (++fmt)[0]) {

        if (!ctrl) {

            if (c == '%') { ctrl = true; tab = 0; } else *dst++ = c;
            continue;
        }

        if (c >= '0' && c <= '9') {

            tab = 10 * tab + (c - '0');
            continue;
        }

        switch (c) {

            case 'p': // Program counter (instruction address)

                dst += disass16(instr.pc, dst, tab);
                break;

            case 'a': // Accumulator

                dst += disass8(instr.a, dst, tab);
                break;

            case 'x': // X register

                dst += disass8(instr.x, dst, tab);
                break;

            case 'y': // Y register

                dst += disass8(instr.y, dst, tab);
                break;

            case 's': // Stack pointer

                dst += disass8(instr.sp, dst, tab);
                break;

            case 'b': // Instruction bytes (1 - 3)

                dst += disassB(instr.byte1, instr.byte2, instr.byte3, dst, tab);
                break;

            case 'i': // Disassembled instruction

                dst += disassI(instr.pc, instr.byte1, instr.byte2, instr.byte3, dst, tab);
                break;

            case 'f': // Flags

                dst += disassF(instr.flags, dst, tab);
                break;

            default:
                fatalError;
        }

        ctrl = false;
    }
    *dst = 0;

    return cpu.getLengthOfInstruction(cpu.readDasm(instr.pc));
}

isize
Disassembler::disass(char *dst, u16 addr) const
{
    return disass(dst, "%i", addr);
}

isize
Disassembler::disass(char *dst, const RecordedInstruction &instr) const
{
    return disass(dst, "%i", instr);
}

isize
Disassembler::disass8(u8 value, char *dst, isize tab) const
{
    StrWriter writer(dst, dataStyle);

    writer << value;
    writer.fill(tab);

    return writer.length();
}

isize
Disassembler::disass16(u16 value, char *dst, isize tab) const
{
    StrWriter writer(dst, dataStyle);

    writer << value;
    writer.fill(tab);

    return writer.length();
}

isize
Disassembler::disassB(u8 byte1, u8 byte2, u8 byte3, char *dst, isize tab) const
{
    StrWriter writer(dst, dataStyle);

    auto count = cpu.getLengthOfInstruction(byte1);

    writer << byte1;
    if (count > 1) writer << ' ' << byte2;
    if (count > 2) writer << ' ' << byte3;
    writer.fill(tab);

    return writer.length();
}

isize
Disassembler::disassI(u16 addr, u8 byte1, u8 byte2, u8 byte3, char *dst, isize tab) const
{
    StrWriter writer(dst, instrStyle);

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
        case ADDR_RELATIVE:     writer << Tab{} << Rel  { (u16)(addr + 2 + (i8)byte2) }; break;

        default:
            break;
    }

    writer << Fin{};
    writer.fill(tab);

    return writer.length();
}

isize
Disassembler::disassF(u8 flags, char *dst, isize tab) const
{
    dst[0] = (flags & N_FLAG) ? 'N' : 'n';
    dst[1] = (flags & V_FLAG) ? 'V' : 'v';
    dst[2] = '-';
    dst[3] = (flags & B_FLAG) ? 'B' : 'b';
    dst[4] = (flags & D_FLAG) ? 'D' : 'd';
    dst[5] = (flags & I_FLAG) ? 'I' : 'i';
    dst[6] = (flags & Z_FLAG) ? 'Z' : 'z';
    dst[7] = (flags & C_FLAG) ? 'C' : 'c';

    return 8;
}

isize
Disassembler::disassemble(char *str, u16 addr) const
{
    return disassemble(str,
                       addr,
                       cpu.readDasm(addr),
                       cpu.readDasm(addr + 1),
                       cpu.readDasm(addr + 2));
}

isize
Disassembler::disassemble(char *str, u16 pc, u8 byte1, u8 byte2, u8 byte3) const
{
    StrWriter writer(str, instrStyle);

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
Disassembler::disassembleFlags(char *str, u8 sr) const
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
    disassembleFlags(str, cpu.getP());
}

void
Disassembler::dumpByte(char *str, u8 value) const
{
    StrWriter(str, dataStyle) << value << '\0';
}

void
Disassembler::dumpWord(char *str, u16 value) const
{
    StrWriter(str, dataStyle) << value << '\0';
}

void
Disassembler::dumpBytes(char *str, u32 addr, isize cnt) const
{
    StrWriter writer(str, dataStyle);

    for (isize i = 0; i < cnt; i++) {

        if (i) writer << " ";
        writer << cpu.readDasm(U16_ADD(addr, i));
    }
    writer << '\0';
}

void
Disassembler::dumpBytes(char *str, u8 values[], isize cnt) const
{
    StrWriter writer(str, dataStyle);

    for (isize i = 0; i < cnt; i++) {

        if (i) writer << " ";
        writer << values[i];
    }
    writer << '\0';
}

void
Disassembler::dumpWords(char *str, u32 addr, isize cnt) const
{
    StrWriter writer(str, dataStyle);

    for (isize i = 0; i < cnt; i++) {

        if (i) writer << " ";
        writer << u16(HI_LO(cpu.readDasm(U16_ADD(addr, 2 * i)),
                            cpu.readDasm(U16_ADD(addr, 2 * i + 1))));
    }
    writer << '\0';
}

void
Disassembler::dumpWords(char *str, u16 values[], isize cnt) const
{
    StrWriter writer(str, dataStyle);

    for (isize i = 0; i < cnt; i++) {

        if (i) writer << " ";
        writer << values[i];
    }
    writer << '\0';
}

void
Disassembler::disassembleRange(std::ostream& os, u16 addr, isize count)
{
    disassembleRange(os, std::pair<u16, u16>(addr, UINT16_MAX), count);
}

void
Disassembler::disassembleRange(std::ostream& os, std::pair<u16, u16> range, isize max)
{
    char data[16];
    char instr[16];
    char address[16];

    u16 addr = range.first;
    auto pc = cpu.getPC0();


    for (isize i = 0; i < max && addr <= range.second; i++) {

        auto numBytes = disassemble(instr, addr);
        dumpBytes(data, addr, numBytes);
        dumpWord(address, addr);

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

        os << std::right << std::setw(4) << address;
        os << "   ";
        os << std::left << std::setw(9) << data;
        os << "   ";
        os << instr;
        os << std::endl;

        U16_INC(addr, numBytes);
    }
}

}
