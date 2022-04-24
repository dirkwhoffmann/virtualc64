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
    ERROR_UNKNOWN,

    // Configuration
    ERROR_OPT_UNSUPPORTED,
    ERROR_OPT_INVARG,
    ERROR_OPT_LOCKED,

    // Property storage
    ERROR_INVALID_KEY,
    ERROR_SYNTAX,
    
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
    ERROR_ROM_DRIVE_MISSING,
    ERROR_ROM_MEGA65_MISMATCH,
    
	// Snapshots
	ERROR_SNAP_TOO_OLD,
	ERROR_SNAP_TOO_NEW,
	ERROR_SNAP_IS_BETA,
	ERROR_SNAP_CORRUPTED,
	
    // Drives
    ERROR_DRV_UNCONNECTED,
    
    // Cartridges
    ERROR_CRT_UNKNOWN,
    ERROR_CRT_UNSUPPORTED,
    ERROR_CRT_TOO_MANY_PACKETS,
    ERROR_CRT_CORRUPTED_PACKET,
    
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
    
    static constexpr long minVal = 0;
    static constexpr long maxVal = ERROR_FS_EXPECTED_MAX;
    static bool isValid(long value) { return value >= minVal && value <= maxVal; }
        
    static const char *prefix() { return "ERROR"; }
    static const char *key(ErrorCode value)
    {
        switch (value) {
                
            case ERROR_OK:                   return "OK";
            case ERROR_UNKNOWN:              return "UNKNOWN";
                
            case ERROR_OPT_UNSUPPORTED:      return "OPT_UNSUPPORTED";
            case ERROR_OPT_INVARG:           return "OPT_INV_ARG";
            case ERROR_OPT_LOCKED:           return "OPT_LOCKED";

            case ERROR_INVALID_KEY:          return "INVALID_KEY";
            case ERROR_SYNTAX:               return "SYNTAX";

            case ERROR_OUT_OF_MEMORY:        return "OUT_OF_MEMORY";
                
            case ERROR_FILE_NOT_FOUND:       return "FILE_NOT_FOUND";
            case ERROR_FILE_TYPE_MISMATCH:   return "FILE_TYPE_MISMATCH";
            case ERROR_FILE_CANT_READ:       return "FILE_CANT_READ";
            case ERROR_FILE_CANT_WRITE:      return "FILE_CANT_WRITE";
            case ERROR_FILE_CANT_CREATE:     return "FILE_CANT_CREATE";
            case ERROR_DIR_CANT_CREATE:      return "DIR_CANT_CREATE";
            case ERROR_DIR_NOT_EMPTY:        return "DIR_NOT_EMPTY";
                
            case ERROR_ROM_BASIC_MISSING:    return "ROM_BASIC_MISSING";
            case ERROR_ROM_CHAR_MISSING:     return "ROM_CHAR_MISSING";
            case ERROR_ROM_KERNAL_MISSING:   return "ROM_KERNAL_MISSING";
            case ERROR_ROM_DRIVE_MISSING:    return "ROM_DRIVE_MISSING";
            case ERROR_ROM_MEGA65_MISMATCH:  return "ROM_MEGA65_MISMATCH";
                
            case ERROR_SNAP_TOO_OLD:         return "SNAP_TOO_OLD";
            case ERROR_SNAP_TOO_NEW:         return "SNAP_TOO_NEW";
			case ERROR_SNAP_IS_BETA:		 return "SNAP_IS_BETA";
			case ERROR_SNAP_CORRUPTED:		 return "SNAP_CORRUPTED";
				
            case ERROR_DRV_UNCONNECTED:      return "DRV_UNCONNECTED";

            case ERROR_CRT_UNKNOWN:          return "ERROR_CRT_UNKNOWN";
            case ERROR_CRT_UNSUPPORTED:      return "CRT_UNSUPPORTED";
            case ERROR_CRT_TOO_MANY_PACKETS: return "CRT_TOO_MANY_PACKETS";
			case ERROR_CRT_CORRUPTED_PACKET: return "CRT_CORRUPTED_PACKET";
                
            case ERROR_FS_UNSUPPORTED:       return "FS_UNSUPPORTED";
            case ERROR_FS_WRONG_CAPACITY:    return "FS_WRONG_CAPACITY";
            case ERROR_FS_CORRUPTED:         return "FS_CORRUPTED";
            case ERROR_FS_HAS_NO_FILES:      return "ERROR_FS_HAS_NO_FILES";
            case ERROR_FS_HAS_CYCLES:        return "FS_HAS_CYCLES";
            case ERROR_FS_CANT_IMPORT:       return "FS_CANT_IMPORT";
            case ERROR_FS_EXPECTED_VAL:      return "FS_EXPECTED_VAL";
            case ERROR_FS_EXPECTED_MIN:      return "FS_EXPECTED_MIN";
            case ERROR_FS_EXPECTED_MAX:      return "FS_EXPECTED_MAX";

            case ERROR_COUNT:                return "???";
        }
        return "???";
    }
};
#endif
