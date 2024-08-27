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

namespace vc64 {

/// Error condition
enum_long(VC64ERROR_CODE)
{
    VC64ERROR_OK,                   ///< No error
    VC64ERROR_UNKNOWN,              ///< Unclassified error condition

    // Execution state
    VC64ERROR_LAUNCH,               ///< Launch error
    VC64ERROR_POWERED_OFF,          ///< The emulator is powered off
    VC64ERROR_POWERED_ON,           ///< The emulator is powered on
    VC64ERROR_RUNNING,              ///< The emulator is running

    // Configuration
    VC64ERROR_OPT_UNSUPPORTED,      ///< Unsupported configuration option
    VC64ERROR_OPT_INV_ARG,          ///< Invalid argument
    VC64ERROR_OPT_INV_ID,           ///< Invalid component id
    VC64ERROR_OPT_LOCKED,           ///< The option is temporarily locked

    // Property storage
    VC64ERROR_INVALID_KEY,          ///< Invalid property key
    VC64ERROR_SYNTAX,               ///< Syntax error

    // CPU
    VC64ERROR_BP_NOT_FOUND,         ///< Breakpoint is not set
    VC64ERROR_BP_ALREADY_SET,       ///< Breakpoint is already set
    VC64ERROR_WP_NOT_FOUND,         ///< Watchpoint is not set
    VC64ERROR_WP_ALREADY_SET,       ///< Watchpoint is already set

    // Memory
    VC64ERROR_OUT_OF_MEMORY,        ///< Out of memory

    // File IO
    VC64ERROR_DIR_NOT_FOUND,        ///< Directory does not exist
    VC64ERROR_DIR_ACCESS_DENIED,    ///< File access denied
    VC64ERROR_DIR_CANT_CREATE,      ///< Unable to create a directory
    VC64ERROR_DIR_NOT_EMPTY,        ///< Directory is not empty
    VC64ERROR_FILE_NOT_FOUND,       ///< File not found error
    VC64ERROR_FILE_EXISTS,          ///< File already exists
    VC64ERROR_FILE_IS_DIRECTORY,    ///< The file is a directory
    VC64ERROR_FILE_ACCESS_DENIED,   ///< File access denied
    VC64ERROR_FILE_TYPE_MISMATCH,   ///< File type mismatch
    VC64ERROR_FILE_TYPE_UNSUPPORTED,///< Unsupported file type
    VC64ERROR_FILE_CANT_READ,       ///< Can't read from file
    VC64ERROR_FILE_CANT_WRITE,      ///< Can't write to file
    VC64ERROR_FILE_CANT_CREATE,     ///< Can't create file

    // Roms
    VC64ERROR_ROM_BASIC_MISSING,    ///< No Basic Rom installed
    VC64ERROR_ROM_CHAR_MISSING,     ///< No Character Rom installed
    VC64ERROR_ROM_KERNAL_MISSING,   ///< No Kernel Rom installed
    VC64ERROR_ROM_DRIVE_MISSING,    ///< No Floppy Drive Rom installed
    VC64ERROR_ROM_MEGA65_MISMATCH,  ///< MEGA65 Roms have different version numbers

    // Recorder
    VC64ERROR_REC_LAUNCH,           ///< Can't launch the screen recorder

    // Snapshots
    VC64ERROR_SNAP_TOO_OLD,         ///< Snapshot was created with an older version
    VC64ERROR_SNAP_TOO_NEW,         ///< Snapshot was created with a later version
    VC64ERROR_SNAP_IS_BETA,         ///< Snapshot was created with a beta release
    VC64ERROR_SNAP_CORRUPTED,       ///< Snapshot data is corrupted

    // Drives
    VC64ERROR_DRV_UNCONNECTED,      ///< Floppy drive is not connected
    VC64ERROR_DRV_NO_DISK,          ///< Floppy drive contains no disk

    // Cartridges
    VC64ERROR_CRT_UNKNOWN,          ///< Unknown cartridge type
    VC64ERROR_CRT_UNSUPPORTED,      ///< Unsupported cartridge type
    VC64ERROR_CRT_TOO_MANY_PACKETS, ///< CRT file contains too many Rom packets
    VC64ERROR_CRT_CORRUPTED_PACKET, ///< CRT file contains a corrupted Rom package

    // Remote servers
    VC64ERROR_SOCK_CANT_CREATE,
    VC64ERROR_SOCK_CANT_CONNECT,
    VC64ERROR_SOCK_CANT_BIND,
    VC64ERROR_SOCK_CANT_LISTEN,
    VC64ERROR_SOCK_CANT_ACCEPT,
    VC64ERROR_SOCK_CANT_RECEIVE,
    VC64ERROR_SOCK_CANT_SEND,
    VC64ERROR_SOCK_DISCONNECTED,
    VC64ERROR_SERVER_PORT_IN_USE,
    VC64ERROR_SERVER_ON,
    VC64ERROR_SERVER_OFF,
    VC64ERROR_SERVER_RUNNING,
    VC64ERROR_SERVER_NOT_RUNNING,
    VC64ERROR_SERVER_NO_CLIENT,

    // File systems
    VC64ERROR_FS_UNSUPPORTED,       ///< Unsupported file system
    VC64ERROR_FS_WRONG_CAPACITY,    ///< Wrong file system capacity
    VC64ERROR_FS_CORRUPTED,         ///< File system is corrupted
    VC64ERROR_FS_HAS_NO_FILES,      ///< File system is empty
    VC64ERROR_FS_HAS_CYCLES,        ///< File system has cyclic links
    VC64ERROR_FS_CANT_IMPORT,       ///< Failed to import a file system
    VC64ERROR_FS_EXPECTED_VAL,      ///< Unexpected value found
    VC64ERROR_FS_EXPECTED_MIN,      ///< Value is too small
    VC64ERROR_FS_EXPECTED_MAX,      ///< Value is too big

    VC64ERROR_COUNT
};
typedef VC64ERROR_CODE ErrorCode;

struct ErrorCodeEnum : util::Reflection<ErrorCodeEnum, ErrorCode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = VC64ERROR_FS_EXPECTED_MAX;

    static const char *prefix() { return "ERROR"; }
    static const char *_key(long value)
    {
        switch (value) {

            case VC64ERROR_OK:                      return "OK";
            case VC64ERROR_UNKNOWN:                 return "UNKNOWN";

            case VC64ERROR_LAUNCH:                  return "LAUNCH";
            case VC64ERROR_POWERED_OFF:             return "POWERED_OFF";
            case VC64ERROR_POWERED_ON:              return "POWERED_ON";
            case VC64ERROR_RUNNING:                 return "RUNNING";

            case VC64ERROR_OPT_UNSUPPORTED:         return "OPT_UNSUPPORTED";
            case VC64ERROR_OPT_INV_ARG:             return "OPT_INV_ARG";
            case VC64ERROR_OPT_INV_ID:              return "OPT_INV_ID";
            case VC64ERROR_OPT_LOCKED:              return "OPT_LOCKED";

            case VC64ERROR_INVALID_KEY:             return "INVALID_KEY";
            case VC64ERROR_SYNTAX:                  return "SYNTAX";

            case VC64ERROR_BP_NOT_FOUND:            return "BP_NOT_FOUND";
            case VC64ERROR_BP_ALREADY_SET:          return "BP_ALREADY_SET";
            case VC64ERROR_WP_NOT_FOUND:            return "WP_NOT_FOUND";
            case VC64ERROR_WP_ALREADY_SET:          return "WP_ALREADY_SET";

            case VC64ERROR_OUT_OF_MEMORY:           return "OUT_OF_MEMORY";

            case VC64ERROR_DIR_NOT_FOUND:           return "DIR_NOT_FOUND";
            case VC64ERROR_DIR_ACCESS_DENIED:       return "DIR_ACCESS_DENIED";
            case VC64ERROR_DIR_CANT_CREATE:         return "DIR_CANT_CREATE";
            case VC64ERROR_DIR_NOT_EMPTY:           return "DIR_NOT_EMPTY";
            case VC64ERROR_FILE_NOT_FOUND:          return "FILE_NOT_FOUND";
            case VC64ERROR_FILE_EXISTS:             return "FILE_EXISTS";
            case VC64ERROR_FILE_IS_DIRECTORY:       return "FILE_IS_DIRECtORY";
            case VC64ERROR_FILE_ACCESS_DENIED:      return "FILE_ACCESS_DENIED";
            case VC64ERROR_FILE_TYPE_MISMATCH:      return "FILE_TYPE_MISMATCH";
            case VC64ERROR_FILE_TYPE_UNSUPPORTED:   return "FILE_TYPE_UNSUPPORTED";
            case VC64ERROR_FILE_CANT_READ:          return "FILE_CANT_READ";
            case VC64ERROR_FILE_CANT_WRITE:         return "FILE_CANT_WRITE";
            case VC64ERROR_FILE_CANT_CREATE:        return "FILE_CANT_CREATE";

            case VC64ERROR_ROM_BASIC_MISSING:       return "ROM_BASIC_MISSING";
            case VC64ERROR_ROM_CHAR_MISSING:        return "ROM_CHAR_MISSING";
            case VC64ERROR_ROM_KERNAL_MISSING:      return "ROM_KERNAL_MISSING";
            case VC64ERROR_ROM_DRIVE_MISSING:       return "ROM_DRIVE_MISSING";
            case VC64ERROR_ROM_MEGA65_MISMATCH:     return "ROM_MEGA65_MISMATCH";

            case VC64ERROR_REC_LAUNCH:              return "REC_LAUNCH";

            case VC64ERROR_SNAP_TOO_OLD:            return "SNAP_TOO_OLD";
            case VC64ERROR_SNAP_TOO_NEW:            return "SNAP_TOO_NEW";
            case VC64ERROR_SNAP_IS_BETA:		    return "SNAP_IS_BETA";
            case VC64ERROR_SNAP_CORRUPTED:		    return "SNAP_CORRUPTED";

            case VC64ERROR_DRV_UNCONNECTED:         return "DRV_UNCONNECTED";
            case VC64ERROR_DRV_NO_DISK:             return "DRV_NO_DISK";

            case VC64ERROR_CRT_UNKNOWN:             return "VC64ERROR_CRT_UNKNOWN";
            case VC64ERROR_CRT_UNSUPPORTED:         return "CRT_UNSUPPORTED";
            case VC64ERROR_CRT_TOO_MANY_PACKETS:    return "CRT_TOO_MANY_PACKETS";
            case VC64ERROR_CRT_CORRUPTED_PACKET:    return "CRT_CORRUPTED_PACKET";

            case VC64ERROR_SOCK_CANT_CREATE:        return "SOCK_CANT_CREATE";
            case VC64ERROR_SOCK_CANT_CONNECT:       return "SOCK_CANT_CONNECT";
            case VC64ERROR_SOCK_CANT_BIND:          return "SOCK_CANT_BIND";
            case VC64ERROR_SOCK_CANT_LISTEN:        return "SOCK_CANT_LISTEN";
            case VC64ERROR_SOCK_CANT_ACCEPT:        return "SOCK_CANT_ACCEPT";
            case VC64ERROR_SOCK_CANT_RECEIVE:       return "SOCK_CANT_RECEIVE";
            case VC64ERROR_SOCK_CANT_SEND:          return "SOCK_CANT_SEND";
            case VC64ERROR_SOCK_DISCONNECTED:       return "SOCK_DISCONNECTED";
            case VC64ERROR_SERVER_PORT_IN_USE:      return "SERVER_PORT_IN_USE";
            case VC64ERROR_SERVER_ON:               return "SERVER_ON";
            case VC64ERROR_SERVER_OFF:              return "SERVER_OFF";
            case VC64ERROR_SERVER_RUNNING:          return "SERVER_RUNNING";
            case VC64ERROR_SERVER_NOT_RUNNING:      return "SERVER_NOT_RUNNING";
            case VC64ERROR_SERVER_NO_CLIENT:        return "SERVER_NO_CLIENT";

            case VC64ERROR_FS_UNSUPPORTED:          return "FS_UNSUPPORTED";
            case VC64ERROR_FS_WRONG_CAPACITY:       return "FS_WRONG_CAPACITY";
            case VC64ERROR_FS_CORRUPTED:            return "FS_CORRUPTED";
            case VC64ERROR_FS_HAS_NO_FILES:         return "VC64ERROR_FS_HAS_NO_FILES";
            case VC64ERROR_FS_HAS_CYCLES:           return "FS_HAS_CYCLES";
            case VC64ERROR_FS_CANT_IMPORT:          return "FS_CANT_IMPORT";
            case VC64ERROR_FS_EXPECTED_VAL:         return "FS_EXPECTED_VAL";
            case VC64ERROR_FS_EXPECTED_MIN:         return "FS_EXPECTED_MIN";
            case VC64ERROR_FS_EXPECTED_MAX:         return "FS_EXPECTED_MAX";

            case VC64ERROR_COUNT:                   return "???";
        }
        return "???";
    }
};

}
