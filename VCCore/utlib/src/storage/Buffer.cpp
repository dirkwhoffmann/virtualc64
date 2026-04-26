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

template <class T> void
Buffer<T>::init(const Buffer& other)
{
    // Reallocate buffer
    alloc(other.size);
    
    // Copy contents
    if (size) std::memcpy(ptr, other.ptr, size * sizeof(T));
}

template <class T> void
Buffer<T>::alloc(isize elements)
{
    assert(usize(elements) <= maxCapacity);
    assert((size == 0) == (ptr == nullptr));

    if (size != elements) try {

        dealloc();

        if (elements) {

            size = elements;
            ptr = new T[size];
            if (managed) *managed = ptr;
        }

    } catch (...) {

        size = 0;
        ptr = nullptr;
        if (managed) *managed = nullptr;
    }
}

template <class T> void
Buffer<T>::dealloc()
{
    assert((size == 0) == (ptr == nullptr));

    if (ptr) {

        delete [] ptr;
        ptr = nullptr;
        if (managed) *managed = nullptr;
        size = 0;
    }
}

template <class T> void
Buffer<T>::init(isize elements)
{
    init(elements, T{});
}

template <class T> void
Buffer<T>::init(isize elements, T value)
{
    alloc(elements);

    if (ptr) {

        for (isize i = 0; i < size; i++) {
            ptr[i] = value;
        }
    }
}

template <class T> void
Buffer<T>::init(const T *buf, isize elements)
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
Buffer<T>::init(const string &str)
{
    init((const T *)str.c_str(), isize(str.length() / sizeof(T)));
}

template <class T> void
Buffer<T>::init(const std::vector<T> &vector)
{
    isize vecsize = isize(vector.size());

    alloc(vecsize);
    for (isize i = 0; i < vecsize; i++) ptr[i] = vector[i];
}

template <class T> void
Buffer<T>::init(const fs::path &path)
{
    // Open stream in binary mode
    std::ifstream stream(path, std::ifstream::binary);

    if (!stream)
        throw IOError(IOError::FILE_CANT_READ, path);
    
    // Read file contents into a string stream
    std::ostringstream sstr(std::ios::binary);
    sstr << stream.rdbuf();

    // Call the proper init delegate
    init(sstr.str());
}

template <class T> void
Buffer<T>::resize(isize elements)
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
Buffer<T>::resize(isize elements, T pad)
{
    auto gap = elements > size ? elements - size : 0;

    resize(elements);
    clear(pad, elements - gap, gap);
}

template <class T> void
Buffer<T>::strip(isize elements)
{
    auto newSize = std::max(size - elements, isize(0));

    for (isize i = 0; i < newSize; i++) {
        ptr[i] = ptr[i + elements];
    }
    resize(newSize);
}

template <class T> void
Buffer<T>::clear(T value, isize offset, isize len)
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
Buffer<T>::copy(T *buf, isize offset, isize len) const
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
Buffer<T>::patch(const u8 *seq, const u8 *subst)
{
    if (ptr) replace((u8 *)ptr, bytesize(), seq, subst);
}

template <class T> void
Buffer<T>::patch(const char *seq, const char *subst)
{
    if (ptr) replace((char *)ptr, bytesize(), seq, subst);
}

template <class T> void
Buffer<T>::compress(std::function<void(u8 *, isize, vector<u8> &)> algo, isize offset)
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
Buffer<T>::uncompress(std::function<void(u8 *, isize, vector<u8> &, isize)> algo, isize offset, isize sizeEstimate)
{
    std::vector<u8> uncompressed;

    // Skip everything up to the offset position
    uncompressed.insert(uncompressed.end(), ptr, ptr + std::min(offset, size));

    // Run the decompressor
    if (size > offset) algo(ptr + offset, size - offset, uncompressed, sizeEstimate);

    // Replace buffer contents with the uncompressed data
    init(uncompressed);
}

template <class T> void
Buffer<T>::write(std::ostream &stream, isize offset, isize len) const
{
    assert(offset >= 0 && len >= 0 && offset + len <= size);
    stream.write((char *)ptr + offset, len);
}

template <class T> void
Buffer<T>::write(const fs::path &path, isize offset, isize len) const
{
    if (utl::isDirectory(path))
        throw IOError(IOError::FILE_IS_DIRECTORY);

    std::ofstream stream(path, std::ofstream::binary);

    if (!stream.is_open())
        throw IOError(IOError::FILE_CANT_WRITE, path);

    write(stream);
}


//
// Template instantiations
//

// template Allocator<T>::Allocator(const Allocator&);

#define INSTANTIATE_BUFFER(T) \
template Buffer<T>& Buffer<T>::operator=(const Buffer<T>& other); \
template void Buffer<T>::alloc(isize bytes); \
template void Buffer<T>::dealloc(); \
template void Buffer<T>::init(isize bytes); \
template void Buffer<T>::init(isize bytes, T value); \
template void Buffer<T>::init(const T *buf, isize len); \
template void Buffer<T>::init(const Buffer<T> &other); \
template void Buffer<T>::init(const fs::path &path); \
template void Buffer<T>::resize(isize elements); \
template void Buffer<T>::resize(isize elements, T value); \
template void Buffer<T>::strip(isize elements); \
template void Buffer<T>::clear(T value, isize offset, isize len); \
template void Buffer<T>::copy(T *buf, isize offset, isize len) const; \
template void Buffer<T>::patch(const u8 *seq, const u8 *subst); \
template void Buffer<T>::patch(const char *seq, const char *subst); \
template void Buffer<T>::write(std::ostream &stream, isize offset, isize len) const; \
template void Buffer<T>::write(const fs::path &path, isize offset, isize len) const;

INSTANTIATE_BUFFER(u8)
INSTANTIATE_BUFFER(u32)
INSTANTIATE_BUFFER(u64)
INSTANTIATE_BUFFER(isize)
INSTANTIATE_BUFFER(float)
INSTANTIATE_BUFFER(bool)

template void Buffer<u8>::compress(std::function<void (u8 *, isize, std::vector<u8> &)>, isize);
template void Buffer<u8>::uncompress(std::function<void (u8 *, isize, std::vector<u8> &, isize)>, isize, isize);

}
