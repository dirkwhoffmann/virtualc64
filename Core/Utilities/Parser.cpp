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

#include "VirtualC64Config.h"
#include "Parser.h"

namespace vc64::util {

bool isBool(const string& token)
{
    try { (void)parseBool(token); }
    catch (std::exception&) { return false; }
    return true;
}

bool isOnOff(const string& token)
{
    try { (void)parseOnOff(token); }
    catch (std::exception&) { return false; }
    return true;
}

bool isNum(const string& token)
{
    try { (void)parseNum(token); }
    catch (std::exception&) { return false; }
    return true;
}

bool
parseBool(const string& token)
{
    if (token == "1" || token == "true" || token == "yes") return true;
    if (token == "0" || token == "false" || token == "no") return false;

    throw ParseBoolError(token);
}

bool
parseOnOff(const string& token)
{
    if (token == "on") return true;
    if (token == "off") return false;

    throw ParseOnOffError(token);
}

long
parseNum(const string& token)
{
    string _token = token;
    int base = 0;
    long result;

    if (token.starts_with("$")) {

        _token.erase(0, 1);
        base = 16;

    } else if (token.starts_with("0x")) {

        _token.erase(0, 2);
        base = 16;

    } else if (token.starts_with("%")) {

        _token.erase(0, 1);
        base = 2;
    }

    try { result = stol(_token, nullptr, base); }
    catch (std::exception&) { throw ParseNumError(token); }

    return result;
}

string
parseSeq(const string& token)
{
    string _token = token;
    string result;

    // Remove prefixes
    if (token.starts_with("$")) _token = _token.erase(0, 1);
    if (token.starts_with("0x")) _token = _token.erase(0, 2);

    // Don't do anything for standard strings
    if (token == _token) return token;

    // Add a trailing '0' for odd-sized strings
    if (_token.length() % 2) _token = '0' + _token;

    // Decode the byte sequence
    for (unsigned int i = 0; i < _token.length(); i += 2) {

        std::string digits = _token.substr(i, 2);
        try { result.push_back((char)stol(digits, nullptr, 16)); }
        catch (std::exception&) { throw ParseNumError(token); }
    }

    return result;
}

}
