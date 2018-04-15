//
//  CPU_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

#ifndef CPU_TYPES_H
#define CPU_TYPES_H

//! @brief    Processor models
typedef enum {
    MOS_6510 = 0,
    MOS_6502 = 1
} CPUChipModel;

//! @brief    Addressing modes
typedef enum {
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
} AddressingMode;

/*! @brief    Error states of the virtual CPU
 *  @details  CPU_OK indicates normal operation. When a (soft or hard) breakpoint is reached,
 *            the CPU enters the CPU_BREAKPOINT_REACHED state. CPU_ILLEGAL_INSTRUCTION is
 *            entered when an opcode is not understood by the CPU. Once the CPU enters a
 *            different state than CPU_OK, the execution thread is terminated.
 */
typedef enum {
    CPU_OK = 0,
    CPU_SOFT_BREAKPOINT_REACHED,
    CPU_HARD_BREAKPOINT_REACHED,
    CPU_ILLEGAL_INSTRUCTION
} ErrorState;

/*! @brief    Breakpoint type
 *  @details  Each memory call is marked with a breakpoint tag. Originally, each cell is
 *            tagged with NO_BREAKPOINT which has no effect. CPU execution will stop if the
 *            memory cell is tagged with one of the following breakpoint types:
 *
 *            HARD_BREAKPOINT: execution is halted
 *            SOFT_BREAKPOINT: execution is halted and the tag is deleted
 */
typedef enum {
    NO_BREAKPOINT   = 0x00,
    HARD_BREAKPOINT = 0x01,
    SOFT_BREAKPOINT = 0x02
} Breakpoint;

//! @brief    Disassembled instruction
typedef struct {
    char formatted[80];
    char instr[16];
    char pc[6];
    char byte[3][4];
    char A[4];
    char X[4];
    char Y[4];
    char SP[4];
    char flags[9];
} DisassembledInstruction;

#endif
