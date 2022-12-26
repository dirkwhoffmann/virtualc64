// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Types.h"
#include "StringUtils.h"

#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <vector>

namespace fs = std::filesystem;

namespace util {

//
// Handling file names
//

// Extracts a certain component from a path
string extractPath(const string &path);
string extractName(const string &path);
string extractSuffix(const string &path);

// Strips a certain component from a path
string stripPath(const string &path);
string stripName(const string &path);
string stripSuffix(const string &path);

// Concatenates two path segments
string appendPath(const string &path, const string &path2);

// Checks or creates an absolute path
bool isAbsolutePath(const string &path);
string makeAbsolutePath(const string &path);

// Makes a file name unique if a file with the provided name already exists
string makeUniquePath(const string &path);


//
// Handling files
//

// Returns the size of a file in bytes
isize getSizeOfFile(const string &path);

// Checks if a file exists
bool fileExists(const string &path);

// Checks if a path points to a directory
bool isDirectory(const string &path);

// Creates a directory
bool createDirectory(const string &path);

// Returns the number of files in a directory
isize numDirectoryItems(const string &path);

// Returns a list of files in a directory
std::vector<string> files(const string &path, const string &suffix = "");
std::vector<string> files(const string &path, std::vector <string> &suffixes);

// Checks the header signature (magic bytes) of a stream or buffer
bool matchingStreamHeader(std::istream &is, const u8 *header, isize len, isize offset = 0);
bool matchingStreamHeader(std::istream &is, const string &header, isize offset = 0);
bool matchingBufferHeader(const u8 *buffer, const u8 *header, isize len, isize offset = 0);

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

isize streamLength(std::istream &stream);

struct dec {
    
    i64 value;
    
    dec(i64 v) : value(v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct hex {
    
    int digits;
    u64 value;
    
    hex(int d, u64 v) : digits(d), value(v) { };
    hex(u64 v) : hex(16, v) { };
    hex(u32 v) : hex(8, v) { };
    hex(u16 v) : hex(4, v) { };
    hex(u8 v) : hex(2, v) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct flt {

    double value;

    flt(double v) : value(v) { };
    flt(float v) : value(double(v)) { };
    std::ostream &operator()(std::ostream &os) const;
};

struct tab {
    
    int pads;
    const string &str;
    
    tab(int p, const string &s) : pads(p), str(s) { };
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

inline std::ostream &operator <<(std::ostream &os, dec v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, hex v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, flt v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, tab v) { return v(os); }
inline std::ostream &operator <<(std::ostream &os, bol v) { return v(os); }

}
