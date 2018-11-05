/*!
 * @header      TimeDelayed.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, all rights reserved.
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _TIME_DELAYED_INC
#define _TIME_DELAYED_INC

template <class T> class TimeDelayed {
    
public:
    
    /*! @brief    Value pipeline (history buffer)
     *  @details  Semantics:
     *            pipeline[0]: Value that was written at time timeStamp
     *            pipeline[n]: Value that was written at time timeStamp - n
     */
    T *pipeline = NULL;
    
private:
    
    //! @brief   Number of elements hold in pipeline
    uint8_t capacity = 0;

    //! @brief  Remembers the time of the most recent call to write()
    int64_t timeStamp = 0;
    
    //! @brief  Number of cycles to elapse until a written value shows up
    uint8_t delay = 0;
    
    //! @brief   Pointer to reference clock
    int64_t *clock = NULL;
    
public:
    
    //! @brief   Constructors
    TimeDelayed(uint8_t delay, uint8_t capacity, uint64_t *clock);
    TimeDelayed(uint8_t delay, uint8_t capacity) : TimeDelayed(delay, capacity, NULL) { };
    TimeDelayed(uint8_t delay, uint64_t *clock) : TimeDelayed(delay, delay + 1, clock) { };
    TimeDelayed(uint8_t delay) : TimeDelayed(delay, delay + 1, NULL) { };
    
    //! @brief   Destructor
    ~TimeDelayed();
    
    //! @brief   Sets the reference clock
    void setClock(uint64_t *clock) { this->clock = (int64_t *)clock; }
    
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
        int64_t offset = timeStamp - *clock + delay;
        if (__builtin_expect(offset <= 0, 1))
            return pipeline[0];
        else
            return pipeline[offset];
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
