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

#ifdef __cplusplus

#include "Types.h"
#include <map>

namespace util {

#define assert_enum(e,v) assert(e##Enum::isValid(v))

template <class T, typename E> struct Reflection {

    // Returns the shortened key as a C string
    static const char *key(long nr) { return T::key((E)nr); }

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

        for (auto &it : pairs()) {

            if (!filter(it.second)) continue;

            if (result != "") result += delim;
            result += it.first;
        }

        return result;
    }

    // DEPRECATED
    /*
    static string keyList(std::function<bool(long)> filter = [](long){ return true; }, const string &delim = ", ") {

        string result;

        for (auto &it : pairs()) {

            if (!filter(it.second)) continue;

            if (result != "") result += delim;
            result += it.first;
        }

        return result;
    }
    */

    // Convinience wrapper
    static string argList(std::function<bool(E)> filter = [](E){ return true; }) {

        return "{ " + keyList(filter, " | ") + " }";
    }
    /*
    static string argList(std::function<bool(long)> filter = [](long){ return true; }) {

        return "{ " + keyList(filter, " | ") + " }";
    }
    */
};

}

#endif
