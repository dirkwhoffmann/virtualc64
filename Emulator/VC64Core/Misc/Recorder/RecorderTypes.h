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

enum class RecState : long
{
    WAIT,
    PREPARE,
    RECORD,
    FINALIZE,
    ABORT
};

struct RecStateEnum : util::Reflection<RecStateEnum, RecState>
{
    static constexpr long minVal = 0;
    static constexpr long maxVal = long(RecState::ABORT);

    static const char *_key(RecState value)
    {
        switch (value) {
                
            case RecState::WAIT:      return "WAIT";
            case RecState::PREPARE:   return "PREPARE";
            case RecState::RECORD:    return "RECORD";
            case RecState::FINALIZE:  return "FINALIZE";
            case RecState::ABORT:     return "ABORT";
        }
        return "???";
    }
    static const char *help(long value)
    {
        return "";
    }
};


//
// Structures
//

typedef struct
{
    // Recording parameters
    isize frameRate;
    isize bitRate;
    isize sampleRate;

    // Aspect ratio
    struct {isize x; isize y; } aspectRatio;
}
RecorderConfig;

typedef struct
{
    // Indicates if FFmpeg is availabe
    bool available;

    // The current recorder state
    RecState state;

    // Duration of the recorded Video
    double duration;
}
RecorderInfo;

}
