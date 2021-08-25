// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Constants
//

#define LOG_BUFFER_CAPACITY 256

#define C_FLAG 0x01
#define Z_FLAG 0x02
#define I_FLAG 0x04
#define D_FLAG 0x08
#define B_FLAG 0x10
#define V_FLAG 0x40
#define N_FLAG 0x80


//
// Bit fields
//

typedef u8 IntSource;

#define INTSRC_CIA  0b00000001
#define INTSRC_VIC  0b00000010
#define INTSRC_VIA1 0b00000100
#define INTSRC_VIA2 0b00001000
#define INTSRC_EXP  0b00010000
#define INTSRC_KBD  0b00100000


//
// Enumerations
//

enum_long(CPUREV)
{
    MOS_6510,
    MOS_6502
};
typedef CPUREV CPURevision;

#ifdef __cplusplus
struct CPURevisionEnum : util::Reflection<CPURevisionEnum, CPURevision> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= MOS_6502;
    }
    
    static const char *prefix() { return nullptr; }
    static const char *key(CPURevision value)
    {
        switch (value) {
                
            case MOS_6510:      return "MOS_6510";
            case MOS_6502:      return "MOS_6502";
        }
        return "???";
    }
};
#endif

enum_long(BPTYPE)
{
    BPTYPE_NONE,
    BPTYPE_HARD,
    BPTYPE_SOFT
};
typedef BPTYPE BreakpointType;

#ifdef __cplusplus
struct BreakpointTypeEnum : util::Reflection<BreakpointTypeEnum, BreakpointType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= BPTYPE_SOFT;
    }
    
    static const char *prefix() { return "BPTYPE"; }
    static const char *key(BreakpointType value)
    {
        switch (value) {
                
            case BPTYPE_NONE:   return "NONE";
            case BPTYPE_HARD:   return "HARD";
            case BPTYPE_SOFT:   return "SOFT";
        }
        return "???";
    }
};
#endif

typedef enum
{
    ADDR_IMPLIED,
    ADDR_ACCUMULATOR,
    ADDR_IMMEDIATE,
    ADDR_ZERO_PAGE,
    ADDR_ZERO_PAGE_X,
    ADDR_ZERO_PAGE_Y,
    ADDR_ABSOLUTE,
    ADDR_ABSOLUTE_X,
    ADDR_ABSOLUTE_Y,
    ADDR_INDIRECT_X,
    ADDR_INDIRECT_Y,
    ADDR_RELATIVE,
    ADDR_DIRECT,
    ADDR_INDIRECT
}
AddressingMode;


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
