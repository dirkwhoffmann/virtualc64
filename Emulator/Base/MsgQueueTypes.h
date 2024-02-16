// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(MSG_TYPE)
{
    MSG_NONE = 0,

    // Emulator state
    MSG_CONFIG,
    MSG_POWER,
    MSG_RUN,
    MSG_PAUSE,
    MSG_STEP,
    MSG_RESET,
    MSG_SHUTDOWN,
    MSG_ABORT,
    MSG_WARP,
    MSG_TRACK,
    MSG_MUTE,

    // Retro shell
    MSG_CONSOLE_CLOSE,
    MSG_CONSOLE_UPDATE,
    MSG_CONSOLE_DEBUGGER,
    MSG_SCRIPT_DONE,
    MSG_SCRIPT_PAUSE,
    MSG_SCRIPT_ABORT,
    MSG_SCRIPT_WAKEUP,

    // ROMs
    MSG_BASIC_ROM_LOADED,
    MSG_CHAR_ROM_LOADED,
    MSG_KERNAL_ROM_LOADED,
    MSG_DRIVE_ROM_LOADED,
    MSG_ROM_MISSING,

    // CPU
    MSG_CPU_JAMMED,
    MSG_CPU_JUMPED,
    MSG_BREAKPOINT_REACHED,
    MSG_WATCHPOINT_REACHED,

    // VIC
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
    MSG_DRIVE_POWER_SAVE_ON,
    MSG_DRIVE_POWER_SAVE_OFF,
    MSG_DRIVE_READ,
    MSG_DRIVE_WRITE,
    MSG_DRIVE_LED_ON,
    MSG_DRIVE_LED_OFF,
    MSG_DRIVE_MOTOR_ON,
    MSG_DRIVE_MOTOR_OFF,
    MSG_DRIVE_STEP,
    MSG_DISK_INSERT,
    MSG_DISK_EJECT,
    MSG_DISK_SAVED,
    MSG_DISK_UNSAVED,
    MSG_DISK_PROTECT,
    MSG_FILE_FLASHED,

    // Peripherals (Datasette)
    MSG_VC1530_CONNECT,
    MSG_VC1530_TAPE,
    MSG_VC1530_PLAY,
    MSG_VC1530_MOTOR,
    MSG_VC1530_COUNTER,

    // Peripherals (Expansion port)
    MSG_CRT_ATTACHED,

    // Peripherals (Keyboard)
    MSG_KB_AUTO_PRESS,
    MSG_KB_AUTO_RELEASE,
    
    // Peripherals (Mouse)
    MSG_SHAKING,
    
    // Snapshots
    MSG_AUTO_SNAPSHOT_TAKEN,
    MSG_USER_SNAPSHOT_TAKEN,
    MSG_SNAPSHOT_RESTORED,
    
    // Screen recording
    MSG_RECORDING_STARTED,
    MSG_RECORDING_STOPPED,
    MSG_RECORDING_ABORTED,

    // Debugging
    MSG_DMA_DEBUG,

    // Scheduled alarms
    MSG_ALARM
};
typedef MSG_TYPE MsgType;

#ifdef __cplusplus
struct MsgTypeEnum : util::Reflection<MsgType, MsgType> {
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = MSG_ALARM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "MSG"; }
    static const char *key(MsgType value)
    {
        switch (value) {
                
            case MSG_NONE:                  return "NONE";

            case MSG_CONFIG:                return "CONFIG";
            case MSG_POWER:                 return "POWER";
            case MSG_RUN:                   return "RUN";
            case MSG_PAUSE:                 return "PAUSE";
            case MSG_RESET:                 return "RESET";
            case MSG_SHUTDOWN:              return "SHUTDOWN";
            case MSG_ABORT:                 return "ABORT";
            case MSG_WARP:                  return "WARP";
            case MSG_TRACK:                 return "TRACK";
            case MSG_MUTE:                  return "MUTE";

            case MSG_CONSOLE_CLOSE:         return "CONSOLE_CLOSE";
            case MSG_CONSOLE_UPDATE:        return "CONSOLE_UPDATE";
            case MSG_CONSOLE_DEBUGGER:      return "CONSOLE_DEBUGGER";
            case MSG_SCRIPT_DONE:           return "SCRIPT_DONE";
            case MSG_SCRIPT_PAUSE:          return "SCRIPT_PAUSE";
            case MSG_SCRIPT_ABORT:          return "SCRIPT_ABORT";
            case MSG_SCRIPT_WAKEUP:         return "SCRIPT_WAKEUP";

            case MSG_BASIC_ROM_LOADED:      return "BASIC_ROM_LOADED";
            case MSG_CHAR_ROM_LOADED:       return "CHAR_ROM_LOADED";
            case MSG_KERNAL_ROM_LOADED:     return "KERNAL_ROM_LOADED";
            case MSG_DRIVE_ROM_LOADED:      return "DRIVE_ROM_LOADED";
            case MSG_ROM_MISSING:           return "ROM_MISSING";
                
            case MSG_CPU_JAMMED:            return "CPU_JAMMED";
            case MSG_BREAKPOINT_REACHED:    return "BREAKPOINT_REACHED";
            case MSG_WATCHPOINT_REACHED:    return "WATCHPOINT_REACHED";
                
            case MSG_PAL:                   return "PAL";
            case MSG_NTSC:                  return "NTSC";
                
            case MSG_IEC_BUS_BUSY:          return "IEC_BUS_BUSY";
            case MSG_IEC_BUS_IDLE:          return "IEC_BUS_IDLE";
                
            case MSG_DRIVE_CONNECT:         return "DRIVE_CONNECT";
            case MSG_DRIVE_DISCONNECT:      return "DRIVE_DISCONNECT";
            case MSG_DRIVE_POWER_ON:        return "DRIVE_POWER_ON";
            case MSG_DRIVE_POWER_OFF:       return "DRIVE_POWER_OFF";
            case MSG_DRIVE_POWER_SAVE_ON:   return "DRIVE_POWER_SAVE_ON";
            case MSG_DRIVE_POWER_SAVE_OFF:  return "DRIVE_POWER_SAVE_OFF";
            case MSG_DRIVE_READ:            return "DRIVE_READ";
            case MSG_DRIVE_WRITE:           return "DRIVE_WRITE";
            case MSG_DRIVE_LED_ON:          return "DRIVE_LED_ON";
            case MSG_DRIVE_LED_OFF:         return "DRIVE_LED_OFF";
            case MSG_DRIVE_MOTOR_ON:        return "DRIVE_MOTOR_ON";
            case MSG_DRIVE_MOTOR_OFF:       return "DRIVE_MOTOR_OFF";
            case MSG_DRIVE_STEP:            return "DRIVE_STEP";
            case MSG_DISK_INSERT:           return "DISK_INSERT";
            case MSG_DISK_EJECT:            return "DISK_EJECT";
            case MSG_DISK_SAVED:            return "DISK_SAVED";
            case MSG_DISK_UNSAVED:          return "DISK_UNSAVED";
            case MSG_DISK_PROTECT:          return "DISK_PROTECT";
            case MSG_FILE_FLASHED:          return "FILE_FLASHED";

            case MSG_VC1530_CONNECT:        return "VC1530_CONNECT";
            case MSG_VC1530_TAPE:           return "VC1530_TAPE";
            case MSG_VC1530_PLAY:           return "VC1530_PLAY";
            case MSG_VC1530_MOTOR:          return "VC1530_MOTOR";
            case MSG_VC1530_COUNTER:        return "VC1530_COUNTER";
                
            case MSG_CRT_ATTACHED:          return "CRT_ATTACHED";
                
            case MSG_KB_AUTO_PRESS:         return "KB_AUTO_PRESS";
            case MSG_KB_AUTO_RELEASE:       return "KB_AUTO_RELEASE";

            case MSG_SHAKING:               return "SHAKING";
                
            case MSG_AUTO_SNAPSHOT_TAKEN:   return "AUTO_SNAPSHOT_TAKEN";
            case MSG_USER_SNAPSHOT_TAKEN:   return "USER_SNAPSHOT_TAKEN";
            case MSG_SNAPSHOT_RESTORED:     return "SNAPSHOT_RESTORED";
                
            case MSG_RECORDING_STARTED:     return "RECORDING_STARTED";
            case MSG_RECORDING_STOPPED:     return "RECORDING_STOPPED";
            case MSG_RECORDING_ABORTED:     return "RECORDING_ABORTED";

            case MSG_DMA_DEBUG:             return "DMA_DEBUG";

            case MSG_ALARM:                 return "ALARM";
        }
        return "???";
    }
};
#endif


//
// Structures
//

typedef struct { u16 pc; } CpuMsg;
typedef struct { i16 nr; i16 value; i16 volume; i16 pan; } DriveMsg;
typedef struct { isize line; i16 delay; } ScriptMsg;

typedef struct
{
    // Header
    MsgType type;

    // Payload
    union {
        i64 value;
        CpuMsg cpu;
        DriveMsg drive;
        ScriptMsg script;
    };
}
Message;


//
// Signatures
//

typedef void Callback(const void *, Message);
