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

#pragma once

#include "BasicTypes.h"

namespace vc64 {

//
// Structures
//

/// Configuration of the video port
typedef struct
{
    bool whiteNoise;                        ///< Show white noise while the machine is off
}

VideoPortConfig;

/// State of the video port
typedef struct
{
    i64 latestGrabbedFrame;                 ///< Number of the most recent frame that was grabbed by the host
}
VideoPortInfo;

/// Statistics of the video port
typedef struct
{
    isize droppedFrames;                    ///< Number of frames that were not grabbed in time
}
VideoPortStats;

}
