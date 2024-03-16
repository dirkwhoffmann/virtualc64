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
#include "Reflection.hpp"

#include "RecorderTypes.h"

struct RecStateEnum : util::Reflection<RecStateEnum, RecState> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = REC_STATE_ABORT;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "REC_STATE"; }
    static const char *key(RecState value)
    {
        switch (value) {

            case REC_STATE_WAIT:        return "REC_STATE.WAIT";
            case REC_STATE_PREPARE:     return "REC_STATE.PREPARE";
            case REC_STATE_RECORD:      return "REC_STATE.RECORD";
            case REC_STATE_FINALIZE:    return "REC_STATE.FINALIZE";
            case REC_STATE_ABORT:       return "REC_STATE.ABORT";
        }
        return "???";
    }
};
