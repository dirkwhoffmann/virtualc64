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

namespace util {

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
    string upperKey;
    for (auto c : key) { upperKey += (char)std::toupper(c); }

    auto p = Enum::pairs();

    auto it = p.find(upperKey);
    if (it == p.end()) throw EnumParseError(key, Enum::keyList());

    return it->second;
}

template <typename Enum> long parsePartialEnum(const string& key, std::function<bool(long)> accept)
{
    string upperKey;
    for (auto c : key) { upperKey += (char)std::toupper(c); }

    for (isize i = Enum::minVal; i <= Enum::maxVal; i++) {

        if (Enum::key(i) == upperKey && accept(i)) return i;
    }

    throw EnumParseError(key, Enum::keyList());
}

}
