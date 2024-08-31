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
/// @file

#pragma once

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// Emulator message
enum_long(MSG_TYPE)
{
    MSG_NONE,               ///< Unclassified

    // Execution state
    MSG_CONFIG,             ///< The configuration has changed
    MSG_POWER,              ///< The emulator has been powered on or off
    MSG_RUN,                ///< The emulator has entered STATE_RUNNING
    MSG_PAUSE,              ///< The emulator has entered STATE_PAUSED
    MSG_STEP,               ///< The emulator has stepped a single instruction
    MSG_RESET,              ///< The emulator has been reset
    MSG_SHUTDOWN,           ///< The emulator has been shut down
    MSG_ABORT,              ///< The emulator requests the GUI to quit the app
    MSG_WARP,               ///< The emulator has entered or exited warp mode
    MSG_TRACK,              ///< The emulator has entered or exited track mode
    MSG_MUTE,               ///< The emulator has been muted or unmuted

    // Retro shell
    MSG_RSH_CLOSE,          ///< RetroShell has been closed
    MSG_RSH_UPDATE,         ///< RetroShell has generated new output
    MSG_RSH_DEBUGGER,       ///< The RetroShell debugger has been opend or closed
    MSG_RSH_WAIT,           ///< Execution has peen postponed due to a wait command
    MSG_RSH_ERROR,          ///< Command execution has been aborted due to an error

    // CPU
    MSG_BREAKPOINT_UPDATED, ///< The breakpoint list has beed modified
    MSG_BREAKPOINT_REACHED, ///< A breakpoint has been reached
    MSG_WATCHPOINT_UPDATED, ///< The watchpoint list has beed modified
    MSG_WATCHPOINT_REACHED, ///< A watchpoint has been reached
    MSG_CPU_JUMPED,         ///< The program counter has been modified manually
    MSG_CPU_JAMMED,         ///< The CPU has halted due to an illegal instruction

    // VIC
    MSG_PAL,                ///< The emulator runs in PAL mode now
    MSG_NTSC,               ///< The emulator runs in NTSC mode now

    // Serial Port (IEC Bus)
    MSG_SER_BUSY,           ///< The IEC bus is transferring data
    MSG_SER_IDLE,           ///< The IEC bus has returned to idle state

    // Floppy drives
    MSG_DRIVE_CONNECT,      ///< A floppy drive has been connected or disconnected
    MSG_DRIVE_POWER,        ///< The emulator has been powered on
    MSG_DRIVE_POWER_SAVE,   ///< Fast paths have been enabled or disabled
    MSG_DRIVE_READ,         ///< The floopy drive has switched to read mode
    MSG_DRIVE_WRITE,        ///< The floppy drive has switched to write mode
    MSG_DRIVE_LED,          ///< The green drive LED has switched on or off
    MSG_DRIVE_MOTOR,        ///< The disk has started or stopped spinning
    MSG_DRIVE_STEP,         ///< The drive head has stepped
    MSG_DISK_INSERT,        ///< A disk has been inserted
    MSG_DISK_EJECT,         ///< A disk has been ejected
    MSG_DISK_PROTECTED,     ///< The write protection status has changed
    MSG_DISK_MODIFIED,      ///< The disk needs saving
    MSG_FILE_FLASHED,       ///< A file has been flashed into memory

    // Peripherals (Datasette)
    MSG_VC1530_CONNECT,     ///< A datasette has been connected or disconnected
    MSG_VC1530_TAPE,        ///< A tape has been inserted or ejected
    MSG_VC1530_PLAY,        ///< The play key has been pressed
    MSG_VC1530_MOTOR,       ///< The drive motor has started or stopped
    MSG_VC1530_COUNTER,     ///< The tape counter has stepped

    // Peripherals (Expansion port)
    MSG_CRT_ATTACHED,       ///< A cartridge has been plugged into the expansion port

    // Peripherals (Keyboard)
    MSG_KB_AUTO_PRESS,      ///< The auto-typing daemon has pressed a key
    MSG_KB_AUTO_RELEASE,    ///< The auto-typing daemon has released a key

    // Peripherals (Mouse)
    MSG_SHAKING,            ///< A shaking mouse has been detected

    // Snapshots
    MSG_SNAPSHOT_TAKEN,     ///< A snapshot has been taken (see OPT_C64_SNAP_AUTO)
    MSG_SNAPSHOT_RESTORED,  ///< A snapshot has been restored

    // Screen recording
    MSG_RECORDING_STARTED,  ///< The screen recorder has started
    MSG_RECORDING_STOPPED,  ///< The screen recorder has stopped
    MSG_RECORDING_ABORTED,  ///< Screen recording has been aborted

    // Debugging
    MSG_DMA_DEBUG,          ///< The DMA debugger has been started or stopped

    // Scheduled alarms
    MSG_ALARM,              ///< A user-set alarm event has fired
    MSG_RS232,              ///< RS232 activity (DEPRECATED)
    MSG_RS232_IN,           ///< RS232 adapter has received data
    MSG_RS232_OUT,          ///< RS232 adapter has sent data

    // Remote server
    MSG_SRV_STATE,
    MSG_SRV_RECEIVE,
    MSG_SRV_SEND
};

typedef MSG_TYPE MsgType;

struct MsgTypeEnum : util::Reflection<MsgTypeEnum, MsgType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = MSG_SRV_SEND;

    static const char *prefix() { return "MSG"; }
    static const char *_key(long value)
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

            case MSG_RSH_CLOSE:             return "RSH_CLOSE";
            case MSG_RSH_UPDATE:            return "RSH_UPDATE";
            case MSG_RSH_DEBUGGER:          return "RSH_DEBUGGER";
            case MSG_RSH_WAIT:              return "RSH_WAIT";
            case MSG_RSH_ERROR:             return "RSH_ERROR";
                
            case MSG_BREAKPOINT_UPDATED:    return "BREAKPOINT_UPDATED";
            case MSG_BREAKPOINT_REACHED:    return "BREAKPOINT_REACHED";
            case MSG_WATCHPOINT_UPDATED:    return "WATCHPOINT_UPDATED";
            case MSG_WATCHPOINT_REACHED:    return "WATCHPOINT_REACHED";
            case MSG_CPU_JUMPED:            return "CPU_JUMPED";
            case MSG_CPU_JAMMED:            return "CPU_JAMMED";

            case MSG_PAL:                   return "PAL";
            case MSG_NTSC:                  return "NTSC";

            case MSG_SER_BUSY:              return "SER_BUSY";
            case MSG_SER_IDLE:              return "SER_IDLE";

            case MSG_DRIVE_CONNECT:         return "DRIVE_CONNECT";
            case MSG_DRIVE_POWER:           return "DRIVE_POWER";
            case MSG_DRIVE_POWER_SAVE:      return "DRIVE_POWER_SAVE";
            case MSG_DRIVE_READ:            return "DRIVE_READ";
            case MSG_DRIVE_WRITE:           return "DRIVE_WRITE";
            case MSG_DRIVE_LED:             return "DRIVE_LED";
            case MSG_DRIVE_MOTOR:           return "DRIVE_MOTOR";
            case MSG_DRIVE_STEP:            return "DRIVE_STEP";
            case MSG_DISK_INSERT:           return "DISK_INSERT";
            case MSG_DISK_EJECT:            return "DISK_EJECT";
            case MSG_DISK_PROTECTED:        return "DISK_PROTECTED";
            case MSG_DISK_MODIFIED:         return "DISK_MODIFIED";
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

            case MSG_SNAPSHOT_TAKEN:        return "SNAPSHOT_TAKEN";
            case MSG_SNAPSHOT_RESTORED:     return "SNAPSHOT_RESTORED";

            case MSG_RECORDING_STARTED:     return "RECORDING_STARTED";
            case MSG_RECORDING_STOPPED:     return "RECORDING_STOPPED";
            case MSG_RECORDING_ABORTED:     return "RECORDING_ABORTED";

            case MSG_DMA_DEBUG:             return "DMA_DEBUG";

            case MSG_ALARM:                 return "ALARM";
            case MSG_RS232:                 return "RS232";
            case MSG_RS232_IN:              return "RS232_IN";
            case MSG_RS232_OUT:             return "RS232_OUT";

            case MSG_SRV_STATE:             return "SRV_STATE";
            case MSG_SRV_RECEIVE:           return "SRV_RECEIVE";
            case MSG_SRV_SEND:              return "SRV_SEND";
        }
        return "???";
    }
};

//
// Structures
//

typedef struct { u16 pc; } CpuMsg;
typedef struct { i16 nr; i16 value; i16 volume; i16 pan; } DriveMsg;
typedef struct { isize line; i16 delay; } ScriptMsg;
typedef struct { void *snapshot; } SnapshotMsg;

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
        SnapshotMsg snapshot;
    };
}
Message;


//
// Signatures
//

typedef void Callback(const void *, Message);

}
