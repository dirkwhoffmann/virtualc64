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

namespace vc64 {

enum class Fault : long
{
    OK,                   ///< No error
    UNKNOWN,              ///< Unclassified error condition

    /*
    // Execution state
    LAUNCH,               ///< Launch error
    POWERED_OFF,          ///< The emulator is powered off
    POWERED_ON,           ///< The emulator is powered on
    RUNNING,              ///< The emulator is running

    // Configuration
    OPT_UNSUPPORTED,      ///< Unsupported configuration option
    OPT_INV_ARG,          ///< Invalid argument
    OPT_INV_ID,           ///< Invalid component id
    OPT_LOCKED,           ///< The option is temporarily locked

    // Property storage
    INVALID_KEY,          ///< Invalid property key
    SYNTAX,               ///< Syntax error

    // CPU
    BP_NOT_FOUND,         ///< Breakpoint is not set
    BP_ALREADY_SET,       ///< Breakpoint is already set
    WP_NOT_FOUND,         ///< Watchpoint is not set
    WP_ALREADY_SET,       ///< Watchpoint is already set

    // Memory
    OUT_OF_MEMORY,        ///< Out of memory
    
    // Roms
    ROM_BASIC_MISSING,    ///< No Basic Rom installed
    ROM_CHAR_MISSING,     ///< No Character Rom installed
    ROM_KERNAL_MISSING,   ///< No Kernel Rom installed
    ROM_DRIVE_MISSING,    ///< No Floppy Drive Rom installed
    ROM_MEGA65_MISMATCH,  ///< MEGA65 Roms have different version numbers
    
    // Drives
    DRV_UNCONNECTED,      ///< Floppy drive is not connected
    DRV_NO_DISK,          ///< Floppy drive contains no disk
    */
    
    // File systems
    FS_UNSUPPORTED,       ///< Unsupported file system
    FS_WRONG_CAPACITY,    ///< Wrong file system capacity
    FS_CORRUPTED,         ///< File system is corrupted
    FS_HAS_NO_FILES,      ///< File system is empty
    FS_HAS_CYCLES,        ///< File system has cyclic links
    FS_CANT_IMPORT,       ///< Failed to import a file system
    FS_EXPECTED_VAL,      ///< Unexpected value found
    FS_EXPECTED_MIN,      ///< Value is too small
    FS_EXPECTED_MAX,      ///< Value is too big

    COUNT
};

struct FaultEnum : Reflectable<FaultEnum, Fault> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Fault::FS_EXPECTED_MAX);

    static const char *_key(Fault value)
    {
        switch (value) {

            case Fault::OK:                      return "OK";
            case Fault::UNKNOWN:                 return "UNKNOWN";

                /*
            case Fault::LAUNCH:                  return "LAUNCH";
            case Fault::POWERED_OFF:             return "POWERED_OFF";
            case Fault::POWERED_ON:              return "POWERED_ON";
            case Fault::RUNNING:                 return "RUNNING";

            case Fault::OPT_UNSUPPORTED:         return "OPT_UNSUPPORTED";
            case Fault::OPT_INV_ARG:             return "OPT_INV_ARG";
            case Fault::OPT_INV_ID:              return "OPT_INV_ID";
            case Fault::OPT_LOCKED:              return "OPT_LOCKED";

            case Fault::INVALID_KEY:             return "INVALID_KEY";
            case Fault::SYNTAX:                  return "SYNTAX";

            case Fault::BP_NOT_FOUND:            return "BP_NOT_FOUND";
            case Fault::BP_ALREADY_SET:          return "BP_ALREADY_SET";
            case Fault::WP_NOT_FOUND:            return "WP_NOT_FOUND";
            case Fault::WP_ALREADY_SET:          return "WP_ALREADY_SET";

            case Fault::OUT_OF_MEMORY:           return "OUT_OF_MEMORY";
                
            case Fault::ROM_BASIC_MISSING:       return "ROM_BASIC_MISSING";
            case Fault::ROM_CHAR_MISSING:        return "ROM_CHAR_MISSING";
            case Fault::ROM_KERNAL_MISSING:      return "ROM_KERNAL_MISSING";
            case Fault::ROM_DRIVE_MISSING:       return "ROM_DRIVE_MISSING";
            case Fault::ROM_MEGA65_MISMATCH:     return "ROM_MEGA65_MISMATCH";
               
            case Fault::DRV_UNCONNECTED:         return "DRV_UNCONNECTED";
            case Fault::DRV_NO_DISK:             return "DRV_NO_DISK";
                 */
                
            case Fault::FS_UNSUPPORTED:          return "FS_UNSUPPORTED";
            case Fault::FS_WRONG_CAPACITY:       return "FS_WRONG_CAPACITY";
            case Fault::FS_CORRUPTED:            return "FS_CORRUPTED";
            case Fault::FS_HAS_NO_FILES:         return "FS_HAS_NO_FILES";
            case Fault::FS_HAS_CYCLES:           return "FS_HAS_CYCLES";
            case Fault::FS_CANT_IMPORT:          return "FS_CANT_IMPORT";
            case Fault::FS_EXPECTED_VAL:         return "FS_EXPECTED_VAL";
            case Fault::FS_EXPECTED_MIN:         return "FS_EXPECTED_MIN";
            case Fault::FS_EXPECTED_MAX:         return "FS_EXPECTED_MAX";

            case Fault::COUNT:                   return "???";
        }
        return "???";
    }
    
    static const char *help(Fault value)
    {
        return "";
    }
};

}
