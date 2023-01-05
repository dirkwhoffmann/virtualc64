// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "PeddleTypes.h"
#include "PeddleUtils.h"
#include "StrWriter.h"

namespace vc64::peddle {

class Disassembler {

    friend class Peddle;
    friend class Debugger;

    // Reference to the connected CPU
    class Peddle &cpu;

public:

    // Currently used disassembler style
    DasmStyle style = {

        .numberFormat   = { .prefix = "", .radix = 16, .upperCase = true, .fill = true },
        .tab            = 4
    };


    //
    // Initializing
    //

public:

    Disassembler(Peddle& ref) : cpu(ref) { };


    //
    // Configuring
    //

    void setNumberFormat(DasmNumberFormat value);
    void setIndentation(int value);


    //
    // Running the disassembler
    //

public:

    // Disassemble an instruction
    isize disassemble(u16 addr, char *str) const;
    isize disassemble(u16 pc, u8 byte1, u8 byte2, u8 byte3, char *str) const;

    // Pretty-prints memory and flags
    void disassembleByte(u8 value, char *str) const;
    void disassembleBytes(u8 values[], isize cnt, char *str) const;
    isize disassembleInstrBytes(u16 addr, char *str) const;
    void disassembleWord(u16 value, char *str) const;
    void disassembleMemory(u16 addr, isize cnt, char *str) const;
    void disassembleFlags(u8 sr, char *str) const;
    void disassembleFlags(char *str) const;

    //
    // OLD API
    //

public:

    // Disassembles a previously recorded instruction
    [[deprecated]] const char *disassembleRecordedInstr(isize i, long *len) const;
    [[deprecated]] const char *disassembleRecordedBytes(isize i) const;
    [[deprecated]] const char *disassembleRecordedFlags(isize i) const;
    [[deprecated]] const char *disassembleRecordedPC(isize i) const;

    // Disassembles the instruction at the specified address
    const char *disassembleInstr(u16 addr, long *len) const;
    const char *disassembleBytes(u16 addr) const;
    const char *disassembleAddr(u16 addr) const;

    // Disassembles the currently executed instruction
    const char *disassembleInstr(long *len) const;
    const char *disassembleBytes() const;
    const char *disassemblePC() const;

    // Disassembles a memory range
    void disassembleRange(std::ostream& os, u16 addr, isize count);
    void disassembleRange(std::ostream& os, std::pair<u16, u16> range, isize max = 255);

private:

    const char *disassembleInstr(RecordedInstruction instr, long *len) const;
    const char *disassembleBytes(const RecordedInstruction &instr) const;
    const char *disassembleRecordedFlags(const RecordedInstruction &instr) const;
};

}
