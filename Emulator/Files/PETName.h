// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _PET_NAME_H
#define _PET_NAME_H

#include "C64Object.h"
// #include "Utils.h"
#include "PETName.h"

template <int len> struct PETName : C64Object {
    
    // PETSCII representation
    u8 pet[len + 1];
    
    // ASCII representation
    char str[len + 1];
    
    // The pad characters (usually 0xA0)
    u8 pad;
    
    
    PETName(const u8 *_pet, u8 _pad = 0xA0) : pad(_pad)
    {
        assert(_pet);
        
        memset(pet, pad, sizeof(pet));
        memset(str, 0x0, sizeof(str));
        
        for (int i = 0; i < len && _pet[i] != pad; i++) {
                    
            str[i] = petscii2printable(_pet[i], '_');
            pet[i] = _pet[i];
        }
    }

    PETName(const char *_str, u8 _pad = 0xA0) : pad(_pad)
    {
        assert(_str);
        
        memset(pet, pad, sizeof(pet));
        memset(str, 0x0, sizeof(str));
        
        for (int i = 0; i < len && _str[i] != 0x00; i++) {
            
            str[i] = _str[i];
            pet[i] = ascii2pet(_str[i]);
        }
    }

    PETName(std::string str) : PETName(str.c_str())
    {
    }
    
    bool operator== (PETName &rhs)
    {
        int i = 0;
        
        while (pet[i] != pad || rhs.pet[i] != pad) {
            if (pet[i] != rhs.pet[i]) return false;
            i++;
        }
        return true;
    }

    void write(u8 *p, size_t length)
    {
        assert(p);
        assert(length <= len);
        
        for (size_t i = 0; i < length; i++) p[i] = pet[i];
    }

    void write(u8 *p) { write(p, len); }
    const char *getDescription() override { return "PETName"; }
    const char *c_str() { return str; }
};

#endif
