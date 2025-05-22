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
#include "StringUtils.h"

#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <vector>

// Namespace aliases
namespace vc64 { namespace fs = std::filesystem; }

namespace vc64::util {

//
// Handling files
//

// Makes a file name unique if a file with the provided name already exists
fs::path makeUniquePath(const fs::path &path);

// Returns the size of a file in bytes
isize getSizeOfFile(const fs::path &path);

// Checks if a file exists
bool fileExists(const fs::path &path);

// Checks if a path points to a directory
bool isDirectory(const fs::path &path);

// Creates a directory
bool createDirectory(const fs::path &path);

// Returns the number of files in a directory
isize numDirectoryItems(const fs::path &path);

// Returns a list of files in a directory
std::vector<fs::path> files(const fs::path &path, const string &suffix = "");
std::vector<fs::path> files(const fs::path &path, std::vector <string> &suffixes);

// Checks the header signature (magic bytes) of a stream or buffer
bool matchingStreamHeader(std::istream &is, const u8 *header, isize len, isize offset = 0);
bool matchingStreamHeader(std::istream &is, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, const u8 *header, isize hlen, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buf, isize blen, const string &header, isize offset = 0);


//
// Pretty printing
//

// Writes an integer into a string in decimal format
void sprint8d(char *s, u8 value);
void sprint16d(char *s, u16 value);

// Writes an integer into a string in hexadecimal format
void sprint8x(char *s, u8 value);
void sprint16x(char *s, u16 value);

// Writes an integer into a string in binary format
void sprint8b(char *s, u8 value);
void sprint16b(char *s, u16 value);


//
// Handling streams
//

struct dec {
    
    i64 value;
    
    dec(i64 v) : value(v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct hex {
    
    isize digits;
    u64 value;
    
    hex(isize d, u64 v) : digits(d), value(v) { };
    hex(u64 v) : hex(16, v) { };
    hex(u32 v) : hex(8, v) { };
    hex(u16 v) : hex(4, v) { };
    hex(u8 v) : hex(2, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct bin {

    isize digits;
    u64 value;

    bin(isize d, u64 v) : digits(d), value(v) { };
    bin(u64 v) : bin(64, v) { };
    bin(u32 v) : bin(32, v) { };
    bin(u16 v) : bin(16, v) { };
    bin(u8 v) : bin(8, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct flt {

    double value;

    flt(double v) : value(v) { };
    flt(float v) : value(double(v)) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct tab {
    
    isize pads;
    const string &str;
    
    tab(isize p, const string &s) : pads(p), str(s) { };
    tab(const string &s) : tab(24, s) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct bol {
    
    static const string& yes;
    static const string& no;
    
    bool value;
    const string &str1, &str2;
    
    bol(bool v, const string &s1, const string &s2) : value(v), str1(s1), str2(s2) { };
    bol(bool v) : bol(v, yes, no) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct str {

    isize characters;
    u64 value;

    str(isize c, u64 v) : characters(c), value(v) { };
    str(u64 v) : str(8, v) { };
    str(u32 v) : str(4, v) { };
    str(u16 v) : str(2, v) { };
    str(u8 v) : str(1, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

inline std::ostream &operator <<(std::ostream &os, dec v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, hex v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, bin v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, flt v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, tab v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, bol v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, str v) { return v(os); }

}
