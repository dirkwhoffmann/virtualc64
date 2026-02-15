// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/storage/Buffer.h"
#include "utl/support/Bits.h"
#include "utl/abilities/Dumpable.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <sstream>

namespace utl {

template <class T>
Allocator<T>::Allocator(const Allocator& other)
{
    ptr = nullptr;
    size = other.size;

    if (size) {

        ptr = new T[size];
        memcpy(ptr, other.ptr, size * sizeof(T));
    }
}

template <class T> Allocator<T>&
Allocator<T>::operator=(const Allocator<T>& other)
{
    // Reallocate buffer
    alloc(other.size);
    assert(size == other.size);

    // Copy buffer
    if (size) std::memcpy(ptr, other.ptr, size * sizeof(T));
    return *this;
}

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
Allocator<T>::init(const string &str)
{
    init((const T *)str.c_str(), isize(str.length() / sizeof(T)));
}

template <class T> void
Allocator<T>::init(const Allocator<T> &other)
{
    init(other.ptr, other.size);
}

template <class T> void
Allocator<T>::init(const std::vector<T> &vector)
{
    isize vecsize = isize(vector.size());

    alloc(vecsize);
    for (isize i = 0; i < vecsize; i++) ptr[i] = vector[i];
}

template <class T> void
Allocator<T>::init(const fs::path &path)
{
    // Open stream in binary mode
    std::ifstream stream(path, std::ifstream::binary);

    // Return an empty buffer if the stream could not be opened
    if (!stream) { dealloc(); return; }

    // Read file contents into a string stream
    std::ostringstream sstr(std::ios::binary);
    sstr << stream.rdbuf();

    // Call the proper init delegate
    init(sstr.str());
}

template <class T> void
Allocator<T>::init(const fs::path &path, const string &name)
{
    init(path / name);
}

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
    if (ptr) replace((u8 *)ptr, bytesize(), seq, subst);
}

template <class T> void
Allocator<T>::patch(const char *seq, const char *subst)
{
    if (ptr) replace((char *)ptr, bytesize(), seq, subst);
}

/*
template <class T> void
Allocator<T>::dump(std::ostream &os, DumpOpt opt)
{
    Dumpable::dump(os, opt, ptr, size);
}

template <class T> void
Allocator<T>::dump(std::ostream &os, DumpOpt opt, const char *fmt)
{
    Dumpable::dump(os, opt, ptr, size, fmt);
}

template <class T> void
Allocator<T>::ascDump(std::ostream &os)
{
    dump(os, { .columns = 64, .offset = true, .ascii = true });
}

template <class T> void
Allocator<T>::hexDump(std::ostream &os)
{
    dump(os, { .base = 16, .columns = 64, .nr = true });
}

template <class T> void
Allocator<T>::memDump(std::ostream &os)
{
    dump(os, { .base = 16, .columns = 64, .offset = true, .ascii = true });
}

template <class T> void
Allocator<T>::type(std::ostream &os, DumpOpt opt)
{
    dump(os, opt, "%a");
}
*/

template <class T> void
Allocator<T>::compress(std::function<void(u8 *, isize, vector<u8> &)> algo, isize offset)
{
    std::vector<u8> compressed;

    // Skip everything up to the offset position
    compressed.insert(compressed.end(), ptr, ptr + std::min(offset, size));

    // Run the compressor
    if (size > offset) algo(ptr + offset, size - offset, compressed);

    // Replace buffer contents with the compressed data
    init(compressed);
}

template <class T> void
Allocator<T>::uncompress(std::function<void(u8 *, isize, vector<u8> &, isize)> algo, isize offset, isize sizeEstimate)
{
    std::vector<u8> uncompressed;

    // Skip everything up to the offset position
    uncompressed.insert(uncompressed.end(), ptr, ptr + std::min(offset, size));

    // Run the decompressor
    if (size > offset) algo(ptr + offset, size - offset, uncompressed, sizeEstimate);

    // Replace buffer contents with the uncompressed data
    init(uncompressed);
}

//
// Template instantiations
//

#define INSTANTIATE_ALLOCATOR(T) \
template Allocator<T>& Allocator<T>::operator=(const Allocator<T>& other); \
template void Allocator<T>::alloc(isize bytes); \
template void Allocator<T>::dealloc(); \
template void Allocator<T>::init(isize bytes, T value); \
template void Allocator<T>::init(const T *buf, isize len); \
template void Allocator<T>::init(const Allocator<T> &other); \
template void Allocator<T>::init(const fs::path &path); \
template void Allocator<T>::init(const fs::path &path, const string &name); \
template void Allocator<T>::resize(isize elements); \
template void Allocator<T>::resize(isize elements, T value); \
template void Allocator<T>::clear(T value, isize offset, isize len); \
template void Allocator<T>::copy(T *buf, isize offset, isize len) const; \
template void Allocator<T>::patch(const u8 *seq, const u8 *subst); \
template void Allocator<T>::patch(const char *seq, const char *subst);

INSTANTIATE_ALLOCATOR(u8)
INSTANTIATE_ALLOCATOR(u32)
INSTANTIATE_ALLOCATOR(u64)
INSTANTIATE_ALLOCATOR(isize)
INSTANTIATE_ALLOCATOR(float)
INSTANTIATE_ALLOCATOR(bool)

template void Allocator<u8>::compress(std::function<void (u8 *, isize, std::vector<u8> &)>, isize);
template void Allocator<u8>::uncompress(std::function<void (u8 *, isize, std::vector<u8> &, isize)>, isize, isize);

}
