// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Dumpable.h"
#include "utl/abilities/Hashable.h"
#include "utl/support/Bits.h"
#include "utl/io.h"
#include <cstdint>
#include <cassert>
#include <fstream>
#include <type_traits>
#include <span>

namespace utl {

template<typename T>
class BaseByteView : public Hashable, public Dumpable {

    static_assert(std::is_same_v<T, u8> || std::is_same_v<T, const u8>);

    std::span<T> sp{};

public:

    constexpr BaseByteView() = default;
    constexpr BaseByteView(T* data, isize size)
    {
        assert(size >= 0);
        sp = std::span(data, size_t(size));
    }

    constexpr explicit BaseByteView(const std::span<T> bytes)
    {
        sp = bytes;
    }

    constexpr BaseByteView(const BaseByteView<u8>& other)
    {
        sp = other.span();
    }

    constexpr T &operator[](isize i) const
    {
        assert(i >= 0 && i < isize(sp.size()));
        return sp[i];
    }

    constexpr operator std::span<T>() const
    {
        return sp;
    }

    constexpr isize size() const {
        return (isize)sp.size();
    }
    constexpr bool empty() const {
        return sp.empty();
    }
    constexpr T *data() const {
        return sp.data();
    }
    constexpr std::span<T> span() const {
        return sp;
    }
    constexpr std::span<T> subspan(isize o, isize c) const {
        assert(o >= 0 && c >= 0);
        return sp.subspan(o, c);
    }
    constexpr void clear(u8 value = 0) const requires (!std::is_const_v<T>) {
        for (usize i = 0; i < sp.size(); ++i) sp[i] = value;
    }
    template<std::size_t N>
    constexpr void clear(const std::array<u8, N>& values) const
    requires (!std::is_const_v<T>) {
        for (usize i = 0; i < sp.size(); ++i) sp[i] = values[i % N];
    }
    constexpr void clear(const vector<u8>& values) const
    requires (!std::is_const_v<T>) {
        for (usize i = 0; i < sp.size(); ++i) sp[i] = values[i % values.size()];
    }

    //
    // Methods from Hashable
    //

    u64 hash(HashAlgorithm algorithm) const override {
        return Hashable::hash(sp.data(), (isize)sp.size(), algorithm);
    }


    //
    // Methods from Dumpable
    //

    Dumpable::DataProvider dataProvider() const override {
        return Dumpable::dataProvider(sp);
    }


    //
    // Word and long word access
    //

    constexpr u16 readBE16(isize offset) const {
        assert(offset >= 0 && offset + 2 <= size());
        const u8* p = data() + offset;
        return u16(p[0] << 8) | u16(p[1]);
    }

    constexpr u16 readLE16(isize offset) const {
        assert(offset >= 0 && offset + 2 <= size());
        const u8* p = data() + offset;
        return u16(p[1] << 8) | u16(p[0]);
    }

    constexpr u32 readBE32(isize offset) const {
        assert(offset >= 0 && offset + 4 <= size());
        const u8* p = data() + offset;
        return u32(p[0] << 24) | u32(p[1] << 16) | u32(p[2] << 8) | u32(p[3]);
    }

    constexpr u32 readLE32(isize offset) const {
        assert(offset >= 0 && offset + 4 <= size());
        const u8* p = data() + offset;
        return u32(p[3] << 24) | u32(p[2] << 16) | u32(p[1] << 8) | u32(p[0]);
    }

    constexpr void writeBE16(isize offset, u16 value) const
    requires (!std::is_const_v<T>) {
        assert(offset >= 0 && offset + 2 <= size());
        u8* p = data() + offset;
        p[0] = u8(value >> 8);
        p[1] = u8(value);
    }

    constexpr void writeLE16(isize offset, u16 value) const
    requires (!std::is_const_v<T>) {
        assert(offset >= 0 && offset + 2 <= size());
        u8* p = data() + offset;
        p[0] = u8(value);
        p[1] = u8(value >> 8);
    }

    constexpr void writeBE32(isize offset, u32 value) const
    requires (!std::is_const_v<T>) {
        assert(offset >= 0 && offset + 4 <= size());
        u8* p = data() + offset;
        p[0] = u8(value >> 24);
        p[1] = u8(value >> 16);
        p[2] = u8(value >>  8);
        p[3] = u8(value);
    }

    constexpr void writeLE32(isize offset, u32 value) const
        requires (!std::is_const_v<T>) {
        assert(offset >= 0 && offset + 4 <= size());
        u8* p = data() + offset;
        p[0] = u8(value);
        p[1] = u8(value >>  8);
        p[2] = u8(value >> 16);
        p[3] = u8(value >> 24);
    }


    //
    // File I/O
    //

    isize writeToStream(std::ostream &stream) const
    {
        stream.write((char *)sp.data(), size());
        return size();
    }

    isize writeToFile(const fs::path &path) const
    {
        if (utl::isDirectory(path))
            throw IOError(IOError::FILE_IS_DIRECTORY);

        std::ofstream stream(path, std::ofstream::binary);

        if (!stream.is_open())
            throw IOError(IOError::FILE_CANT_WRITE, path);

        return writeToStream(stream);
    }


    //
    // Iterator
    //

    class iterator {

        const BaseByteView* view_;
        isize pos_;

    public:

        using iterator_category = std::random_access_iterator_tag;
        using value_type        = u8;
        using difference_type   = isize;
        using pointer           = void;
        using reference         = std::conditional_t<std::is_const_v<T>, const u8&, u8&>;

        constexpr iterator(const BaseByteView* view, isize pos) : view_(view), pos_(pos) {

            assert(view_);
        }

        // Dereference
        constexpr reference operator*() const { return (*view_)[pos_]; }

        // Increment / Decrement
        constexpr iterator& operator++() { ++pos_; return *this; }
        constexpr iterator& operator--() { --pos_; return *this; }

        // Random access
        constexpr reference operator[](difference_type n) const { return *(*this + n); }

    private:

        friend constexpr iterator operator+(const iterator& it, difference_type n) {
            return iterator(it.view_, it.pos_ + n);
        }
        friend constexpr iterator operator+(difference_type n, const iterator& it) {
            return iterator(it.view_, it.pos_ + n);
        }
        friend constexpr iterator operator-(const iterator& it, difference_type n) {
            return iterator(it.view_, it.pos_ - n);
        }
        friend constexpr difference_type operator-(const iterator& lhs, const iterator& rhs) {
            assert(lhs.view_ == rhs.view_); return lhs.pos_ - rhs.pos_;
        }
        friend constexpr bool operator==(const iterator& lhs, const iterator& rhs) {
            return lhs.view_ == rhs.view_ && lhs.pos_ == rhs.pos_;
        }
        friend constexpr bool operator!=(const iterator& lhs, const iterator& rhs) {
            return !(lhs == rhs);
        }
        friend constexpr bool operator<(const iterator& lhs, const iterator& rhs) {
            assert(lhs.view_ == rhs.view_); return lhs.pos_ < rhs.pos_;
        }
        friend constexpr bool operator<=(const iterator& lhs, const iterator& rhs) {
            return !(rhs < lhs);
        }
        friend constexpr bool operator>(const iterator& lhs, const iterator& rhs) {
            return rhs < lhs;
        }
        friend constexpr bool operator>=(const iterator& lhs, const iterator& rhs) {
            return !(lhs < rhs);
        }
    };

    constexpr iterator begin() const { return iterator(this, 0); }
    constexpr iterator end()   const { return iterator(this, size()); }


    //
    // Cyclic iterator
    //

    class cyclic_iterator {

        const BaseByteView* view_;
        isize pos_;

    public:

        using iterator_category = std::input_iterator_tag;
        using value_type        = u8;
        using difference_type   = isize;
        using pointer           = void;
        using reference         = std::conditional_t<std::is_const_v<T>, const u8&, u8&>;

        constexpr cyclic_iterator(const BaseByteView* view, isize pos = 0) : view_(view), pos_(pos) {

            assert(view_);
            assert(!view_->empty());
        }

        constexpr isize offset() const {

            auto n = view_->size();
            auto i = pos_ % n;
            return i < 0 ? i + n : i;
        }

        // Dereference with wrap
        constexpr reference operator*() const {

            return (*view_)[offset()];
        }

        // Increment / Decrement
        constexpr cyclic_iterator& operator++() {
            ++pos_; return *this;
        }
        constexpr cyclic_iterator& operator--() {
            --pos_; return *this;
        }
        constexpr cyclic_iterator operator++(int) {
            auto tmp = *this; ++(*this); return tmp;
        }
        constexpr cyclic_iterator operator--(int) {
            auto tmp = *this; ++(*this); return tmp;
        }

        // Random access
        constexpr reference operator[](difference_type n) const { return *(*this + n); }

    private:

        friend constexpr cyclic_iterator operator+(const cyclic_iterator& it, difference_type n) {
            return cyclic_iterator(it.view_, it.pos_ + n);
        }
        friend constexpr cyclic_iterator operator-(const cyclic_iterator& it, difference_type n) {
            return cyclic_iterator(it.view_, it.pos_ - n);
        }
        friend constexpr difference_type operator-(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            assert(lhs.view_ == rhs.view_); return lhs.pos_ - rhs.pos_;
        }
        friend constexpr bool operator==(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            return lhs.view_ == rhs.view_ && lhs.pos_ == rhs.pos_;
        }
        friend constexpr bool operator!=(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            return !(lhs == rhs);
        }
        friend constexpr bool operator<(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            assert(lhs.view_ == rhs.view_); return lhs.pos_ < rhs.pos_;
        }
        friend constexpr bool operator<=(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            return !(rhs < lhs);
        }
        friend constexpr bool operator>(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            return rhs < lhs;
        }
        friend constexpr bool operator>=(const cyclic_iterator& lhs, const cyclic_iterator& rhs) {
            return !(lhs < rhs);
        }
    };

    constexpr cyclic_iterator cyclic_begin(isize pos = 0) {
        return cyclic_iterator(this, pos);
    }
};

using ByteView        = BaseByteView<const u8>;
using MutableByteView = BaseByteView<u8>;

}
