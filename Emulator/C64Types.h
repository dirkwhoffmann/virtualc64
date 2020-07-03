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

#include "CPUTypes.h"
#include "MemoryTypes.h"
#include "TODTypes.h"
#include "CIATypes.h"
#include "VICTypes.h"
#include "SIDTypes.h"
#include "ControlPortTypes.h"
#include "ExpansionPortTypes.h"
#include "CartridgeTypes.h"
#include "DriveTypes.h"
#include "DiskTypes.h"
#include "MouseTypes.h"
#include "FileTypes.h"

//! @brief    C64 model
typedef enum {
    C64_PAL,
    C64_II_PAL,
    C64_OLD_PAL,
    C64_NTSC,
    C64_II_NTSC,
    C64_OLD_NTSC,
    C64_CUSTOM
} C64Model;

inline bool isC64Model(C64Model model) {
    return model >= C64_PAL && model <= C64_OLD_NTSC;
}

//! @brief    C64 configuration
typedef struct {
    VICModel vic;
    bool grayDotBug;
    CIAModel cia;
    bool timerBBug;
    SIDModel sid;
    bool sidFilter;
    GlueLogic glue;
    RamInitPattern pattern;
} C64Configuration;

//! @brief    Configurations of standard C64 models
/*! @note     sidFilter should be true for all known configurations. We have
 *            disabled them by default, because the filter emulation is broken
 *            in the currently used reSID code. Once the reSID bug has been
 *            fixed, it should be set to true again.
 */
static const C64Configuration configurations[] = {
    
    // C64 PAL
    { PAL_6569_R3, false, MOS_6526, true, MOS_6581, false, GLUE_DISCRETE, INIT_PATTERN_C64 },
    
    // C64_II_PAL
    { PAL_8565, true, MOS_8521, false, MOS_8580, false, GLUE_CUSTOM_IC, INIT_PATTERN_C64C },
    
    // C64_OLD_PAL
    { PAL_6569_R1, false, MOS_6526, true, MOS_6581, false, GLUE_DISCRETE, INIT_PATTERN_C64 },

    // C64_NTSC
    { NTSC_6567, false, MOS_6526, false, MOS_6581, false, GLUE_DISCRETE, INIT_PATTERN_C64 },

    // C64_II_NTSC
    { NTSC_8562, true, MOS_8521, true, MOS_8580, false, GLUE_CUSTOM_IC, INIT_PATTERN_C64C },
    
    // C64_OLD_NTSC
    { NTSC_6567_R56A, false, MOS_6526, false, MOS_6581, false, GLUE_DISCRETE, INIT_PATTERN_C64 }
};

/*! @brief    Message types
 *  @details  List of all possible message id's
 */
typedef enum {
    
    MSG_NONE = 0,
    
    // Running the emulator
    MSG_READY_TO_RUN,
    MSG_RUN,
    MSG_HALT,

    // ROM and snapshot handling
    MSG_BASIC_ROM_LOADED,
    MSG_CHAR_ROM_LOADED,
    MSG_KERNAL_ROM_LOADED,
    MSG_VC1541_ROM_LOADED,
    MSG_ROM_MISSING,
    MSG_SNAPSHOT_TAKEN,

    // CPU related messages
    MSG_CPU_OK,
    MSG_CPU_SOFT_BREAKPOINT_REACHED,
    MSG_CPU_HARD_BREAKPOINT_REACHED,
    MSG_CPU_ILLEGAL_INSTRUCTION,
    MSG_WARP_ON,
    MSG_WARP_OFF,
    MSG_ALWAYS_WARP_ON,
    MSG_ALWAYS_WARP_OFF,

    // VIC related messages
    MSG_PAL,
    MSG_NTSC,

    // IEC Bus
    MSG_IEC_BUS_BUSY,
    MSG_IEC_BUS_IDLE,

    // Keyboard
    MSG_KEYMATRIX,
    MSG_CHARSET,
    
    // Peripherals (Disk drive)
    MSG_VC1541_ATTACHED,
    MSG_VC1541_ATTACHED_SOUND,
    MSG_VC1541_DETACHED,
    MSG_VC1541_DETACHED_SOUND,
    MSG_VC1541_DISK,
    MSG_VC1541_DISK_SOUND,
    MSG_VC1541_NO_DISK,
    MSG_VC1541_NO_DISK_SOUND,
    MSG_VC1541_RED_LED_ON,
    MSG_VC1541_RED_LED_OFF,
    MSG_VC1541_MOTOR_ON,
    MSG_VC1541_MOTOR_OFF,
    MSG_VC1541_HEAD_UP,
    MSG_VC1541_HEAD_UP_SOUND,
    MSG_VC1541_HEAD_DOWN,
    MSG_VC1541_HEAD_DOWN_SOUND,

    // Peripherals (Disk)
    MSG_DISK_SAVED,
    MSG_DISK_UNSAVED,
    
    // Peripherals (Datasette)
    MSG_VC1530_TAPE,
    MSG_VC1530_NO_TAPE,
    MSG_VC1530_PROGRESS,

    // Peripherals (Expansion port)
    MSG_CARTRIDGE,
    MSG_NO_CARTRIDGE,
    MSG_CART_SWITCH

} MessageType;

/*! @brief    Message
 *  @details  Each message consists of type and a data field.
 */
typedef struct {
    MessageType type;
    long data;
} Message;

//! @brief    Callback function signature
typedef void Callback(const void *, int, long);

#endif
