// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once


#include "utl/primitives/ByteView.h"

namespace utl {

template <typename T>
class BaseBitView {

public:

    template <typename> friend class BaseBitView;
    static_assert(std::is_same_v<T, u8> || std::is_same_v<T, const u8>);

    std::span<T> sp{};  // Underlying data buffer
    isize first = 0;    // View window start (inclusive)
    isize last  = 0;    // View window end (exclusive)

public:

    BaseBitView() = default;
    BaseBitView(const BaseBitView&) = default;
    BaseBitView& operator=(const BaseBitView&) = default;
    BaseBitView(BaseBitView&&) = default;
    BaseBitView& operator=(BaseBitView&&) = default;

    constexpr BaseBitView(T* data, isize bitCount)
    : sp(data, (bitCount + 7) / 8), first(0), last(bitCount)
    {
        assert(bitCount >= 0);
        assert(isize(sp.size()) * 8 >= bitCount);
    }

    constexpr BaseBitView(std::span<T> bytes, isize bitCount)
    : sp(bytes), first(0), last(bitCount)
    {
        assert(bitCount >= 0);
        assert(isize(sp.size()) * 8 >= bitCount);
    }

    constexpr BaseBitView(std::span<T> bytes, isize first, isize last)
    : sp(bytes), first(first), last(last)
    {
        assert(first >= 0);
        assert(last >= first);
        assert(isize(sp.size()) * 8 >= last);
    }

    // Allows const-view from mutable-view
    constexpr BaseBitView(const BaseBitView<u8>& other)
    requires std::is_const_v<T>
    : sp(other.bytes()), first(other.first), last(other.last)
    {}

    // Provides a byte-level view
    constexpr auto byteView() const
    {
        if constexpr (std::is_const_v<T>) {
            return ByteView(sp);
        } else {
            return MutableByteView(sp);
        }
    }

    constexpr BaseBitView subview(isize bitOffset, isize bitCount) const
    {
        assert(bitCount >= 0);
        assert(bitOffset >= 0);
        assert(bitOffset + bitCount <= size());

        return BaseBitView(sp, first + bitOffset, first + bitOffset + bitCount);
    }

    constexpr BaseBitView slice(isize from, isize to) const
    {
        assert(from >= 0);
        assert(to >= from);
        assert(to <= size());

        return BaseBitView(sp, first + from, first + to);
    }

    // Reads a single bit
    constexpr bool operator[](isize i) const
    {
        // assert(i >= 0 && i < size());
        isize abs = first + normalize(i);
        return (sp[abs >> 3] >> (7 - (abs & 7))) & 1;
    }

    constexpr isize normalize(isize i) const
    {
        isize n = size();

        if (i < 0 || i >= n) {

            i %= n;
            if (i < 0) i += n;
        }
        return i;
    }
    
    constexpr u8 getByte(isize bitIndex) const
    {
        assert(!empty());

        isize n   = size();
        isize pos = normalize(bitIndex);
        isize abs = first + pos;
        u8    val = 0;

        if (((abs & 7) == 0) && ((n & 7) == 0)) {

            // Fast path: Byte-aligned read inside a byte-aligned view
            val = sp[abs >> 3];

        } else {

            // Slow path: Bitwise fallback
            for (int b = 0; b < 8; ++b) {

                isize i = first + ((pos + b) % n);
                val <<= 1;
                val |= (sp[i >> 3] >> (7 - (i & 7))) & 1;
            }
        }
        return val;
    }

    constexpr u64 getBits(isize bitIndex, int count) const
    {
        assert(!empty());
        assert(count >= 1 && count <= 64);

        u64 val = 0;
        isize pos = bitIndex;

        // Read whole bytes
        while (count >= 8) {

            val = (val << 8) | getByte(pos);
            pos += 8;
            count -= 8;
        }

        // Read remaining bits
        if (count > 0) {

            u8 tail = getByte(pos);
            val = (val << count) | (tail >> (8 - count));
        }

        return val;
    }

    constexpr void set(isize bitIndex, bool value)
    requires (!std::is_const_v<T>)
    {
        isize i = first + normalize(bitIndex);
        auto& byte = sp[i >> 3];
        auto mask = u8(1 << (7 - (i & 7)));
        value ? byte |= mask : byte &= ~mask;
    }

    constexpr void setByte(isize bitIndex, u8 val)
    requires (!std::is_const_v<T>)
    {
        assert(!empty());

        isize n   = size();
        isize pos = normalize(bitIndex);
        isize abs = first + pos;

        if (((abs & 7) == 0) && ((n & 7) == 0)) {

            // Fast path: Byte-aligned write inside a byte-aligned view
            sp[abs >> 3] = val;

        } else {
            
            // Slow path: Bitwise fallback
            for (int b = 0; b < 8; ++b) {
                
                isize i = first + ((pos + b) % n);
                u8 mask = u8(1 << (7 - (i & 7)));
                
                if (val & (1 << (7 - b)))
                    sp[i >> 3] |= mask;
                else
                    sp[i >> 3] &= ~mask;
            }
        }
    }

    constexpr void setBytes(isize bitIndex, const std::vector<u8> &values)
    {
        for (auto &value : values) {
            setByte(bitIndex, value); bitIndex += 8;
        }
    }
    
    constexpr isize size()  const { return last - first; }
    constexpr bool  empty() const { return size() == 0; }
    constexpr std::span<T> bytes() const { return sp; }
    constexpr T* data() const { return sp.data(); }

    // -----------------------------------------------------------------
    // Iterator (read-only bit iterator — by value, like std::vector<bool>)
    // -----------------------------------------------------------------

    class iterator {

        const BaseBitView* view_;
        isize pos_;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = bool;
        using difference_type   = isize;
        using pointer           = void;
        using reference         = bool;

        constexpr iterator(const BaseBitView* v, isize p)
        : view_(v), pos_(p)
        {
            assert(view_);
        }

        constexpr bool operator*() const { return (*view_)[pos_]; }

        constexpr iterator& operator++() { ++pos_; return *this; }
        constexpr iterator& operator--() { --pos_; return *this; }

        constexpr bool operator[](difference_type n) const {
            return *(*this + n);
        }
        constexpr iterator& operator+=(difference_type n) {
            pos_ += n; return *this;
        }
        constexpr iterator& operator-=(difference_type n) {
            pos_ -= n; return *this;
        }
        friend constexpr iterator operator+(iterator it, difference_type n) {
            it.pos_ += n; return it;
        }
        friend constexpr iterator operator-(iterator it, difference_type n) {
            it.pos_ -= n; return it;
        }
        friend constexpr difference_type operator-(iterator a, iterator b) {
            assert(a.view_ == b.view_); return a.pos_ - b.pos_;
        }
        friend constexpr bool operator==(iterator a, iterator b) {
            return a.view_ == b.view_ && a.pos_ == b.pos_;
        }
    };

    constexpr iterator begin() const { return iterator(this, 0); }
    constexpr iterator end()   const { return iterator(this, size()); }


    // -----------------------------------------------------------------
    // Cyclic iterator
    // -----------------------------------------------------------------

    class cyclic_iterator {

        const BaseBitView* view_;
        isize pos_;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = bool;
        using difference_type   = isize;
        using pointer           = void;
        using reference         = bool;

        constexpr cyclic_iterator(const BaseBitView* v, isize p = 0)
        : view_(v), pos_(p)
        {
            assert(view_);
            assert(!view_->empty());
        }

        constexpr bool operator*() const
        {
            isize n = view_->size();
            isize i = pos_ % n;
            if (i < 0) i += n;
            return (*view_)[i];
        }

        constexpr cyclic_iterator& operator++() { ++pos_; return *this; }
        constexpr cyclic_iterator& operator--() { --pos_; return *this; }

        constexpr bool operator[](difference_type n) const {
            return *(*this + n);
        }
        constexpr u8 readByte() {
            auto result = view_->getByte(pos_); pos_ += 8; return result;
        }
        constexpr u64 readBits(isize count) {
            auto result = view_->getBits(*this, count); pos_ += count; return result;
        }
        constexpr isize offset() const {
            return pos_;
        }
        constexpr cyclic_iterator& operator+=(difference_type n) {
            pos_ += n; return *this;
        }
        constexpr cyclic_iterator& operator-=(difference_type n) {
            pos_ -= n; return *this;
        }
        friend constexpr cyclic_iterator operator+(cyclic_iterator it, difference_type n) {
            it.pos_ += n; return it;
        }
        friend constexpr cyclic_iterator operator-(cyclic_iterator it, difference_type n) {
            it.pos_ -= n; return it;
        }
    };

    constexpr cyclic_iterator cyclic_begin(isize pos = 0) const {
        return cyclic_iterator(this, pos);
    }

    // Advances the iterator until the given bit pattern is found.
    // On success, the iterator is positioned at the first bit of the match.
    // On failure, the iterator remains unchanged.
    bool forward(cyclic_iterator &it, u64 pattern, isize bits) const
    {
        assert(bits > 0 && bits <= 64);

        auto probe   = it;
        u64 shiftreg = 0;
        u64 mask     = (bits == 64) ? ~u64(0) : ((u64(1) << bits) - 1);
        u64 target   = pattern & mask;

        // Prefill the shift register
        for (isize i = 0; i < bits; ++i, ++probe)
            shiftreg = (shiftreg << 1) | u64(probe[0]);

        // Search for the pattern
        for (isize i = 0; i < size(); ++i, ++probe) {

            if ((shiftreg & mask) == target) {
                it = probe; //  - bits;
                return true;
            }

            shiftreg = (shiftreg << 1) | u64(probe[0]);
        }

        return false;
    }
};

using BitView        = BaseBitView<const u8>;
using MutableBitView = BaseBitView<u8>;

}

