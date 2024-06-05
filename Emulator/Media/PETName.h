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

#include "CoreObject.h"
#include <assert.h>

namespace vc64 {

template <int len> class PETName : CoreObject {

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

    PETName(const u8 *_pet, u8 _pad = 0xA0) : pad(_pad)
    {
        assert(_pet);
        
        memset(pet, pad, sizeof(pet));
        memset(asc, 0x0, sizeof(asc));
        
        for (int i = 0; i < len && _pet[i] != pad; i++) {

            asc[i] = petscii2printable(_pet[i], '_');
            pet[i] = _pet[i];
        }
    }

    PETName(const char *_str, u8 _pad = 0xA0) : pad(_pad)
    {
        assert(_str);
        
        memset(pet, pad, sizeof(pet));
        memset(asc, 0x0, sizeof(asc));
        
        for (int i = 0; i < len && _str[i] != 0x00; i++) {
            
            asc[i] = _str[i];
            pet[i] = ascii2pet(_str[i]);
        }
    }

    PETName(string str) : PETName(str.c_str()) { }
    
    void setPad(u8 _pad) {

        for (int i = 0; i < len; i++) {
            if (pet[i] == pad) pet[i] = _pad;
        }
        pad = _pad;
    }

    const char *objectName() const override { return "PETName"; }
    
    bool operator== (PETName &rhs)
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
    
    void write(u8 *p, isize length)
    {
        assert(p);
        assert(length <= len);
        
        for (isize i = 0; i < length; i++) p[i] = pet[i];
    }
    
    void write(u8 *p) { write(p, len); }
    
    const char *c_str() { return asc; }
    string str() { return string(asc); }
};

}
