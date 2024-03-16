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

//
// Enumerations
//

/// Emulator command
enum_long(CMD_TYPE)
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
