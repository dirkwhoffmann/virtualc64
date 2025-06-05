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
enum class Cmd : long
{
    NONE,                   ///< None

    // Emulator
    CONFIG,                 ///< Configure the emulator
    CONFIG_ALL,             ///< Configure the emulator
    CONFIG_SCHEME,          ///< IConfigure the emulator with a predefined scheme
    HARD_RESET,             ///< Perform a hard reset
    SOFT_RESET,             ///< Perform a soft reset
    POWER_ON,               ///< Switch power on
    POWER_OFF,              ///< Switch power off
    RUN,                    ///< Start emulation
    PAUSE,                  ///< Pause emulation
    WARP_ON,                ///< Switch on warp mode
    WARP_OFF,               ///< Switch off warp mode
    HALT,                   ///< Terminate the emulator thread

    // C64
    ALARM_ABS,              ///< Schedule an alarm (absolute cycle)
    ALARM_REL,              ///< Schedule an alarm (relative cycle)
    INSPECTION_TARGET,      ///< Sets the auto-inspection mask
    
    // CPU
    CPU_BRK,                ///< Let the CPU execute a BRK instruction
    CPU_NMI,                ///< Emulate an external expansion port NMI

    // CPU (Breakpoints)
    BP_SET_AT,              ///< Set a breakpoint
    BP_MOVE_TO,             ///< Change the address of breakpoint
    BP_REMOVE_NR,           ///< Remove the n-th breakpoint
    BP_REMOVE_AT,           ///< Remove the breakpoint at an address
    BP_REMOVE_ALL,          ///< Remove all brekpoints
    BP_ENABLE_NR,           ///< Enable the n-th breakpoint
    BP_ENABLE_AT,           ///< Enable the breakpoint at an address
    BP_ENABLE_ALL,          ///< Enable all brekpoints
    BP_DISABLE_NR,          ///< Disable the n-th breakpoint
    BP_DISABLE_AT,          ///< Disable the breakpoint at an address
    BP_DISABLE_ALL,         ///< Disable all brekpoints

    // CPU (Watchpoints)
    WP_SET_AT,              ///< Set a watchpoint
    WP_MOVE_TO,             ///< Change the address of watchpoint
    WP_REMOVE_NR,           ///< Remove the n-th watchpoint
    WP_REMOVE_AT,           ///< Remove the watchpoint at an address
    WP_REMOVE_ALL,          ///< Remove all watchpoints
    WP_ENABLE_NR,           ///< Enable the n-th watchpoint
    WP_ENABLE_AT,           ///< Enable the watchpoint at an address
    WP_ENABLE_ALL,          ///< Enable all watchpoints
    WP_DISABLE_NR,          ///< Disable the n-th watchpoint
    WP_DISABLE_AT,          ///< Disable the watchpoint at an address
    WP_DISABLE_ALL,         ///< Disable all watchpoints

    // Keyboard
    KEY_PRESS,              ///< Press a key on the C64 keyboard
    KEY_RELEASE,            ///< Release a key on the C64 keyboard
    KEY_RELEASE_ALL,        ///< Clear the keyboard matrix
    KEY_TOGGLE,             ///< Press or release a key on the C64 keyboard

    // Mouse
    MOUSE_MOVE_ABS,         ///< Signal a mouse movement (absolute)
    MOUSE_MOVE_REL,         ///< Signal a mouse movement (relative)
    MOUSE_BUTTON,           ///< Signal a mouse button event

    // Joystick
    JOY_EVENT,              ///< Signal a joystick button event

    // Floppy disk
    DSK_TOGGLE_WP,          ///< Toggle write-protection
    DSK_MODIFIED,           ///< Signal that the disk has been saved
    DSK_UNMODIFIED,         ///< Signan that the disk needs saving

    // Datasette
    DATASETTE_PLAY,         ///< Press the datasette play key
    DATASETTE_STOP,         ///< Press the datasette stop key
    DATASETTE_REWIND,       ///< Rewind the tape

    // Cartridge
    CRT_BUTTON_PRESS,       ///< Press a cartridge button
    CRT_BUTTON_RELEASE,     ///< Release a cartridge button
    CRT_SWITCH_LEFT,        ///< Pull the cartridge switch left
    CRT_SWITCH_NEUTRAL,     ///< Put the cartridge switch in neutral position
    CRT_SWITCH_RIGHT,       ///< Pull the cartridge switch right

    // RetroShell
    RSH_EXECUTE,            ///< Execute a script command

    // Host machine
    FOCUS                   ///< The emulator windows got or lost focus
};

struct CmdEnum : Reflection<CmdEnum, Cmd> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Cmd::FOCUS);

    static const char *_key(Cmd value)
    {
        switch (value) {

            case Cmd::NONE:                  return "NONE";

            case Cmd::CONFIG:                return "CONFIG";
            case Cmd::CONFIG_ALL:            return "CONFIG_ALL";
            case Cmd::CONFIG_SCHEME:         return "CONFIG_SCHEME";
            case Cmd::HARD_RESET:            return "HARD_RESET";
            case Cmd::SOFT_RESET:            return "SOFT_RESET";
            case Cmd::POWER_ON:              return "POWER_ON";
            case Cmd::POWER_OFF:             return "POWER_OFF";
            case Cmd::RUN:                   return "RUN";
            case Cmd::PAUSE:                 return "PAUSE";
            case Cmd::WARP_ON:               return "WARP_ON";
            case Cmd::WARP_OFF:              return "WARP_OFF";
            case Cmd::HALT:                  return "HALT";

            case Cmd::ALARM_ABS:             return "ALARM_ABS";
            case Cmd::ALARM_REL:             return "ALARM_REL";
            case Cmd::INSPECTION_TARGET:     return "INSPECTION_TARGET";

            case Cmd::CPU_BRK:               return "CPU_BRK";
            case Cmd::CPU_NMI:               return "CPU_NMI";

            case Cmd::BP_SET_AT:             return "BP_SET_AT";
            case Cmd::BP_MOVE_TO:            return "BP_MOVE_TO";
            case Cmd::BP_REMOVE_NR:          return "BP_REMOVE_NR";
            case Cmd::BP_REMOVE_AT:          return "BP_REMOVE_AT";
            case Cmd::BP_REMOVE_ALL:         return "BP_REMOVE_ALL";
            case Cmd::BP_ENABLE_NR:          return "BP_ENABLE_NR";
            case Cmd::BP_ENABLE_AT:          return "BP_ENABLE_AT";
            case Cmd::BP_ENABLE_ALL:         return "BP_ENABLE_ALL";
            case Cmd::BP_DISABLE_NR:         return "BP_DISABLE_NR";
            case Cmd::BP_DISABLE_AT:         return "BP_DISABLE_AT";
            case Cmd::BP_DISABLE_ALL:        return "BP_DISABLE_ALL";

            case Cmd::WP_SET_AT:             return "WP_SET_AT";
            case Cmd::WP_MOVE_TO:            return "WP_MOVE_TO";
            case Cmd::WP_REMOVE_NR:          return "WP_REMOVE_NR";
            case Cmd::WP_REMOVE_AT:          return "WP_REMOVE_AT";
            case Cmd::WP_REMOVE_ALL:         return "WP_REMOVE_ALL";
            case Cmd::WP_ENABLE_NR:          return "WP_ENABLE_NR";
            case Cmd::WP_ENABLE_AT:          return "WP_ENABLE_AT";
            case Cmd::WP_ENABLE_ALL:         return "WP_ENABLE_ALL";
            case Cmd::WP_DISABLE_NR:         return "WP_DISABLE_NR";
            case Cmd::WP_DISABLE_AT:         return "WP_DISABLE_AT";
            case Cmd::WP_DISABLE_ALL:        return "WP_DISABLE_ALL";

            case Cmd::KEY_PRESS:             return "KEY_PRESS";
            case Cmd::KEY_RELEASE:           return "KEY_RELEASE";
            case Cmd::KEY_RELEASE_ALL:       return "KEY_RELEASE_ALL";
            case Cmd::KEY_TOGGLE:            return "KEY_TOGGLE";

            case Cmd::MOUSE_MOVE_ABS:        return "MOUSE_MOVE_ABS";
            case Cmd::MOUSE_MOVE_REL:        return "MOUSE_MOVE_REL";
            case Cmd::MOUSE_BUTTON:          return "MOUSE_BUTTON";

            case Cmd::JOY_EVENT:             return "JOY_EVENT";

            case Cmd::DSK_TOGGLE_WP:         return "DSK_TOGGLE_WP";
            case Cmd::DSK_MODIFIED:          return "DSK_MODIFIED";
            case Cmd::DSK_UNMODIFIED:        return "DSK_UNMODIFIED";

            case Cmd::DATASETTE_PLAY:        return "DATASETTE_PLAY";
            case Cmd::DATASETTE_STOP:        return "DATASETTE_STOP";
            case Cmd::DATASETTE_REWIND:      return "DATASETTE_REWIND";

            case Cmd::CRT_BUTTON_PRESS:      return "CRT_BUTTON_PRESS";
            case Cmd::CRT_BUTTON_RELEASE:    return "CRT_BUTTON_RELEASE";
            case Cmd::CRT_SWITCH_LEFT:       return "CRT_SWITCH_LEFT";
            case Cmd::CRT_SWITCH_NEUTRAL:    return "CRT_SWITCH_NEUTRAL";
            case Cmd::CRT_SWITCH_RIGHT:      return "CRT_SWITCH_RIGHT";

            case Cmd::RSH_EXECUTE:           return "RSH_EXECUTE";

            case Cmd::FOCUS:                 return "FOCUS";

        }
        return "???";
    }
    
    static const char *help(Cmd value)
    {
        return "";
    }
};


//
// Structures
//

typedef struct
{
    Opt option;
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

struct Command
{
    // Header
    Cmd type;

    // Sender
    void *sender;
    
    // Payload
    union {

        struct { i64 value; i64 value2; };
        AlarmCmd alarm;
        ConfigCmd config;
        CoordCmd coord;
        GamePadCmd action;
        KeyCmd key;
        TapeCmd tape;
    };

    Command() { }
    Command(Cmd type, i64 v1 = 0, i64 v2 = 0) : type(type), value(v1), value2(v2) { }
    Command(Cmd type, void *s, i64 v1 = 0, i64 v2 = 0) : type(type), sender(s), value(v1), value2(v2) { }
    Command(Cmd type, const AlarmCmd &cmd) : type(type), alarm(cmd) { }
    Command(Cmd type, const ConfigCmd &cmd) : type(type), config(cmd) { }
    Command(Cmd type, const CoordCmd &cmd) : type(type), coord(cmd) { }
    Command(Cmd type, const GamePadCmd &cmd) : type(type), action(cmd) { }
    Command(Cmd type, const KeyCmd &cmd) : type(type), key(cmd) { }
    Command(Cmd type, const TapeCmd &cmd) : type(type), tape(cmd) { }
};

}
