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
    CMD_KEY_PRESS,
    CMD_KEY_RELEASE,
    CMD_KEY_TOGGLE,
};
typedef CMD_TYPE CmdType;

#ifdef __cplusplus
struct CmdTypeEnum : util::Reflection<CmdType, CmdType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CMD_KEY_TOGGLE;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CMD"; }
    static const char *key(CmdType value)
    {
        switch (value) {

            case CMD_NONE:                  return "NONE";
            case CMD_KEY_PRESS:             return "KEY_PRESS";
            case CMD_KEY_RELEASE:           return "KEY_RELEASE";
            case CMD_KEY_TOGGLE:            return "KEY_TOGGLE";
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
    u8 keycode;
    double delay;
};

typedef struct
{
    // Header
    CmdType type;

    // Payload
    union {
        
        i64 value;
        struct KeyCmd key;
    };
}
Cmd;
