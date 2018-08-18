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

template TimeDelayed<bool>::TimeDelayed(uint8_t delay, uint64_t& clock);
template TimeDelayed<uint8_t>::TimeDelayed(uint8_t delay, uint64_t& clock);
template TimeDelayed<uint64_t>::TimeDelayed(uint8_t delay, uint64_t& clock);


template <class T>
TimeDelayed<T>::~TimeDelayed()
{
    assert(pipeline != NULL);
    delete pipeline;
}

template TimeDelayed<bool>::~TimeDelayed();
template TimeDelayed<uint8_t>::~TimeDelayed();
template TimeDelayed<uint64_t>::~TimeDelayed();


template <class T>
void TimeDelayed<T>::reset(T value)
{
    for (unsigned i = 0; i < delay + 1; i++) {
        pipeline[i] = value;
    }
}

template void TimeDelayed<bool>::reset(bool value);
template void TimeDelayed<uint8_t>::reset(uint8_t value);
template void TimeDelayed<uint64_t>::reset(uint64_t value);


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

template void TimeDelayed<bool>::write(bool value);
template void TimeDelayed<uint8_t>::write(uint8_t value);
template void TimeDelayed<uint64_t>::write(uint64_t value);


template <class T>
T TimeDelayed<T>::readWithDelay(uint8_t delay)
{
    assert(delay <= this->delay);

    // Determine correct pipeline position by comparing timeStamp with clock
    int64_t offset = MAX(0, (int64_t)timeStamp - (int64_t)clock + delay);
    return pipeline[offset];
}

template bool TimeDelayed<bool>::readWithDelay(uint8_t delay);
template uint8_t TimeDelayed<uint8_t>::readWithDelay(uint8_t delay);
template uint64_t TimeDelayed<uint64_t>::readWithDelay(uint8_t delay);


template <class T>
void TimeDelayed<T>::debug()
{
    for (int i = delay; i >= 0; i--) {
        printf("%d ", (int)pipeline[i]);
    }
    printf("\n");
    
    for (int i = 0; i < delay; i++) {
        printf("readWithDelay(%d) = %lld\n", i, (uint64_t)readWithDelay(i));
    }
}

template void TimeDelayed<bool>::debug();
template void TimeDelayed<uint8_t>::debug();
template void TimeDelayed<uint64_t>::debug();


template <class T>
size_t TimeDelayed<T>::stateSize()
{
    return (delay + 1) * sizeof(uint64_t) + sizeof(timeStamp) + sizeof(delay);
}

template size_t TimeDelayed<bool>::stateSize();
template size_t TimeDelayed<uint8_t>::stateSize();
template size_t TimeDelayed<uint64_t>::stateSize();


template <class T>
void TimeDelayed<T>::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < delay + 1; i++) {
        pipeline[i] = (T)read64(buffer);
    }
    timeStamp = read64(buffer);
    delay = read8(buffer);
 
    assert(*buffer - old == stateSize());
}

template void TimeDelayed<bool>::loadFromBuffer(uint8_t **buffer);
template void TimeDelayed<uint8_t>::loadFromBuffer(uint8_t **buffer);
template void TimeDelayed<uint64_t>::loadFromBuffer(uint8_t **buffer);


template <class T>
void TimeDelayed<T>::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < delay + 1; i++) {
        write64(buffer, (uint64_t)pipeline[i]);
    }
    write64(buffer, timeStamp);
    write8(buffer, delay);
    
    assert(*buffer - old == stateSize());
}

template void TimeDelayed<bool>::saveToBuffer(uint8_t **buffer);
template void TimeDelayed<uint8_t>::saveToBuffer(uint8_t **buffer);
template void TimeDelayed<uint64_t>::saveToBuffer(uint8_t **buffer);
