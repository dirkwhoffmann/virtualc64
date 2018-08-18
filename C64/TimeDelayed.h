/*!
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann, 2018. All rights reserved.
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

#include <stdint.h>

template <class T> class TimeDelayed {
    
    /*! @brief    Value pipeline (history buffer)
     *  @details  Semantics:
     *            pipeline[0]: Value that was written at time timeStamp
     *            pipeline[n]: Value that was written at time timeStamp - n
     */
    T *pipeline = NULL;
    
    //! @brief  Remembers the time of the most recent call to write()
    uint64_t timeStamp;
    
    //! @brief   Cycles to elapse until a written value shows up.
    uint8_t delay;
    
    //! @brief   Reference clock for this variable
    uint64_t& clock;
    
public:
    
    //! @brief   Constructor
    TimeDelayed(uint8_t delay, uint64_t& clock);
    
    //! @brief   Destructor
    ~TimeDelayed();
    
    //! @brief   Reset with a default value
    void reset(T value);

    //! @brief   Write a value into the pipeline.
    void write(T value) { writeWithClock(value, clock); }

    //! @brief   Work horse for write().
    void writeWithClock(T value, uint64_t clock);
    
    //! @brief   Reads a value from the pipeline with the standard delay.
    T read() { return readWithDelay(delay); }

    //! @brief   Work horse for read().
    T readWithDelay(uint8_t delay);

    size_t stateSize();
    void loadFromBuffer(uint8_t **buffer);
    void saveToBuffer(uint8_t **buffer);
    
    void debug();
};

#endif
