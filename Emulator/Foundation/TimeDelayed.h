// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _TIME_DELAYED_INC
#define _TIME_DELAYED_INC

template <class T> class TimeDelayed {
    
    private:
    
    /*! @brief    Value pipeline (history buffer)
     *  @details  Semantics:
     *            pipeline[0]: Value that was written at time timeStamp
     *            pipeline[n]: Value that was written at time timeStamp - n
     */
    T *pipeline = NULL;
    
    //! @brief   Number of elements hold in pipeline
    uint8_t capacity = 0;
    
    //! @brief  Remembers the time of the most recent call to write()
    i64 timeStamp = 0;
    
    //! @brief  Number of cycles to elapse until a written value shows up
    uint8_t delay = 0;
    
    //! @brief   Pointer to reference clock
    i64 *clock = NULL;
    
    public:
    
    //! @brief   Constructors
    TimeDelayed(uint8_t delay, uint8_t capacity, u64 *clock);
    TimeDelayed(uint8_t delay, uint8_t capacity) : TimeDelayed(delay, capacity, NULL) { };
    TimeDelayed(uint8_t delay, u64 *clock) : TimeDelayed(delay, delay + 1, clock) { };
    TimeDelayed(uint8_t delay) : TimeDelayed(delay, delay + 1, NULL) { };
    
    //! @brief   Destructor
    ~TimeDelayed();
    
    /*! @brief   Sets the reference clock
     *  @param   clock is either the clock of the C64 CPU or the clock of the
     *           a drive CPU.
     */
    void setClock(u64 *clock) { this->clock = (i64 *)clock; }
    
    //! @brief   Overwrites all pipeline entries with a reset value.
    void reset(T value) {
        for (unsigned i = 0; i < capacity; i++) pipeline[i] = value;
        timeStamp = 0;
    }
    
    //! @brief   Zeroes out all pipeline entries.
    void clear() { reset((T)0); }
    
    //! @brief   Write a value into the pipeline.
    void write(T value) { writeWithDelay(value, 0); }
    
    //! @brief   Work horse for writing a value.
    void writeWithDelay(T value, uint8_t waitCycles);
    
    //! @brief   Reads the most recent pipeline element.
    T current() { return pipeline[0]; }
    
    //! @brief   Reads a value from the pipeline with the standard delay.
    // T delayed() { return pipeline[MAX(0, timeStamp - *clock + delay)]; }
    T delayed() {
        i64 offset = timeStamp - *clock + delay;
        if (__builtin_expect(offset <= 0, 1)) {
            return pipeline[0];
        } else {
            return pipeline[offset];
        }
    }
    
    //! @brief   Reads a value from the pipeline with a custom delay.
    T readWithDelay(uint8_t delay) {
        assert(delay <= this->capacity);
        return pipeline[MAX(0, timeStamp - *clock + delay)];
    }
    
    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    void debug();
};

#endif
