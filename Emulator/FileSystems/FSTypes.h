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

inline const char *sFSType(DOSType value)
{
    switch (value) {
            
        case DOSType_NODOS:  return "NODOS";
        case DOSType_CBM:    return "CBM";
        default:             return "???";
    }
}

enum_long(FSFileType)
{
    FSFileType_DEL = 0,
    FSFileType_SEQ = 1,
    FSFileType_PRG = 2,
    FSFileType_USR = 3,
    FSFileType_REL = 4
};

inline bool isFSFileType(long value)
{
    return (unsigned long)value <= FSFileType_REL;
}

inline const char *sFSFileType(FSFileType value)
{
    switch (value) {
            
        case FSFileType_PRG:  return "PRG";
        case FSFileType_SEQ:  return "SEQ";
        case FSFileType_REL:  return "REL";
        case FSFileType_USR:  return "USR";
        default:      return "???";
    }
}

enum_long(FSBlockType)
{
    FSBlockType_UNKNOWN,
    FSBlockType_BAM,
    FSBlockType_DIR,
    FSBlockType_DATA
};

inline bool
isFSBlockType(long value)
{
    return (unsigned long)value <= FSBlockType_DATA;
}

inline const char *
sFSBlockType(FSBlockType type)
{
    assert(isFSBlockType(type));

    switch (type) {
            
        case FSBlockType_UNKNOWN:  return "UNKNOWN";
        case FSBlockType_BAM:      return "BAM";
        case FSBlockType_DIR:      return "DIR";
        case FSBlockType_DATA:     return "DATA";
        default:                   return "???";
    }
}

enum_long(FSUsage)
{
    FSUsage_UNKNOWN,
    FSUsage_UNUSED,
    FSUsage_DOS_VERSION,
    FSUsage_DOS_TYPE,
    FSUsage_ALLOCATION_BITS,
    FSUsage_DISK_ID,
    FSUsage_DISK_NAME,
    FSUsage_TRACK_LINK,
    FSUsage_SECTOR_LINK,
    FSUsage_FIRST_FILE_TRACK,
    FSUsage_FIRST_FILE_SECTOR,
    FSUsage_FIRST_REL_TRACK,
    FSUsage_FIRST_REL_SECTOR,
    FSUsage_FIRST_DIR_TRACK,
    FSUsage_FIRST_DIR_SECTOR,
    FSUsage_FILE_TYPE,
    FSUsage_FILE_NAME,
    FSUsage_FILE_LENGTH_LO,
    FSUsage_FILE_LENGTH_HI,
    FSUsage_REL_RECORD_LENGTH,
    FSUsage_GEOS,
    FSUsage_DATA
};

inline bool
isFSBlockItem(long value)
{
    return (unsigned long)value <= FSUsage_DATA;
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

inline const char *sFSError(FSError value)
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
