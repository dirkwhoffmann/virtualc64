// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include <algorithm>
#include <unordered_set>

namespace utl {

template <typename T>
struct Range {

    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                  "Range<T> requires an ordered scalar type");

    // Bounds in [lower; upper) format
    T lower{}, upper{};

    constexpr bool valid() const noexcept {
        return lower <= upper;
    }

    [[deprecated]] constexpr bool inside(isize min, isize max) const noexcept {
        return lower >= min && lower <= upper && upper <= max;
    }

    constexpr bool subset(const Range<T> &other) const noexcept {
        return lower >= other.lower && lower <= upper && upper <= other.upper;
    }

    constexpr bool contains(T value) const noexcept {
        return value >= lower && value < upper;
    }

    constexpr T size() const noexcept {
        return upper - lower;
    }

    constexpr T translate(T offset) const {

        if (offset >= T{} && offset < size()) return lower + offset;
        throw Error(offset, "Range offset out of bounds");
    }
    
    static std::vector<Range<T>> coalesce(const std::vector<T> &values) {
        
        std::vector<Range<T>> result;
        
        if (!values.empty()) {

            std::vector<T> copy = values;
            std::sort(copy.begin(), copy.end());
            
            T start = copy[0];
            T prev  = copy[0];
            
            for (usize i = 1; i < copy.size(); ++i) {
                
                if (copy[i] == prev + 1) {
                    prev = copy[i];
                } else {
                    result.emplace_back(Range<T>{ start, prev + 1 });
                    start = prev = copy[i];
                }
            }
            
            result.emplace_back(Range<T>{ start, prev + 1 });
        }
        
        return result;
    }
    
    static std::vector<Range<T>> coalesce(const std::unordered_set<T> &values) {
        
        std::vector<T> vec;
        vec.reserve(values.size());
        
        for (auto val : values)
            vec.push_back(val);
        
        return Range<T>::coalesce(std::move(vec));
    }
};

template <typename T>
struct ClosedRange {

    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                  "Range<T> requires an ordered scalar type");

    // Bounds in [lower; upper] format
    T lower{}, upper{};

    constexpr bool valid() const noexcept {
        return lower <= upper;
    }

    [[deprecated]] constexpr bool inside(isize min, isize max) const noexcept {
        return lower >= min && lower <= upper && upper <= max;
    }

    constexpr bool subset(const ClosedRange<T> &other) const noexcept {
        return lower >= other.lower && lower <= upper && upper <= other.upper;
    }
    
    constexpr bool contains(T value) const noexcept {
        return value >= lower && value <= upper;
    }

    constexpr T size() const noexcept {
        return upper - lower + T{1};
    }

    constexpr T translate(T offset) const {

        if (offset >= T{} && offset <= size()) return lower + offset;
        throw Error(offset, "Range offset out of bounds");
    }
};

}
