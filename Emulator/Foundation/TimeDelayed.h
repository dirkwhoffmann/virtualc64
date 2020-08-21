// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _TIME_DELAYED_H
#define _TIME_DELAYED_H

template <class T> class TimeDelayed {
    
    /* Value pipeline (history buffer)
     *
     *    pipeline[0] : Value that was written at time timeStamp
     *    pipeline[n] : Value that was written at time timeStamp - n
     */
    T *pipeline = NULL;
    
    // Number of elements hold in pipeline
    u8 capacity = 0;
    
    // Remembers the time of the most recent call to write()
    i64 timeStamp = 0;
    
    // Number of cycles to elapse until a written value shows up
    u8 delay = 0;
    
    /// @brief Pointer to reference clock
    /// @todo  Change it to a reference
    i64 *clock = NULL;

    
    //
    // Initializing
    //
    
public:
    
    TimeDelayed(u8 delay, u8 capacity, u64 *clock);
    TimeDelayed(u8 delay, u8 capacity) : TimeDelayed(delay, capacity, NULL) { };
    TimeDelayed(u8 delay, u64 *clock) : TimeDelayed(delay, delay + 1, clock) { };
    TimeDelayed(u8 delay) : TimeDelayed(delay, delay + 1, NULL) { };
    
    ~TimeDelayed();
      
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
    
    void dump();

    //
    // Serializing
    //
    
public:
        
    size_t stateSize();
    void loadFromBuffer(u8 **buffer);
    void saveToBuffer(u8 **buffer);

    
    //
    // Accessing
    //
    
    // Write a value into the pipeline
    void write(T value) { writeWithDelay(value, 0); }
    
    // Work horse for writing a value
    void writeWithDelay(T value, u8 waitCycles);
    
    // Reads the most recent pipeline element
    T current() { return pipeline[0]; }
    
    // Reads a value from the pipeline with the standard delay
    // T delayed() { return pipeline[MAX(0, timeStamp - *clock + delay)]; }
    T delayed() {
        i64 offset = timeStamp - *clock + delay;
        if (__builtin_expect(offset <= 0, 1)) {
            return pipeline[0];
        } else {
            return pipeline[offset];
        }
    }
    
    // Reads a value from the pipeline with a custom delay
    T readWithDelay(u8 delay) {
        assert(delay <= this->capacity);
        return pipeline[MAX(0, timeStamp - *clock + delay)];
    }
};

#endif
