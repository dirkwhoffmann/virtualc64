// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS Technology 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

#pragma once

#include "Types.h"

#ifdef __cplusplus
namespace peddle {
#endif

//
// Constants
//

#ifdef __cplusplus
static constexpr isize LOG_BUFFER_CAPACITY = 256;

static constexpr isize C_FLAG = 0x01;
static constexpr isize Z_FLAG = 0x02;
static constexpr isize I_FLAG = 0x04;
static constexpr isize D_FLAG = 0x08;
static constexpr isize B_FLAG = 0x10;
static constexpr isize V_FLAG = 0x40;
static constexpr isize N_FLAG = 0x80;
#endif


//
// Bit fields
//

// Interrupt source
typedef u8 IntSource;

/* State flags
 *
 * CPU_LOG_INSTRUCTION:
 *
 *     This flag is set if instruction logging is enabled. If set, the
 *     CPU records the current register contents in a log buffer.
 *
 * CPU_CHECK_BP, CPU_CHECK_WP, CPU_CHECK_CP:
 *
 *    These flags indicate whether the CPU should check for breakpoints,
 *    watchpoints, or catchpoints.
 */
#ifdef __cplusplus
static constexpr int CPU_LOG_INSTRUCTION    = (1 << 0);
static constexpr int CPU_CHECK_BP           = (1 << 1);
static constexpr int CPU_CHECK_WP           = (1 << 2);
static constexpr int CPU_CHECK_CP           = (1 << 3);
#endif


//
// Enumerations
//

enum_long(CPUREV)
{
    MOS_6502,
    MOS_6507,
    MOS_6510,
};
typedef CPUREV CPURevision;

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

#ifdef __cplusplus
}
#endif
