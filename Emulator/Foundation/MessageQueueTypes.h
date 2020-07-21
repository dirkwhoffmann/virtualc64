// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// This file must conform to standard ANSI-C to be compatible with Swift.

#ifndef _MESSAGE_QUEUE_T_H
#define _MESSAGE_QUEUE_T_H

typedef enum
{
    MSG_NONE = 0,
    
    // Emulator state
    MSG_CONFIG,
    MSG_POWER_ON,
    MSG_POWER_OFF,
    MSG_RUN,
    MSG_PAUSE,
    MSG_RESET,

    // ROM and snapshot handling
    MSG_BASIC_ROM_LOADED,
    MSG_CHAR_ROM_LOADED,
    MSG_KERNAL_ROM_LOADED,
    MSG_DRIVE_ROM_LOADED,
    MSG_ROM_MISSING,
    MSG_SNAPSHOT_TAKEN,

    // CPU related messages
    MSG_CPU_OK,
    MSG_CPU_SOFT_BREAKPOINT_REACHED,
    MSG_CPU_HARD_BREAKPOINT_REACHED,
    MSG_CPU_ILLEGAL_INSTRUCTION,
    MSG_WARP_ON,
    MSG_WARP_OFF,

    // VIC related messages
    MSG_PAL,
    MSG_NTSC,

    // IEC Bus
    MSG_IEC_BUS_BUSY,
    MSG_IEC_BUS_IDLE,
    
    // Peripherals (Disk drive)
    MSG_DRIVE_CONNECT,
    MSG_DRIVE_DISCONNECT,
    MSG_DRIVE_READ,
    MSG_DRIVE_WRITE,
    MSG_DRIVE_LED_ON,
    MSG_DRIVE_LED_OFF,
    MSG_DRIVE_DISK,
    MSG_DRIVE_NO_DISK,
    MSG_DRIVE_MOTOR_ON,
    MSG_DRIVE_MOTOR_OFF,
    MSG_DRIVE_HEAD,

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
}
MessageType;

typedef struct
{
    MessageType type;
    long data;
}
Message;

// Callback function signature
typedef void Callback(const void *, int, long);

#endif 
