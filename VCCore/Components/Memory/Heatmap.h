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

/* Counts memory accesses for the X-Ray visualizations. It keeps running
 * read / write counters per address. It is neither exposed through the API nor
 * snapshotted. Recording is off unless an X-Ray mode that needs it is active,
 * so the cost for everyone else is a single branch per access.
 */
class AccessTrace final {

public:

    // Running access counters per address (they wrap around, use differences)
    u32 reads[65536] = { };
    u32 writes[65536] = { };

    // Enables or disables recording
    bool enabled = false;

    void record(u16 addr, bool write)
    {
        (write ? writes : reads)[addr]++;
    }
};

/* The heatmap shows the recent activity of the 64 KB address space as a grid
 * of 256 x 256 cells. Neighboring addresses are mapped to neighboring cells by
 * interleaving the address bits, so that address ranges form compact blocks.
 * A cell lights up when its address is accessed and fades out over time.
 */
class Heatmap final {

public:

    // Grid dimensions after accumulating 4 x 8 cells
    static constexpr isize cols = 64;
    static constexpr isize rows = 32;

private:

    // Heat per cell
    float heatmap[256][256] = { };

    // Access counters at the time of the last update
    u32 history[65536] = { };


    //
    // Methods
    //

public:

    // Forgets all heat and starts counting from the current access counters
    void reset(const AccessTrace &trace);

    // Updates the heat (call once per frame)
    void update(const AccessTrace &trace);

    // Computes the brightness (0 ... 1) of each block of 4 x 8 cells
    void compute(float (&levels)[rows][cols]) const;
};

}
