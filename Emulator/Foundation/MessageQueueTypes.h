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

enum_long(MSG)
{
    MSG_NONE = 0,
    
    // Emulator state
    MSG_CONFIG,
    MSG_POWER_ON,
    MSG_POWER_OFF,
    MSG_RUN,
    MSG_PAUSE,
    MSG_RESET,
    MSG_WARP_ON,
    MSG_WARP_OFF,
    MSG_MUTE_ON,
    MSG_MUTE_OFF,

    // ROMs
    MSG_BASIC_ROM_LOADED,
    MSG_CHAR_ROM_LOADED,
    MSG_KERNAL_ROM_LOADED,
    MSG_DRIVE_ROM_LOADED,
    MSG_ROM_MISSING,

    // CPU related messages
    MSG_CPU_OK,
    MSG_CPU_JAMMED,
    MSG_BREAKPOINT_REACHED,
    MSG_WATCHPOINT_REACHED,

    // VIC related messages
    MSG_PAL,
    MSG_NTSC,

    // IEC Bus
    MSG_IEC_BUS_BUSY,
    MSG_IEC_BUS_IDLE,
    
    // Floppy drives
    MSG_DRIVE_CONNECT,
    MSG_DRIVE_DISCONNECT,
    MSG_DRIVE_POWER_ON,
    MSG_DRIVE_POWER_OFF,
    MSG_DRIVE_ACTIVE,
    MSG_DRIVE_INACTIVE,
    MSG_DRIVE_READ,
    MSG_DRIVE_WRITE,
    MSG_DRIVE_LED_ON,
    MSG_DRIVE_LED_OFF,
    MSG_DRIVE_MOTOR_ON,
    MSG_DRIVE_MOTOR_OFF,
    MSG_DRIVE_HEAD,
    MSG_DISK_INSERTED,
    MSG_DISK_EJECTED,
    MSG_DISK_SAVED,
    MSG_DISK_UNSAVED,
    MSG_DISK_PROTECT,
    MSG_FILE_FLASHED,

    // Peripherals (Datasette)
    MSG_VC1530_TAPE,
    MSG_VC1530_NO_TAPE,
    MSG_VC1530_PROGRESS,

    // Peripherals (Expansion port)
    MSG_CARTRIDGE,
    MSG_NO_CARTRIDGE,
    MSG_CART_SWITCH,

    // Peripherals (Keyboard)
    MSG_KB_AUTO_RELEASE,
    
    // Snapshots
    MSG_AUTO_SNAPSHOT_TAKEN,
    MSG_USER_SNAPSHOT_TAKEN,
    MSG_SNAPSHOT_RESTORED
};

typedef struct
{
    MSG type;
    long data;
}
Message;

// Callback function signature
typedef void Callback(const void *, long, long);

#endif 
