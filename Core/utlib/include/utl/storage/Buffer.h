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

template <class T> struct Buffer : public Hashable, public Dumpable {

    static constexpr isize maxCapacity = 512 * 1024 * 1024;

    T *ptr;
    isize size;
    T **managed;
        
    Buffer() : ptr(nullptr), size(0), managed(nullptr) { }
    Buffer(T **managed) : ptr(nullptr), size(0), managed(managed) { *managed = nullptr; }
    Buffer(const Buffer& other) : Buffer<T>() { init(other); }
    Buffer(isize bytes) : Buffer<T>() { this->init(bytes); }
    Buffer(isize bytes, T value) : Buffer<T>() { this->init(bytes, value); }
    Buffer(const T *buf, isize len) : Buffer<T>() { this->init(buf, len); }
    Buffer(const string &str) : Buffer<T>() { this->init(str); }
    Buffer(const std::vector<T> &vec) : Buffer<T>() { this->init(vec); }
    Buffer(const fs::path &path) : Buffer<T>() { this->init(path); }
    Buffer(const fs::path &path, const string &name) : Buffer<T>() { this->init(path, name); }
    ~Buffer() { dealloc(); }
    
    void init(const Buffer<T> &other);
    void init(isize elements);
    void init(isize elements, T value);
    void init(const T *buf, isize elements);
    void init(const string &str);
    void init(const std::vector<T> &vector);
    void init(const fs::path &path);
    // void init(const fs::path &path, const string &name);

    void manage(T** p) { managed = p; *p = ptr; }

    Buffer& operator = (const Buffer &other) { init(other); return *this; }
    T operator [] (isize i) const { return ptr[i]; }
    T &operator [] (isize i) { return ptr[i]; }
    

    
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

    // Allocates and frees memory
    void alloc(isize elements);
    void dealloc();

    // Resizes an existing buffer
    void resize(isize elements);
    void resize(isize elements, T pad);

    // Shrinks the buffer by removing elements at the front
    void strip(isize elements);
    
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

    // Exports the buffer
    void write(std::ostream &stream) const { write(stream, 0, size); }
    void write(std::ostream &stream, isize offset, isize len) const;
    void write(const fs::path &path) const { write(path, 0, size); }
    void write(const fs::path &path, isize offset, isize len) const;
    
private:

    void compress(std::function<void(u8 *,isize,std::vector<u8>&)> algo, isize offset = 0);
    void uncompress(std::function<void(u8 *,isize,std::vector<u8>&, isize)> algo, isize offset = 0, isize sizeEstimate = 0);
};

// Stream operators
template <class T>
std::ostream &operator<<(std::ostream &os, const Buffer<T> &buffer) {

    if (buffer.ptr && buffer.size > 0) {
        os.write((const char *)buffer.ptr, buffer.size * sizeof(T));
    }
    return os;
}

}
