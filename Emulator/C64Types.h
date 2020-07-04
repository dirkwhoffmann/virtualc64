// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef C64TYPES_H
#define C64TYPES_H

#include "Aliases.h"

#include "CartridgeTypes.h"
#include "CIATypes.h"
#include "ControlPortTypes.h"
#include "CPUTypes.h"
#include "DiskTypes.h"
#include "DriveTypes.h"
#include "ExpansionPortTypes.h"
#include "FileTypes.h"
#include "MemoryTypes.h"
#include "MessageQueueTypes.h"
#include "MouseTypes.h"
#include "SIDTypes.h"
#include "TODTypes.h"
#include "VICTypes.h"

//
// Enumerations
//

typedef enum
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

inline bool isC64Model(C64Model model) {
    return model >= C64_PAL && model <= C64_OLD_NTSC;
}

typedef enum
{
    STATE_OFF,
    STATE_PAUSED,
    STATE_RUNNING
}
EmulatorState;

typedef struct
{
    VICModel vic;
    bool grayDotBug;
    CIAModel cia;
    bool timerBBug;
    SIDModel sid;
    bool sidFilter;
    GlueLogic glue;
    RamInitPattern pattern;
}
C64Configuration;

// Configurations of standard C64 models
static const C64Configuration configurations[] = {
    
    // C64 PAL
    { PAL_6569_R3, false, MOS_6526, true, MOS_6581, true, GLUE_DISCRETE, INIT_PATTERN_C64 },
    
    // C64_II_PAL
    { PAL_8565, true, MOS_8521, false, MOS_8580, true, GLUE_CUSTOM_IC, INIT_PATTERN_C64C },
    
    // C64_OLD_PAL
    { PAL_6569_R1, false, MOS_6526, true, MOS_6581, true, GLUE_DISCRETE, INIT_PATTERN_C64 },

    // C64_NTSC
    { NTSC_6567, false, MOS_6526, false, MOS_6581, true, GLUE_DISCRETE, INIT_PATTERN_C64 },

    // C64_II_NTSC
    { NTSC_8562, true, MOS_8521, true, MOS_8580, true, GLUE_CUSTOM_IC, INIT_PATTERN_C64C },
    
    // C64_OLD_NTSC
    { NTSC_6567_R56A, false, MOS_6526, false, MOS_6581, true, GLUE_DISCRETE, INIT_PATTERN_C64 }
};

#endif
