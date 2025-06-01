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
enum class RetroShellKey
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    DEL,
    CUT,
    BACKSPACE,
    HOME,
    END,
    TAB,
    RETURN,
    CR
};

struct RetroShellKeyEnum : Reflection<RetroShellKeyEnum, RetroShellKey>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RetroShellKey::CR);
    
    static const char *_key(RetroShellKey value)
    {
        switch (value) {
                
            case RetroShellKey::UP:          return "UP";
            case RetroShellKey::DOWN:        return "DOWN";
            case RetroShellKey::LEFT:        return "LEFT";
            case RetroShellKey::RIGHT:       return "RIGHT";
            case RetroShellKey::DEL:         return "DEL";
            case RetroShellKey::CUT:         return "CUT";
            case RetroShellKey::BACKSPACE:   return "BACKSPACE";
            case RetroShellKey::HOME:        return "HOME";
            case RetroShellKey::END:         return "END";
            case RetroShellKey::TAB:         return "TAB";
            case RetroShellKey::RETURN:      return "RETURN";
            case RetroShellKey::CR:          return "CR";
        }
        return "???";
    }
    static const char *help(RetroShellKey value)
    {
        return "";
    }
};

}
