/*!
 * @header      CPU_types.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef CPU_TYPES_H
#define CPU_TYPES_H

#include <stdint.h>

//! @brief    Processor model
typedef enum {
    MOS_6510 = 0,
    MOS_6502 = 1
} CPUChipModel;

//! @brief    Addressing mode
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

/*! @brief    Breakpoint type
 *  @details  Each memory call is marked with a breakpoint tag. Originally,
 *            each cell is tagged with NO_BREAKPOINT which has no effect. CPU
 *            execution will stop if the memory cell is tagged with one of the
 *            following breakpoint types:
 *            HARD_BREAKPOINT : Execution is halted.
 *            SOFT_BREAKPOINT : Execution is halted and the tag is deleted.
 */
typedef enum {
    NO_BREAKPOINT   = 0x00,
    HARD_BREAKPOINT = 0x01,
    SOFT_BREAKPOINT = 0x02
} Breakpoint;


/*! @brief    Error state of the virtual CPU
 *  @details  CPU_OK indicates normal operation. When a (soft or hard)
 *            breakpoint is reached, state CPU_BREAKPOINT_REACHED is entered.
 *            CPU_ILLEGAL_INSTRUCTION is set when an opcode is not understood
 *            by the CPU. Once the CPU enters a different state than CPU_OK,
 *            the execution thread is terminated.
 */
typedef enum {
    CPU_OK = 0,
    CPU_SOFT_BREAKPOINT_REACHED,
    CPU_HARD_BREAKPOINT_REACHED,
    CPU_ILLEGAL_INSTRUCTION
} ErrorState;

/*! @brief    CPU info
 *  @details  Used by getInfo() to collect debug information
 */
typedef struct {
    uint64_t cycle;
    uint16_t pc;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    bool nFlag;
    bool vFlag;
    bool bFlag;
    bool dFlag;
    bool iFlag;
    bool zFlag;
    bool cFlag;
} CPUInfo;

//! @brief    Recorded instruction
typedef struct {
    uint64_t cycle;
    uint16_t pc;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t a;
    uint8_t x;
    uint8_t y;
    uint8_t sp;
    uint8_t flags;
} RecordedInstruction;

//! @brief    Disassembled instruction
typedef struct {
    uint64_t cycle;
    uint16_t addr; 
    uint8_t size;
    char byte1[4];
    char byte2[4];
    char byte3[4];
    char pc[6];
    char a[4];
    char x[4];
    char y[4];
    char sp[4];
    char flags[9];
    char command[16];
} DisassembledInstruction;

#endif
