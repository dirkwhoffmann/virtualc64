// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/CBM/FSTypes.h"
#include <cstring>
#include <ostream>
#include <regex>

namespace retro::vault::cbm {

template <int len> class PETName {

    // PETSCII representation
    u8 pet[len + 1];

    // ASCII representation
    char asc[len + 1];

    // The pad characters (usually 0xA0)
    u8 pad;

public:

    static u8 petscii2printable(u8 c, u8 subst)
    {
        if (c >= 0x20 && c <= 0x7E) return c; // 0x20 = ' ', 0x7E = '~'
        return subst;
    }

    static u8 ascii2pet(u8 asciichar)
    {
        if (asciichar == 0x00) return 0x00;

        asciichar = (u8)std::toupper(asciichar);
        return asciichar >= 0x20 && asciichar <= 0x5D ? asciichar : ' ';
    }

    explicit PETName(const u8 *_pet, u8 _pad = 0xA0) : pad(_pad)
    {
        assert(_pet);

        memset(pet, pad, sizeof(pet));
        memset(asc, 0x0, sizeof(asc));

        for (int i = 0; i < len && _pet[i] != pad; i++) {

            asc[i] = petscii2printable(_pet[i], '_');
            pet[i] = _pet[i];
        }
    }

    explicit PETName(const char *_str, u8 _pad = 0xA0) : pad(_pad)
    {
        assert(_str);

        memset(pet, pad, sizeof(pet));
        memset(asc, 0x0, sizeof(asc));

        for (int i = 0; i < len && _str[i] != 0x00; i++) {

            asc[i] = _str[i];
            pet[i] = ascii2pet(_str[i]);
        }
    }

    explicit PETName(string str) : PETName(str.c_str()) { }
    explicit PETName(fs::path path) : PETName(path.filename().string()) { }

    void setPad(u8 _pad) {

        for (int i = 0; i < len; i++) {
            if (pet[i] == pad) pet[i] = _pad;
        }
        pad = _pad;
    }
    
    bool operator== (const PETName &rhs)
    {
        int i = 0;

        while (pet[i] != pad || rhs.pet[i] != pad) {
            if (pet[i] != rhs.pet[i]) return false;
            i++;
        }
        return true;
    }

    PETName<len> stripped(u8 c)
    {
        PETName<len> name = *this;

        auto length = isize(strlen(name.asc));
        for (isize i = length; i > 0 && name.asc[i - 1] == c; i--) {

            name.asc[i - 1] = 0;
            name.pet[i - 1] = 0xA0;
        }

        return name;
    }

    void write(u8 *p, isize length) const noexcept
    {
        assert(p);
        assert(length <= len);

        for (isize i = 0; i < length; i++) p[i] = pet[i];
    }

    void write(u8 *p) const noexcept { write(p, len); }

    const char *c_str() const noexcept { return asc; }
    string str() const noexcept { return string(asc); }
};

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

struct FSPattern {

    string glob;
    std::regex regex;

    explicit FSPattern(const string str);
    explicit FSPattern(const char *str) : FSPattern(string(str)) { };

    std::vector<FSPattern> splitted() const;
    bool isAbsolute() const { return !glob.empty() && glob[0] == '/'; }
    bool match(const FSString &name) const;
};

}
