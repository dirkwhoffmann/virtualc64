// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/PosixViewTypes.h"
#include "FileSystems/CBM/FSObjects.h"
#include "FileSystems/CBM/FSBlock.h"
#include "utl/chrono.h"
#include "utl/support.h"
#include <algorithm>
#include <cstring>
#include <unordered_set>
#include <sys/stat.h>

namespace retro::vault::cbm {

FSString::FSString(const string &cpp, isize limit) : str(cpp), limit(limit)
{

}

FSString::FSString(const char *c, isize limit) : limit(limit)
{
    assert(c != nullptr);

    str.assign(c, strnlen(c, limit));
}

FSString::FSString(const u8 *bcpl, isize limit) : limit(limit)
{
    assert(bcpl != nullptr);

    auto length = (isize)bcpl[0];
    auto firstChar = (const char *)(bcpl + 1);

    str.assign(firstChar, std::min(length, limit));
}

char
FSString::capital(char c, FSFormat dos)
{
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

bool
FSString::operator== (const FSString &rhs) const
{
    return utl::uppercased(str) == utl::uppercased(rhs.str);
}

u32
FSString::hashValue(FSFormat dos) const
{
    u32 result = (u32)length();
    for (auto c : str) {

        result = (result * 13 + (u32)capital(c, dos)) & 0x7FF;
    }

    return result;
}

void
FSString::write(u8 *p)
{
    assert(p != nullptr);

    // Write name as a BCPL string (first byte is string length)
    *p++ = (u8)length();
    for (auto c : str) { *p++ = c; }
}

bool
FSString::operator<(const FSString& other) const
{
    return utl::uppercased(cpp_str()) < utl::uppercased(other.cpp_str());
}

std::ostream &operator<<(std::ostream &os, const FSString &str) {

    os << str.cpp_str();
    return os;
}



FSPattern::FSPattern(const string glob) : glob(glob)
{
    // Create regex string
    std::string re = "^";

    for (char c : glob) {

        switch (c) {

            case '*': re += ".*"; break;
            case '?': re += "."; break;
            case '.': re += "\\."; break;
            case '\\': re += "\\\\"; break;

            default:
                if (std::isalnum(u8(c))) {
                    re += c;
                } else {
                    re += '\\';
                    re += c;
                }
        }
    }
    re += "$";

    try {
        regex = std::regex(re, std::regex::ECMAScript | std::regex::icase);
    } catch (const std::regex_error &) {
        throw FSError(FSError::FS_INVALID_ARGUMENT, glob);
    }
}

std::vector<FSPattern>
FSPattern::splitted() const
{
    std::vector<FSPattern> result;

    // Extract the volume identifier (if any)
    auto vec = split(glob, ':');

    switch (vec.size()) {

        case 1: // Relative path

            for (auto &it : split(vec[0], '/')) {
                if (!it.empty()) result.push_back(FSPattern(it));
            }
            break;

        case 2: // Absolute path

            result.push_back(FSPattern(vec[0] + ":"));
            for (auto &it : split(vec[1], '/')) {
                if (!it.empty()) result.push_back(FSPattern(it));
            }
            break;

        default:

            throw FSError(FSError::FS_INVALID_PATH, glob);
    }

    return result;
}

bool
FSPattern::match(const FSString &name) const
{
    return std::regex_match(name.cpp_str(), regex);
}

}
