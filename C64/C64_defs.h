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
#define V_MINOR 7
#define V_SUBMINOR 0

/*! @brief    Message layout
 *  @details  A message queue is used to communicate with the GUI. Each message consists of 
 *            a message id and an additional integer parameter.
 */
typedef struct {
    int id;
    int i;
} Message;

/*! @brief    Color schemes
 *  @details  Predefined RGB color values
 */
typedef enum {
    VICE            = 0x00,
    CCS64           = 0x01,
    FRODO           = 0x02,
    PC64            = 0x03,
    C64S            = 0x04,
    ALEC64          = 0x05,
    WIN64           = 0x06,
    C64ALIVE_0_9    = 0x07,
    GODOT           = 0x08,
    C64SALLY        = 0x09,
    PEPTO           = 0x0A,
    GRAYSCALE       = 0x0B
} ColorScheme;

/*! @brief    Rom types
 *  @details  VirtualC64 needs all four ROMs to run
 */
typedef enum {
    BASIC_ROM  = 1,
    CHAR_ROM   = 2,
    KERNEL_ROM = 4,
    VC1541_ROM = 8
} RomType;

/*! @brief    Message types
 *  @details  List of all possible message id's
 */
typedef enum {
    
    // Running the emulator
    MSG_READY_TO_RUN = 1,
    MSG_RUN,
    MSG_HALT,

    // ROM and snapshot handling
    MSG_ROM_LOADED,
    MSG_ROM_MISSING,
    MSG_SNAPSHOT,

    // CPU related messages
    MSG_CPU,
    MSG_WARP,
    MSG_ALWAYS_WARP,

    // VIC related messages
    MSG_PAL,
    MSG_NTSC,

    // Peripherals (Disk drive)
    MSG_VC1541_ATTACHED,
    MSG_VC1541_ATTACHED_SOUND,
    MSG_VC1541_DISK,
    MSG_VC1541_DISK_SOUND,
    MSG_VC1541_LED,
    MSG_VC1541_DATA,
    MSG_VC1541_MOTOR,
    MSG_VC1541_HEAD,
    MSG_VC1541_HEAD_SOUND,
    
    // Peripherals (Datasette)
    MSG_VC1530_TAPE,
    MSG_VC1530_PLAY,
    MSG_VC1530_PROGRESS,

    // Peripherals (Expansion port)
    MSG_CARTRIDGE,
    
    // MSG_LOG,
} VC64Message;

//! @brief    Fingerprint that uniquely identifies a key combination on the virtual C64 keyboard
typedef unsigned short C64KeyFingerprint;

//! @brief    Fingerprints of special keys on the C64 keyboard
enum {
    C64KEY_F1 = 0x80,
    C64KEY_F2,
    C64KEY_F3,
    C64KEY_F4,
    C64KEY_F5,
    C64KEY_F6,
    C64KEY_F7,
    C64KEY_F8,
    C64KEY_POUND,
    C64KEY_HOME,
    C64KEY_CLR,
    C64KEY_DEL,
    C64KEY_INST,
    C64KEY_RET,
    C64KEY_CL,
    C64KEY_CR,
    C64KEY_CU,
    C64KEY_CD,
    C64KEY_LEFTARROW,
    C64KEY_UPARROW,
    C64KEY_RUNSTOP,
    C64KEY_RESTORE,
    
    // Flags (combinable with all other keys)
    C64KEY_SHIFT = 0x1000,
    C64KEY_COMMODORE = 0x2000,
    C64KEY_CTRL = 0x4000
};

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
    SOFT_BREAKPOINT_REACHED,
    HARD_BREAKPOINT_REACHED,
    ILLEGAL_INSTRUCTION
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

/*! @brief    Memory type
 *  @details  This datatype defines a constant value for the different kinds of memory.
 */
typedef enum {
    MEM_RAM,
    MEM_ROM,
    MEM_IO
} MemoryType;

/*! @brief    Sound chip models
 *  @details  This enum reflects enum "chip_model" used by reSID.
 */
typedef enum {
    MOS_6581,
    MOS_8580
} SIDChipModel;

/*! @brief    Sampling method
 *  @details  This enum reflects enum "sampling_method" used by reSID.
 */
typedef enum {
    SID_SAMPLE_FAST,
    SID_SAMPLE_INTERPOLATE,
    SID_SAMPLE_RESAMPLE_INTERPOLATE,
    SID_SAMPLE_RESAMPLE_FAST
} SamplingMethod;

/*! @brief    Joystick directions
 */
typedef enum {
    
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT,
    JOYSTICK_FIRE,
    
} JoystickDirection;

/*! @brief    Joystick events
 */
typedef enum {
    
    PULL_UP,
    PULL_DOWN,
    PULL_LEFT,
    PULL_RIGHT,
    PRESS_FIRE,
    RELEASE_X,
    RELEASE_Y,
    RELEASE_XY,
    RELEASE_FIRE
    
} JoystickEvent;


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
typedef enum {
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
} ContainerType;

typedef enum {
    CRT_NORMAL = 0,
    CRT_ACTION_REPLAY = 1,
    CRT_KCS_POWER = 2,
    CRT_FINAL_III = 3,
    CRT_SIMONS_BASIC = 4,
    CRT_OCEAN = 5,
    CRT_EXPERT = 6,
    CRT_FUNPLAY = 7,
    CRT_SUPER_GAMES = 8,
    CRT_ATOMIC_POWER = 9,
    CRT_EPYX_FASTLOAD = 10,
    CRT_WESTERMANN = 11,
    CRT_REX = 12,
    CRT_FINAL_I = 13,
    CRT_MAGIC_FORMEL = 14,
    CRT_GAME_SYSTEM_SYSTEM_3 = 15,
    CRT_WARPSPEED = 16,
    CRT_DINAMIC = 17,
    CRT_ZAXXON = 18,
    CRT_MAGIC_DESK = 19,
    CRT_SUPER_SNAPSHOT_V5 = 20,
    CRT_COMAL80 = 21,
    CRT_STRUCTURE_BASIC = 22,
    CRT_ROSS = 23,
    CRT_DELA_EP64 = 24,
    CRT_DELA_EP7x8 = 25,
    CRT_DELA_EP256 = 26,
    CRT_REX_EP256 = 27,
    CRT_MIKRO_ASSEMBLER = 28,
    CRT_FINAL_PLUS = 29,
    CRT_ACTION_REPLAY4 = 30,
    CRT_STARDOS = 31,
    CRT_EASYFLASH = 32,
    CRT_EASYFLASH_XBANK = 33,
    CRT_CAPTURE = 34,
    CRT_ACTION_REPLAY3 = 35,
    CRT_RETRO_REPLAY = 36,
    CRT_MMC64 = 37,
    CRT_MMC_REPLAY = 38,
    CRT_IDE64 = 39,
    CRT_SUPER_SNAPSHOT = 40,
    CRT_IEEE488 = 41,
    CRT_GAME_KILLER = 42,
    CRT_P64 = 43,
    CRT_EXOS = 44,
    CRT_FREEZE_FRAME = 45,
    CRT_FREEZE_MACHINE = 46,
    CRT_SNAPSHOT64 = 47,
    CRT_SUPER_EXPLODE_V5 = 48,
    CRT_MAGIC_VOICE = 49,
    CRT_ACTION_REPLAY2 = 50,
    CRT_MACH5 = 51,
    CRT_DIASHOW_MAKER = 52,
    CRT_PAGEFOX = 53,
    CRT_KINGSOFT = 54,
    CRT_SILVERROCK_128 = 55,
    CRT_FORMEL64 = 56,
    CRT_RGCD = 57,
    CRT_RRNETMK3 = 58,
    CRT_EASYCALC = 59,
    CRT_GMOD2 = 60,
    CRT_NONE = 255
} CartridgeType;

#endif
