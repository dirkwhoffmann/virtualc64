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

typedef struct
{
    // Refresh rate of the host display
    isize refreshRate;
    
    // Audio sample rate of the host computer
    isize sampleRate;
    
    // Framebuffer dimensions
    isize frameBufferWidth;
    isize frameBufferHeight;
}
HostConfig;

}
