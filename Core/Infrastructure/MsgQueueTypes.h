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
enum class Msg : long
{
    NONE,               ///< Unclassified

    // Execution state
    CONFIG,             ///< The configuration has changed
    POWER,              ///< The emulator has been powered on or off
    RUN,                ///< The emulator has entered ExecState::RUNNING
    PAUSE,              ///< The emulator has entered ExecState::PAUSED
    STEP,               ///< The emulator has stepped a single instruction
    EOL_TRAP,           ///< The emulator has been paused at the end of the current line
    EOF_TRAP,           ///< The emulator has been paused at the end of the current frame
    RESET,              ///< The emulator has been reset
    SHUTDOWN,           ///< The emulator has been shut down
    ABORT,              ///< The emulator requests the GUI to quit the app
    WARP,               ///< The emulator has entered or exited warp mode
    TRACK,              ///< The emulator has entered or exited track mode
    MUTE,               ///< The emulator has been muted or unmuted

    // Retro shell
    RSH_CLOSE,          ///< RetroShell has been closed
    RSH_UPDATE,         ///< RetroShell has generated new output
    RSH_DEBUGGER,       ///< The RetroShell debugger has been opend or closed
    RSH_WAIT,           ///< Execution has peen postponed due to a wait command
    RSH_ERROR,          ///< Command execution has been aborted due to an error

    // CPU
    BREAKPOINT_UPDATED, ///< The breakpoint list has beed modified
    BREAKPOINT_REACHED, ///< A breakpoint has been reached
    WATCHPOINT_UPDATED, ///< The watchpoint list has beed modified
    WATCHPOINT_REACHED, ///< A watchpoint has been reached
    CPU_JUMPED,         ///< The program counter has been modified manually
    CPU_JAMMED,         ///< The CPU has halted due to an illegal instruction

    // VIC
    PAL,                ///< The emulator runs in PAL mode now
    NTSC,               ///< The emulator runs in NTSC mode now

    // Serial Port (IEC Bus)
    SER_BUSY,           ///< The IEC bus is transferring data
    SER_IDLE,           ///< The IEC bus has returned to idle state

    // Monitor
    MON_SETTING,        ///< The monitor configuration has changed
    
    // Floppy drives
    DRIVE_CONNECT,      ///< A floppy drive has been connected or disconnected
    DRIVE_POWER,        ///< The emulator has been powered on
    DRIVE_POWER_SAVE,   ///< Fast paths have been enabled or disabled
    DRIVE_READ,         ///< The floopy drive has switched to read mode
    DRIVE_WRITE,        ///< The floppy drive has switched to write mode
    DRIVE_LED,          ///< The green drive LED has switched on or off
    DRIVE_MOTOR,        ///< The disk has started or stopped spinning
    DRIVE_STEP,         ///< The drive head has stepped
    DISK_INSERT,        ///< A disk has been inserted
    DISK_EJECT,         ///< A disk has been ejected
    DISK_PROTECTED,     ///< The write protection status has changed
    DISK_MODIFIED,      ///< The disk needs saving
    FILE_FLASHED,       ///< A file has been flashed into memory

    // Datasette
    VC1530_CONNECT,     ///< A datasette has been connected or disconnected
    VC1530_TAPE,        ///< A tape has been inserted or ejected
    VC1530_PLAY,        ///< The play key has been pressed
    VC1530_MOTOR,       ///< The drive motor has started or stopped
    VC1530_COUNTER,     ///< The tape counter has stepped

    // Expansion port
    CRT_ATTACHED,       ///< A cartridge has been plugged into the expansion port

    // Keyboard
    KB_AUTO_PRESS,      ///< The auto-typing daemon has pressed a key
    KB_AUTO_RELEASE,    ///< The auto-typing daemon has released a key

    // Mouse
    SHAKING,            ///< A shaking mouse has been detected

    // Snapshots
    SNAPSHOT_TAKEN,     ///< A snapshot has been taken (see OPT_SNAP_AUTO)
    SNAPSHOT_RESTORED,  ///< A snapshot has been restored

    // Screen recording
    RECORDING_STARTED,  ///< The screen recorder has started
    RECORDING_STOPPED,  ///< The screen recorder has stopped
    RECORDING_ABORTED,  ///< Screen recording has been aborted

    // Debugging
    DMA_DEBUG,          ///< The DMA debugger has been started or stopped

    // Scheduled alarms
    ALARM,              ///< A user-set alarm event has fired
    RS232,              ///< RS232 activity (DEPRECATED)
    RS232_IN,           ///< RS232 adapter has received data
    RS232_OUT,          ///< RS232 adapter has sent data

    // Remote server
    SRV_STATE,
    SRV_RECEIVE,
    SRV_SEND
};

struct MsgEnum : Reflection<MsgEnum, Msg> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Msg::SRV_SEND);

    static const char *_key(Msg value)
    {
        switch (value) {

            case Msg::NONE:                  return "NONE";

            case Msg::CONFIG:                return "CONFIG";
            case Msg::POWER:                 return "POWER";
            case Msg::RUN:                   return "RUN";
            case Msg::PAUSE:                 return "PAUSE";
            case Msg::STEP:                  return "STEP";
            case Msg::EOL_TRAP:              return "EOL_TRAP";
            case Msg::EOF_TRAP:              return "EOF_TRAP";
            case Msg::RESET:                 return "RESET";
            case Msg::SHUTDOWN:              return "SHUTDOWN";
            case Msg::ABORT:                 return "ABORT";
            case Msg::WARP:                  return "WARP";
            case Msg::TRACK:                 return "TRACK";
            case Msg::MUTE:                  return "MUTE";

            case Msg::RSH_CLOSE:             return "RSH_CLOSE";
            case Msg::RSH_UPDATE:            return "RSH_UPDATE";
            case Msg::RSH_DEBUGGER:          return "RSH_DEBUGGER";
            case Msg::RSH_WAIT:              return "RSH_WAIT";
            case Msg::RSH_ERROR:             return "RSH_ERROR";
                
            case Msg::BREAKPOINT_UPDATED:    return "BREAKPOINT_UPDATED";
            case Msg::BREAKPOINT_REACHED:    return "BREAKPOINT_REACHED";
            case Msg::WATCHPOINT_UPDATED:    return "WATCHPOINT_UPDATED";
            case Msg::WATCHPOINT_REACHED:    return "WATCHPOINT_REACHED";
            case Msg::CPU_JUMPED:            return "CPU_JUMPED";
            case Msg::CPU_JAMMED:            return "CPU_JAMMED";

            case Msg::PAL:                   return "PAL";
            case Msg::NTSC:                  return "NTSC";

            case Msg::SER_BUSY:              return "SER_BUSY";
            case Msg::SER_IDLE:              return "SER_IDLE";

            case Msg::MON_SETTING:           return "MON_SETTING";

            case Msg::DRIVE_CONNECT:         return "DRIVE_CONNECT";
            case Msg::DRIVE_POWER:           return "DRIVE_POWER";
            case Msg::DRIVE_POWER_SAVE:      return "DRIVE_POWER_SAVE";
            case Msg::DRIVE_READ:            return "DRIVE_READ";
            case Msg::DRIVE_WRITE:           return "DRIVE_WRITE";
            case Msg::DRIVE_LED:             return "DRIVE_LED";
            case Msg::DRIVE_MOTOR:           return "DRIVE_MOTOR";
            case Msg::DRIVE_STEP:            return "DRIVE_STEP";
            case Msg::DISK_INSERT:           return "DISK_INSERT";
            case Msg::DISK_EJECT:            return "DISK_EJECT";
            case Msg::DISK_PROTECTED:        return "DISK_PROTECTED";
            case Msg::DISK_MODIFIED:         return "DISK_MODIFIED";
            case Msg::FILE_FLASHED:          return "FILE_FLASHED";

            case Msg::VC1530_CONNECT:        return "VC1530_CONNECT";
            case Msg::VC1530_TAPE:           return "VC1530_TAPE";
            case Msg::VC1530_PLAY:           return "VC1530_PLAY";
            case Msg::VC1530_MOTOR:          return "VC1530_MOTOR";
            case Msg::VC1530_COUNTER:        return "VC1530_COUNTER";

            case Msg::CRT_ATTACHED:          return "CRT_ATTACHED";

            case Msg::KB_AUTO_PRESS:         return "KB_AUTO_PRESS";
            case Msg::KB_AUTO_RELEASE:       return "KB_AUTO_RELEASE";

            case Msg::SHAKING:               return "SHAKING";

            case Msg::SNAPSHOT_TAKEN:        return "SNAPSHOT_TAKEN";
            case Msg::SNAPSHOT_RESTORED:     return "SNAPSHOT_RESTORED";

            case Msg::RECORDING_STARTED:     return "RECORDING_STARTED";
            case Msg::RECORDING_STOPPED:     return "RECORDING_STOPPED";
            case Msg::RECORDING_ABORTED:     return "RECORDING_ABORTED";

            case Msg::DMA_DEBUG:             return "DMA_DEBUG";

            case Msg::ALARM:                 return "ALARM";
            case Msg::RS232:                 return "RS232";
            case Msg::RS232_IN:              return "RS232_IN";
            case Msg::RS232_OUT:             return "RS232_OUT";

            case Msg::SRV_STATE:             return "SRV_STATE";
            case Msg::SRV_RECEIVE:           return "SRV_RECEIVE";
            case Msg::SRV_SEND:              return "SRV_SEND";
        }
        return "???";
    }
    
    static const char *help(Msg value)
    {
        return "";
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
    Msg type;

    // Payload
    union {
        struct { i64 value; i64 value2; };
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
