// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "basic.h"
#include "TimeDelayed.h"

template <class T>
TimeDelayed<T>::TimeDelayed(uint8_t delay, uint8_t capacity, u64 *clock)
{
    assert(delay < capacity);
    
    pipeline = new T[capacity];
    timeStamp = 0;
    this->capacity = capacity;
    this->delay = delay;
    this->clock = (i64 *)clock;
    clear();
}

template TimeDelayed<bool>::TimeDelayed(uint8_t, uint8_t, u64 *);
template TimeDelayed<uint8_t>::TimeDelayed(uint8_t, uint8_t, u64 *);
template TimeDelayed<u16>::TimeDelayed(uint8_t, uint8_t, u64 *);
template TimeDelayed<u32>::TimeDelayed(uint8_t, uint8_t, u64 *);
template TimeDelayed<u64>::TimeDelayed(uint8_t, uint8_t, u64 *);

template <class T>
TimeDelayed<T>::~TimeDelayed()
{
    assert(pipeline != NULL);
    delete pipeline;
}
template TimeDelayed<bool>::~TimeDelayed();
template TimeDelayed<uint8_t>::~TimeDelayed();
template TimeDelayed<u16>::~TimeDelayed();
template TimeDelayed<u32>::~TimeDelayed();
template TimeDelayed<u64>::~TimeDelayed();


template <class T>
void TimeDelayed<T>::writeWithDelay(T value, uint8_t waitCycles)
{
    i64 referenceTime = *clock + waitCycles;
    
    // Shift pipeline
    i64 diff = referenceTime - timeStamp;
    for (int i = this->capacity - 1; i >= 0; i--) {
        assert((i - diff <= 0) || (i - diff <= this->capacity));
        pipeline[i] = (i - diff > 0) ? pipeline[i - diff] : pipeline[0];
    }
    
    // Assign new value
    timeStamp = referenceTime;
    pipeline[0] = value;
}
template void TimeDelayed<bool>::writeWithDelay(bool, uint8_t);
template void TimeDelayed<uint8_t>::writeWithDelay(uint8_t, uint8_t);
template void TimeDelayed<u16>::writeWithDelay(u16, uint8_t);
template void TimeDelayed<u32>::writeWithDelay(u32, uint8_t);
template void TimeDelayed<u64>::writeWithDelay(u64, uint8_t);

template <class T>
void TimeDelayed<T>::debug()
{
    for (int i = delay; i >= 0; i--) {
        printf("%llX ", (u64)pipeline[i]);
    }
    printf("\n");
    
    printf("delayed() = %llX\n", (u64)delayed());
    for (int i = 0; i <= delay; i++) {
        printf("readWithDelay(%d) = %llX\n", i, (u64)readWithDelay(i));
    }
    printf("timeStamp = %lld clock = %lld delay = %d\n", timeStamp, *clock, delay);
}
template void TimeDelayed<bool>::debug();
template void TimeDelayed<uint8_t>::debug();
template void TimeDelayed<u16>::debug();
template void TimeDelayed<u32>::debug();
template void TimeDelayed<u64>::debug();


template <class T>
size_t TimeDelayed<T>::stateSize()
{
    return capacity * sizeof(u64) + sizeof(timeStamp);
}
template size_t TimeDelayed<bool>::stateSize();
template size_t TimeDelayed<uint8_t>::stateSize();
template size_t TimeDelayed<u16>::stateSize();
template size_t TimeDelayed<u32>::stateSize();
template size_t TimeDelayed<u64>::stateSize();


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
template void TimeDelayed<u16>::loadFromBuffer(uint8_t **);
template void TimeDelayed<u32>::loadFromBuffer(uint8_t **);
template void TimeDelayed<u64>::loadFromBuffer(uint8_t **);


template <class T>
void TimeDelayed<T>::saveToBuffer(uint8_t **buffer)
{
    uint8_t *old = *buffer;
    
    for (unsigned i = 0; i < capacity; i++) {
        write64(buffer, (u64)pipeline[i]);
    }
    write64(buffer, timeStamp);
    /*
    printf("SAVING: capacity = %d reference = %lld timeStamp = %lld\n",
           capacity, *clock, timeStamp);
    */
    
    assert(*buffer - old == stateSize());
}
template void TimeDelayed<bool>::saveToBuffer(uint8_t **);
template void TimeDelayed<uint8_t>::saveToBuffer(uint8_t **);
template void TimeDelayed<u16>::saveToBuffer(uint8_t **);
template void TimeDelayed<u32>::saveToBuffer(uint8_t **);
template void TimeDelayed<u64>::saveToBuffer(uint8_t **);
