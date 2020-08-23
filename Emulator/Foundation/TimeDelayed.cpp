// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "Utils.h"
#include "TimeDelayed.h"

/*
template <class T>
TimeDelayed<T>::TimeDelayed(u8 delay, u8 capacity, u64 *clock)
{
    assert(delay < capacity);
    
    timeStamp = 0;
    this->delay = delay;
    this->clock = (i64 *)clock;
    clear();
}
*/

/*
template TimeDelayed<bool>::TimeDelayed(u8, u8, u64 *);
template TimeDelayed<u8>::TimeDelayed(u8, u8, u64 *);
template TimeDelayed<u16>::TimeDelayed(u8, u8, u64 *);
template TimeDelayed<u32>::TimeDelayed(u8, u8, u64 *);
template TimeDelayed<u64>::TimeDelayed(u8, u8, u64 *);
*/

/*
template <class T>
void TimeDelayed<T>::writeWithDelay(T value, u8 waitCycles)
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
template void TimeDelayed<bool>::writeWithDelay(bool, u8);
template void TimeDelayed<u8>::writeWithDelay(u8, u8);
template void TimeDelayed<u16>::writeWithDelay(u16, u8);
template void TimeDelayed<u32>::writeWithDelay(u32, u8);
template void TimeDelayed<u64>::writeWithDelay(u64, u8);
*/

/*
template <class T>
void TimeDelayed<T>::dump()
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
template void TimeDelayed<bool>::dump();
template void TimeDelayed<u8>::dump();
template void TimeDelayed<u16>::dump();
template void TimeDelayed<u32>::dump();
template void TimeDelayed<u64>::dump();
*/

/*
template <class T>
size_t TimeDelayed<T>::stateSize()
{
    return capacity * sizeof(u64) + sizeof(timeStamp);
}
template size_t TimeDelayed<bool>::stateSize();
template size_t TimeDelayed<u8>::stateSize();
template size_t TimeDelayed<u16>::stateSize();
template size_t TimeDelayed<u32>::stateSize();
template size_t TimeDelayed<u64>::stateSize();


template <class T>
void TimeDelayed<T>::loadFromBuffer(u8 **buffer)
{
    u8 *old = *buffer;
    
    for (unsigned i = 0; i < capacity; i++) {
        pipeline[i] = (T)read64(buffer);
    }
    timeStamp = read64(buffer);
 
    assert(*buffer - old == stateSize());
}
template void TimeDelayed<bool>::loadFromBuffer(u8 **);
template void TimeDelayed<u8>::loadFromBuffer(u8 **);
template void TimeDelayed<u16>::loadFromBuffer(u8 **);
template void TimeDelayed<u32>::loadFromBuffer(u8 **);
template void TimeDelayed<u64>::loadFromBuffer(u8 **);


template <class T>
void TimeDelayed<T>::saveToBuffer(u8 **buffer)
{
    u8 *old = *buffer;
    
    for (unsigned i = 0; i < capacity; i++) {
        write64(buffer, (u64)pipeline[i]);
    }
    write64(buffer, timeStamp);
    
    assert(*buffer - old == stateSize());
}
template void TimeDelayed<bool>::saveToBuffer(u8 **);
template void TimeDelayed<u8>::saveToBuffer(u8 **);
template void TimeDelayed<u16>::saveToBuffer(u8 **);
template void TimeDelayed<u32>::saveToBuffer(u8 **);
template void TimeDelayed<u64>::saveToBuffer(u8 **);
*/
