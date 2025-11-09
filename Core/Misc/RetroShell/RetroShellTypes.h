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

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// RetroShell special key
enum class RSKey
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    PAGE_UP,
    PAGE_DOWN,
    DEL,
    CUT,
    BACKSPACE,
    HOME,
    END,
    TAB,
    RETURN,
    CR
};

struct RSKeyEnum : Reflection<RSKeyEnum, RSKey>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RSKey::CR);
    
    static const char *_key(RSKey value)
    {
        switch (value) {
                
            case RSKey::UP:          return "UP";
            case RSKey::DOWN:        return "DOWN";
            case RSKey::LEFT:        return "LEFT";
            case RSKey::RIGHT:       return "RIGHT";
            case RSKey::PAGE_UP:     return "PAGE_UP";
            case RSKey::PAGE_DOWN:   return "PAGE_DOWN";
            case RSKey::DEL:         return "DEL";
            case RSKey::CUT:         return "CUT";
            case RSKey::BACKSPACE:   return "BACKSPACE";
            case RSKey::HOME:        return "HOME";
            case RSKey::END:         return "END";
            case RSKey::TAB:         return "TAB";
            case RSKey::RETURN:      return "RETURN";
            case RSKey::CR:          return "CR";
        }
        return "???";
    }
    static const char *help(RSKey value)
    {
        return "";
    }
};

//
// Structures
//

struct InputLine {

    enum class Source { USER, SCRIPT, RPC };

    // Line number, RPC identifier, etc.
    isize id;

    // Indicates where the command comes from
    Source type;

    // Echo the input in the console when executed
    bool echo;

    // The command to execute
    string input;

    bool isUserCommand() const { return type == Source::USER; }
    bool isScriptCommand() const { return type == Source::SCRIPT; }
    bool isRpcCommand() const { return type == Source::RPC; }
};

typedef struct
{
    // Active console
    isize console;
    
    // Relative position of the cursor
    isize cursorRel;
}
RetroShellInfo;

// Used in operator overloads
struct vspace { isize lines = 0; };

}
