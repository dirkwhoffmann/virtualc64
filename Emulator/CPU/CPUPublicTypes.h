// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"

#define LOG_BUFFER_CAPACITY 256 // TODO: CLEAN THIS UP. STILL USED?
#define CPUINFO_INSTR_COUNT 256 // TODO: CLEAN THIS UP. STILL USED? 

//
// Enumerations
//

enum_long(CPUREV)
{
    MOS_6510,
    MOS_6502,
    CPUREV_COUNT
};
typedef CPUREV CPURevision;

enum_byte(INTSRC)
{
    INTSRC_CIA  = 0x01,
    INTSRC_VIC  = 0x02,
    INTSRC_VIA1 = 0x04,
    INTSRC_VIA2 = 0x08,
    INTSRC_EXP  = 0x10,
    INTSRC_KBD  = 0x20
};
typedef INTSRC IntSource;

enum_long(BPTYPE)
{
    BPTYPE_NONE,
    BPTYPE_HARD,
    BPTYPE_SOFT,
    BPTYPE_COUNT
};
typedef BPTYPE BreakpointType;

//
// Structures
//

typedef struct
{
    bool n;               // Negative flag
    bool v;               // Overflow flag
    bool b;               // Break flag
    bool d;               // Decimal flag
    bool i;               // Interrupt flag
    bool z;               // Zero flag
    bool c;               // Carry flag
}
StatusRegister;
    
typedef struct
{    
    u16 pc;   // Program counter
    u16 pc0;  // Frozen program counter (beginning of current instruction)
    
    u8 sp;    // Stack pointer

    u8 a;     // Accumulator
    u8 x;     // First index register
    u8 y;     // Second index register

    u8 adl;   // Address data (low byte)
    u8 adh;   // Address data (high byte)
    u8 idl;   // Input data latch (indirect addressing modes)
    u8 d;     // Data buffer
    
    bool ovl; // Overflow indicator (page boundary crossings)

    StatusRegister sr;
}
Registers;

typedef struct
{
    u64 cycle;
    
    u8 byte1;
    u8 byte2;
    u8 byte3;
    
    u16 pc;
    u8 sp;
    u8 a;
    u8 x;
    u8 y;
    u8 flags;
}
RecordedInstruction;

typedef struct
{
    u64 cycle;

    Registers reg;
    
    u8 irq;
    u8 nmi;
    bool rdy;
    bool jammed;
    
    u8 processorPort;
    u8 processorPortDir;
}
CPUInfo;
