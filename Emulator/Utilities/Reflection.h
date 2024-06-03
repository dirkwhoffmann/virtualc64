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

#include "Types.h"
#include <functional>
#include <map>

namespace vc64::util {

#define assert_enum(e,v) assert(e##Enum::isValid(v))

template <class T, typename E> struct Reflection {

    // Returns the key including the section prefix (if any)
    static const char *key(isize nr) { return T::key((E)nr); }

    // Returns the key without the section prefix (if any)
    static const char *plainkey(isize nr) {
        auto *p = key(nr);
        for (isize i = 0; p[i]; i++) if (p[i] == '.') return p + i + 1;
        return p;
    }

    // Collects all key / value pairs
    static std::map <string, E> pairs() {

        std::map <string, E> result;

        for (auto i = T::minVal; i <= T::maxVal; i++) {
            if (T::isValid(i)) result.insert(std::make_pair(key(i), E(i)));
        }

        return result;
    }

    // Returns a list in form of a colon seperated string
    static string keyList(std::function<bool(E)> filter = [](E){ return true; }, const string &delim = ", ") {

        string result;

        for (auto i = T::minVal; i <= T::maxVal; i++) {
            
            if (T::isValid(i) && filter(E(i))) {

                if (result != "") result += delim;
                result += (key(i));
            }
        }

        return result;
    }

    // Convenience wrapper
    static string argList(std::function<bool(E)> filter = [](E){ return true; }) {

        return "{ " + keyList(filter, " | ") + " }";
    }
};

}
