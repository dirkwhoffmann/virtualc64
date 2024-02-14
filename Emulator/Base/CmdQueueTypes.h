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

enum_long(CMD_TYPE)
{
    CMD_NONE = 0,

    // Emulator
    CMD_POWER_ON,
    CMD_POWER_OFF,
    CMD_RUN,
    CMD_PAUSE,
    CMD_SUSPEND,
    CMD_RESUME,
    CMD_HALT,

    // C64
    CMD_BRK,
    CMD_SNAPSHOT_AUTO,
    CMD_SNAPSHOT_USER,
    CMD_ALARM_ABS,
    CMD_ALARM_REL,

    // Keyboard
    CMD_KEY_PRESS,
    CMD_KEY_RELEASE,
    CMD_KEY_TOGGLE,

    // Datasette
    CMD_DATASETTE_INSERT,
    CMD_DATASETTE_EJECT,
    CMD_DATASETTE_PLAY,
    CMD_DATASETTE_STOP,
    CMD_DATASETTE_REWIND,
};
typedef CMD_TYPE CmdType;

#ifdef __cplusplus
struct CmdTypeEnum : util::Reflection<CmdType, CmdType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CMD_DATASETTE_REWIND;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CMD"; }
    static const char *key(CmdType value)
    {
        switch (value) {

            case CMD_NONE:                  return "NONE";

            case CMD_POWER_ON:              return "POWER_ON";
            case CMD_POWER_OFF:             return "POWER_OFF";
            case CMD_RUN:                   return "RUN";
            case CMD_PAUSE:                 return "PAUSE";
            case CMD_SUSPEND:               return "SUSPEND";
            case CMD_RESUME:                return "RESUME";
            case CMD_HALT:                  return "HALT";

            case CMD_BRK:                   return "BRK";
            case CMD_SNAPSHOT_AUTO:         return "SNAPSHOT_AUTO";
            case CMD_SNAPSHOT_USER:         return "SNAPSHOT_USER";
            case CMD_ALARM_ABS:             return "ALARM_ABS";
            case CMD_ALARM_REL:             return "ALARM_REL";

            case CMD_KEY_PRESS:             return "KEY_PRESS";
            case CMD_KEY_RELEASE:           return "KEY_RELEASE";
            case CMD_KEY_TOGGLE:            return "KEY_TOGGLE";

            case CMD_DATASETTE_INSERT:      return "DATASETTE_INSERT";
            case CMD_DATASETTE_EJECT:       return "DATASETTE_EJECT";
            case CMD_DATASETTE_PLAY:        return "DATASETTE_PLAY";
            case CMD_DATASETTE_STOP:        return "DATASETTE_STOP";
            case CMD_DATASETTE_REWIND:      return "DATASETTE_REWIND";
        }
        return "???";
    }
};
#endif


//
// Structures
//

typedef struct
{
    u8 keycode;
    double delay;
}
KeyCmd;

typedef struct
{
    void *tape;
}
TapeCmd;

typedef struct
{
    i64 cycle;
    i64 value;
}
AlarmCmd;

typedef struct
{
    // Header
    CmdType type;

    // Payload
    union {
        
        i64 value;
        KeyCmd key;
        TapeCmd tape;
        AlarmCmd alarm;
    };
}
Cmd;
