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
enum_long(RSKEY_KEY)
{
    RSKEY_UP,
    RSKEY_DOWN,
    RSKEY_LEFT,
    RSKEY_RIGHT,
    RSKEY_DEL,
    RSKEY_CUT,
    RSKEY_BACKSPACE,
    RSKEY_HOME,
    RSKEY_END,
    RSKEY_TAB,
    RSKEY_RETURN,
    RSKEY_SHIFT_RETURN,
    RSKEY_CR
};
typedef RSKEY_KEY RetroShellKey;

struct RetroShellKeyEnum : util::Reflection<RetroShellKeyEnum, RetroShellKey>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = RSKEY_CR;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "RSKEY"; }
    static const char *key(long value)
    {
        switch (value) {

            case RSKEY_UP:              return "UP";
            case RSKEY_DOWN:            return "DOWN";
            case RSKEY_LEFT:            return "LEFT";
            case RSKEY_RIGHT:           return "RIGHT";
            case RSKEY_DEL:             return "DEL";
            case RSKEY_CUT:             return "CUT";
            case RSKEY_BACKSPACE:       return "BACKSPACE";
            case RSKEY_HOME:            return "HOME";
            case RSKEY_END:             return "END";
            case RSKEY_TAB:             return "TAB";
            case RSKEY_RETURN:          return "RETURN";
            case RSKEY_SHIFT_RETURN:    return "SHIFT_RETURN";
            case RSKEY_CR:              return "CR";
        }
        return "???";
    }
};

}
