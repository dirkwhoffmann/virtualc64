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

/* Records memory accesses for the X-Ray visualizations. It keeps running
 * read / write counters per 256-byte page and, optionally, a ring buffer with
 * the most recent accesses. It is neither exposed through the API nor
 * snapshotted. Recording is off unless an X-Ray heat map mode is active, so
 * the cost for everyone else is a single branch per access.
 */
class AccessTrace final {

public:

    // Number of remembered accesses (must be a power of two)
    static constexpr usize capacity = 1 << 14;

    struct Entry {

        u16 addr;
        bool write;
    };

    Entry entries[capacity] = { };

    // Running access counters per page (they wrap around, use differences)
    u32 reads[256] = { };
    u32 writes[256] = { };

    // Enables the ring buffer
    bool ring = false;

    // Total number of recorded accesses (the next slot is head % capacity)
    u64 head = 0;

    // Enables or disables recording
    bool enabled = false;

    void record(u16 addr, bool write)
    {
        (write ? writes : reads)[addr >> 8]++;
        if (ring) entries[head++ & (capacity - 1)] = { addr, write };
    }

    // Returns the number of valid entries
    usize count() const { return head < capacity ? usize(head) : capacity; }

    // Returns an entry by age (0 = oldest, count() - 1 = most recent)
    const Entry &operator[](usize age) const
    {
        return entries[(head - count() + age) & (capacity - 1)];
    }

    void clear() { head = 0; }
};

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
