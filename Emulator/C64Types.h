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

enum_long(ConfigOption)
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
    OPT_DEBUGCART
};

inline bool isConfigOption(long value)
{
    return (unsigned long)value <= OPT_DEBUGCART;
}

enum_long(C64Model)
{
    C64_PAL,
    C64_II_PAL,
    C64_OLD_PAL,
    C64_NTSC,
    C64_II_NTSC,
    C64_OLD_NTSC,
    C64_CUSTOM
};

enum_long(RomType)
{
    ROM_BASIC,
    ROM_CHAR,
    ROM_KERNAL,
    ROM_VC1541
};

inline bool isC64Model(long value) {
    return value >= C64_PAL && value <= C64_OLD_NTSC;
}

enum_long(EmulatorState)
{
    STATE_OFF,
    STATE_PAUSED,
    STATE_RUNNING
};

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

enum_long(InspectionTarget)
{
    INSPECT_NONE,
    INSPECT_CPU,
    INSPECT_MEM,
    INSPECT_CIA,
    INSPECT_VIC,
    INSPECT_SID
};

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
