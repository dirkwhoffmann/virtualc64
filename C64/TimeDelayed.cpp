/*!
 * @file       TimeDelayed.cpp
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "basic.h"
#include "TimeDelayed.h"

template <class T>
TimeDelayed<T>::TimeDelayed(uint8_t delay, uint8_t capacity, uint64_t *clock)
{
    assert(delay < capacity);
    
    pipeline = new T[capacity];
    timeStamp = 0;
    this->capacity = capacity;
    this->delay = delay;
    this->clock = (int64_t *)clock;
    clear();
}

template TimeDelayed<bool>::TimeDelayed(uint8_t, uint8_t, uint64_t *);
template TimeDelayed<uint8_t>::TimeDelayed(uint8_t, uint8_t, uint64_t *);
template TimeDelayed<uint16_t>::TimeDelayed(uint8_t, uint8_t, uint64_t *);
template TimeDelayed<uint32_t>::TimeDelayed(uint8_t, uint8_t, uint64_t *);
template TimeDelayed<uint64_t>::TimeDelayed(uint8_t, uint8_t, uint64_t *);

template <class T>
TimeDelayed<T>::~TimeDelayed()
{
    assert(pipeline != NULL);
    delete pipeline;
}
template TimeDelayed<bool>::~TimeDelayed();
template TimeDelayed<uint8_t>::~TimeDelayed();
template TimeDelayed<uint16_t>::~TimeDelayed();
template TimeDelayed<uint32_t>::~TimeDelayed();
template TimeDelayed<uint64_t>::~TimeDelayed();


template <class T>
void TimeDelayed<T>::writeWithDelay(T value, uint8_t waitCycles)
{
    int64_t referenceTime = *clock + waitCycles;
    
    // Shift pipeline
    int64_t diff = referenceTime - timeStamp;
    for (int i = this->capacity; i >= 0; i--) {
        assert((i - diff <= 0) || (i - diff <= this->capacity));
        pipeline[i] = (i - diff > 0) ? pipeline[i - diff] : pipeline[0];
    }
    
    // Assign new value
    timeStamp = referenceTime;
    pipeline[0] = value;
}
template void TimeDelayed<bool>::writeWithDelay(bool, uint8_t);
template void TimeDelayed<uint8_t>::writeWithDelay(uint8_t, uint8_t);
template void TimeDelayed<uint16_t>::writeWithDelay(uint16_t, uint8_t);
template void TimeDelayed<uint32_t>::writeWithDelay(uint32_t, uint8_t);
template void TimeDelayed<uint64_t>::writeWithDelay(uint64_t, uint8_t);

template <class T>
void TimeDelayed<T>::debug()
{
    for (int i = delay; i >= 0; i--) {
        printf("%llX ", (uint64_t)pipeline[i]);
    }
    printf("\n");
    
    printf("delayed() = %llX\n", (uint64_t)delayed());
    for (int i = 0; i <= delay; i++) {
        printf("readWithDelay(%d) = %llX\n", i, (uint64_t)readWithDelay(i));
    }
    printf("timeStamp = %lld clock = %lld delay = %d\n", timeStamp, *clock, delay);
}
template void TimeDelayed<bool>::debug();
template void TimeDelayed<uint8_t>::debug();
template void TimeDelayed<uint16_t>::debug();
template void TimeDelayed<uint32_t>::debug();
template void TimeDelayed<uint64_t>::debug();


template <class T>
size_t TimeDelayed<T>::stateSize()
{
    return capacity * sizeof(uint64_t) + sizeof(timeStamp);
}
template size_t TimeDelayed<bool>::stateSize();
template size_t TimeDelayed<uint8_t>::stateSize();
template size_t TimeDelayed<uint16_t>::stateSize();
template size_t TimeDelayed<uint32_t>::stateSize();
template size_t TimeDelayed<uint64_t>::stateSize();


template <class T>
void TimeDelayed<T>::loadFromBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < capacity; i++) {
        pipeline[i] = (T)read64(buffer);
    }
    timeStamp = read64(buffer);
 
    assert(*buffer - old == stateSize());
}
template void TimeDelayed<bool>::loadFromBuffer(uint8_t **);
template void TimeDelayed<uint8_t>::loadFromBuffer(uint8_t **);
template void TimeDelayed<uint16_t>::loadFromBuffer(uint8_t **);
template void TimeDelayed<uint32_t>::loadFromBuffer(uint8_t **);
template void TimeDelayed<uint64_t>::loadFromBuffer(uint8_t **);


template <class T>
void TimeDelayed<T>::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < capacity; i++) {
        write64(buffer, (uint64_t)pipeline[i]);
    }
    write64(buffer, timeStamp);
    
    assert(*buffer - old == stateSize());
}
template void TimeDelayed<bool>::saveToBuffer(uint8_t **);
template void TimeDelayed<uint8_t>::saveToBuffer(uint8_t **);
template void TimeDelayed<uint16_t>::saveToBuffer(uint8_t **);
template void TimeDelayed<uint32_t>::saveToBuffer(uint8_t **);
template void TimeDelayed<uint64_t>::saveToBuffer(uint8_t **);
