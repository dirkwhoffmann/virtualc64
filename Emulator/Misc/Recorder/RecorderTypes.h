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

enum_long(REC_STATE)
{
    REC_STATE_WAIT,                     ///< The recorder is ready
    REC_STATE_PREPARE,                  ///< The recorder is preparing to record
    REC_STATE_RECORD,                   ///< Recording in progress
    REC_STATE_FINALIZE,                 ///< The recorder is finalizing the video
    REC_STATE_ABORT                     ///< Recording has been aborted
};
typedef REC_STATE RecState;

struct RecStateEnum : util::Reflection<RecStateEnum, RecState> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = REC_STATE_ABORT;

    static const char *prefix() { return "REC_STATE"; }
    static const char *_key(long value)
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
