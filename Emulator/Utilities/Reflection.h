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
#include <functional>
#include <vector>

/* The purpose of the Reflection interface is to make the symbolic names of
 * an enumeration type available inside the application. I.e., it provides
 * several functions for converting enum values to strings and vice versa.
 *
 * The interface distinguishes two enumeration types:
 *
 * - Standard enumerations
 *
 *   The enumeration members must be numbered 0, 1, 2, etc. Each member of the
 *   enumeration is treated as a stand-alone option.
 *
 * - Bit field enumerations
 *
 *   The enumeration members must be numbered 1, 2, 4, etc. Each member of the
 *   enumeration is treated as flag of a combined bit field.
 */

namespace vc64::util {

#define assert_enum(e,v) assert(e##Enum::isValid(v))

template <class T, typename E> struct Reflection {

    // Checks whether this enum is a bit fiels rather than a standard enum
    static constexpr bool isBitField() { return T::minVal == 1; }

    // Checks if the provides value is inside the valid range
    static constexpr bool isValid(auto value) { return long(value) >= T::minVal && long(value) <= T::maxVal; }

    // Returns the key as a C string (including the section prefix)
    static const char *rawkey(isize value) { return T::_key((E)value); }

    // Returns the key as a C string (excluding the section prefix)
    static const char *key(isize value) {

        auto *p = rawkey(value);
        for (isize i = 0; p[i]; i++) if (p[i] == '.') return p + i + 1;
        return p;
    }
    
    // Returns a textual representation for a bit mask
    static const char *mask(isize mask) {

        static string result;
        result = "";

        if (isBitField()) {

            for (isize i = T::minVal; i <= T::maxVal; i *= 2) {
                if (mask & i) result += (result.empty() ? "" : " | ") + string(T::_key((E)i));
            }

        } else {

            for (isize i = T::minVal; i <= T::maxVal; i++) {
                if (mask & (1 << i)) result += (result.empty() ? "" : " | ") + string(T::_key((E)i));
            }
        }

        return result.c_str();
    }

    // Collects all key / value pairs
    static std::vector <std::pair<string,long>> 
    pairs(std::function<bool(E)> filter = [](E) { return true; }) {

        std::vector <std::pair<string,long>> result;

        if (isBitField()) {

            for (isize i = T::minVal; i <= T::maxVal; i *= 2) {
                if (filter(E(i))) result.push_back(std::make_pair(key(i), i));
            }

        } else {

            for (isize i = T::minVal; i <= T::maxVal; i++) {
                if (filter(E(i))) result.push_back(std::make_pair(key(i), i));
            }
        }
        return result;
    }

    // Returns all keys in form of a textual list representation
    static string keyList(std::function<bool(E)> filter = [](E){ return true; }, const string &delim = ", ") {

        string result;

        for (const auto &pair : pairs(filter)) {
            result += (result.empty() ? "" : delim) + pair.first;
        }
        return result;
    }

    // Convenience wrapper
    static string argList(std::function<bool(E)> filter = [](E){ return true; }) {

        return "{ " + keyList(filter, " | ") + " }";
    }
};

}
