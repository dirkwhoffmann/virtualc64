// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "BasicTypes.h"
#include "PeddleTypes.h"

namespace vc64 {

//
// Bit fields
//

inline constexpr int INTSRC_CIA = 0b00000001;
inline constexpr int INTSRC_VIC = 0b00000010;
inline constexpr int INTSRC_VIA1 = 0b00000100;
inline constexpr int INTSRC_VIA2 = 0b00001000;
inline constexpr int INTSRC_EXP = 0b00010000;
inline constexpr int INTSRC_KBD = 0b00100000; //
// Enumerations
//

using vc64::peddle::CPURevision;

struct CPURevisionEnum : utl::Reflectable<CPURevisionEnum, CPURevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(CPURevision::MOS_8502);

    static const char *_key(CPURevision value)
    {
        switch (value) {

            case CPURevision::MOS_6502: return "MOS_6502";
            case CPURevision::MOS_6507: return "MOS_6507";
            case CPURevision::MOS_6510: return "MOS_6510";
            case CPURevision::MOS_8502: return "MOS_8502";
        }
        return "???";
    }
    
    static const char *help(vc64::peddle::CPURevision value)
    {
        return "";
    }
};

enum class DasmNumbers : long
{
    HEX,
    HEX0,
    DEC,
    DEC0,
};

struct DasmNumbersEnum : utl::Reflectable<DasmNumbersEnum, DasmNumbers>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DasmNumbers::DEC0);

    static const char *_key(DasmNumbers value)
    {
        switch (value) {

            case DasmNumbers::HEX:     return "DASM_HEX";
            case DasmNumbers::HEX0:    return "DASM_HEX0";
            case DasmNumbers::DEC:     return "DASM_DEC";
            case DasmNumbers::DEC0:    return "DASM_DEC0";
        }
        return "???";
    }
    
    static const char *help(DasmNumbers value)
    {
        return "";
    }
};


//
// Structures
//

enum class GuardType : long
{
    BREAKPOINT,
    WATCHPOINT
};

struct GuardTypeEnum : utl::Reflectable<GuardTypeEnum, GuardType>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(GuardType::WATCHPOINT);

    static const char *_key(GuardType value)
    {
        switch (value) {

            case GuardType::BREAKPOINT:  return "BREAKPOINT";
            case GuardType::WATCHPOINT:  return "WATCHPOINT";
        }
        return "???";
    }
    static const char *help(GuardType value)
    {
        switch (value) {

            case GuardType::BREAKPOINT:  return "Breakpoint";
            case GuardType::WATCHPOINT:  return "Watchpoint";
        }
        return "???";
    }
};

/// Configuration of the CPU
typedef struct
{
    DasmNumbers dasmNumbers;                ///< Disassembler number format
}
CPUConfig;

/// State of the CPU
typedef struct
{
    i64 cycle;                              ///< Cycle counter

    u16 pc0;                                ///< Address of the current instruction
    u16 pc;                                 ///< Program counter
    u8 sp;                                  ///< Stack pointer
    u8 a;                                   ///< Accumulator
    u8 x;                                   ///< X register
    u8 y;                                   ///< Y register
    u8 sr;                                  ///< Status register

    u8 irq;                                 ///< State of the IRQ lines
    u8 nmi;                                 ///< State of the NMI lines
    bool rdy;                               ///< State of the RDY line
    bool jammed;                            ///< The CPU is jammed

    long next;                              ///< Next micro instruction to be executed

    u8 processorPort;                       ///< Processor port register
    u8 processorPortDir;                    ///< Processor port data direction register
}
CPUInfo;

/// Description of a breakpoint or watchpoint
typedef struct
{
    u32 addr;                               ///< Observed address
    bool enabled;                           ///< The guard is enabled
    isize hits;                             ///< Number of times the guard was hit
    isize ignore;                           ///< Number of hits that are ignored before the guard triggers
}
GuardInfo;

}
