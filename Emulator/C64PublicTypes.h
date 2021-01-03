// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

/* This file defines all constants and data types that are exposed to the GUI.
 * All definitions comply to standard ANSI-C to make the file accessible by
 * Swift. Note that the Swift GUI does not interact directly with any of the
 * public API methods of the emulator. Since Swift cannot deal with C++ code
 * directly yet, all API accesses are routed through the proxy layer written in
 * Objective-C.
 */

#ifndef C64_PUBLIC_TYPES_H
#define C64_PUBLIC_TYPES_H

#include "Aliases.h"

#include "CartridgePublicTypes.h"
#include "CIAPublicTypes.h"
#include "CPUPublicTypes.h"
#include "DiskPublicTypes.h"
#include "DrivePublicTypes.h"
#include "FilePublicTypes.h"
#include "FSPublicTypes.h"
#include "MemoryPublicTypes.h"
#include "MsgQueuePublicTypes.h"
#include "MousePublicTypes.h"
#include "PortPublicTypes.h"
#include "SIDPublicTypes.h"
#include "VICIIPublicTypes.h"

//
// Enumerations
//

enum_long(OPT)
{
    // VICII
    OPT_VIC_REVISION,
    OPT_PALETTE,
    OPT_GRAY_DOT_BUG,
    OPT_HIDE_SPRITES,
    OPT_DMA_DEBUG,
    OPT_DMA_CHANNEL_R,
    OPT_DMA_CHANNEL_I,
    OPT_DMA_CHANNEL_C,
    OPT_DMA_CHANNEL_G,
    OPT_DMA_CHANNEL_P,
    OPT_DMA_CHANNEL_S,
    OPT_DMA_COLOR_R,
    OPT_DMA_COLOR_I,
    OPT_DMA_COLOR_C,
    OPT_DMA_COLOR_G,
    OPT_DMA_COLOR_P,
    OPT_DMA_COLOR_S,
    OPT_DMA_DISPLAY_MODE,
    OPT_DMA_OPACITY,
    OPT_CUT_LAYERS,
    OPT_CUT_OPACITY,
    OPT_SS_COLLISIONS,
    OPT_SB_COLLISIONS,

    // Logic board
    OPT_GLUE_LOGIC,

    // CIA
    OPT_CIA_REVISION,
    OPT_TIMER_B_BUG,
    
    // SID
    OPT_SID_ENABLE,
    OPT_SID_ADDRESS,
    OPT_SID_REVISION,
    OPT_SID_FILTER,
    OPT_AUDPAN,
    OPT_AUDVOL,
    OPT_AUDVOLL,
    OPT_AUDVOLR,
    
    // Sound synthesis
    OPT_SID_ENGINE,
    OPT_SID_SAMPLING,
    
    // Memory
    OPT_RAM_PATTERN,
    
    // Drive
    OPT_DRIVE_TYPE,
    OPT_DRIVE_CONNECT,
    OPT_DRIVE_POWER_SWITCH,
    
    // Debugging
    OPT_DEBUGCART,
    
    OPT_COUNT
};
typedef OPT Option;

enum_long(C64_MODEL)
{
    C64_MODEL_PAL,
    C64_MODEL_PAL_II,
    C64_MODEL_PAL_OLD,
    C64_MODEL_NTSC,
    C64_MODEL_NTSC_II,
    C64_MODEL_NTSC_OLD,
    C64_MODEL_CUSTOM,
    C64_MODEL_COUNT
};
typedef C64_MODEL C64Model;

enum_long(ROM_TYPE)
{
    ROM_TYPE_BASIC,
    ROM_TYPE_CHAR,
    ROM_TYPE_KERNAL,
    ROM_TYPE_VC1541,
    ROM_TYPE_COUNT
};
typedef ROM_TYPE RomType;

enum_long(EMULATOR_STATE)
{
    EMULATOR_STATE_OFF,
    EMULATOR_STATE_PAUSED,
    EMULATOR_STATE_RUNNING,
    EMULATOR_STATE_COUNT
};
typedef EMULATOR_STATE EmulatorState;

enum_long(INSPECTION_TARGET)
{
    INSPECTION_TARGET_NONE,
    INSPECTION_TARGET_CPU,
    INSPECTION_TARGET_MEM,
    INSPECTION_TARGET_CIA,
    INSPECTION_TARGET_VIC,
    INSPECTION_TARGET_SID,
    INSPECTION_TARGET_COUNT
};
typedef INSPECTION_TARGET InspectionTarget;

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
    ERROR_OK,
    ERROR_UNKNOWN,

    // Memory errors
    ERROR_OUT_OF_MEMORY,

    // File errors
    ERROR_FILE_NOT_FOUND,
    ERROR_INVALID_TYPE,
    ERROR_CANT_READ,
    ERROR_CANT_WRITE,

    // Snapshots
    ERROR_UNSUPPORTED_SNAPSHOT,

    // Cartridges
    ERROR_UNSUPPORTED_CRT,
    
    // File system errors
    ERROR_UNSUPPORTED,
    ERROR_WRONG_CAPACITY,
    ERROR_HAS_CYCLES,
    ERROR_CORRUPTED,
    ERROR_IMPORT_ERROR,

    // Block errros
    ERROR_EXPECTED,
    ERROR_EXPECTED_MIN,
    ERROR_EXPECTED_MAX,

    // Export errors
    ERROR_DIRECTORY_NOT_EMPTY,
    ERROR_CANNOT_CREATE_DIR,
    ERROR_CANNOT_CREATE_FILE
};
typedef ERRORCode ErrorCode;

inline bool isErrorCode(long value)
{
    return (unsigned long)value <= ERROR_CANNOT_CREATE_FILE;
}

inline const char *ErrorCodeName(ErrorCode value)
{
    switch (value) {
            
        case ERROR_OK:                    return "OK";
        case ERROR_UNKNOWN:               return "UNKNOWN";

        case ERROR_OUT_OF_MEMORY:         return "OUT_OF_MEMORY";

        case ERROR_FILE_NOT_FOUND:        return "FILE_NOT_FOUND";
        case ERROR_INVALID_TYPE:          return "INVALID_TYPE";
        case ERROR_CANT_READ:             return "CANT_READ";
        case ERROR_CANT_WRITE:            return "CANT_WRITE";

        case ERROR_UNSUPPORTED_SNAPSHOT:  return "UNSUPPORTED_SNAPSHOT";
            
        case ERROR_UNSUPPORTED_CRT:       return "UNSUPPORTED_CRT";

        case ERROR_UNSUPPORTED:           return "UNSUPPORTED";
        case ERROR_WRONG_CAPACITY:        return "WRONG_CAPACITY";
        case ERROR_HAS_CYCLES:            return "HAS_CYCLES";
        case ERROR_CORRUPTED:             return "CORRUPTED";
        case ERROR_IMPORT_ERROR:          return "IMPORT_ERROR";

        case ERROR_DIRECTORY_NOT_EMPTY:   return "DIRECTORY_NOT_EMPTY";
        case ERROR_CANNOT_CREATE_DIR:     return "CANNOT_CREATE_DIR";
        case ERROR_CANNOT_CREATE_FILE:    return "CANNOT_CREATE_FILE";

        case ERROR_EXPECTED:              return "EXPECTED";
        case ERROR_EXPECTED_MIN:          return "EXPECTED_MIN";
        case ERROR_EXPECTED_MAX:          return "EXPECTED_MAX";
    }
    return "???";
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
    VICRevision vic;
    bool grayDotBug;
    CIARevision cia;
    bool timerBBug;
    SIDRevision sid;
    bool sidFilter;
    GlueLogic glue;
    RamPattern pattern;
}
C64ConfigurationDeprecated;

// Configurations of standard C64 models
static const C64ConfigurationDeprecated configurations[] = {
    
    // C64 PAL
    { VICREV_PAL_6569_R3, false, MOS_6526, true, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 },
    
    // C64_II_PAL
    { VICREV_PAL_8565, true, MOS_8521, false, MOS_8580, true, GLUE_LOGIC_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_PAL
    { VICREV_PAL_6569_R1, false, MOS_6526, true, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 },

    // C64_NTSC
    { VICREV_NTSC_6567, false, MOS_6526, false, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 },

    // C64_II_NTSC
    { VICREV_NTSC_8562, true, MOS_8521, true, MOS_8580, true, GLUE_LOGIC_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_NTSC
    { VICREV_NTSC_6567_R56A, false, MOS_6526, false, MOS_6581, true, GLUE_LOGIC_DISCRETE, RAM_PATTERN_C64 }
};

#endif
