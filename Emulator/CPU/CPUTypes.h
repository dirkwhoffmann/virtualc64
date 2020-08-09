// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef CPU_TYPES_H
#define CPU_TYPES_H

#include "Aliases.h"

#define LOG_BUFFER_CAPACITY 256 // TODO: CLEAN THIS UP. STILL USED?
#define CPUINFO_INSTR_COUNT 256 // TODO: CLEAN THIS UP. STILL USED? 

#define C_FLAG 0x01
#define Z_FLAG 0x02
#define I_FLAG 0x04
#define D_FLAG 0x08
#define B_FLAG 0x10
#define V_FLAG 0x40
#define N_FLAG 0x80

//
// Enumerations
//

typedef enum
{
    MOS_6510 = 0,
    MOS_6502 = 1
}
CPUModel;

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

typedef enum
{
    CPU_OK = 0,
    CPU_JAMMED,
    CPU_HALTED_BY_BREAKPOINT,
    CPU_HALTED_BY_WATCHPOINT
}
CPUState;

typedef enum : u8
{
    INTSRC_CIA  = 0x01,
    INTSRC_VIC  = 0x02,
    INTSRC_VIA1 = 0x04,
    INTSRC_VIA2 = 0x08,
    INTSRC_EXP  = 0x10,
    INTSRC_KBD  = 0x20
}
IntSource;

typedef enum
{
    NO_BREAKPOINT   = 0x00,
    HARD_BREAKPOINT = 0x01,
    SOFT_BREAKPOINT = 0x02
}
Breakpoint;

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

    u8 irq;
    u8 nmi;
    bool rdy;
    bool halted;
    
    u8 processorPort;
    u8 processorPortDir;
}
CPUInfo;

#endif
