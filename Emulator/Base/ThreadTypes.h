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

/// Emulator state
enum_long(EMULATOR_STATE)
{
    STATE_OFF,          ///< Powered off
    STATE_PAUSED,       ///< Powered on, but currently paused
    STATE_RUNNING,      ///< Up and running
    STATE_SUSPENDED,    ///< Shortly paused for an internal state change
    STATE_HALTED        ///< Shut down
};
typedef EMULATOR_STATE EmulatorState;
