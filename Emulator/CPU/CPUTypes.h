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

#define LOG_BUFFER_CAPACITY 256

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
    NO_BREAKPOINT   = 0x00,
    HARD_BREAKPOINT = 0x01,
    SOFT_BREAKPOINT = 0x02
}
Breakpoint;

typedef enum
{
    CPU_OK = 0,
    CPU_BREAKPOINT_REACHED,
    CPU_ILLEGAL_INSTRUCTION
}
ErrorState;

typedef enum : u8
{
    C_FLAG = 0x01,
    Z_FLAG = 0x02,
    I_FLAG = 0x04,
    D_FLAG = 0x08,
    B_FLAG = 0x10,
    V_FLAG = 0x40,
    N_FLAG = 0x80
}
Flag;

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

//
// Structures
//

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
    u16 addr; 
    u8 size;
    char byte1[4]; // DEPRECATED, REPLACED BY data
    char byte2[4]; // DEPRECATED, REPLACED BY data
    char byte3[4]; // DEPRECATED, REPLACED BY data
    char data[13];
    char pc[6];
    char a[4];
    char x[4];
    char y[4];
    char sp[4];
    char flags[9];
    char command[16];
}
DisassembledInstruction;

#define CPUINFO_INSTR_COUNT 256

typedef struct
{
    u64 cycle;

    u16 pc;
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
    
    // Start address of the disassembled instructions
    u16 start;

    // Disassembled instructions, starting at 'start'
    DisassembledInstruction instr[CPUINFO_INSTR_COUNT];

    // Disassembled instructions from the log buffer
    DisassembledInstruction loggedInstr[CPUINFO_INSTR_COUNT];
}
CPUInfo;

#endif
