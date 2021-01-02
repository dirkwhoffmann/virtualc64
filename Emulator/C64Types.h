// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef C64_TYPES_H
#define C64_TYPES_H

#include "Aliases.h"

#include "CartridgeTypes.h"
#include "CIATypes.h"
#include "PortTypes.h"
#include "CPUTypes.h"
#include "DiskTypes.h"
#include "DriveTypes.h"
#include "FileTypes.h"
#include "FSTypes.h"
#include "MemoryTypes.h"
#include "MessageQueueTypes.h"
#include "MouseTypes.h"
#include "SIDTypes.h"
#include "VICIITypes.h"

//
// Enumerations
//

enum_long(Option)
{
    // VICII
    Option_VIC_REVISION,
    Option_PALETTE,
    Option_GRAY_DOT_BUG,
    Option_HIDE_SPRITES,
    Option_DMA_DEBUG,
    Option_DMA_CHANNEL_R,
    Option_DMA_CHANNEL_I,
    Option_DMA_CHANNEL_C,
    Option_DMA_CHANNEL_G,
    Option_DMA_CHANNEL_P,
    Option_DMA_CHANNEL_S,
    Option_DMA_COLOR_R,
    Option_DMA_COLOR_I,
    Option_DMA_COLOR_C,
    Option_DMA_COLOR_G,
    Option_DMA_COLOR_P,
    Option_DMA_COLOR_S,
    Option_DMA_DISPLAY_MODE,
    Option_DMA_OPACITY,
    Option_CUT_LAYERS,
    Option_CUT_OPACITY,
    Option_SS_COLLISIONS,
    Option_SB_COLLISIONS,

    // Logic board
    Option_GLUE_LOGIC,

    // CIA
    Option_CIA_REVISION,
    Option_TIMER_B_BUG,
    
    // SID
    Option_SID_ENABLE,
    Option_SID_ADDRESS,
    Option_SID_REVISION,
    Option_SID_FILTER,
    Option_AUDPAN,
    Option_AUDVOL,
    Option_AUDVOLL,
    Option_AUDVOLR,
    
    // Sound synthesis
    Option_SID_ENGINE,
    Option_SID_SAMPLING,
    
    // Memory
    Option_RAM_PATTERN,
    
    // Drive
    Option_DRIVE_TYPE,
    Option_DRIVE_CONNECT,
    Option_DRIVE_POWER_SWITCH,
    
    // Debugging
    Option_DEBUGCART
};

inline bool isConfigOption(long value)
{
    return (unsigned long)value <= Option_DEBUGCART;
}

enum_long(C64Model)
{
    C64Model_PAL,
    C64Model_PAL_II,
    C64Model_PAL_OLD,
    C64Model_NTSC,
    C64Model_NTSC_II,
    C64Model_NTSC_OLD,
    C64Model_CUSTOM
};

inline bool isC64Model(long value) {
    return (unsigned long)value <= C64Model_CUSTOM;
}

enum_long(ROM_TYPE)
{
    ROM_TYPE_BASIC,
    ROM_TYPE_CHAR,
    ROM_TYPE_KERNAL,
    ROM_TYPE_VC1541
};
typedef ROM_TYPE RomType;

inline bool isRomType(long value) {
    return (unsigned long)value <= ROM_TYPE_VC1541;
}

enum_long(STATE)
{
    STATE_OFF,
    STATE_PAUSED,
    STATE_RUNNING
};
typedef STATE State;

inline bool isEmulatorState(long value) {
    return (unsigned long)value <=  STATE_RUNNING;
}

enum_int(ACTION_FLAG)
{
    ACTION_FLAG_STOP          = 0b0000001,
    ACTION_FLAG_CPU_JAMMED    = 0b0000010,
    ACTION_FLAG_INSPECT       = 0b0000100,
    ACTION_FLAG_BREAKPOINT    = 0b0001000,
    ACTION_FLAG_WATCHPOINT    = 0b0010000,
    ACTION_FLAG_AUTO_SNAPSHOT = 0b0100000,
    ACTION_FLAG_USER_SNAPSHOT = 0b1000000
};
typedef ACTION_FLAG ActionFlag;

enum_long(INSPECTION_TARGET)
{
    INSPECTION_TARGET_NONE,
    INSPECTION_TARGET_CPU,
    INSPECTION_TARGET_MEM,
    INSPECTION_TARGET_CIA,
    INSPECTION_TARGET_VIC,
    INSPECTION_TARGET_SID
};
typedef INSPECTION_TARGET InspectionTarget;

inline bool isInspectionTarget(long value) {
    return (unsigned long)value <= INSPECTION_TARGET_SID;
}

inline const char *InspectionTargetName(InspectionTarget value)
{
    switch (value) {
            
        case INSPECTION_TARGET_NONE:  return "NONE";
        case INSPECTION_TARGET_CPU:   return "CPU";
        case INSPECTION_TARGET_MEM:   return "IMEM";
        case INSPECTION_TARGET_CIA:   return "CIA";
        case INSPECTION_TARGET_VIC:   return "VIC";
        case INSPECTION_TARGET_SID:   return "SID";

        default:
            return isInspectionTarget(value) ? "???" : "<invalid>";
    }
}

typedef enum
{
    ERR_OK,
    ERR_ROM_MISSING,
    ERR_ROM_MEGA65_MISMATCH
}
RomErrorCode;

inline bool isRomErrorCode(long value) {
    return value >= ERR_OK && value <= ERR_ROM_MEGA65_MISMATCH;
}

enum_long(ERRORCode)
{
    Error_OK,
    Error_UNKNOWN,

    // Memory errors
    ERROR_OUT_OF_MEMORY,

    // File errors
    Error_FILE_NOT_FOUND,
    Error_INVALID_TYPE,
    Error_CANT_READ,
    Error_CANT_WRITE,

    // File system errors
    Error_UNSUPPORTED,
    Error_WRONG_CAPACITY,
    Error_HAS_CYCLES,
    Error_CORRUPTED,
    Error_IMPORT_ERROR,

    // Export errors
    Error_DIRECTORY_NOT_EMPTY,
    Error_CANNOT_CREATE_DIR,
    Error_CANNOT_CREATE_FILE,

    // Block errros
    Error_EXPECTED,
    Error_EXPECTED_MIN,
    Error_EXPECTED_MAX,
            
    // Snapshot errors
    Error_UNSUPPORTED_SNAPSHOT
};
typedef ERRORCode ErrorCode;

inline bool isErrorCode(long value)
{
    return (unsigned long)value <= Error_UNSUPPORTED_SNAPSHOT;
}

inline const char *ErrorCodeName(ErrorCode value)
{
    switch (value) {
            
        case Error_OK:                    return "OK";
        case Error_UNKNOWN:               return "UNKNOWN";

        case ERROR_OUT_OF_MEMORY:         return "OUT_OF_MEMORY";

        case Error_FILE_NOT_FOUND:        return "FILE_NOT_FOUND";
        case Error_INVALID_TYPE:          return "INVALID_TYPE";
        case Error_CANT_READ:             return "CANT_READ";
        case Error_CANT_WRITE:            return "CANT_WRITE";

        case Error_UNSUPPORTED:           return "UNSUPPORTED";
        case Error_WRONG_CAPACITY:        return "WRONG_CAPACITY";
        case Error_HAS_CYCLES:            return "HAS_CYCLES";
        case Error_CORRUPTED:             return "CORRUPTED";
        case Error_IMPORT_ERROR:          return "IMPORT_ERROR";

        case Error_DIRECTORY_NOT_EMPTY:   return "DIRECTORY_NOT_EMPTY";
        case Error_CANNOT_CREATE_DIR:     return "CANNOT_CREATE_DIR";
        case Error_CANNOT_CREATE_FILE:    return "CANNOT_CREATE_FILE";

        case Error_EXPECTED:              return "EXPECTED";
        case Error_EXPECTED_MIN:          return "EXPECTED_MIN";
        case Error_EXPECTED_MAX:          return "EXPECTED_MAX";
                    
        case Error_UNSUPPORTED_SNAPSHOT:  return "UNSUPPORTED_SNAPSHOT";
            
        default:
            return isErrorCode(value) ? "<other>" : "???";
    }
}

//
// Structures
//

typedef struct
{
    VICConfig vic;
    CIAConfig cia1;
    CIAConfig cia2;
    SIDConfig sid;
    MemConfig mem;
}
C64Configuration;

typedef struct
{
    VICRev vic;
    bool grayDotBug;
    CIARev cia;
    bool timerBBug;
    SIDRev sid;
    bool sidFilter;
    GlueLogic glue;
    RamPattern pattern;
}
C64ConfigurationDeprecated;

// Configurations of standard C64 models
static const C64ConfigurationDeprecated configurations[] = {
    
    // C64 PAL
    { VICRev_PAL_6569_R3, false, MOS_6526, true, MOS_6581, true, GlueLogic_DISCRETE, RAM_PATTERN_C64 },
    
    // C64_II_PAL
    { VICRev_PAL_8565, true, MOS_8521, false, MOS_8580, true, GlueLogic_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_PAL
    { VICRev_PAL_6569_R1, false, MOS_6526, true, MOS_6581, true, GlueLogic_DISCRETE, RAM_PATTERN_C64 },

    // C64_NTSC
    { VICRev_NTSC_6567, false, MOS_6526, false, MOS_6581, true, GlueLogic_DISCRETE, RAM_PATTERN_C64 },

    // C64_II_NTSC
    { VICRev_NTSC_8562, true, MOS_8521, true, MOS_8580, true, GlueLogic_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_NTSC
    { VICRev_NTSC_6567_R56A, false, MOS_6526, false, MOS_6581, true, GlueLogic_DISCRETE, RAM_PATTERN_C64 }
};

#endif
