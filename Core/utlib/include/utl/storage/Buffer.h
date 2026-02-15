// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Compressible.h"
#include "utl/abilities/Hashable.h"
#include "utl/abilities/Dumpable.h"
#include "utl/primitives/BitView.h"
#include <ostream>
#include <type_traits>


namespace utl {

template <class T> struct Allocator : public Hashable, public Dumpable {

    static constexpr isize maxCapacity = 512 * 1024 * 1024;

    T *&ptr;
    isize size;

    Allocator(T *&ptr) : ptr(ptr), size(0) { ptr = nullptr; }
    Allocator(const Allocator&);
    Allocator& operator= (const Allocator&);
    ~Allocator() { dealloc(); }

    //
    // Methods from Hashable
    //

    u64 hash(HashAlgorithm algorithm) const noexcept override {
        return Hashable::hash((const u8 *)ptr, size, algorithm);
    }


    //
    // Methods from Dumpable
    //

    Dumpable::DataProvider dataProvider() const override {
        return Dumpable::dataProvider((const u8 *)ptr, size);
    }


    //
    // Methods
    //


    // Queries the buffer state
    isize bytesize() const { return size * sizeof(T); }
    bool empty() const { return size == 0; }
    explicit operator bool() const { return !empty(); }
    ByteView byteView() const { return ByteView(ptr, size); }


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

    // Compresses or uncompresses a buffer
    void gzip(isize offset = 0) {
        compress(Compressible::gzip, offset);
    }
    void gunzip(isize offset = 0, isize sizeEstimate = 0) {
        uncompress(Compressible::gunzip, offset, sizeEstimate);
    }
    void lz4(isize offset = 0) {
        compress(Compressible::lz4, offset);
    }
    void unlz4(isize offset = 0, isize sizeEstimate = 0) {
        uncompress(Compressible::unlz4, offset, sizeEstimate);
    }
    void rle2(isize offset = 0) {
        compress(Compressible::rle2, offset);
    }
    void unrle2(isize offset = 0, isize sizeEstimate = 0) {
        uncompress(Compressible::unrle2, offset, sizeEstimate);
    }
    void rle3(isize offset = 0) {
        compress(Compressible::rle3, offset);
    }
    void unrle3(isize offset = 0, isize sizeEstimate = 0) {
        uncompress(Compressible::unrle3, offset, sizeEstimate);
    }

private:

    void compress(std::function<void(u8 *,isize,std::vector<u8>&)> algo, isize offset = 0);
    void uncompress(std::function<void(u8 *,isize,std::vector<u8>&, isize)> algo, isize offset = 0, isize sizeEstimate = 0);
};

template <class T> struct Buffer : public Allocator <T> {

    T *ptr = nullptr;

    Buffer() : Allocator<T>(ptr) { };
    Buffer(isize bytes) : Allocator<T>(ptr) { this->init(bytes); }
    Buffer(isize bytes, T value) : Allocator<T>(ptr) { this->init(bytes, value); }
    Buffer(const T *buf, isize len) : Allocator<T>(ptr) { this->init(buf, len); }
    Buffer(const fs::path &path) : Allocator<T>(ptr) { this->init(path); }
    Buffer(const fs::path &path, const string &name) : Allocator<T>(ptr) { this->init(path, name); }

    Buffer& operator= (const Buffer& other) { Allocator<T>::operator=(other); return *this; }

    T operator [] (isize i) const { return ptr[i]; }
    T &operator [] (isize i) { return ptr[i]; }
};

// Stream operators
template <class T>
std::ostream &operator<<(std::ostream &os, const Allocator<T> &buffer) {

    if (buffer.ptr && buffer.size > 0) {
        os.write((const char *)buffer.ptr, buffer.size * sizeof(T));
    }
    return os;
}

}
