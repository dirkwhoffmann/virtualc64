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

enum_long(RomType)
{
    ROMType_BASIC,
    ROMType_CHAR,
    ROMType_KERNAL,
    ROMType_VC1541
};

inline bool isRomType(long value) {
    return (unsigned long)value <= ROMType_VC1541;
}

enum_long(State)
{
    State_OFF,
    State_PAUSED,
    State_RUNNING
};

inline bool isEmulatorState(long value) {
    return (unsigned long)value <=  State_RUNNING;
}

enum_int(ActionFlag)
{
    ActionFlag_STOP          = 0b0000001,
    ActionFlag_CPU_JAMMED    = 0b0000010,
    ActionFlag_INSPECT       = 0b0000100,
    ActionFlag_BREAKPOINT    = 0b0001000,
    ActionFlag_WATCHPOINT    = 0b0010000,
    ActionFlag_AUTO_SNAPSHOT = 0b0100000,
    ActionFlag_USER_SNAPSHOT = 0b1000000
};

enum_long(InspectionTarget)
{
    InspectionTarget_NONE,
    InspectionTarget_CPU,
    InspectionTarget_MEM,
    InspectionTarget_CIA,
    InspectionTarget_VIC,
    InspectionTarget_SID
};

inline bool isInspectionTarget(long value) {
    return (unsigned long)value <= InspectionTarget_SID;
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
    VICRev vic;
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
