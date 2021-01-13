// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "MsgQueuePublicTypes.h"
#include "Reflection.h"

//
// Reflection APIs
//

struct MSGTypeEnum : Reflection<MSGTypeEnum, MsgType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < MSG_COUNT;
    }

    static const char *prefix() { return "MSG"; }
    static const char *key(MsgType value)
    {
        switch (value) {
                
            case MSG_NONE:                 return "NONE";
                
            case MSG_REGISTER:             return "MSG_REGISTER";
            case MSG_UNREGISTER:           return "MSG_UNREGISTER";

            case MSG_CONFIG:               return "CONFIG";
            case MSG_POWER_ON:             return "POWER_ON";
            case MSG_POWER_OFF:            return "POWER_OFF";
            case MSG_RUN:                  return "RUN";
            case MSG_PAUSE:                return "PAUSE";
            case MSG_RESET:                return "RESET";
            case MSG_WARP_ON:              return "WARP_ON";
            case MSG_WARP_OFF:             return "WARP_OFF";
            case MSG_MUTE_ON:              return "MUTE_ON";
            case MSG_MUTE_OFF:             return "MUTE_OFF";
                
            case MSG_BASIC_ROM_LOADED:     return "BASIC_ROM_LOADED";
            case MSG_CHAR_ROM_LOADED:      return "CHAR_ROM_LOADED";
            case MSG_KERNAL_ROM_LOADED:    return "KERNAL_ROM_LOADED";
            case MSG_DRIVE_ROM_LOADED:     return "DRIVE_ROM_LOADED";
            case MSG_ROM_MISSING:          return "ROM_MISSING";
                
            case MSG_CPU_OK:               return "CPU_OK";
            case MSG_CPU_JAMMED:           return "CPU_JAMMED";
            case MSG_BREAKPOINT_REACHED:   return "BREAKPOINT_REACHED";
            case MSG_WATCHPOINT_REACHED:   return "WATCHPOINT_REACHED";
                
            case MSG_PAL:                  return "PAL";
            case MSG_NTSC:                 return "NTSC";
                
            case MSG_IEC_BUS_BUSY:         return "IEC_BUS_BUSY";
            case MSG_IEC_BUS_IDLE:         return "IEC_BUS_IDLE";
                
            case MSG_DRIVE_CONNECT:        return "DRIVE_CONNECT";
            case MSG_DRIVE_DISCONNECT:     return "DRIVE_DISCONNECT";
            case MSG_DRIVE_POWER_ON:       return "DRIVE_POWER_ON";
            case MSG_DRIVE_POWER_OFF:      return "DRIVE_POWER_OFF";
            case MSG_DRIVE_ACTIVE:         return "DRIVE_ACTIVE";
            case MSG_DRIVE_INACTIVE:       return "DRIVE_INACTIVE";
            case MSG_DRIVE_READ:           return "DRIVE_READ";
            case MSG_DRIVE_WRITE:          return "DRIVE_WRITE";
            case MSG_DRIVE_LED_ON:         return "DRIVE_LED_ON";
            case MSG_DRIVE_LED_OFF:        return "DRIVE_LED_OFF";
            case MSG_DRIVE_MOTOR_ON:       return "DRIVE_MOTOR_ON";
            case MSG_DRIVE_MOTOR_OFF:      return "DRIVE_MOTOR_OFF";
            case MSG_DRIVE_HEAD:           return "DRIVE_HEAD";
            case MSG_DISK_INSERTED:        return "DISK_INSERTED";
            case MSG_DISK_EJECTED:         return "DISK_EJECTED";
            case MSG_DISK_SAVED:           return "DISK_SAVED";
            case MSG_DISK_UNSAVED:         return "DISK_UNSAVED";
            case MSG_DISK_PROTECT:         return "DISK_PROTECT";
            case MSG_FILE_FLASHED:         return "FILE_FLASHED";
                
            case MSG_VC1530_TAPE:          return "VC1530_TAPE";
            case MSG_VC1530_NO_TAPE:       return "VC1530_NO_TAPE";
            case MSG_VC1530_PROGRESS:      return "VC1530_PROGRESS";
                
            case MSG_CRT_UNSUPPORTED:      return "CRT_UNSUPPORTED";
            case MSG_CRT_ATTACHED:         return "CRT_ATTACHED";
            case MSG_CRT_DETACHED:         return "CRT_DETACHED";
            case MSG_CART_SWITCH:          return "CART_SWITCH";
                
            case MSG_KB_AUTO_RELEASE:      return "KB_AUTO_RELEASE";

            case MSG_SNAPSHOT_TOO_OLD:     return "SNAPSHOT_TOO_OLD";
            case MSG_SNAPSHOT_TOO_NEW:     return "SNAPSHOT_TOO_NEW";
            case MSG_AUTO_SNAPSHOT_TAKEN:  return "AUTO_SNAPSHOT_TAKEN";
            case MSG_USER_SNAPSHOT_TAKEN:  return "USER_SNAPSHOT_TAKEN";
            case MSG_SNAPSHOT_RESTORED:    return "SNAPSHOT_RESTORED";
                
            case MSG_COUNT:                return "???";
        }
        return "???";
    }
};
