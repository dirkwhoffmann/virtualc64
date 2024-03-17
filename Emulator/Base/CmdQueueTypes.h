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
#include "JoystickTypes.h"
#include "Reflection.h"

//
// Enumerations
//

/// Emulator command
new_enum_long(CMD_TYPE)
{
    CMD_NONE = 0,               ///< None

    // C64
    CMD_SNAPSHOT_AUTO,          ///< Take a snapshot (emulator triggered)
    CMD_SNAPSHOT_USER,          ///< Take a snapshot (user triggered)
    CMD_ALARM_ABS,              ///< Schedule an alarm (absolute cycle)
    CMD_ALARM_REL,              ///< Schedule an alarm (relative cycle)

    // CPU
    CMD_CPU_BRK,                ///< Let the CPU execute a BRK instruction
    CMD_CPU_NMI,                ///< Emulate an external expansion port NMI

    // Keyboard
    CMD_KEY_PRESS,              ///< Press a key on the C64 keyboard
    CMD_KEY_RELEASE,            ///< Release a key on the C64 keyboard
    CMD_KEY_RELEASE_ALL,        ///< Clear the keyboard matrix
    CMD_KEY_TOGGLE,             ///< Press or release a key on the C64 keyboard

    // Mice
    CMD_MOUSE_MOVE_ABS,         ///< Signal a mouse movement (absolut coordinates)
    CMD_MOUSE_MOVE_REL,         ///< Signal a mouse movement (relative coordinates)
    CMD_MOUSE_EVENT,            ///< Signal a mouse button event

    // Joysticks
    CMD_JOY_EVENT,              ///< Signal a joystick button event

    // Floppy disks
    CMD_DSK_TOGGLE_WP,          ///< Toggle write-protection
    CMD_DSK_MODIFIED,           ///< Signal that the disk has been saved
    CMD_DSK_UNMODIFIED,         ///< Signan that the disk needs saving

    // Datasette
    CMD_DATASETTE_PLAY,         ///< Press the datasette play key
    CMD_DATASETTE_STOP,         ///< Press the datasette stop key
    CMD_DATASETTE_REWIND,       ///< Rewind the tape

    // Cartridges
    CMD_CRT_BUTTON_PRESS,       ///< Press a cartridge button
    CMD_CRT_BUTTON_RELEASE,     ///< Release a cartridge button
    CMD_CRT_SWITCH_LEFT,        ///< Pull the cartridge switch left
    CMD_CRT_SWITCH_NEUTRAL,     ///< Put the cartridge switch in neutral position
    CMD_CRT_SWITCH_RIGHT,       ///< Pull the cartridge switch right

    // RetroShell
    CMD_RSH_EXECUTE             ///< Execute a script command
};
typedef CMD_TYPE CmdType;

#ifdef __cplusplus
struct CmdTypeEnum : util::Reflection<CmdType, CmdType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CMD_RSH_EXECUTE;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CMD"; }
    static const char *key(long value)
    {
        switch (value) {

            case CMD_NONE:                  return "NONE";

            case CMD_SNAPSHOT_AUTO:         return "SNAPSHOT_AUTO";
            case CMD_SNAPSHOT_USER:         return "SNAPSHOT_USER";
            case CMD_ALARM_ABS:             return "ALARM_ABS";
            case CMD_ALARM_REL:             return "ALARM_REL";

            case CMD_CPU_BRK:               return "CPU_BRK";
            case CMD_CPU_NMI:               return "CPU_NMI";

            case CMD_KEY_PRESS:             return "KEY_PRESS";
            case CMD_KEY_RELEASE:           return "KEY_RELEASE";
            case CMD_KEY_RELEASE_ALL:       return "KEY_RELEASE_ALL";
            case CMD_KEY_TOGGLE:            return "KEY_TOGGLE";

            case CMD_MOUSE_MOVE_ABS:        return "MOUSE_MOVE_ABS";
            case CMD_MOUSE_MOVE_REL:        return "MOUSE_MOVE_REL";
            case CMD_MOUSE_EVENT:           return "MOUSE_EVENT";

            case CMD_JOY_EVENT:             return "JOY_EVENT";

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

            case CMD_RSH_EXECUTE:           return "RSH_EXECUTE";
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
    const char *command;
}
ShellCmd;

#ifdef __cplusplus
struct Cmd
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
        ShellCmd shell;
    };

    Cmd() { }
    Cmd(CmdType type, i64 value = 0) : type(type), value(value) { }
    Cmd(CmdType type, const KeyCmd &cmd) : type(type), key(cmd) { }
    Cmd(CmdType type, const CoordCmd &cmd) : type(type), coord(cmd) { }
    Cmd(CmdType type, const GamePadCmd &cmd) : type(type), action(cmd) { }
    Cmd(CmdType type, const TapeCmd &cmd) : type(type), tape(cmd) { }
    Cmd(CmdType type, const AlarmCmd &cmd) : type(type), alarm(cmd) { }
    Cmd(CmdType type, const ShellCmd &cmd) : type(type), shell(cmd) { }
};
#endif
