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

#include "Aliases.h"
#include "Reflection.hpp"

#include "MsgQueueTypes.h"

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
            case MSG_SCRIPT_ABORT:          return "SCRIPT_ABORT";

            case MSG_BREAKPOINT_UPDATED:    return "BREAKPOINT_UPDATED";
            case MSG_BREAKPOINT_REACHED:    return "BREAKPOINT_REACHED";
            case MSG_WATCHPOINT_UPDATED:    return "WATCHPOINT_UPDATED";
            case MSG_WATCHPOINT_REACHED:    return "WATCHPOINT_REACHED";
            case MSG_CPU_JUMPED:            return "CPU_JUMPED";
            case MSG_CPU_JAMMED:            return "CPU_JAMMED";

            case MSG_PAL:                   return "PAL";
            case MSG_NTSC:                  return "NTSC";

            case MSG_IEC_BUS_BUSY:          return "IEC_BUS_BUSY";
            case MSG_IEC_BUS_IDLE:          return "IEC_BUS_IDLE";

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
