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
    // Duration of the recorded Video
    double duration;
}
RecorderInfo;
