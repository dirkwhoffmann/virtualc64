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
    // ERROR_UNKNOWN,

    // Memory
    ERROR_OUT_OF_MEMORY,

    // File IO
    ERROR_FILE_NOT_FOUND,
    ERROR_FILE_TYPE_MISMATCH,
    ERROR_FILE_CANT_READ,
    ERROR_FILE_CANT_WRITE,
    ERROR_FILE_CANT_CREATE,
    ERROR_DIR_CANT_CREATE,
    ERROR_DIR_NOT_EMPTY,

    // Snapshots
    ERROR_SNP_UNSUPPORTED,

    // Cartridges
    ERROR_CRT_UNSUPPORTED,
    
    // File system errors
    ERROR_FS_UNSUPPORTED,
    ERROR_FS_WRONG_CAPACITY,
    ERROR_FS_CORRUPTED,
    ERROR_FS_HAS_CYCLES,
    ERROR_FS_CANT_IMPORT,
    ERROR_FS_EXPECTED_VAL,
    ERROR_FS_EXPECTED_MIN,
    ERROR_FS_EXPECTED_MAX
};
typedef ERRORCode ErrorCode;

inline bool isErrorCode(long value)
{
    return (unsigned long)value <= ERROR_FILE_CANT_CREATE;
}

inline const char *ErrorCodeName(ErrorCode value)
{
    switch (value) {
            
        case ERROR_OK:                  return "OK";

        case ERROR_OUT_OF_MEMORY:       return "OUT_OF_MEMORY";

        case ERROR_FILE_NOT_FOUND:      return "FILE_NOT_FOUND";
        case ERROR_FILE_TYPE_MISMATCH:  return "FILE_TYPE_MISMATCH";
        case ERROR_FILE_CANT_READ:      return "FILE_CANT_READ";
        case ERROR_FILE_CANT_WRITE:     return "FILE_CANT_WRITE";
        case ERROR_FILE_CANT_CREATE:    return "FILE_CANT_CREATE";
        case ERROR_DIR_CANT_CREATE:     return "DIR_CANT_CREATE";
        case ERROR_DIR_NOT_EMPTY:       return "DIR_NOT_EMPTY";

        case ERROR_SNP_UNSUPPORTED:     return "SNP_UNSUPPORTED";
            
        case ERROR_CRT_UNSUPPORTED:     return "CRT_UNSUPPORTED";

        case ERROR_FS_UNSUPPORTED:      return "FS_UNSUPPORTED";
        case ERROR_FS_WRONG_CAPACITY:   return "FS_WRONG_CAPACITY";
        case ERROR_FS_CORRUPTED:        return "FS_CORRUPTED";
        case ERROR_FS_HAS_CYCLES:       return "FS_HAS_CYCLES";
        case ERROR_FS_CANT_IMPORT:      return "FS_CANT_IMPORT";
        case ERROR_FS_EXPECTED_VAL:     return "FS_EXPECTED_VAL";
        case ERROR_FS_EXPECTED_MIN:     return "FS_EXPECTED_MIN";
        case ERROR_FS_EXPECTED_MAX:     return "FS_EXPECTED_MAX";
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
