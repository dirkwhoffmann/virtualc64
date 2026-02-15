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

#pragma once

#include "BasicTypes.h"

namespace vc64 {

struct CoreError : public Error {

    static constexpr long OK                =   0; ///< No error
    static constexpr long UNKNOWN           =   1; ///< Unclassified error condition

    // Execution state
    static constexpr long LAUNCH            = 100; ///< Launch error
    static constexpr long POWERED_OFF       = 101; ///< The emulator is powered off
    static constexpr long POWERED_ON        = 102; ///< The emulator is powered on
    static constexpr long RUNNING           = 103; ///< The emulator is running

    // Configuration
    static constexpr long OPT_UNSUPPORTED   = 200; ///< Unsupported configuration option
    static constexpr long OPT_INV_ARG       = 201; ///< Invalid argument
    static constexpr long OPT_INV_ID        = 202; ///< Invalid component id
    static constexpr long OPT_LOCKED        = 203; ///< The option is temporarily locked

    // Property storage
    static constexpr long INVALID_KEY       = 300; ///< Invalid property key
    static constexpr long SYNTAX            = 301; ///< Syntax error

    // CPU
    static constexpr long CPU_UNSUPPORTED   = 400; ///< Unsupported CPU model
    static constexpr long GUARD_NOT_FOUND   = 401; ///< Guard is not set
    static constexpr long GUARD_ALREADY_SET = 402; ///< Guard is already set
    static constexpr long BP_NOT_FOUND      = 403; ///< Breakpoint is not set (DEPRECATED)
    static constexpr long BP_ALREADY_SET    = 404; ///< Breakpoint is already set (DEPRECATED)
    static constexpr long WP_NOT_FOUND      = 405; ///< Watchpoint is not set (DEPRECATED)
    static constexpr long WP_ALREADY_SET    = 406; ///< Watchpoint is already set (DEPRECATED)

    // Memory
    static constexpr long OUT_OF_MEMORY     = 500; ///< Out of memory

    /* File IO
    DIR_NOT_FOUND,        ///< Directory does not exist
    DIR_ACCESS_DENIED,    ///< File access denied
    DIR_CANT_CREATE,      ///< Unable to create a directory
    DIR_NOT_EMPTY,        ///< Directory is not empty
    FILE_NOT_FOUND,       ///< File not found error
    FILE_EXISTS,          ///< File already exists
    FILE_IS_DIRECTORY,    ///< The file is a directory
    FILE_ACCESS_DENIED,   ///< File access denied
    FILE_TYPE_MISMATCH,   ///< File type mismatch
    FILE_TYPE_UNSUPPORTED,///< Unsupported file type
    FILE_CANT_READ,       ///< Can't read from file
    FILE_CANT_WRITE,      ///< Can't write to file
    FILE_CANT_CREATE,     ///< Can't create file
    */
    
    // Roms
    static constexpr long ROM_BASIC_MISSING     = 600; ///< No Basic Rom installed
    static constexpr long ROM_CHAR_MISSING      = 601; ///< No Character Rom installed
    static constexpr long ROM_KERNAL_MISSING    = 602; ///< No Kernal Rom installed
    static constexpr long ROM_DRIVE_MISSING     = 603; ///< No Floppy Drive Rom installed
    static constexpr long ROM_MEGA65_MISMATCH   = 604; ///< MEGA65 Roms have different version numbers

    /*
    // Snapshots
    static constexpr long SNAP_TOO_OLD          = 700; ///< Snapshot was created with an older version
    static constexpr long SNAP_TOO_NEW          = 701; ///< Snapshot was created with a later version
    static constexpr long SNAP_IS_BETA          = 702; ///< Snapshot was created with a beta release
    static constexpr long SNAP_CORRUPTED        = 703; ///< Snapshot data is corrupted
    */
    
    // Drives
    static constexpr long DRV_UNCONNECTED       = 700; ///< Floppy drive is not connected
    static constexpr long DRV_NO_DISK           = 701; ///< Floppy drive contains no disk

    /*
    // Cartridges
    static constexpr long CRT_NO_CARTRIDGE      = 801; ///< No cartridge attached
    static constexpr long CRT_UNKNOWN           = 802; ///< Unknown cartridge type
    static constexpr long CRT_UNSUPPORTED       = 803; ///< Unsupported cartridge type
    static constexpr long CRT_TOO_MANY_PACKETS  = 804; ///< CRT file contains too many Rom packets
    static constexpr long CRT_CORRUPTED_PACKET  = 805; ///< CRT file contains a corrupted Rom package
    */
    /*
    // Remote servers
    static constexpr long SOCK_CANT_CREATE      = 500;
    static constexpr long SOCK_CANT_CONNECT     = 500;
    static constexpr long SOCK_CANT_BIND        = 500;
    static constexpr long SOCK_CANT_LISTEN    = 500;
    static constexpr long SOCK_CANT_ACCEPT    = 500;
    static constexpr long SOCK_CANT_RECEIVE    = 500;
    static constexpr long SOCK_CANT_SEND    = 500;
    static constexpr long SOCK_DISCONNECTED    = 500;
    static constexpr long SERVER_PORT_IN_USE    = 500;
    static constexpr long SERVER_ON    = 500;
    static constexpr long SERVER_OFF    = 500;
    static constexpr long SERVER_RUNNING    = 500;
    static constexpr long SERVER_NOT_RUNNING    = 500;
    static constexpr long SERVER_NO_CLIENT    = 500;

    // DAP server
    static constexpr long DAP_INVALID_FORMAT    = 500;
    static constexpr long DAP_UNRECOGNIZED_CMD    = 500;
    static constexpr long DAP_UNSUPPORTED_CMD    = 500;

    // GDB server
    static constexpr long GDB_NO_ACK    = 500;
    static constexpr long GDB_INVALID_FORMAT    = 500;
    static constexpr long GDB_INVALID_CHECKSUM    = 500;
    static constexpr long GDB_UNRECOGNIZED_CMD    = 500;
    static constexpr long GDB_UNSUPPORTED_CMD    = 500;
    */
    /*
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
    */

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                    return "OK";
            case UNKNOWN:               return "UNKNOWN";

            case LAUNCH:                return "LAUNCH";
            case POWERED_OFF:           return "POWERED_OFF";
            case POWERED_ON:            return "POWERED_ON";
            case RUNNING:               return "RUNNING";

            case OPT_UNSUPPORTED:       return "OPT_UNSUPPORTED";
            case OPT_INV_ARG:           return "OPT_INV_ARG";
            case OPT_INV_ID:            return "OPT_INV_ID";
            case OPT_LOCKED:            return "OPT_LOCKED";

            case INVALID_KEY:           return "INVALID_KEY";
            case SYNTAX:                return "SYNTAX";

            case CPU_UNSUPPORTED:       return "CPU_UNSUPPORTED";
            case GUARD_NOT_FOUND:       return "GUARD_NOT_FOUND";
            case GUARD_ALREADY_SET:     return "GUARD_ALREADY_SET";
            case BP_NOT_FOUND:          return "BP_NOT_FOUND";
            case BP_ALREADY_SET:        return "BP_ALREADY_SET";
            case WP_NOT_FOUND:          return "WP_NOT_FOUND";
            case WP_ALREADY_SET:        return "WP_ALREADY_SET";

            case OUT_OF_MEMORY:         return "OUT_OF_MEMORY";

            case ROM_BASIC_MISSING:     return "ROM_BASIC_MISSING";
            case ROM_CHAR_MISSING:      return "ROM_CHAR_MISSING";
            case ROM_KERNAL_MISSING:    return "ROM_KERNAL_MISSING";
            case ROM_DRIVE_MISSING:     return "ROM_DRIVE_MISSING";
            case ROM_MEGA65_MISMATCH:   return "ROM_MEGA65_MISMATCH";

            case DRV_UNCONNECTED:       return "DRV_UNCONNECTED";
            case DRV_NO_DISK:           return "DRV_NO_DISK";
        }
        return "???";
    }

    explicit CoreError(long fault, const string &s);
    explicit CoreError(long fault, const char *s) : CoreError(fault, string(s)) { };
    explicit CoreError(long fault, const fs::path &p) : CoreError(fault, p.string()) { };
    explicit CoreError(long fault, std::integral auto v) : CoreError(fault, std::to_string(v)) { };
    explicit CoreError(long fault) : CoreError(fault, "") { }
};

}
