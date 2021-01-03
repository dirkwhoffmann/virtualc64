// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _REFLECTION_H
#define _REFLECTION_H

#include "Aliases.h"
#include <stdio.h>
// #include <string>

// using std::string;

template <class T, typename E> struct Reflection {

    // Returns the shortened key as a C string
    static const char *key(long nr) { return T::key((E)nr); }
    
    // Returns the key component or the full key as a C++ string
    /*
    static string suffix(long nr) { return string(key(nr)); }
    static string prefix() { return suffix(-1); }
    static string fullKey(long nr) {
        return prefix() + "_" + (T::isValid(nr) ? suffix(nr) : "???");
    }
    */
    
    // Verifies a key (used by the configuration methods)
    static bool verify(long nr, long min = 1) {
        
        if (T::isValid(nr)) return true;
        
        printf("%ld doesn't specify a valid key. Valid keys are: ", nr);
        
        for (long i = 0 ;; i++) {
            
            if (T::isValid(i)) {
                
                if (i) printf(", ");
                if (T::prefix()) printf("%s_", T::prefix());
                printf("%s", key(i));
           
            } else if (i >= min) break;
        }
        
        printf("\n");
        return false;
    }
};

#endif
