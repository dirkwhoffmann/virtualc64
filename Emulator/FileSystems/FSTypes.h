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
enum_long(DOS_TYPE)
{
    DOS_TYPE_NODOS, ///< No file system
    DOS_TYPE_CBM    ///< C64 CBM file system
};
typedef DOS_TYPE DOSType;

struct DOSTypeEnum : util::Reflection<DOSTypeEnum, DOSType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DOS_TYPE_CBM;

    static const char *prefix() { return "DOS_TYPE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case DOS_TYPE_NODOS:  return "NODOS";
            case DOS_TYPE_CBM:    return "CBM";
        }
        return "???";
    }
};

enum_long(FS_FILETYPE)
{
    FS_FILETYPE_DEL,
    FS_FILETYPE_SEQ,
    FS_FILETYPE_PRG,
    FS_FILETYPE_USR,
    FS_FILETYPE_REL
};
typedef FS_FILETYPE FSFileType;

struct FSFileTypeEnum : util::Reflection<FSFileTypeEnum, FSFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FS_FILETYPE_REL;

    static const char *prefix() { return "FS_FILETYPE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case FS_FILETYPE_DEL:    return "DEL";
            case FS_FILETYPE_SEQ:    return "SEQ";
            case FS_FILETYPE_PRG:    return "PRG";
            case FS_FILETYPE_USR:    return "USR";
            case FS_FILETYPE_REL:    return "REL";
        }
        return "???";
    }
};

enum_long(FS_BLOCKTYPE)
{
    FS_BLOCKTYPE_UNKNOWN,
    FS_BLOCKTYPE_EMPTY,
    FS_BLOCKTYPE_BAM,
    FS_BLOCKTYPE_DIR,
    FS_BLOCKTYPE_DATA
};
typedef FS_BLOCKTYPE FSBlockType;

struct FSBlockTypeEnum : util::Reflection<FSBlockTypeEnum, FSBlockType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FS_BLOCKTYPE_DATA;

    static const char *prefix() { return "FS_BLOCKTYPE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case FS_BLOCKTYPE_UNKNOWN:  return "UNKNOWN";
            case FS_BLOCKTYPE_EMPTY:    return "EMPTY";
            case FS_BLOCKTYPE_BAM:      return "BAM";
            case FS_BLOCKTYPE_DIR:      return "DIR";
            case FS_BLOCKTYPE_DATA:     return "DATA";
        }
        return "???";
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
    static const char *_key(long value) { return "???"; }
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
