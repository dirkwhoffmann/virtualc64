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

#include "JoystickTypes.h"
#include "Reflection.h"
#include "Option.h"

namespace vc64 {

//
// Enumerations
//

/// Emulator command
enum_long(CMD_TYPE)
{
    CMD_NONE = 0,               ///< None

    // Emulator
    CMD_CONFIG,                 ///< Configure the emulator

    // C64
    CMD_ALARM_ABS,              ///< Schedule an alarm (absolute cycle)
    CMD_ALARM_REL,              ///< Schedule an alarm (relative cycle)
    CMD_INSPECTION_TARGET,      ///< Sets the auto-inspection component
    
    // CPU
    CMD_CPU_BRK,                ///< Let the CPU execute a BRK instruction
    CMD_CPU_NMI,                ///< Emulate an external expansion port NMI

    // CPU (Breakpoints)
    CMD_BP_SET_AT,              ///< Set a breakpoint
    CMD_BP_MOVE_TO,             ///< Change the address of breakpoint
    CMD_BP_REMOVE_NR,           ///< Remove the n-th breakpoint
    CMD_BP_REMOVE_AT,           ///< Remove the breakpoint at an address
    CMD_BP_REMOVE_ALL,          ///< Remove all brekpoints
    CMD_BP_ENABLE_NR,           ///< Enable the n-th breakpoint
    CMD_BP_ENABLE_AT,           ///< Enable the breakpoint at an address
    CMD_BP_ENABLE_ALL,          ///< Enable all brekpoints
    CMD_BP_DISABLE_NR,          ///< Disable the n-th breakpoint
    CMD_BP_DISABLE_AT,          ///< Disable the breakpoint at an address
    CMD_BP_DISABLE_ALL,         ///< Disable all brekpoints

    // CPU (Watchpoints)
    CMD_WP_SET_AT,              ///< Set a watchpoint
    CMD_WP_MOVE_TO,             ///< Change the address of watchpoint
    CMD_WP_REMOVE_NR,           ///< Remove the n-th watchpoint
    CMD_WP_REMOVE_AT,           ///< Remove the watchpoint at an address
    CMD_WP_REMOVE_ALL,          ///< Remove all watchpoints
    CMD_WP_ENABLE_NR,           ///< Enable the n-th watchpoint
    CMD_WP_ENABLE_AT,           ///< Enable the watchpoint at an address
    CMD_WP_ENABLE_ALL,          ///< Enable all watchpoints
    CMD_WP_DISABLE_NR,          ///< Disable the n-th watchpoint
    CMD_WP_DISABLE_AT,          ///< Disable the watchpoint at an address
    CMD_WP_DISABLE_ALL,         ///< Disable all watchpoints

    // Keyboard
    CMD_KEY_PRESS,              ///< Press a key on the C64 keyboard
    CMD_KEY_RELEASE,            ///< Release a key on the C64 keyboard
    CMD_KEY_RELEASE_ALL,        ///< Clear the keyboard matrix
    CMD_KEY_TOGGLE,             ///< Press or release a key on the C64 keyboard

    // Mouse
    CMD_MOUSE_MOVE_ABS,         ///< Signal a mouse movement (absolute)
    CMD_MOUSE_MOVE_REL,         ///< Signal a mouse movement (relative)
    CMD_MOUSE_EVENT,            ///< Signal a mouse button event

    // Joystick
    CMD_JOY_EVENT,              ///< Signal a joystick button event

    // Floppy disk
    CMD_DSK_TOGGLE_WP,          ///< Toggle write-protection
    CMD_DSK_MODIFIED,           ///< Signal that the disk has been saved
    CMD_DSK_UNMODIFIED,         ///< Signan that the disk needs saving

    // Datasette
    CMD_DATASETTE_PLAY,         ///< Press the datasette play key
    CMD_DATASETTE_STOP,         ///< Press the datasette stop key
    CMD_DATASETTE_REWIND,       ///< Rewind the tape

    // Cartridge
    CMD_CRT_BUTTON_PRESS,       ///< Press a cartridge button
    CMD_CRT_BUTTON_RELEASE,     ///< Release a cartridge button
    CMD_CRT_SWITCH_LEFT,        ///< Pull the cartridge switch left
    CMD_CRT_SWITCH_NEUTRAL,     ///< Put the cartridge switch in neutral position
    CMD_CRT_SWITCH_RIGHT,       ///< Pull the cartridge switch right

    // RetroShell
    CMD_RSH_EXECUTE,            ///< Execute a script command

    // Host machine
    CMD_FOCUS                   ///< The emulator windows got or lost focus
};
typedef CMD_TYPE CmdType;

struct CmdTypeEnum : util::Reflection<CmdType, CmdType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CMD_FOCUS;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CMD"; }
    static const char *key(long value)
    {
        switch (value) {

            case CMD_NONE:                  return "NONE";

            case CMD_CONFIG:                return "CONFIG";

            case CMD_ALARM_ABS:             return "ALARM_ABS";
            case CMD_ALARM_REL:             return "ALARM_REL";
            case CMD_INSPECTION_TARGET:     return "INSPECTION_TARGET";

            case CMD_CPU_BRK:               return "CPU_BRK";
            case CMD_CPU_NMI:               return "CPU_NMI";

            case CMD_BP_SET_AT:             return "BP_SET_AT";
            case CMD_BP_REMOVE_NR:          return "BP_REMOVE_NR";
            case CMD_BP_REMOVE_AT:          return "BP_REMOVE_AT";
            case CMD_BP_REMOVE_ALL:         return "BP_REMOVE_ALL";
            case CMD_BP_ENABLE_NR:          return "BP_ENABLE_NR";
            case CMD_BP_ENABLE_AT:          return "BP_ENABLE_AT";
            case CMD_BP_ENABLE_ALL:         return "BP_ENABLE_ALL";
            case CMD_BP_DISABLE_NR:         return "BP_DISABLE_NR";
            case CMD_BP_DISABLE_AT:         return "BP_DISABLE_AT";
            case CMD_BP_DISABLE_ALL:        return "BP_DISABLE_ALL";

            case CMD_WP_SET_AT:             return "WP_SET_AT";
            case CMD_WP_REMOVE_NR:          return "WP_REMOVE_NR";
            case CMD_WP_REMOVE_AT:          return "WP_REMOVE_AT";
            case CMD_WP_REMOVE_ALL:         return "WP_REMOVE_ALL";
            case CMD_WP_ENABLE_NR:          return "WP_ENABLE_NR";
            case CMD_WP_ENABLE_AT:          return "WP_ENABLE_AT";
            case CMD_WP_ENABLE_ALL:         return "WP_ENABLE_ALL";
            case CMD_WP_DISABLE_NR:         return "WP_DISABLE_NR";
            case CMD_WP_DISABLE_AT:         return "WP_DISABLE_AT";
            case CMD_WP_DISABLE_ALL:        return "WP_DISABLE_ALL";

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

            case CMD_FOCUS:                 return "FOCUS";

        }
        return "???";
    }
};

//
// Structures
//

typedef struct
{
    Option option;
    i64 value;
    isize id;
}
ConfigCmd;

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

struct Cmd
{
    // Header
    CmdType type;

    // Payload
    union {

        struct { i64 value; i64 value2; };
        AlarmCmd alarm;
        ConfigCmd config;
        CoordCmd coord;
        GamePadCmd action;
        KeyCmd key;
        ShellCmd shell;
        TapeCmd tape;
    };

    Cmd() { }
    Cmd(CmdType type, i64 v1 = 0, i64 v2 = 0) : type(type), value(v1), value2(v2) { }
    Cmd(CmdType type, const AlarmCmd &cmd) : type(type), alarm(cmd) { }
    Cmd(CmdType type, const ConfigCmd &cmd) : type(type), config(cmd) { }
    Cmd(CmdType type, const CoordCmd &cmd) : type(type), coord(cmd) { }
    Cmd(CmdType type, const GamePadCmd &cmd) : type(type), action(cmd) { }
    Cmd(CmdType type, const KeyCmd &cmd) : type(type), key(cmd) { }
    Cmd(CmdType type, const ShellCmd &cmd) : type(type), shell(cmd) { }
    Cmd(CmdType type, const TapeCmd &cmd) : type(type), tape(cmd) { }
};

}
