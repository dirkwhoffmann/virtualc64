// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "utl/support/Strings.h"
#include <sstream>

namespace utl {

string createStr(const u8 *buf, isize maxLen)
{
    string result;

    for (isize i = 0; i < maxLen && buf[i] != 0; i++) {
        result += char(buf[i]);
    }
    return result;
}

string createAscii(const u8 *buf, isize len, char fill)
{
    string result;

    for (isize i = 0; i < len; i++) {
        result += isprint(int(buf[i])) ? char(buf[i]) : fill;
    }
    return result;
}

string repeat(const string &s, isize times) {

    string result;
    result.reserve(s.size() * times);
    for (isize i = 0; i < times; i++) result += s;
    return result;
}

bool
parseHex(const string &s, isize *result)
{
    try {
        size_t pos = 0;
        auto num = std::stoll(s, &pos, 16);

        if (pos == s.size()) {

            *result = (isize)num;
            return true;
        }
        return false;
    } catch (...) {
        return false;
    }
}

string
lowercased(const string& s)
{
    string result;
    for (auto c : s) { result += (char)std::tolower(c); }
    return result;
}

string
uppercased(const string& s)
{
    string result;
    for (auto c : s) { result += (char)std::toupper(c); }
    return result;
}

string
makePrintable(const string& s)
{
    string result;
    for (auto c : s) {
        if (isprint(c)) {
            result += c;
        } else {
            result += "[" + hexstr<2>(c) + "]";
        }
    }
    return result;
}

string
ltrim(const string &s, const string &characters)
{
    auto pos = s.find_first_not_of(characters);
    return (pos == string::npos) ? "" : s.substr(pos);
}

string
rtrim(const string &s, const string &characters)
{
    auto pos = s.find_last_not_of(characters);
    return (pos == string::npos) ? "" : s.substr(0, pos + 1);
}

string
trim(const string &s, const string &characters)
{
    return ltrim(rtrim(s, characters), characters);
}

string_view unquote(string_view sv)
{
    bool quoted = sv.size() >= 2 && sv.front() == '"' && sv.back() == '"';
    return quoted ? sv.substr(1, sv.size() - 2) : sv;
}

string
commonPrefix(const string &s1, const string &s2, bool caseSensitive)
{
    auto len = std::min(s1.length(), s2.length());

    usize count = 0;
    if (caseSensitive) {
        for (; count < len && s1[count] == s2[count]; count++);
    } else {
        for (; count < len && std::toupper(s1[count]) == std::toupper(s2[count]); count++);
    }

    return s1.substr(0, count);
}

string
commonPrefix(const std::vector<string> &vec, bool caseSensitive)
{
    if (vec.empty()) return "";

    string result = vec[0];
    for (usize i = 1; i < vec.size(); i++) result = commonPrefix(result, vec[i], caseSensitive);

    return result;
}

string
padString(const string &s, isize width, char align)
{
    isize pad = width - isize(s.length());
    if (pad <= 0) return s;

    switch (align) {

        case 'l': return s + string(pad, ' ');
        case 'r': return string(pad, ' ') + s;
        case 'c': return string(pad/2, ' ') + s + string(pad - pad/2, ' ');

        default:
            fatalError;
    }
}

std::vector<string>
split(const string &s, char delimiter)
{
    std::vector<string> result;
    string::size_type start = 0;

    for (;;) {

        auto pos = s.find(delimiter, start);
        if (pos == string::npos) {

            result.emplace_back(s.substr(start));
            break;
        }
        result.emplace_back(s.substr(start, pos - start));
        start = pos + 1;
    }

    return result;
}

std::vector<string>
split(const std::vector<string> &sv, char delimiter)
{
    std::vector<string> result;

    for (const auto &s : sv) {

        auto splitted = split(s, delimiter);

        if (splitted.empty()) {
            result.push_back("");
        } else {
            result.insert(result.end(), splitted.begin(), splitted.end());
        }
    }
    return result;
}

std::pair<vector<string>, string>
splitLast(const vector<string> &vec)
{
    if (vec.empty()) return { vec, "" };
    std::vector<string> prefix(vec.begin(), vec.end() - 1);
    return { prefix, vec.back() };
}

string
concat(const string &s1, const string &s2, char delim)
{
    if (s1.empty()) return s2;
    if (s2.empty()) return s1;
    return s1.back() == delim ? s1 + s2 : s1 + delim + s2;
}

string
concat(const std::vector<string> &s, const string &delim, const string &ldelim, const string &rdelim)
{
    string result;
    isize count = 0;

    for (auto &it : s) {

        if (it.empty()) continue;
        if (count++) result += delim;
        result += it;
    }

    return count > 1 ? ldelim + result + rdelim : result;
}

template <isize digits> string hexstr(isize number)
{
    char str[digits + 1];

    str[digits] = 0;
    for (isize i = digits - 1; i >= 0; i--, number >>= 4) {

        auto nibble = number & 0xF;
        str[i] = (char)(nibble < 10 ? '0' + nibble : 'a' + (nibble - 10));
    }

    return string(str, digits);
}

template string hexstr <1> (isize number);
template string hexstr <2> (isize number);
template string hexstr <4> (isize number);
template string hexstr <6> (isize number);
template string hexstr <8> (isize number);
template string hexstr <16> (isize number);
template string hexstr <32> (isize number);
template string hexstr <64> (isize number);

string byteCountAsString(isize size)
{
    auto kb = size / 1024;
    auto mb = size / (1024 * 1024);
    auto gb = size / (1024 * 1024 * 1024);
    auto kbfrac = (size * 100 / 1024) % 100;
    auto mbfrac = (size * 100 / (1024 * 1024)) % 100;
    auto gbfrac = (size * 100 / (1024 * 1024 * 1024)) % 100;

    if (size < 1024) {

        return std::to_string(size) + " Bytes";
    }
    if (size < 1024 * 1024) {

        auto frac = kbfrac == 0 ? "" : ("." + std::to_string(kbfrac));
        return std::to_string(kb) + frac + " KB";
    }
    if (size < 1024 * 1024 * 1024) {

        auto frac = mbfrac == 0 ? "" : ("." + std::to_string(mbfrac));
        return std::to_string(mb) + frac + " MB";
    }

    auto frac = gbfrac == 0 ? "" : ("." + std::to_string(gbfrac));
    return std::to_string(gb) + frac + " GB";
}

string fillLevelAsString(double percentage)
{
    if (percentage < 1.0) {
        return "0." + std::to_string(int(percentage * 100)) + "%";
    } else {
        return std::to_string(int(percentage)) + "%";
    }
}

}
