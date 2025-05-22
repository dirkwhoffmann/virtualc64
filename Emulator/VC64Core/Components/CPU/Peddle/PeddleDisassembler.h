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

    // Visual style for disassembled instructions
    DasmStyle instrStyle;

    // Visual style for data dumps
    DasmStyle dataStyle;


    //
    // Initializing
    //

public:

    Disassembler(Peddle& ref);


    //
    // Configuring
    //

    void setNumberFormat(DasmNumberFormat instrFormat, DasmNumberFormat dataFormat);
    void setNumberFormat(DasmNumberFormat format) { setNumberFormat(format, format); }
    void setIndentation(int value);


    //
    // Running the disassembler
    //

public:

    // Experimental
    isize disass(char *dst, const char *fmt, u16 addr) const;
    isize disass(char *dst, const char *fmt, const RecordedInstruction &instr) const;
    isize disass(char *dst, u16 addr) const;
    isize disass(char *dst, const RecordedInstruction &instr) const;

private:

    isize disass8(u8 value, char *dst, isize tab) const;
    isize disass16(u16 value, char *dst, isize tab) const;
    isize disassB(u8 byte1, u8 byte2, u8 byte3, char *dst, isize tab) const;
    isize disassI(u16 addr, u8 byte1, u8 byte2, u8 byte3, char *dst, isize tab) const;
    isize disassF(u8 flags, char *dst, isize tab) const;

public:

    // Disassembles an instruction (DEPRECATED)
    isize disassemble(char *str, u16 addr) const;
    isize disassemble(char *str, u16 pc, u8 byte1, u8 byte2, u8 byte3) const;

    // Creates a textual representation for the status register (DEPRECATED)
    void disassembleFlags(char *str, u8 sr) const;
    void disassembleFlags(char *str) const;

    // Creates a textual representation for a single byte or word
    void dumpByte(char *str, u8 value) const;
    void dumpWord(char *str, u16 value) const;

    // Creates a textual representation for a sequence of bytes or words
    void dumpBytes(char *str, u32 addr, isize cnt) const;
    void dumpBytes(char *str, u8 values[], isize cnt) const;
    void dumpWords(char *str, u32 addr, isize cnt) const;
    void dumpWords(char *str, u16 values[], isize cnt) const;

    // Disassembles larger code sections
    void disassembleRange(std::ostream& os, u16 addr, isize count);
    void disassembleRange(std::ostream& os, std::pair<u16, u16> range, isize max = 255);
};

}
