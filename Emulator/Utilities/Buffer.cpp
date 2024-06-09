// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Buffer.h"
#include "IOUtils.h"
#include "MemUtils.h"
#include <fstream>

namespace vc64::util {

template <class T> void
Allocator<T>::alloc(isize elements)
{
    assert(usize(elements) <= maxCapacity);
    assert((size == 0) == (ptr == nullptr));
    
    if (size != elements) try {
        
        dealloc();
        
        if (elements) {
            
            size = elements;
            ptr = new T[size];
        }
        
    } catch (...) {
        
        size = 0;
        ptr = nullptr;
    }
}

template <class T> void
Allocator<T>::dealloc()
{
    assert((size == 0) == (ptr == nullptr));
    
    if (ptr) {
        
        delete [] ptr;
        ptr = nullptr;
        size = 0;
    }
}

template <class T> void
Allocator<T>::init(isize elements, T value)
{
    alloc(elements);
    
    if (ptr) {
        
        for (isize i = 0; i < size; i++) {
            ptr[i] = value;
        }
    }
}

template <class T> void
Allocator<T>::init(const T *buf, isize elements)
{
    assert(buf);
    
    alloc(elements);
    
    if (ptr) {
        
        for (isize i = 0; i < size; i++) {
            ptr[i] = buf[i];
        }
    }
}

template <class T> void
Allocator<T>::init(const Allocator<T> &other)
{
    init(other.ptr, other.size);
}

template <class T> void
Allocator<T>::init(const fs::path &path)
{
    // Open stream in binary mode
    std::ifstream stream(path, std::ifstream::binary);
    
    // Return an empty buffer if the stream could not be opened
    if (!stream) { dealloc(); return; }
    
    // Get the stream length in bytes
    auto length = streamLength(stream);
    
    // Create a buffer of proper size
    if (length % sizeof(T)) {
        init(length / sizeof(T) + 1);
    } else {
        alloc(length / sizeof(T));
    }
    
    // Read from stream
    stream.read((char *)ptr, length);
}

/*
template <class T> void
Allocator<T>::init(const fs::path &path, const string &name)
{
    init(path + "/" + name);
}
*/

template <class T> void
Allocator<T>::resize(isize elements)
{
    assert((size == 0) == (ptr == nullptr));
    
    if (size != elements) {
        
        if (elements == 0) {
            
            dealloc();
            
        } else try {
            
            auto newPtr = new T[elements];
            copy(newPtr, 0, std::min(size, elements));
            dealloc();
            ptr = (T *)newPtr;
            size = elements;
            
        } catch (...) {
            
            size = 0;
            ptr = nullptr;
        }
    }
}

template <class T> void
Allocator<T>::resize(isize elements, T pad)
{
    auto gap = elements > size ? elements - size : 0;
    
    resize(elements);
    clear(pad, elements - gap, gap);
}

template <class T> void
Allocator<T>::clear(T value, isize offset, isize len)
{
    assert((size == 0) == (ptr == nullptr));
    assert(offset >= 0 && len >= 0 && offset + len <= size);
    
    if (ptr) {
        
        for (isize i = 0; i < len; i++) {
            ptr[i + offset] = value;
        }
    }
}

template <class T> void
Allocator<T>::copy(T *buf, isize offset, isize len) const
{
    assert(buf);
    assert((size == 0) == (ptr == nullptr));
    assert(offset >= 0 && len >= 0 && offset + len <= size);
    
    if (ptr) {
        
        for (isize i = 0; i < len; i++) {
            buf[i] = ptr[i + offset];
        }
    }
}

template <class T> void
Allocator<T>::patch(const u8 *seq, const u8 *subst)
{
    if (ptr) util::replace((u8 *)ptr, bytesize(), seq, subst);
}

template <class T> void
Allocator<T>::patch(const char *seq, const char *subst)
{
    if (ptr) util::replace((char *)ptr, bytesize(), seq, subst);
}

//
// Template instantiations
//

#define INSTANTIATE_ALLOCATOR(T) \
template void Allocator<T>::alloc(isize bytes); \
template void Allocator<T>::dealloc(); \
template void Allocator<T>::init(isize bytes, T value); \
template void Allocator<T>::init(const T *buf, isize len); \
template void Allocator<T>::init(const Allocator<T> &other); \
template void Allocator<T>::init(const fs::path &path); \
template void Allocator<T>::resize(isize elements); \
template void Allocator<T>::resize(isize elements, T value); \
template void Allocator<T>::clear(T value, isize offset, isize len); \
template void Allocator<T>::copy(T *buf, isize offset, isize len) const; \
template void Allocator<T>::patch(const u8 *seq, const u8 *subst); \
template void Allocator<T>::patch(const char *seq, const char *subst);

INSTANTIATE_ALLOCATOR(u8)
INSTANTIATE_ALLOCATOR(u32)
INSTANTIATE_ALLOCATOR(float)

}
