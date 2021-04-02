// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64Types.h"

template <class T, usize capacity> struct RingBuffer
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
    void clear(T t) { for (usize i = 0; i < capacity; i++) elements[i] = t; clear(); }
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

    usize cap() const { return capacity; }
    usize count() const { return (capacity + w - r) % capacity; }
    usize free() const { return capacity - count() - 1; }
    double fillLevel() const { return (double)count() / capacity; }
    bool isEmpty() const { return r == w; }
    bool isFull() const { return free() == 0; }

    
    //
    // Working with indices
    //

    static int next(int i) { return (capacity + i + 1) % capacity; }
    static int prev(int i) { return (capacity + i - 1) % capacity; }

    int begin() const { return r; }
    int end() const { return w; }


    //
    // Reading and writing elements
    //

    const T& current() const
    {
        return elements[r];
    }

    const T& current(int offset) const
    {
        return elements[(r + offset) % capacity];
    }
    
    const T& read()
    {
        assert(!isEmpty());

        auto oldr = r;
        r = next(r);
        return elements[oldr];
    }

    const T& read(T fallback)
    {
        if (isEmpty()) write(fallback);
        return read();
    }
    
    void write(T element)
    {
        assert(!isFull());

        auto oldw = w;
        w = next(w);
        elements[oldw] = element;
    }
};
