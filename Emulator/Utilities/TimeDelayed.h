// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Macros.h"
#include <algorithm>

namespace util {

template <class T, isize delay> class TimeDelayed {
    
    static constexpr isize capacity = delay + 1;
    
    /* Value pipeline (history buffer)
     *
     *    pipeline[0] : Value that was written at time timeStamp
     *    pipeline[n] : Value that was written at time timeStamp - n
     */
    T pipeline[capacity];
    
    // Remembers the time of the most recent call to write()
    i64 timeStamp = 0;
    
    // Pointer to the reference clock
    i64 *clock = nullptr;
    
    
    //
    // Initializing
    //
    
public:
    
    TimeDelayed(u64 *clock) {
        
        timeStamp = 0;
        this->clock = (i64 *)clock;
        clear();
    }
    
    TimeDelayed() : TimeDelayed(nullptr) { };
    
    // Sets the reference clock (either the C64 clock or a drive clock)
    void setClock(u64 *clock) { this->clock = (i64 *)clock; }
    
    // Overwrites all pipeline entries with a reset value
    void reset(T value) {
        for (isize i = 0; i < capacity; i++) pipeline[i] = value;
        timeStamp = 0;
    }
    
    // Zeroes out all pipeline entries
    void clear() { reset((T)0); }
    
    // Checks if the pipeline is zeroed out
    bool isClear() {
        for (isize i = 0; i < capacity; i++) if (pipeline[i]) return false;
        return true;
    }
    
    
    //
    // Serializing
    //
    
public:
    
    template <class W>
    void operator<<(W& worker)
    {
        worker
        
        << pipeline
        << timeStamp;
    }
    
    
    //
    // Accessing
    //
    
    // Write a value into the pipeline
    void write(T value) { writeWithDelay(value, 0); }
    
    // Work horse for writing a value
    void writeWithDelay(T value, u8 waitCycles) {
        
        i64 referenceTime = *clock + waitCycles;
        
        // Shift pipeline
        i64 diff = referenceTime - timeStamp;
        for (isize i = capacity - 1; i >= 0; i--) {
            assert((i - diff <= 0) || (i - diff <= capacity));
            pipeline[i] = (i - diff > 0) ? pipeline[i - diff] : pipeline[0];
        }
        
        // Assign new value
        timeStamp = referenceTime;
        pipeline[0] = value;
    }
    
    // Reads the most recent pipeline element
    T current() const { return pipeline[0]; }
    
    // Reads a value from the pipeline with the standard delay
    T delayed() const {
        i64 offset = timeStamp - *clock + delay;
        if (likely(offset <= 0)) {
            return pipeline[0];
        } else {
            return pipeline[offset];
        }
    }
    
    // Reads a value from the pipeline with a custom delay
    T readWithDelay(u8 d) const {
        assert(d <= this->capacity);
        return pipeline[std::max(0LL, timeStamp - *clock + d)];
    }
};

}
