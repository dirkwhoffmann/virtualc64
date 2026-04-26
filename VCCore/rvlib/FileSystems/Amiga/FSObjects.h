// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/Amiga/FSTypes.h"
#include <ostream>
#include <regex>

namespace retro::vault::amiga {

struct FSString {
    
    // File system identifier
    string str;
    
    // Maximum number of permitted characters
    isize limit = 0;

    static char capital(char c, FSFormat dos);

    FSString(const string &cppS, isize limit = 1024);
    FSString(const char *c, isize limit = 1024);
    FSString(const u8 *bcpl, isize limit = 1024);

    const char *c_str() const { return str.c_str(); }
    string cpp_str() const { return str; }

    bool operator== (const FSString &rhs) const;
    isize length() const { return (isize)str.length(); }
    bool empty() const { return str.empty(); }
    u32 hashValue(FSFormat dos) const;

    void write(u8 *p);

    bool operator<(const FSString& other) const;
    friend std::ostream &operator<<(std::ostream &os, const FSString &str);
};

struct FSName : FSString {

    // Makes a file name compatible with the host file system
    static fs::path sanitize(const string &filename);

    // Makes a file name compatible with the Amiga file system
    static string unsanitize(const fs::path &filename);

    // Constructors
    explicit FSName() : FSName("") { }
    explicit FSName(const string &cpp);
    explicit FSName(const char *c);
    explicit FSName(const u8 *bcpl);
    explicit FSName(const fs::path &path);
    explicit FSName(const std::map<string,string> map, const string &cpp, const string fallback);

    fs::path path() const { return sanitize(str); }
};

struct FSPath {

    using component_type = FSName;

    optional<FSName> volume;
    vector<FSName>   components;

    explicit FSPath(const string &cpp);
    explicit FSPath(const fs::path &path);
    explicit FSPath(const char *str) : FSPath(string(str)) { };

    string cpp_str() const;

    bool empty() const { return !volume.has_value() && components.empty(); }
    bool absolute() const { return volume.has_value(); }

    FSName filename() const;
    FSPath parentPath() const;

    FSPath &operator/=(const FSName &);
    FSPath &operator/=(const FSPath &);
    FSPath operator/(const FSName &rhs) const;
    FSPath operator/(const FSPath &rhs) const;

    auto begin() const { return components.begin(); }
    auto end() const { return components.end(); }
};

struct FSComment : FSString {
    
    FSComment(const char *cString) : FSString(cString, 91) { }
    FSComment(const u8 *bcplString) : FSString(bcplString, 91) { }
};

struct FSPattern {

    string glob;
    std::regex regex;

    explicit FSPattern(const string str);
    explicit FSPattern(const char *str) : FSPattern(string(str)) { };

    std::vector<FSPattern> splitted() const;
    bool isAbsolute() const { return !glob.empty() && glob[0] == '/'; }
    bool match(const FSString &name) const;
};

struct FSTime {
    
    u32 days;
    u32 mins;
    u32 ticks;
    
    FSTime() : days(0), mins(0), ticks(0) { }
    FSTime(time_t t);
    FSTime(const u8 *p);

    time_t time() const;
    void write(u8 *p);

    string dateStr() const;
    string timeStr() const;
    string str() const;
};

struct FSAttr {

    isize size;         // File size in bytes
    isize blocks;       // Number of occupied blocks
    u32 prot;           // Protection flags
    bool isDir;         // Is it a directory?
    FSTime ctime;       // Creation time
    FSTime mtime;       // Last modification time

    u32 mode() const;
};

struct FSStat {

    // Capacity information
    FSTraits traits;

    // Usage information
    isize freeBlocks;
    isize usedBlocks;
    isize cachedBlocks;
    isize dirtyBlocks;
    double fill;
    
    // Root block metadata
    FSName name;
    FSTime bDate;
    FSTime mDate;

    // Access statistics
    isize generation;
};

struct FSBootStat {

    // Name of the boot block
    string name;

    // Boot block type
    BootBlockType type;

    // Infection status
    bool hasVirus;
};

}
