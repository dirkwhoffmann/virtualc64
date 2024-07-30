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
#include <map>

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

    // Returns the key as a C string
    static const char *key(isize value) {

        static string result;

        result = "";
        if constexpr (T::minVal == 1) {

            // The enum is a bit field
            for (isize i = T::minVal; i <= T::maxVal; i *= 2) {
                if (value & i) result += (result.empty() ? "" : " | ") + string(T::_key((E)i));
            }

        } else {

            // The enum is a standard enumeration
            result = string(T::_key((E)value));
        }

        return result.c_str();
    }

    // Returns the key without the section prefix (if any)
    static const char *plainkey(isize nr) {

        // Don't call this method for bit fields
        assert(T::minVal == 0);

        auto *p = T::key((E)nr);
        for (isize i = 0; p[i]; i++) if (p[i] == '.') return p + i + 1;
        return p;
    }

    // Collects all key / value pairs
    static std::map <string,long> pairs(std::function<bool(E)> filter = [](E){ return true; }) {

        std::map <string,long> result;

        for (isize i = T::minVal; i <= T::maxVal; i++) {
            if (T::isValid(i) && filter(E(i))) result.insert(std::make_pair(key(i), i));
        }

        return result;
    }

    // Returns a list in form of a colon seperated string
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
