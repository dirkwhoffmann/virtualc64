//
//  C64_defs.h
//  V64
//
//  Created by Dirk Hoffmann on 05.06.17.
//
//

#ifndef C64_DEFS_H
#define C64_DEFS_H

//! @brief Snapshot version number of this release
#define V_MAJOR 1
#define V_MINOR 5
#define V_SUBMINOR 0

/*! @brief    Message layout
 *  @details  A message queue is used to communicate with the GUI. Each message consists of 
 *            a message id and an additional integer parameter.
 */
typedef struct {
    int id;
    int i;
} Message;

/*! @brief    Rom types
 *  @details  VirtualC64 needs all four ROMs to run
 */
enum {
    BASIC_ROM = 1,
    CHAR_ROM = 2,
    KERNEL_ROM = 4,
    VC1541_ROM = 8
};

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

//! @brief    Keycodes of special keys
enum C64Key {
    C64KEY_F1 = 0x80,
    C64KEY_F2,
    C64KEY_F3,
    C64KEY_F4,
    C64KEY_F5,
    C64KEY_F6,
    C64KEY_F7,
    C64KEY_F8,
    C64KEY_DEL,
    C64KEY_INS,
    C64KEY_RET,
    C64KEY_CL,
    C64KEY_CR,
    C64KEY_CU,
    C64KEY_CD,
    C64KEY_ARROW,
    C64KEY_RUNSTOP,
    C64KEY_RESTORE,
    C64KEY_COMMODORE = 0x0100, // flag that is combinable with all other keys
    C64KEY_CTRL = 0x0200
};

//! @brief    Processor models
enum CPUChipModel {
    MOS_6510 = 0,
    MOS_6502 = 1
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

/*! @brief    Memory type
 *  @details  This datatype defines a constant value for the different kinds of memory.
 */
enum MemoryType {
    MEM_RAM,
    MEM_ROM,
    MEM_IO
};

/*! @brief    Sound chip models
 *  @details  This enum reflects enum "chip_model" used by reSID.
 */
enum SIDChipModel {
    MOS_6581,
    MOS_8580
};

/*! @brief    Sampling method
 *  @details  This enum reflects enum "sampling_method" used by reSID.
 */
enum SamplingMethod {
    SID_SAMPLE_FAST,
    SID_SAMPLE_INTERPOLATE,
    SID_SAMPLE_RESAMPLE_INTERPOLATE,
    SID_SAMPLE_RESAMPLE_FAST
};

/*! @brief    Joystick directions
 */
enum JoystickDirection
{
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_FIRE,
    JOYSTICK_RELEASED
};

/*! @enum     ContainerType
 *  @brief    The type of a container
 *  @constant CRT_CONTAINER A cartridge that can be plugged into the expansion port.
 *  @constant V64_CONTAINER A snapshot file (contains a frozen C64 state).
 *  @constant D64_CONTAINER A floppy disk image with multiply files.
 *  @constant T64_CONTAINER A tape archive with multiple files.
 *  @constant PRG_CONTAINER A program archive containing a single file.
 *  @constant P00_CONTAINER A program archive containing a single file.
 *  @constant G64_CONTAINER A collection of bit-streams resembling a floppy disk.
 *  @constant NIB_CONTAINER A collection of bit-streams resembling a floppy disk.
 *  @constant TAP_CONTAINER A bit-stream resembling a datasette tape.
 *  @constant FILE_CONTAINER An arbitrary file that is interpreted as raw data.
 */
enum ContainerType {
    UNKNOWN_CONTAINER_FORMAT = 0,
    CRT_CONTAINER,
    V64_CONTAINER,
    D64_CONTAINER,
    T64_CONTAINER,
    PRG_CONTAINER,
    P00_CONTAINER,
    G64_CONTAINER,
    NIB_CONTAINER,
    TAP_CONTAINER,
    FILE_CONTAINER
};


#endif
