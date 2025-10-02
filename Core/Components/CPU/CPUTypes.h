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

#include "Reflection.h"
#include "PeddleTypes.h"

namespace vc64 {

//
// Bit fields
//

#define INTSRC_CIA  0b00000001
#define INTSRC_VIC  0b00000010
#define INTSRC_VIA1 0b00000100
#define INTSRC_VIA2 0b00001000
#define INTSRC_EXP  0b00010000
#define INTSRC_KBD  0b00100000


//
// Enumerations
//

using vc64::peddle::CPURevision;

struct CPURevisionEnum : Reflection<CPURevisionEnum, CPURevision> {

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

struct DasmNumbersEnum : Reflection<DasmNumbersEnum, DasmNumbers>
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

typedef struct
{
    DasmNumbers dasmNumbers;    
}
CPUConfig;

typedef struct
{
    i64 cycle;

    u16 pc0;
    u16 pc;
    u8 sp;
    u8 a;
    u8 x;
    u8 y;
    u8 sr;

    u8 irq;
    u8 nmi;
    bool rdy;

    long next;

    u8 processorPort;
    u8 processorPortDir;
}
CPUInfo;

}
