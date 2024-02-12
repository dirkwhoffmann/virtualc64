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

    // Keyboard
    CMD_KEY_PRESSED,
    CMD_KEY_RELEASED
};
typedef CMD_TYPE CmdType;

#ifdef __cplusplus
struct CmdTypeEnum : util::Reflection<CmdType, CmdType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CMD_KEY_RELEASED;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CMD"; }
    static const char *key(CmdType value)
    {
        switch (value) {

            case CMD_NONE:                  return "NONE";

            case CMD_KEY_PRESSED:           return "KEY_PRESSED";
            case CMD_KEY_RELEASED:          return "KEY_RELEASED";
        }
        return "???";
    }
};
#endif


//
// Structures
//

struct KeyCmd
{
    int test;
    /* SOMETHING LIKE THIS...
    Keyboard::Key      code;     //!< Code of the key that has been pressed
    Keyboard::Scancode scancode; //!< Physical code of the key that has been pressed
    bool               alt;      //!< Is the Alt key pressed?
    bool               control;  //!< Is the Control key pressed?
    bool               shift;    //!< Is the Shift key pressed?
    bool               system;   //!< Is the System key pressed?
    */
};

typedef struct
{
    // Header
    CmdType type;

    // Payload
    union {
        
        i64 value;
        KeyCmd key;
    };
}
Cmd;
