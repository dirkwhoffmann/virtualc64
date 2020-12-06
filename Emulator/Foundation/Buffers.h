/// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _BUFFERS_H
#define _BUFFERS_H

#include "C64Types.h"

template <class T, size_t capacity> struct RingBuffer
{
    // Element storage
    T elements[capacity];

    // Read and write pointers
    int r, w;

    //
    // Initializing
    //

    RingBuffer() { clear(); }
    
    void clear() { r = w = 0; }
    void clear(T t) { for (size_t i = 0; i < capacity; i++) elements[i] = t; clear(); }
    void align(int offset) { w = (r + offset) % capacity; }

    //
    // Serializing
    //

    template <class W>
    void applyToItems(W& worker)
    {
        worker & elements & r & w;
    }

    
    //
    // Querying the fill status
    //

    size_t cap() { return capacity; }
    size_t count() const { return (capacity + w - r) % capacity; }
    size_t free() const { return capacity - count() - 1; }
    double fillLevel() const { return (double)count() / capacity; }
    bool isEmpty() const { return r == w; }
    bool isFull() const { return free() == 0; }

    
    //
    // Working with indices
    //

    int begin() const { return r; }
    int end() const { return w; }
    static int next(int i) { return (capacity + i + 1) % capacity; }
    static int prev(int i) { return (capacity + i - 1) % capacity; }


    //
    // Reading and writing elements
    //

    T& current()
    {
        return elements[r];
    }

    T& current(int offset)
    {
        return elements[(r + offset) % capacity];
    }
    
    T& read()
    {
        assert(!isEmpty());

        int oldr = r;
        r = next(r);
        return elements[oldr];
    }

    void write(T element)
    {
        assert(!isFull());

        int oldw = w;
        w = next(w);
        elements[oldw] = element;
    }
};

#endif
