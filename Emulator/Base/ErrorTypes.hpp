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

#include "Aliases.h"
#include "Reflection.hpp"

#include "ErrorTypes.h"

struct ErrorCodeEnum : util::Reflection<ErrorCodeEnum, ErrorCode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = ERROR_FS_EXPECTED_MAX;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "ERROR"; }
    static const char *key(ErrorCode value)
    {
        switch (value) {

            case ERROR_OK:                      return "OK";
            case ERROR_UNKNOWN:                 return "UNKNOWN";

            case ERROR_POWERED_OFF:             return "POWERED_OFF";
            case ERROR_POWERED_ON:              return "POWERED_ON";
            case ERROR_RUNNING:                 return "RUNNING";

            case ERROR_OPT_UNSUPPORTED:         return "OPT_UNSUPPORTED";
            case ERROR_OPT_INVARG:              return "OPT_INV_ARG";
            case ERROR_OPT_LOCKED:              return "OPT_LOCKED";

            case ERROR_INVALID_KEY:             return "INVALID_KEY";
            case ERROR_SYNTAX:                  return "SYNTAX";

            case ERROR_BP_NOT_FOUND:            return "BP_NOT_FOUND";
            case ERROR_BP_ALREADY_SET:          return "BP_ALREADY_SET";
            case ERROR_WP_NOT_FOUND:            return "WP_NOT_FOUND";
            case ERROR_WP_ALREADY_SET:          return "WP_ALREADY_SET";

            case ERROR_OUT_OF_MEMORY:           return "OUT_OF_MEMORY";

            case ERROR_DIR_NOT_FOUND:           return "DIR_NOT_FOUND";
            case ERROR_DIR_ACCESS_DENIED:       return "DIR_ACCESS_DENIED";
            case ERROR_DIR_CANT_CREATE:         return "DIR_CANT_CREATE";
            case ERROR_DIR_NOT_EMPTY:           return "DIR_NOT_EMPTY";
            case ERROR_FILE_NOT_FOUND:          return "FILE_NOT_FOUND";
            case ERROR_FILE_EXISTS:             return "FILE_EXISTS";
            case ERROR_FILE_IS_DIRECTORY:       return "FILE_IS_DIRECtORY";
            case ERROR_FILE_ACCESS_DENIED:      return "FILE_ACCESS_DENIED";
            case ERROR_FILE_TYPE_MISMATCH:      return "FILE_TYPE_MISMATCH";
            case ERROR_FILE_TYPE_UNSUPPORTED:   return "FILE_TYPE_UNSUPPORTED";
            case ERROR_FILE_CANT_READ:          return "FILE_CANT_READ";
            case ERROR_FILE_CANT_WRITE:         return "FILE_CANT_WRITE";
            case ERROR_FILE_CANT_CREATE:        return "FILE_CANT_CREATE";

            case ERROR_ROM_BASIC_MISSING:       return "ROM_BASIC_MISSING";
            case ERROR_ROM_CHAR_MISSING:        return "ROM_CHAR_MISSING";
            case ERROR_ROM_KERNAL_MISSING:      return "ROM_KERNAL_MISSING";
            case ERROR_ROM_DRIVE_MISSING:       return "ROM_DRIVE_MISSING";
            case ERROR_ROM_MEGA65_MISMATCH:     return "ROM_MEGA65_MISMATCH";

            case ERROR_REC_LAUNCH:              return "REC_LAUNCH";

            case ERROR_SNAP_TOO_OLD:            return "SNAP_TOO_OLD";
            case ERROR_SNAP_TOO_NEW:            return "SNAP_TOO_NEW";
            case ERROR_SNAP_IS_BETA:            return "SNAP_IS_BETA";
            case ERROR_SNAP_CORRUPTED:            return "SNAP_CORRUPTED";

            case ERROR_DRV_UNCONNECTED:         return "DRV_UNCONNECTED";

            case ERROR_CRT_UNKNOWN:             return "ERROR_CRT_UNKNOWN";
            case ERROR_CRT_UNSUPPORTED:         return "CRT_UNSUPPORTED";
            case ERROR_CRT_TOO_MANY_PACKETS:    return "CRT_TOO_MANY_PACKETS";
            case ERROR_CRT_CORRUPTED_PACKET:    return "CRT_CORRUPTED_PACKET";

            case ERROR_FS_UNSUPPORTED:          return "FS_UNSUPPORTED";
            case ERROR_FS_WRONG_CAPACITY:       return "FS_WRONG_CAPACITY";
            case ERROR_FS_CORRUPTED:            return "FS_CORRUPTED";
            case ERROR_FS_HAS_NO_FILES:         return "ERROR_FS_HAS_NO_FILES";
            case ERROR_FS_HAS_CYCLES:           return "FS_HAS_CYCLES";
            case ERROR_FS_CANT_IMPORT:          return "FS_CANT_IMPORT";
            case ERROR_FS_EXPECTED_VAL:         return "FS_EXPECTED_VAL";
            case ERROR_FS_EXPECTED_MIN:         return "FS_EXPECTED_MIN";
            case ERROR_FS_EXPECTED_MAX:         return "FS_EXPECTED_MAX";

            case ERROR_COUNT:                   return "???";
        }
        return "???";
    }
};
