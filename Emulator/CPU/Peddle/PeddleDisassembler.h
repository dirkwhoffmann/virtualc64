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

    // Currently used disassembler style
    DasmStyle style = {

        .numberFormat   = { .prefix = "", .radix = 16, .upperCase = true },
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

    // Disassembles a previously recorded instruction
    const char *disassembleRecordedInstr(isize i, long *len) const;
    const char *disassembleRecordedBytes(isize i) const;
    const char *disassembleRecordedFlags(isize i) const;
    const char *disassembleRecordedPC(isize i) const;

    // Disassembles the instruction at the specified address
    const char *disassembleInstr(u16 addr, long *len) const;
    const char *disassembleBytes(u16 addr) const;
    const char *disassembleAddr(u16 addr) const;

    // Disassembles the currently executed instruction
    const char *disassembleInstr(long *len) const;
    const char *disassembleBytes() const;
    const char *disassemblePC() const;

    // Dumps a portion of the log buffer
    void dumpLogBuffer(std::ostream& os, isize count);
    void dumpLogBuffer(std::ostream& os);

    // Disassembles a memory range
    void disassembleRange(std::ostream& os, u16 addr, isize count);
    void disassembleRange(std::ostream& os, std::pair<u16, u16> range, isize max = 255);

private:

    const char *disassembleInstr(RecordedInstruction instr, long *len) const;
    const char *disassembleBytes(const RecordedInstruction &instr) const;
    const char *disassembleRecordedFlags(const RecordedInstruction &instr) const;


//     const char *disassembleRecordedInstrNew(RecordedInstruction instr, long *len) const;
};

}
