// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "MemoryPublicTypes.h"
#include "Reflection.h"

//
// Reflection APIs
//

struct MemoryTypeEnum : Reflection<MemoryTypeEnum, MemoryType> {
    
    static bool isValid(long value)
    {
        return value >= 1 && value < M_COUNT;
    }
    
    static const char *prefix() { return "M"; }
    static const char *key(MemoryType value)
    {
        switch (value) {
                
            case M_RAM:     return "M_RAM";
            case M_CHAR:    return "M_CHAR";
            case M_KERNAL:  return "M_KERNAL";
            case M_BASIC:   return "M_BASIC";
            case M_IO:      return "M_IO";
            case M_CRTLO:   return "M_CRTLO";
            case M_CRTHI:   return "M_CRTHI";
            case M_PP:      return "M_PP";
            case M_NONE:    return "M_NONE";
            case M_COUNT:   return "???";
        }
        return "???";
    }
};

struct RamPatternEnum : Reflection<RamPatternEnum, RamPattern> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < RAM_PATTERN_COUNT;
    }
    
    static const char *prefix() { return "RAM_PATTERN"; }
    static const char *key(RamPattern value)
    {
        switch (value) {
                
            case RAM_PATTERN_C64:    return "C64";
            case RAM_PATTERN_C64C:   return "C64C";
            case RAM_PATTERN_COUNT:  return "???";
        }
        return "???";
    }
};
