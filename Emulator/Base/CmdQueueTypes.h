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
#include "JoystickTypes.h"

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

    // Mice
    CMD_MOUSE_MOVE_ABS,
    CMD_MOUSE_MOVE_REL,
    CMD_MOUSE_EVENT,

    // Joysticks
    CMD_JOY_EVENT,

    // Floppy disks
    /*
    CMD_DSK_PROTECT,
    CMD_DSK_UNPROTECT,
    */
    CMD_DSK_TOGGLE_WP,
    CMD_DSK_MODIFIED,
    CMD_DSK_UNMODIFIED,

    // Datasette
    CMD_DATASETTE_PLAY,
    CMD_DATASETTE_STOP,
    CMD_DATASETTE_REWIND,

    // Cartridges
    CMD_CRT_BUTTON_PRESS,
    CMD_CRT_BUTTON_RELEASE,
    CMD_CRT_SWITCH_LEFT,
    CMD_CRT_SWITCH_NEUTRAL,
    CMD_CRT_SWITCH_RIGHT
};
typedef CMD_TYPE CmdType;

#ifdef __cplusplus
struct CmdTypeEnum : util::Reflection<CmdType, CmdType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CMD_CRT_SWITCH_RIGHT;
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

            case CMD_MOUSE_MOVE_ABS:        return "MOUSE_MOVE_ABS";
            case CMD_MOUSE_MOVE_REL:        return "MOUSE_MOVE_REL";
            case CMD_MOUSE_EVENT:           return "MOUSE_EVENT";

            case CMD_JOY_EVENT:             return "JOY_EVENT";

                /*
            case CMD_DSK_PROTECT:           return "DSK_PROTECT";
            case CMD_DSK_UNPROTECT:         return "DSK_UNPROTECT";
                 */
            case CMD_DSK_TOGGLE_WP:         return "DSK_TOGGLE_WP";
            case CMD_DSK_MODIFIED:          return "DSK_MODIFIED";
            case CMD_DSK_UNMODIFIED:        return "DSK_UNMODIFIED";

            case CMD_DATASETTE_PLAY:        return "DATASETTE_PLAY";
            case CMD_DATASETTE_STOP:        return "DATASETTE_STOP";
            case CMD_DATASETTE_REWIND:      return "DATASETTE_REWIND";

            case CMD_CRT_BUTTON_PRESS:      return "CRT_BUTTON_PRESS";
            case CMD_CRT_BUTTON_RELEASE:    return "CRT_BUTTON_RELEASE";
            case CMD_CRT_SWITCH_LEFT:       return "CRT_SWITCH_LEFT";
            case CMD_CRT_SWITCH_NEUTRAL:    return "CRT_SWITCH_NEUTRAL";
            case CMD_CRT_SWITCH_RIGHT:      return "CRT_SWITCH_RIGHT";
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
    isize port;
    double x;
    double y;
}
CoordCmd;

typedef struct
{
    isize port;
    GamePadAction action;
}
GamePadCmd;

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
        CoordCmd coord;
        GamePadCmd action;
        TapeCmd tape;
        AlarmCmd alarm;
    };
}
Cmd;
