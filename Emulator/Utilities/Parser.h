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
#include "Exception.h"
#include <functional>

namespace vc64::util {

struct ParseError : public std::exception {

    string token;
    string expected;

    ParseError(const string &t) : token(t) { }
    ParseError(const string &t, const string &e) : token(t), expected(e) { }

    const char *what() const throw() override { return token.c_str(); }
};

struct ParseBoolError : public ParseError {
    using ParseError::ParseError;
};

struct ParseOnOffError : public ParseError {
    using ParseError::ParseError;
};

struct ParseNumError : public ParseError {
    using ParseError::ParseError;
};

struct EnumParseError : public ParseError {
    using ParseError::ParseError;
};

bool isBool(const string& token);
bool isOnOff(const string& token);
bool isNum(const string& token);

bool parseBool(const string& token) throws;
bool parseOnOff(const string& token) throws;
long parseNum(const string& token) throws;
string parseSeq(const string& token) throws;

template <typename Enum> long parseEnum(const string& key)
{
    return parsePartialEnum <Enum> (key, [](long){ return true; });
}

template <typename R, typename Enum> R parseEnum(const string& key)
{
    return (R)parseEnum <Enum> (key);
}

template <typename Enum> long parsePartialEnum(const string& key, std::function<bool(long)> accept)
{
    string upper, prefix, suffix;

    // Convert the search string to upper case
    for (auto c : key) { upper += (char)std::toupper(c); }

    // Search all keys
    for (isize i = Enum::minVal; i <= Enum::maxVal; i++) {

        if (!accept(i)) continue;

        auto enumkey = string(Enum::key(i));

        // Check if the full key matches
        if (enumkey == upper) return i;

        // If a section marker is present, check the plain key, too
        if (auto pos = enumkey.find('.'); pos != std::string::npos) {
            if (enumkey.substr(pos + 1, string::npos) == upper) return i;
        }
    }

    throw EnumParseError(key, Enum::keyList());
}

template <typename R, typename Enum> R parsePartialEnum(const string& key, std::function<bool(long)> accept)
{
    return (R)parsePartialEnum<Enum>(key, accept);
}

}
