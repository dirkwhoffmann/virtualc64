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

//#include <stdio.h>
#include "basic.h"
#include "TimeDelayed.h"

template <class T>
TimeDelayed<T>::TimeDelayed(uint8_t delay, uint64_t& clock) : clock(clock), delay(delay)
{
    pipeline = new T[delay + 1];
    reset(0);
}

template <class T>
TimeDelayed<T>::~TimeDelayed()
{
    assert(pipeline != NULL);
    delete pipeline;
}

template <class T>
void TimeDelayed<T>::reset(T value)
{
    for (unsigned i = 0; i < delay + 1; i++) {
        pipeline[i] = value;
    }
}

template <class T>
void TimeDelayed<T>::write(T value)
{
    // Shift pipeline
    int64_t diff = (int64_t)(clock - timeStamp);
    for (int i = delay; i >= 0; i--) {
        pipeline[i] = (i - diff > 0) ? pipeline[i - diff] : pipeline[0];
    }
    
    // Assign new value
    timeStamp = clock;
    pipeline[0] = value;
}

template <class T>
T TimeDelayed<T>::readWithDelay(long delay)
{
    assert(delay <= this->delay);

    // Determine correct pipeline position by comparing timeStamp with clock
    int64_t offset = MAX(0, (int64_t)timeStamp - (int64_t)clock + delay);
    return pipeline[offset];
}

template <class T>
void TimeDelayed<T>::debug()
{
    for (int i = delay; i >= 0; i--) {
        printf("%d ", (int)pipeline[i]);
    }
    printf("\n");
    
    for (int i = 0; i < delay; i++) {
        printf("readWithDelay(%d) = %d\n", i, readWithDelay(i));
    }
}



template <class T>
size_t TimeDelayed<T>::stateSize()
{
    return sizeof(uint64_t) * (delay + 1) + sizeof(timeStamp) + sizeof(delay);
}

template <class T>
void TimeDelayed<T>::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < delay + 1; i++) {
        pipeline[i] = (T)read64(buffer);
    }
    timeStamp = read64(buffer);
    delay = read8(buffer);
 
    assert(old - *buffer == stateSize());
}

template <class T>
void TimeDelayed<T>::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < delay + 1; i++) {
        write64(buffer, (uint64_t)pipeline[i]);
    }
    write64(timeStamp);
    write8(delay);
    
    assert(old - *buffer == stateSize());
}


