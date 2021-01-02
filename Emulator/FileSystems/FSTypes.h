// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _FS_TYPES_H
#define _FS_TYPES_H

#include "Aliases.h"

//
// Enumerations
//

enum_long(DOSType)
{
    DOSType_NODOS,
    DOSType_CBM
};

inline bool isDOSType(long value)
{
    return (unsigned long)value < DOSType_CBM;
}

inline const char *DOSTypeName(DOSType value)
{
    switch (value) {
            
        case DOSType_NODOS:  return "NODOS";
        case DOSType_CBM:    return "CBM";
        default:             return "???";
    }
}

enum_long(FS_FILETYPE)
{
    FS_FILETYPE_DEL = 0,
    FS_FILETYPE_SEQ = 1,
    FS_FILETYPE_PRG = 2,
    FS_FILETYPE_USR = 3,
    FS_FILETYPE_REL = 4
};
typedef FS_FILETYPE FSFileType;

inline bool isFSFileType(long value)
{
    return (unsigned long)value <= FS_FILETYPE_REL;
}

inline const char *FSFileTypeName(FSFileType value)
{
    switch (value) {
            
        case FS_FILETYPE_DEL:  return "DEL";
        case FS_FILETYPE_SEQ:  return "SEQ";
        case FS_FILETYPE_PRG:  return "PRG";
        case FS_FILETYPE_USR:  return "USR";
        case FS_FILETYPE_REL:  return "REL";
    }
    return "???";
}

enum_long(FS_BLOCKTYPE)
{
    FS_BLOCKTYPE_UNKNOWN,
    FS_BLOCKTYPE_BAM,
    FS_BLOCKTYPE_DIR,
    FS_BLOCKTYPE_DATA
};
typedef FS_BLOCKTYPE FSBlockType;

inline bool
isFSBlockType(long value)
{
    return (unsigned long)value <= FS_BLOCKTYPE_DATA;
}

inline const char *
sFSBlockType(FSBlockType type)
{
    switch (type) {
            
        case FS_BLOCKTYPE_UNKNOWN:  return "UNKNOWN";
        case FS_BLOCKTYPE_BAM:      return "BAM";
        case FS_BLOCKTYPE_DIR:      return "DIR";
        case FS_BLOCKTYPE_DATA:     return "DATA";
    }
    return "???";
}

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

inline bool isFSUsage(long value)
{
    return (unsigned long)value <= FS_USAGE_DATA;
}

enum_long(FSError)
{
    FSError_OK,
    
    // File system errors
    FSError_UNKNOWN,
    FSError_UNSUPPORTED,
    FSError_WRONG_CAPACITY,
    FSError_OUT_OF_MEMORY,
    FSError_HAS_CYCLES,
    FSError_CORRUPTED,
    FSError_IMPORT_ERROR,

    // Export errors
    FSError_DIRECTORY_NOT_EMPTY,
    FSError_CANNOT_CREATE_DIR,
    FSError_CANNOT_CREATE_FILE,

    // Block errros
    FSError_EXPECTED,
    FSError_EXPECTED_MIN,
    FSError_EXPECTED_MAX
};

inline bool isFSError(long value)
{
    return (unsigned long)value <= FSError_EXPECTED_MAX;
}

inline const char *FSErrorName(FSError value)
{
    switch (value) {
            
        case FSError_OK:                   return "OK";
        case FSError_UNKNOWN:              return "UNKNOWN";
        case FSError_UNSUPPORTED:          return "UNSUPPORTED";
        case FSError_WRONG_CAPACITY:       return "WRONG_CAPACITY";
        case FSError_HAS_CYCLES:           return "HAS_CYCLES";
        case FSError_CORRUPTED:            return "CORRUPTED";
        case FSError_IMPORT_ERROR:         return "IMPORT_ERROR";

        case FSError_DIRECTORY_NOT_EMPTY:  return "DIRECTORY_NOT_EMPTY";
        case FSError_CANNOT_CREATE_DIR:    return "CANNOT_CREATE_DIR";
        case FSError_CANNOT_CREATE_FILE:   return "CANNOT_CREATE_FILE";

        case FSError_EXPECTED:             return "EXPECTED";
        case FSError_EXPECTED_MIN:         return "EXPECTED_MIN";
        case FSError_EXPECTED_MAX:         return "EXPECTED_MAX";

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
