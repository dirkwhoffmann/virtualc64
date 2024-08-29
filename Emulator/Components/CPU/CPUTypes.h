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

struct CPURevisionEnum : util::Reflection<CPURevisionEnum, vc64::peddle::CPURevision> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = vc64::peddle::MOS_8502;

    static const char *prefix() { return nullptr; }
    static const char *_key(long value)
    {
        switch (value) {

            case vc64::peddle::MOS_6502:  return "MOS_6502";
            case vc64::peddle::MOS_6507:  return "MOS_6507";
            case vc64::peddle::MOS_6510:  return "MOS_6510";
            case vc64::peddle::MOS_8502:  return "MOS_8502";
        }
        return "???";
    }
};


//
// Structures
//

typedef struct
{
    i64 cycle;

    u16 pc0;
    u8 sp;
    u8 a;
    u8 x;
    u8 y;
    u8 sr;

    u8 irq;
    u8 nmi;
    bool rdy;

    u8 processorPort;
    u8 processorPortDir;
}
CPUInfo;

}
