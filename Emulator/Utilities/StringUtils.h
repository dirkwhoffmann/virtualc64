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
#include <vector>

namespace vc64::util {

//
// Creating
//

// Creates a string from a buffer
string createStr(const u8 *buf, isize maxLen);
string createAscii(const u8 *buf, isize len, char fill = '.');


//
// Converting
//

// Parses a hexadecimal number in string format
bool parseHex(const string &s, isize *result);

// Converts an integer value to a hexadecimal string representation
template <isize digits> string hexstr(isize number);


//
// Transforming
//

// Converts the capitalization of a string
string lowercased(const string& s);
string uppercased(const string& s);

// Replaces all unprintable characters
string makePrintable(const string& s);


//
// Stripping off characters
//

string ltrim(const string &s, const string &characters = " ");
string rtrim(const string &s, const string &characters = " ");
string trim(const string &s, const string &characters = " ");


//
// Splitting and concatenating
//

std::vector<string> split(const string &s, char delimiter);
string concat(std::vector<string> &s, string delimiter);


//
// Pretty printing
//

// Returns a textual description for a byte count
string byteCountAsString(isize bytes);

// Returns a textual description for a fill level
string fillLevelAsString(double percentage);

}
