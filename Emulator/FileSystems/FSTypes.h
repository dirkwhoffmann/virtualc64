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

enum_long(FSVolumeType)
{
    FS_NODOS = 0,
    FS_CBM_DOS = 1
};

inline bool isFSVolumeType(long value)
{
    return value >= FS_NODOS && value <= FS_CBM_DOS;
}

inline const char *sFSVolumeType(FSVolumeType value)
{
    switch (value) {
        case FS_NODOS:    return "No DOS";
        case FS_CBM_DOS:  return "Commdore CBM DOS";
        default:          return "???";
    }
}

enum_long(FSFileType)
{
    FS_PRG = 1,
    FS_SEQ,
    FS_REL,
    FS_USR,
    FS_DEL
};

inline bool isFSFileType(long value)
{
    return value >= FS_PRG && value <= FS_DEL;
}

inline const char *sFSFileType(FSFileType value)
{
    switch (value) {
        case FS_PRG:  return "PRG";
        case FS_SEQ:  return "SEQ";
        case FS_REL:  return "REL";
        case FS_USR:  return "USR";
        default:      return "???";
    }
}

enum_long(FSBlockType)
{
    FS_UNKNOWN_BLOCK,
    FS_BAM_BLOCK,
    FS_DIR_BLOCK,
    FS_DATA_BLOCK
};

inline bool
isFSBlockType(long value)
{
    return value >= 0 && value <= FS_DATA_BLOCK;
}

inline const char *
sFSBlockType(FSBlockType type)
{
    assert(isFSBlockType(type));

    switch (type) {
        case FS_UNKNOWN_BLOCK:  return "FS_UNKNOWN_BLOCK";
        case FS_BAM_BLOCK:      return "FS_BAM_BLOCK";
        case FS_DIR_BLOCK:      return "FS_DIR_BLOCK";
        case FS_DATA_BLOCK:     return "FS_DATA_BLOCK";
        default:                return "???";
    }
}

enum_long(FSItemType)
{
    FSI_UNKNOWN,
    FSI_UNUSED,
    FSI_TRACK_REF,
    FSI_SECTOR_REF,
    FSI_BAM_BLOCK,
    FSI_DIR_BLOCK,
    FSI_DATA
};

inline bool
isFSBlockItem(long value)
{
    return value >= 0 && value <= FSI_DATA;
}

enum_long(FSError)
{
    FS_OK,
    
    // File system errors
    FS_UNKNOWN,
    FS_UNSUPPORTED,
    FS_WRONG_CAPACITY,
    FS_OUT_OF_MEMORY,
    FS_HAS_CYCLES,
    FS_CORRUPTED,
    FS_IMPORT_ERROR,

    // Export errors
    FS_DIRECTORY_NOT_EMPTY,
    FS_CANNOT_CREATE_DIR,
    FS_CANNOT_CREATE_FILE,

    // Block errros
    FS_EXPECTED_VALUE,
    FS_EXPECTED_SMALLER_VALUE
};

inline bool isFSError(FSError value)
{
    return value >= FS_OK && value <= FS_EXPECTED_VALUE;
}

inline const char *sFSError(FSError value)
{
    switch (value) {
            
        case FS_OK:                      return "FS_OK";
        case FS_UNKNOWN:                 return "FS_UNKNOWN";
        case FS_UNSUPPORTED:             return "FS_UNSUPPORTED";
        case FS_WRONG_CAPACITY:          return "FS_WRONG_CAPACITY";
        case FS_HAS_CYCLES:              return "FS_HAS_CYCLES";
        case FS_CORRUPTED:               return "FS_CORRUPTED";
            
        case FS_DIRECTORY_NOT_EMPTY:     return "FS_DIRECTORY_NOT_EMPTY";
        case FS_CANNOT_CREATE_DIR:       return "FS_CANNOT_CREATE_DIR";
        case FS_CANNOT_CREATE_FILE:      return "FS_CANNOT_CREATE_FILE";

        case FS_EXPECTED_VALUE:          return "FS_EXPECTED_VALUE";
        
        default:
            return isFSError(value) ? "<other>" : "???";
    }
}

typedef struct
{
    Track t;
    Sector s;
}
BlockRef;

typedef struct
{
    long bitmapErrors;
    long corruptedBlocks;
    long firstErrorBlock;
    long lastErrorBlock;
}
FSErrorReport;

#endif
