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

#include "BasicTypes.h"
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

struct DOSTypeEnum : Reflectable<DOSTypeEnum, DOSType> {

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

struct FSFileTypeEnum : Reflectable<FSFileTypeEnum, FSFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSFileType::REL);

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

struct FSBlockTypeEnum : Reflectable<FSBlockTypeEnum, FSBlockType> {

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

enum class FSUsage
{
    UNKNOWN,
    UNUSED,
    DOS_VERSION,
    DOS_TYPE,
    ALLOCATION_BITS,
    DISK_ID,
    DISK_NAME,
    TRACK_LINK,
    SECTOR_LINK,
    FIRST_FILE_TRACK,
    FIRST_FILE_SECTOR,
    FIRST_REL_TRACK,
    FIRST_REL_SECTOR,
    FIRST_DIR_TRACK,
    FIRST_DIR_SECTOR,
    FILE_TYPE,
    FILE_NAME,
    FILE_LENGTH_LO,
    FILE_LENGTH_HI,
    REL_RECORD_LENGTH,
    GEOS,
    DATA
};

struct FSUsageEnum : Reflectable<FSUsageEnum, FSUsage> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FSUsage::DATA);

    static const char *_key(FSUsage value)
    {
        switch (value) {

            case FSUsage::UNKNOWN:              return "UNKNOWN";
            case FSUsage::UNUSED:               return "UNUSED";
            case FSUsage::DOS_VERSION:          return "DOS_VERSION";
            case FSUsage::DOS_TYPE:             return "DOS_TYPE";
            case FSUsage::ALLOCATION_BITS:      return "ALLOCATION_BITS";
            case FSUsage::DISK_ID:              return "DISK_ID";
            case FSUsage::DISK_NAME:            return "DISK_NAME";
            case FSUsage::TRACK_LINK:           return "TRACK_LINK";
            case FSUsage::SECTOR_LINK:          return "SECTOR_LINK";
            case FSUsage::FIRST_FILE_TRACK:     return "FIRST_FILE_TRACK";
            case FSUsage::FIRST_FILE_SECTOR:    return "FIRST_FILE_SECTOR";
            case FSUsage::FIRST_REL_TRACK:      return "FIRST_REL_TRACK";
            case FSUsage::FIRST_REL_SECTOR:     return "FIRST_REL_SECTOR";
            case FSUsage::FIRST_DIR_TRACK:      return "FIRST_DIR_TRACK";
            case FSUsage::FIRST_DIR_SECTOR:     return "FIRST_DIR_SECTOR";
            case FSUsage::FILE_TYPE:            return "FILE_TYPE";
            case FSUsage::FILE_NAME:            return "FILE_NAME";
            case FSUsage::FILE_LENGTH_LO:       return "FILE_LENGTH_LO";
            case FSUsage::FILE_LENGTH_HI:       return "FILE_LENGTH_HI";
            case FSUsage::REL_RECORD_LENGTH:    return "REL_RECORD_LENGTH";
            case FSUsage::GEOS:                 return "GEOS";
            case FSUsage::DATA:                 return "DATA";
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
