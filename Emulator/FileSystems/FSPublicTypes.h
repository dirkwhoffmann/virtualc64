// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef FS_PUBLIC_TYPES_H
#define FS_PUBLIC_TYPES_H

#include "Aliases.h"

//
// Enumerations
//

enum_long(DOS_TYPE)
{
    DOS_TYPE_NODOS,
    DOS_TYPE_CBM,
    DOS_TYPE_COUNT
};
typedef DOS_TYPE DOSType;

enum_long(FS_FILETYPE)
{
    FS_FILETYPE_DEL = 0,
    FS_FILETYPE_SEQ = 1,
    FS_FILETYPE_PRG = 2,
    FS_FILETYPE_USR = 3,
    FS_FILETYPE_REL = 4,
    FS_FILETYPE_COUNT
};
typedef FS_FILETYPE FSFileType;

enum_long(FS_BLOCKTYPE)
{
    FS_BLOCKTYPE_UNKNOWN,
    FS_BLOCKTYPE_BAM,
    FS_BLOCKTYPE_DIR,
    FS_BLOCKTYPE_DATA,
    FS_BLOCKTYPE_COUNT
};
typedef FS_BLOCKTYPE FSBlockType;

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
    FS_USAGE_DATA,
    FS_USAGE_COUNT
};
typedef FS_USAGE FSUsage;

enum_long(FS_ERROR)
{
    FS_ERROR_OK,
    
    // File system errors
    FS_ERROR_UNKNOWN,
    FS_ERROR_UNSUPPORTED,
    FS_ERROR_WRONG_CAPACITY,
    FS_ERROR_OUT_OF_MEMORY,
    FS_ERROR_HAS_CYCLES,
    FS_ERROR_CORRUPTED,
    FS_ERROR_IMPORT_ERROR,

    // Export errors
    FS_ERROR_DIRECTORY_NOT_EMPTY,
    FS_ERROR_CANNOT_CREATE_DIR,
    FS_ERROR_CANNOT_CREATE_FILE,

    // Block errros
    FS_ERROR_EXPECTED,
    FS_ERROR_EXPECTED_MIN,
    FS_ERROR_EXPECTED_MAX
};
typedef FS_ERROR FSError;

inline bool isFSError(long value)
{
    return (unsigned long)value <= FS_ERROR_EXPECTED_MAX;
}

inline const char *FSErrorName(FSError value)
{
    switch (value) {
            
        case FS_ERROR_OK:                   return "OK";
        case FS_ERROR_UNKNOWN:              return "UNKNOWN";
        case FS_ERROR_UNSUPPORTED:          return "UNSUPPORTED";
        case FS_ERROR_WRONG_CAPACITY:       return "WRONG_CAPACITY";
        case FS_ERROR_HAS_CYCLES:           return "HAS_CYCLES";
        case FS_ERROR_CORRUPTED:            return "CORRUPTED";
        case FS_ERROR_IMPORT_ERROR:         return "IMPORT_ERROR";

        case FS_ERROR_DIRECTORY_NOT_EMPTY:  return "DIRECTORY_NOT_EMPTY";
        case FS_ERROR_CANNOT_CREATE_DIR:    return "CANNOT_CREATE_DIR";
        case FS_ERROR_CANNOT_CREATE_FILE:   return "CANNOT_CREATE_FILE";

        case FS_ERROR_EXPECTED:             return "EXPECTED";
        case FS_ERROR_EXPECTED_MIN:         return "EXPECTED_MIN";
        case FS_ERROR_EXPECTED_MAX:         return "EXPECTED_MAX";

        default:
            return isFSError(value) ? "<other>" : "???";
    }
}

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

#endif
