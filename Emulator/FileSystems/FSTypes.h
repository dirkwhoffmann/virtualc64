// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FSPublicTypes.h"
#include "Reflection.h"

//
// Reflection APIs
//

struct DOSTypeEnum : util::Reflection<DOSTypeEnum, DOSType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= DOS_TYPE_CBM;
    }
    
    static const char *prefix() { return "DOS_TYPE"; }
    static const char *key(DOSType value)
    {
        switch (value) {
                
            case DOS_TYPE_NODOS:  return "NODOS";
            case DOS_TYPE_CBM:    return "CBM";
            default:              return "???";
        }
    }
};

struct FSFileTypeEnum : util::Reflection<FSFileTypeEnum, FSFileType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < FS_FILETYPE_COUNT;
    }
    
    static const char *prefix() { return "FS_FILETYPE"; }
    static const char *key(FSFileType value)
    {
        switch (value) {
                
            case FS_FILETYPE_DEL:    return "DEL";
            case FS_FILETYPE_SEQ:    return "SEQ";
            case FS_FILETYPE_PRG:    return "PRG";
            case FS_FILETYPE_USR:    return "USR";
            case FS_FILETYPE_REL:    return "REL";
            case FS_FILETYPE_COUNT:  return "???";
        }
        return "???";
    }
};

struct FSBlockTypeEnum : util::Reflection<FSBlockTypeEnum, FSBlockType> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < FS_BLOCKTYPE_COUNT;
    }
    
    static const char *prefix() { return "FS_BLOCKTYPE"; }
    static const char *key(FSBlockType value)
    {
        switch (value) {
                
            case FS_BLOCKTYPE_UNKNOWN:  return "UNKNOWN";
            case FS_BLOCKTYPE_BAM:      return "BAM";
            case FS_BLOCKTYPE_DIR:      return "DIR";
            case FS_BLOCKTYPE_DATA:     return "DATA";
            case FS_BLOCKTYPE_COUNT:    return "???";
        }
        return "???";
    }
};

struct FSUsageEnum : util::Reflection<FSUsageEnum, FSUsage> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value <= FS_BLOCKTYPE_DATA;
    }
    
    static const char *prefix() { return "FS_USAGE"; }
    static const char *key(FSUsage value) { return "???"; }
};
