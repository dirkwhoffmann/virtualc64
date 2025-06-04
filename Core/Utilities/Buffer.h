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

#pragma once

#include "BasicTypes.h"
#include "Checksum.h"
#include <filesystem>
#include <vector>

namespace vc64::util {

namespace fs = ::std::filesystem;

template <class T> struct Allocator {
    
    static constexpr isize maxCapacity = 512 * 1024 * 1024;
    
    T *&ptr;
    isize size;
    
    Allocator(T *&ptr) : ptr(ptr), size(0) { ptr = nullptr; }
    Allocator(const Allocator&) = delete;
    ~Allocator() { dealloc(); }
    Allocator& operator= (const Allocator& other);
    
    // Queries the buffer state
    isize bytesize() const { return size * sizeof(T); }
    bool empty() const { return size == 0; }
    explicit operator bool() const { return !empty(); }
    
    // Initializers
    void alloc(isize elements);
    void dealloc();
    void init(isize elements, T value = 0);
    void init(const T *buf, isize elements);
    void init(const string &str);
    void init(const Allocator<T> &other);
    void init(const std::vector<T> &vector);
    void init(const fs::path &path);
    void init(const fs::path &path, const string &name);

    // Resizes an existing buffer
    void resize(isize elements);
    void resize(isize elements, T pad);
    
    // Overwrites elements with a default value
    void clear(T value, isize offset, isize len);
    void clear(T value = 0, isize offset = 0) { clear(value, offset, size - offset); }
    
    // Imports or exports the buffer contents
    void copy(T *buf, isize offset, isize len) const;
    void copy(T *buf) const { copy(buf, 0, size); }
    
    // Replaces a byte or character sequence by another one
    void patch(const u8 *seq, const u8 *subst);
    void patch(const char *seq, const char *subst);
    
    // Computes a checksum of a certain kind
    u32 fnv32() const { return ptr ? util::fnv32((u8 *)ptr, bytesize()) : 0; }
    u64 fnv64() const { return ptr ? util::fnv64((u8 *)ptr, bytesize()) : 0; }
    u16 crc16() const { return ptr ? util::crc16((u8 *)ptr, bytesize()) : 0; }
    u32 crc32() const { return ptr ? util::crc32((u8 *)ptr, bytesize()) : 0; }
    string md5() const { return ptr ? util::md5((u8 *)ptr, bytesize()) : 0; }

    // Compresses or uncompresses a buffer
    void compress(isize n = 2, isize offset = 0);
    void uncompress(isize n = 2, isize offset = 0);
};

template <class T> struct Buffer : public Allocator <T> {
    
    T *ptr = nullptr;
    
    Buffer()
    : Allocator<T>(ptr) { };
    Buffer(isize bytes)
    : Allocator<T>(ptr) { this->init(bytes); }
    Buffer(isize bytes, T value)
    : Allocator<T>(ptr) { this->init(bytes, value); }
    Buffer(const T *buf, isize len)
    : Allocator<T>(ptr) { this->init(buf, len); }
    Buffer(const fs::path &path)
    : Allocator<T>(ptr) { this->init(path); }
    Buffer(const fs::path &path, const string &name)
    : Allocator<T>(ptr) { this->init(path, name); }
    
    Buffer& operator= (const Buffer& other) { Allocator<T>::operator=(other); return *this; }

    T operator [] (isize i) const { return ptr[i]; }
    T &operator [] (isize i) { return ptr[i]; }
};

}
