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

/// Execution state
enum class ExecState : long
{
    OFF,          ///< Powered off
    PAUSED,       ///< Powered on, but currently paused
    RUNNING,      ///< Up and running
    HALTED        ///< Shut down
};

struct ExecStateEnum : Reflection<ExecStateEnum, ExecState>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(ExecState::HALTED);

    static const char *_key(ExecState value)
    {
        switch (value) {

            case ExecState::OFF:          return "OFF";
            case ExecState::PAUSED:       return "PAUSED";
            case ExecState::RUNNING:      return "RUNNING";
            case ExecState::HALTED:       return "HALTED";
        }
        return "???";
    }
    
    static const char *help(ExecState value)
    {
        return "";
    }
};

}
