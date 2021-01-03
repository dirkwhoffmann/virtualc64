// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef FILE_TYPES_H
#define FILE_TYPES_H

#include "FilePublicTypes.h"
#include "Reflection.h"

//
// Reflection APIs
//

struct FileTypeEnum : Reflection<FileTypeEnum, FileType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < FILETYPE_COUNT;
    }
    
    static const char *prefix() { return "FILETYPE"; }
    static const char *key(FileType value)
    {
        switch (value) {
                
            case FILETYPE_UNKNOWN:    return "UNKNOWN";
            case FILETYPE_V64:        return "V64";
            case FILETYPE_CRT:        return "CRT";
            case FILETYPE_T64:        return "T64";
            case FILETYPE_PRG:        return "PRG";
            case FILETYPE_FOLDER:     return "FOLDER";
            case FILETYPE_P00:        return "P00";
            case FILETYPE_D64:        return "D64";
            case FILETYPE_G64:        return "G64";
            case FILETYPE_TAP:        return "TAP";
            case FILETYPE_BASIC_ROM:  return "ROM";
            case FILETYPE_CHAR_ROM:   return "ROM";
            case FILETYPE_KERNAL_ROM: return "ROM";
            case FILETYPE_VC1541_ROM: return "ROM";
            case FILETYPE_COUNT:      return "???";
        }
        return "???";
    }
};

struct TAPVersionEnum : Reflection<TAPVersionEnum, TAPVersion> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= TAP_VERSION_ADVANCED;
    }
    
    static const char *prefix() { return "TAP_VERSION"; }
    static const char *key(TAPVersion value)
    {
        switch (value) {
                
            case TAP_VERSION_ORIGINAL:  return "ORIGINAL";
            case TAP_VERSION_ADVANCED:  return "ADVANCED";
            default:                    return "???";
        }
    }
};

#endif
