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
/// @file

#pragma once

#include "Reflection.h"
#include "DriveTypes.h"

namespace vc64 {

//
// Enumerations
//

/// File system type
enum class DOSType : long
{
    NODOS, ///< No file system
    CBM    ///< C64 CBM file system
};

struct DOSTypeEnum : util::Reflection<DOSTypeEnum, DOSType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(DOSType::CBM);

    static const char *_key(DOSType value)
    {
        switch (value) {

            case DOSType::NODOS:  return "NODOS";
            case DOSType::CBM:    return "CBM";
        }
        return "???";
    }
    
    static const char *help(DOSType value)
    {
        return "";
    }
};

enum class FSFileType : long
{
    DEL,
    SEQ,
    PRG,
    USR,
    REL
};

struct FSFileTypeEnum : util::Reflection<FSFileTypeEnum, FSFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSFileType::REL);

    static const char *prefix() { return "FS_FILETYPE"; }
    static const char *_key(FSFileType value)
    {
        switch (value) {

            case FSFileType::DEL:    return "DEL";
            case FSFileType::SEQ:    return "SEQ";
            case FSFileType::PRG:    return "PRG";
            case FSFileType::USR:    return "USR";
            case FSFileType::REL:    return "REL";
        }
        return "???";
    }
    
    static const char *help(FSFileType value)
    {
        return "";
    }
};

enum class FSBlockType : long
{
    UNKNOWN,
    EMPTY,
    BAM,
    DIR,
    DATA
};

struct FSBlockTypeEnum : util::Reflection<FSBlockTypeEnum, FSBlockType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSBlockType::DATA);

    static const char *_key(FSBlockType value)
    {
        switch (value) {

            case FSBlockType::UNKNOWN:  return "UNKNOWN";
            case FSBlockType::EMPTY:    return "EMPTY";
            case FSBlockType::BAM:      return "BAM";
            case FSBlockType::DIR:      return "DIR";
            case FSBlockType::DATA:     return "DATA";
        }
        return "???";
    }
    
    static const char *help(FSBlockType value)
    {
        return "";
    }
};

enum_long(FS_USAGE)
{
    FS_USAGE_UNKNOWN,
    FS_USAGE_UNUSED,
    FS_USAGE_DOS_VERSION,
    FS_USAGE_DOS_TYPE,
    FS_USAGE_ALLOCATION_BITS,
    FS_USAGE_DISK_ID,
    FS_USAGE_DISK_NAME,
    FS_USAGE_TRACK_LINK,
    FS_USAGE_SECTOR_LINK,
    FS_USAGE_FIRST_FILE_TRACK,
    FS_USAGE_FIRST_FILE_SECTOR,
    FS_USAGE_FIRST_REL_TRACK,
    FS_USAGE_FIRST_REL_SECTOR,
    FS_USAGE_FIRST_DIR_TRACK,
    FS_USAGE_FIRST_DIR_SECTOR,
    FS_USAGE_FILE_TYPE,
    FS_USAGE_FILE_NAME,
    FS_USAGE_FILE_LENGTH_LO,
    FS_USAGE_FILE_LENGTH_HI,
    FS_USAGE_REL_RECORD_LENGTH,
    FS_USAGE_GEOS,
    FS_USAGE_DATA
};
typedef FS_USAGE FSUsage;

struct FSUsageEnum : util::Reflection<FSUsageEnum, FSUsage> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FS_USAGE_DATA;

    static const char *prefix() { return "FS_USAGE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case FS_USAGE_UNKNOWN:              return "FS_USAGE.UNKNOWN";
            case FS_USAGE_UNUSED:               return "FS_USAGE.UNUSED";
            case FS_USAGE_DOS_VERSION:          return "FS_USAGE.DOS_VERSION";
            case FS_USAGE_DOS_TYPE:             return "FS_USAGE.DOS_TYPE";
            case FS_USAGE_ALLOCATION_BITS:      return "FS_USAGE.ALLOCATION_BITS";
            case FS_USAGE_DISK_ID:              return "FS_USAGE.DISK_ID";
            case FS_USAGE_DISK_NAME:            return "FS_USAGE.DISK_NAME";
            case FS_USAGE_TRACK_LINK:           return "FS_USAGE.TRACK_LINK";
            case FS_USAGE_SECTOR_LINK:          return "FS_USAGE.SECTOR_LINK";
            case FS_USAGE_FIRST_FILE_TRACK:     return "FS_USAGE.FIRST_FILE_TRACK";
            case FS_USAGE_FIRST_FILE_SECTOR:    return "FS_USAGE.FIRST_FILE_SECTOR";
            case FS_USAGE_FIRST_REL_TRACK:      return "FS_USAGE.FIRST_REL_TRACK";
            case FS_USAGE_FIRST_REL_SECTOR:     return "FS_USAGE.FIRST_REL_SECTOR";
            case FS_USAGE_FIRST_DIR_TRACK:      return "FS_USAGE.FIRST_DIR_TRACK";
            case FS_USAGE_FIRST_DIR_SECTOR:     return "FS_USAGE.FIRST_DIR_SECTOR";
            case FS_USAGE_FILE_TYPE:            return "FS_USAGE.FILE_TYPE";
            case FS_USAGE_FILE_NAME:            return "FS_USAGE.FILE_NAME";
            case FS_USAGE_FILE_LENGTH_LO:       return "FS_USAGE.FILE_LENGTH_LO";
            case FS_USAGE_FILE_LENGTH_HI:       return "FS_USAGE.FILE_LENGTH_HI";
            case FS_USAGE_REL_RECORD_LENGTH:    return "FS_USAGE.REL_RECORD_LENGTH";
            case FS_USAGE_GEOS:                 return "FS_USAGE.GEOS";
            case FS_USAGE_DATA:                 return "FS_USAGE.DATA";
        }
        return "???";
    }
    
    static const char *help(FSUsage value)
    {
        return "";
    }
};

//
// Structures
//

typedef struct
{
    Track t;
    Sector s;
}
TSLink;

typedef struct
{
    long bitmapErrors;
    long corruptedBlocks;
    long firstErrorBlock;
    long lastErrorBlock;
}
FSErrorReport;

}
