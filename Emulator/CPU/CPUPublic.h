// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef CPU_PUBLIC_H
#define CPU_PUBLIC_H

#include "Aliases.h"

#define LOG_BUFFER_CAPACITY 256 // TODO: CLEAN THIS UP. STILL USED?
#define CPUINFO_INSTR_COUNT 256 // TODO: CLEAN THIS UP. STILL USED? 

/*
#define C_FLAG 0x01
#define Z_FLAG 0x02
#define I_FLAG 0x04
#define D_FLAG 0x08
#define B_FLAG 0x10
#define V_FLAG 0x40
#define N_FLAG 0x80
*/

//
// Enumerations
//

enum_long(CPUREV)
{
    MOS_6510,
    MOS_6502
};
typedef CPUREV CPURevision;

inline bool isCPURevision(long value)
{
    return (unsigned long)value <= MOS_6502;
}

inline const char *CPURevisionName(CPURevision value)
{
    switch (value) {
            
        case MOS_6510:  return "MOS_6510";
        case MOS_6502:  return "MOS_6502";
    }
    return "???";
}

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

inline bool isIntSource(long value)
{
    return (unsigned long)value <= INTSRC_KBD;
}

inline const char *IntSourceName(IntSource value)
{
    switch (value) {
            
        case INTSRC_CIA:   return "CIA";
        case INTSRC_VIC:   return "VIC";
        case INTSRC_VIA1:  return "VIA1";
        case INTSRC_VIA2:  return "VIA2";
        case INTSRC_EXP:   return "EXP";
        case INTSRC_KBD:   return "KBD";
    }
    return "???";
}

enum_long(BPTYPE)
{
    BPTYPE_NONE,
    BPTYPE_HARD,
    BPTYPE_SOFT
};
typedef BPTYPE BreakpointType;

inline bool isBreakpointType(long value)
{
    return (unsigned long)value <= BPTYPE_SOFT;
}

inline const char *BreakpointTypeName(BreakpointType value)
{
    switch (value) {
            
        case BPTYPE_NONE:  return "NONE";
        case BPTYPE_HARD:  return "HARD";
        case BPTYPE_SOFT:  return "SOFT";
    }
    return "???";
}

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

    /*
    u16 pc0;
    u8 sp;
    u8 a;
    u8 x;
    u8 y;
    
    bool nFlag;
    bool vFlag;
    bool bFlag;
    bool dFlag;
    bool iFlag;
    bool zFlag;
    bool cFlag;
    */
    
    Registers reg;
    
    u8 irq;
    u8 nmi;
    bool rdy;
    bool jammed;
    
    u8 processorPort;
    u8 processorPortDir;
}
CPUInfo;

#endif
