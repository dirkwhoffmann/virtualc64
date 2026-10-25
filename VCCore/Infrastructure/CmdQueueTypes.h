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

#include "Peripherals/Joystick/JoystickTypes.h"
#include "Components/CPU/CPUTypes.h"
#include "Infrastructure/Option.h"

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
    TRACK_ON,               ///< Switch on track mode
    TRACK_OFF,              ///< Switch off track mode
    STEP_INTO,             ///< Run the emulator for a single step
    STEP_OVER,             ///< Run the emulator for a single step
    STEP_CYCLE,            ///< Run the emulator for a single step
    FINISH_LINE,           ///< Run the emulator for a single step
    FINISH_FRAME,          ///< Run the emulator for a single step
    HALT,                   ///< Terminate the emulator thread

    // C64
    ALARM_ABS,              ///< Schedule an alarm (absolute cycle)
    ALARM_REL,              ///< Schedule an alarm (relative cycle)
    INSPECTION_TARGET,      ///< Sets the auto-inspection mask
    SAVE_WORKSPACE,         ///< Save the workspace to a folder (path attached)
    SAVE_SNAPSHOT,          ///< Save a snapshot to a file (path attached, value = compressor)
    HIBERNATE,              ///< Hibernate into an SVM (path attached, value = bit 0: snapshot, bit 1: workspace)
    
    // CPU
    CPU_BRK,                ///< Let the CPU execute a BRK instruction
    CPU_NMI,                ///< Emulate an external expansion port NMI

    // CPU (Breakpoints, Watchpoints)
    GUARD_SET_AT,           ///< Set a guard
    GUARD_REMOVE_NR,        ///< Remove the n-th guard
    GUARD_MOVE_NR,          ///< Change the address of a guard
    GUARD_REMOVE_AT,        ///< Remove the guard at an address
    GUARD_REMOVE_ALL,       ///< Remove all guards
    GUARD_ENABLE_NR,        ///< Enable the n-th guard
    GUARD_ENABLE_AT,        ///< Enable the guard at an address
    GUARD_ENABLE_ALL,       ///< Enable all guards
    GUARD_DISABLE_NR,       ///< Disable the n-th guard
    GUARD_DISABLE_AT,       ///< Disable the guard at an address
    GUARD_DISABLE_ALL,      ///< Disable all guards
    GUARD_TOGGLE_NR,        ///< Toggle the n-th guard

    // Keyboard
    KEY_PRESS,              ///< Press a key on the C64 keyboard
    KEY_RELEASE,            ///< Release a key on the C64 keyboard
    KEY_RELEASE_ALL,        ///< Clear the keyboard matrix
    KEY_TOGGLE,             ///< Press or release a key on the C64 keyboard
    KEY_LOCK,               ///< Lock a key on the C64 keyboard
    KEY_UNLOCK,             ///< Unlock a key on the C64 keyboard
    KEY_UNLOCK_ALL,         ///< Unlock all keys on the C64 keyboard

    // Mouse
    MOUSE_MOVE_ABS,         ///< Signal a mouse movement (absolute)
    MOUSE_MOVE_REL,         ///< Signal a mouse movement (relative)
    MOUSE_BUTTON,           ///< Signal a mouse button event

    // Joystick
    JOY_EVENT,              ///< Signal a joystick button event
    JOY_STATE,              ///< Set the complete joystick state (directions and fire button)

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

struct CmdEnum : Reflectable<CmdEnum, Cmd> {

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
            case Cmd::TRACK_ON:              return "TRACK_ON";
            case Cmd::TRACK_OFF:             return "TRACK_OFF";
            case Cmd::STEP_INTO:             return "STEP_INTO";
            case Cmd::STEP_OVER:             return "STEP_OVER";
            case Cmd::STEP_CYCLE:            return "STEP_CYCLE";
            case Cmd::FINISH_LINE:           return "FINISH_LINE";
            case Cmd::FINISH_FRAME:          return "FINISH_FRAME";
            case Cmd::HALT:                  return "HALT";

            case Cmd::ALARM_ABS:             return "ALARM_ABS";
            case Cmd::ALARM_REL:             return "ALARM_REL";
            case Cmd::INSPECTION_TARGET:     return "INSPECTION_TARGET";
            case Cmd::SAVE_WORKSPACE:        return "SAVE_WORKSPACE";
            case Cmd::SAVE_SNAPSHOT:         return "SAVE_SNAPSHOT";
            case Cmd::HIBERNATE:             return "HIBERNATE";

            case Cmd::CPU_BRK:               return "CPU_BRK";
            case Cmd::CPU_NMI:               return "CPU_NMI";

            case Cmd::GUARD_SET_AT:          return "GUARD_SET_AT";
            case Cmd::GUARD_MOVE_NR:         return "GUARD_MOVE_NR";
            case Cmd::GUARD_REMOVE_NR:       return "GUARD_REMOVE_NR";
            case Cmd::GUARD_REMOVE_AT:       return "GUARD_REMOVE_AT";
            case Cmd::GUARD_REMOVE_ALL:      return "GUARD_REMOVE_ALL";
            case Cmd::GUARD_ENABLE_NR:       return "GUARD_ENABLE_NR";
            case Cmd::GUARD_ENABLE_AT:       return "GUARD_ENABLE_AT";
            case Cmd::GUARD_ENABLE_ALL:      return "GUARD_ENABLE_ALL";
            case Cmd::GUARD_DISABLE_NR:      return "GUARD_DISABLE_NR";
            case Cmd::GUARD_DISABLE_AT:      return "GUARD_DISABLE_AT";
            case Cmd::GUARD_DISABLE_ALL:     return "GUARD_DISABLE_ALL";
            case Cmd::GUARD_TOGGLE_NR:       return "GUARD_TOGGLE_NR";

            case Cmd::KEY_PRESS:             return "KEY_PRESS";
            case Cmd::KEY_RELEASE:           return "KEY_RELEASE";
            case Cmd::KEY_RELEASE_ALL:       return "KEY_RELEASE_ALL";
            case Cmd::KEY_TOGGLE:            return "KEY_TOGGLE";
            case Cmd::KEY_LOCK:              return "KEY_LOCK";
            case Cmd::KEY_UNLOCK:            return "KEY_UNLOCK";
            case Cmd::KEY_UNLOCK_ALL:        return "KEY_UNLOCK_ALL";

            case Cmd::MOUSE_MOVE_ABS:        return "MOUSE_MOVE_ABS";
            case Cmd::MOUSE_MOVE_REL:        return "MOUSE_MOVE_REL";
            case Cmd::MOUSE_BUTTON:          return "MOUSE_BUTTON";

            case Cmd::JOY_EVENT:             return "JOY_EVENT";
            case Cmd::JOY_STATE:             return "JOY_STATE";

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
    isize port;
    u32 state;      // Bits 0-4: current state, bits 8-12: previous state (up, down, left, right, fire)
}
JoyStateCmd;

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

// The guard list to modify is identified by a GuardType
typedef struct
{
    GuardType list;       // The guard list to modify
    isize nr;           // Number of a guard in the list
    u32 addr;           // Target of a guard
    isize ignores;      // Ignore count
}
GuardCmd;

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
        GuardCmd guard;
        JoyStateCmd joyState;
        KeyCmd key;
        TapeCmd tape;
    };

    /* Attached string (may be null). Set by the command queue when a string
     * is attached to the command. The string is owned by the queue and stays
     * valid until the queue has recycled the slot it lives in.
     */
    const char *str = nullptr;

    Command() { }
    Command(Cmd type, i64 v1 = 0, i64 v2 = 0) : type(type), value(v1), value2(v2) { }
    Command(Cmd type, void *s, i64 v1 = 0, i64 v2 = 0) : type(type), sender(s), value(v1), value2(v2) { }
    Command(Cmd type, const AlarmCmd &cmd) : type(type), alarm(cmd) { }
    Command(Cmd type, const ConfigCmd &cmd) : type(type), config(cmd) { }
    Command(Cmd type, const CoordCmd &cmd) : type(type), coord(cmd) { }
    Command(Cmd type, const GamePadCmd &cmd) : type(type), action(cmd) { }
    Command(Cmd type, const GuardCmd &cmd) : type(type), guard(cmd) { }
    Command(Cmd type, const JoyStateCmd &cmd) : type(type), joyState(cmd) { }
    Command(Cmd type, const KeyCmd &cmd) : type(type), key(cmd) { }
    Command(Cmd type, const TapeCmd &cmd) : type(type), tape(cmd) { }
};

}
