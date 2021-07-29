// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

enum_long(ERROR_CODE)
{
    ERROR_OK,

    // Configuration
    ERROR_OPT_INV_ARG,
    ERROR_OPT_LOCKED,
    
    // Memory
    ERROR_OUT_OF_MEMORY,

    // File IO
    ERROR_FILE_NOT_FOUND,
    ERROR_FILE_TYPE_MISMATCH,
    ERROR_FILE_CANT_READ,
    ERROR_FILE_CANT_WRITE,
    ERROR_FILE_CANT_CREATE,
    ERROR_DIR_CANT_CREATE,
    ERROR_DIR_NOT_EMPTY,

    // Roms
    ERROR_ROM_BASIC_MISSING,
    ERROR_ROM_CHAR_MISSING,
    ERROR_ROM_KERNAL_MISSING,
    ERROR_ROM_MEGA65_MISMATCH,
    
    // Snapshots
    ERROR_SNP_TOO_OLD,
    ERROR_SNP_TOO_NEW,

    // Drives
    ERROR_DRV_UNCONNECTED,
    
    // Cartridges
    ERROR_CRT_UNSUPPORTED,
    
    // File systems
    ERROR_FS_UNSUPPORTED,
    ERROR_FS_WRONG_CAPACITY,
    ERROR_FS_CORRUPTED,
    ERROR_FS_HAS_NO_FILES,
    ERROR_FS_HAS_CYCLES,
    ERROR_FS_CANT_IMPORT,
    ERROR_FS_EXPECTED_VAL,
    ERROR_FS_EXPECTED_MIN,
    ERROR_FS_EXPECTED_MAX,
    
    ERROR_COUNT
};
typedef ERROR_CODE ErrorCode;

#ifdef __cplusplus
struct ErrorCodeEnum : util::Reflection<ErrorCodeEnum, ErrorCode> {
    
    static bool isValid(long value)
    {
        return (unsigned long)value < ERROR_COUNT;
    }
    
    static const char *prefix() { return "ERROR"; }
    static const char *key(ErrorCode value)
    {
        switch (value) {
                
            case ERROR_OK:                  return "OK";
                
            case ERROR_OPT_INV_ARG:         return "OPT_INV_ARG";
            case ERROR_OPT_LOCKED:          return "OPT_LOCKED";

            case ERROR_OUT_OF_MEMORY:       return "OUT_OF_MEMORY";
                
            case ERROR_FILE_NOT_FOUND:      return "FILE_NOT_FOUND";
            case ERROR_FILE_TYPE_MISMATCH:  return "FILE_TYPE_MISMATCH";
            case ERROR_FILE_CANT_READ:      return "FILE_CANT_READ";
            case ERROR_FILE_CANT_WRITE:     return "FILE_CANT_WRITE";
            case ERROR_FILE_CANT_CREATE:    return "FILE_CANT_CREATE";
            case ERROR_DIR_CANT_CREATE:     return "DIR_CANT_CREATE";
            case ERROR_DIR_NOT_EMPTY:       return "DIR_NOT_EMPTY";
                
            case ERROR_ROM_BASIC_MISSING:   return "ROM_BASIC_MISSING";
            case ERROR_ROM_CHAR_MISSING:    return "ROM_CHAR_MISSING";
            case ERROR_ROM_KERNAL_MISSING:  return "ROM_KERNAL_MISSING";
            case ERROR_ROM_MEGA65_MISMATCH: return "ROM_MEGA65_MISMATCH";
                
            case ERROR_SNP_TOO_OLD:         return "SNP_TOO_OLD";
            case ERROR_SNP_TOO_NEW:         return "SNP_TOO_NEW";

            case ERROR_DRV_UNCONNECTED:     return "DRV_UNCONNECTED";

            case ERROR_CRT_UNSUPPORTED:     return "CRT_UNSUPPORTED";
                
            case ERROR_FS_UNSUPPORTED:      return "FS_UNSUPPORTED";
            case ERROR_FS_WRONG_CAPACITY:   return "FS_WRONG_CAPACITY";
            case ERROR_FS_CORRUPTED:        return "FS_CORRUPTED";
            case ERROR_FS_HAS_NO_FILES:     return "ERROR_FS_HAS_NO_FILES";
            case ERROR_FS_HAS_CYCLES:       return "FS_HAS_CYCLES";
            case ERROR_FS_CANT_IMPORT:      return "FS_CANT_IMPORT";
            case ERROR_FS_EXPECTED_VAL:     return "FS_EXPECTED_VAL";
            case ERROR_FS_EXPECTED_MIN:     return "FS_EXPECTED_MIN";
            case ERROR_FS_EXPECTED_MAX:     return "FS_EXPECTED_MAX";

            case ERROR_COUNT:               return "???";
        }
        return "???";
    }
};
#endif
