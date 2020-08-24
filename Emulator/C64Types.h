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
#include "MemoryTypes.h"
#include "MessageQueueTypes.h"
#include "MouseTypes.h"
#include "SIDTypes.h"
#include "VICIITypes.h"

//
// Enumerations
//

typedef enum : long
{
    // VICII
    OPT_VIC_REVISION,
    OPT_PALETTE,
    OPT_GRAY_DOT_BUG,
    
    // Logic board
    OPT_GLUE_LOGIC,

    // CIA
    OPT_CIA_REVISION,
    OPT_TIMER_B_BUG,
    
    // SID
    OPT_SID_REVISION,
    OPT_SID_FILTER,
    
    // Sound synthesis
    OPT_SID_ENGINE,
    OPT_SID_SAMPLING,
    
    // Memory
    OPT_RAM_PATTERN,
    
    // Drive
    OPT_DRIVE_CONNECT,
    OPT_DRIVE_TYPE,
}
ConfigOption;

inline bool isConfigOption(long value)
{
    return value >= OPT_VIC_REVISION && value <= OPT_SID_REVISION;
}

typedef enum : long
{
    C64_PAL,
    C64_II_PAL,
    C64_OLD_PAL,
    C64_NTSC,
    C64_II_NTSC,
    C64_OLD_NTSC,
    C64_CUSTOM
}
C64Model;

inline bool isC64Model(long value) {
    return value >= C64_PAL && value <= C64_OLD_NTSC;
}

typedef enum : long
{
    STATE_OFF,
    STATE_PAUSED,
    STATE_RUNNING
}
EmulatorState;

inline bool isEmulatorState(long value) {
    return value >= STATE_OFF && value <= STATE_RUNNING;
}

typedef enum
{
    RL_STOP               = 0b0000001,
    RL_CPU_JAMMED         = 0b0000010,
    RL_INSPECT            = 0b0000100,
    RL_BREAKPOINT_REACHED = 0b0001000,
    RL_WATCHPOINT_REACHED = 0b0010000,
    RL_AUTO_SNAPSHOT      = 0b0100000,
    RL_USER_SNAPSHOT      = 0b1000000
}
RunLoopControlFlag;

typedef enum : long
{
    INSPECT_NONE,
    INSPECT_CPU,
    INSPECT_MEM,
    INSPECT_CIA,
    INSPECT_VIC,
    INSPECT_SID
}
InspectionTarget;

inline bool isInspectionTarget(long value) {
    return value >= INSPECT_NONE && value <= INSPECT_SID;
}

typedef enum
{
    ERR_OK,
    ERR_ROM_MISSING,
    ERR_ROM_MEGA65_MISMATCH
}
ErrorCode;

inline bool isErrorCode(long value) {
    return value >= ERR_OK && value <= ERR_ROM_MEGA65_MISMATCH;
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
    { PAL_6569_R3, false, MOS_6526, true, MOS_6581, true, GLUE_DISCRETE, RAM_PATTERN_C64 },
    
    // C64_II_PAL
    { PAL_8565, true, MOS_8521, false, MOS_8580, true, GLUE_CUSTOM_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_PAL
    { PAL_6569_R1, false, MOS_6526, true, MOS_6581, true, GLUE_DISCRETE, RAM_PATTERN_C64 },

    // C64_NTSC
    { NTSC_6567, false, MOS_6526, false, MOS_6581, true, GLUE_DISCRETE, RAM_PATTERN_C64 },

    // C64_II_NTSC
    { NTSC_8562, true, MOS_8521, true, MOS_8580, true, GLUE_CUSTOM_IC, RAM_PATTERN_C64C },
    
    // C64_OLD_NTSC
    { NTSC_6567_R56A, false, MOS_6526, false, MOS_6581, true, GLUE_DISCRETE, RAM_PATTERN_C64 }
};

#endif
