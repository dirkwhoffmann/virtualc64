// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/abilities/Reflectable.h"
#include "utl/io/ParseError.h"

namespace utl {

/*
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
*/

bool isBool(const string& token);
bool isOnOff(const string& token);
bool isNum(const string& token);

bool parseBool(const string& token);
bool parseOnOff(const string& token);
long parseNum(const string& token);
string parseSeq(const string& token);

template <typename Enum> long parseEnum(const string& key)
{
    if (auto result = Enum::parseEnum(key)) {
        return long(*result);
    }
    throw NewParseError(NewParseError::PARSE_ENUM_ERROR, Enum::keyList());
}

template <typename R, typename Enum> R parseEnum(const string& key)
{
    if (auto result = Enum::parseEnum(key)) {
        return result;
    }
    throw NewParseError(NewParseError::PARSE_ENUM_ERROR, Enum::keyList());
}

template <typename Enum> long parsePartialEnum(const string& key, std::function<bool(long)> accept)
{
    if (auto result = (long)Enum::parsePartialEnum(key, accept)) {
        return result;
    }
    throw NewParseError(NewParseError::PARSE_ENUM_ERROR, Enum::keyList());
}

template <typename R, typename Enum> R parsePartialEnum(const string& key, std::function<bool(long)> accept)
{
    if (auto result = Enum::parsePartialEnum(key, accept)) {
        return result;
    }
    throw NewParseError(NewParseError::PARSE_ENUM_ERROR, Enum::keyList());
}

}
