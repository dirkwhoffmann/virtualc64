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

#include "MemoryTypes.h"

namespace vc64 {

class Heatmap final {

public:

    // Heapmap data
    float heatmap[256][256] = { };

    // Cached accesses
    isize history[65536] = { };

    // Color palette
    u32 palette[256];


    //
    // Methods
    //

public:
    
    Heatmap();

    // Updates heatmap data
    void update(const class Memory &mem);

    // Draws a heatmap
    void draw(u32 *buffer, isize width, isize height) const;
};

}
