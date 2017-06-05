//
//  C64_defs.h
//  V64
//
//  Created by Dirk Hoffmann on 05.06.17.
//
//

#ifndef C64_DEFS_H
#define C64_DEFS_H

/*! @brief    Message layout
 *  @details  A message queue is used to communicate with the GUI. Each message consists of 
 *            a message id and an additional integer parameter.
 */
typedef struct {
    int id;
    int i;
} Message;


/*! @brief    Message types
 *  @details  List of all possible message id's
 */
enum {
    MSG_ROM_LOADED = 1,
    MSG_ROM_MISSING,
    MSG_ROM_COMPLETE,
    MSG_RUN,
    MSG_HALT,
    MSG_CPU,
    MSG_WARP,
    MSG_ALWAYS_WARP,
    MSG_LOG,
    MSG_VC1541_ATTACHED,
    MSG_VC1541_ATTACHED_SOUND,
    MSG_VC1541_DISK,
    MSG_VC1541_DISK_SOUND,
    MSG_VC1541_LED,
    MSG_VC1541_DATA,
    MSG_VC1541_MOTOR,
    MSG_VC1541_HEAD,
    MSG_VC1541_HEAD_SOUND,
    MSG_CARTRIDGE,
    MSG_VC1530_TAPE,
    MSG_VC1530_PLAY,
    MSG_VC1530_PROGRESS,
    MSG_JOYSTICK_ATTACHED,
    MSG_JOYSTICK_REMOVED,
    MSG_PAL,
    MSG_NTSC
};

//! @brief    Processor models
enum CPUChipModel {
    MOS6510 = 0,
    MOS6502 = 1
};

//! @brief    Addressing modes
enum AddressingMode {
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
};

/*! @brief    Error states of the virtual CPU
 *  @details  CPU_OK indicates normal operation. When a (soft or hard) breakpoint is reached,
 *            the CPU enters the CPU_BREAKPOINT_REACHED state. CPU_ILLEGAL_INSTRUCTION is
 *            entered when an opcode is not understood by the CPU. Once the CPU enters a
 *            different state than CPU_OK, the execution thread is terminated.
 */
enum ErrorState {
    CPU_OK = 0,
    SOFT_BREAKPOINT_REACHED,
    HARD_BREAKPOINT_REACHED,
    ILLEGAL_INSTRUCTION
};

/*! @brief    Breakpoint type
 *  @details  Each memory call is marked with a breakpoint tag. Originally, each cell is
 *            tagged with NO_BREAKPOINT which has no effect. CPU execution will stop if the
 *            memory cell is tagged with one of the following breakpoint types:
 *
 *            HARD_BREAKPOINT: execution is halted
 *            SOFT_BREAKPOINT: execution is halted and the tag is deleted
 */
enum Breakpoint {
    NO_BREAKPOINT   = 0x00,
    HARD_BREAKPOINT = 0x01,
    SOFT_BREAKPOINT = 0x02
};

#endif
