// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

template <class T, int capacity> class TimeDelayed {
    
    /* Value pipeline (history buffer)
     *
     *    pipeline[0] : Value that was written at time timeStamp
     *    pipeline[n] : Value that was written at time timeStamp - n
     */
    T pipeline[capacity];
        
    // Remembers the time of the most recent call to write()
    i64 timeStamp = 0;
    
    // Number of cycles to elapse until a written value shows up
    u8 delay = 0;
    
    // Pointer to reference clock
    i64 *clock = nullptr;

    
    //
    // Initializing
    //
    
public:
    
    TimeDelayed(u8 delay, u64 *clock) {

        assert(delay < capacity);
        
        timeStamp = 0;
        this->delay = delay;
        this->clock = (i64 *)clock;
        clear();
    }
    
    TimeDelayed(u8 delay) : TimeDelayed(delay, nullptr) { };
          
    // Sets the reference clock (either the C64 clock or a drive clock)
    void setClock(u64 *clock) { this->clock = (i64 *)clock; }

    // Overwrites all pipeline entries with a reset value
    void reset(T value) {
        for (unsigned i = 0; i < capacity; i++) pipeline[i] = value;
        timeStamp = 0;
    }
    
    // Zeroes out all pipeline entries
    void clear() { reset((T)0); }
    
    
    //
    // Analyzing
    //
    
public:
    
    // void dump();

    //
    // Serializing
    //
    
public:
        
    template <class W>
    void applyToItems(W& worker)
    {
        worker & pipeline & timeStamp;
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
        for (int i = capacity - 1; i >= 0; i--) {
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
    // T delayed() { return pipeline[MAX(0, timeStamp - *clock + delay)]; }
    T delayed() const {
        i64 offset = timeStamp - *clock + delay;
        if (__builtin_expect(offset <= 0, 1)) {
            return pipeline[0];
        } else {
            return pipeline[offset];
        }
    }
    
    // Reads a value from the pipeline with a custom delay
    T readWithDelay(u8 delay) const {
        assert(delay <= this->capacity);
        return pipeline[MAX(0, timeStamp - *clock + delay)];
    }
};
