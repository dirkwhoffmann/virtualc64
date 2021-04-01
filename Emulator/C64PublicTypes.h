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

#pragma once

#include "Aliases.h"

#include "CartridgePublicTypes.h"
#include "DiskPublicTypes.h"
#include "DrivePublicTypes.h"
#include "FSPublicTypes.h"
#include "MemoryPublicTypes.h"
#include "MsgQueuePublicTypes.h"

//
// Enumerations
//

enum_long(OPT)
{
    // C64
    OPT_CYCLE_LIMIT,
    
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


//
// Structures
//

typedef struct
{
    Cycle cycleLimit;
}
C64Config;
