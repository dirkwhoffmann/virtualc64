//  Created by Dirk Hoffmann

#ifndef C64_TYPES_H
#define C64_TYPES_H

#include "CPU_types.h"
#include "Memory_types.h"
#include "TOD_types.h"
#include "CIA_types.h"
#include "VIC_types.h"
#include "SID_types.h"
#include "ControlPort_types.h"
#include "Cartridge_types.h"
#include "Disk_types.h"
#include "Mouse_types.h"

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
    MSG_VC1541_DATA_ON,
    MSG_VC1541_DATA_OFF,
    MSG_VC1541_MOTOR_ON,
    MSG_VC1541_MOTOR_OFF,
    MSG_VC1541_HEAD_UP,
    MSG_VC1541_HEAD_UP_SOUND,
    MSG_VC1541_HEAD_DOWN,
    MSG_VC1541_HEAD_DOWN_SOUND,

    // Peripherals (Datasette)
    MSG_VC1530_TAPE,
    MSG_VC1530_NO_TAPE,
    MSG_VC1530_PROGRESS,

    // Peripherals (Expansion port)
    MSG_CARTRIDGE,
    MSG_NO_CARTRIDGE

} VC64Message;

#endif
