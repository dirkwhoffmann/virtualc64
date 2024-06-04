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

/// Execution state
enum_long(EXEC_STATE)
{
    STATE_OFF,          ///< Powered off
    STATE_PAUSED,       ///< Powered on, but currently paused
    STATE_RUNNING,      ///< Up and running
    STATE_SUSPENDED,    ///< Shortly paused for an internal state change
    STATE_HALTED        ///< Shut down
};
typedef EXEC_STATE ExecState;

struct ExecStateEnum : util::Reflection<ExecStateEnum, ExecState>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = STATE_HALTED;
    static bool isValid(auto val) { return val >= minVal && val <= maxVal; }

    static const char *prefix() { return "STATE"; }
    static const char *key(long value)
    {
        switch (value) {

            case STATE_OFF:          return "OFF";
            case STATE_PAUSED:       return "PAUSED";
            case STATE_RUNNING:      return "RUNNING";
            case STATE_SUSPENDED:    return "SUSPENDED";
            case STATE_HALTED:       return "HALTED";
        }
        return "???";
    }
};

}
