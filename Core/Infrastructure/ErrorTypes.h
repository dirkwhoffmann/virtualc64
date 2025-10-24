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

enum class Fault : long
{
    OK,                   ///< No error
    UNKNOWN,              ///< Unclassified error condition

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

    // File IO
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

    // Roms
    ROM_BASIC_MISSING,    ///< No Basic Rom installed
    ROM_CHAR_MISSING,     ///< No Character Rom installed
    ROM_KERNAL_MISSING,   ///< No Kernel Rom installed
    ROM_DRIVE_MISSING,    ///< No Floppy Drive Rom installed
    ROM_MEGA65_MISMATCH,  ///< MEGA65 Roms have different version numbers

    // Snapshots
    SNAP_TOO_OLD,         ///< Snapshot was created with an older version
    SNAP_TOO_NEW,         ///< Snapshot was created with a later version
    SNAP_IS_BETA,         ///< Snapshot was created with a beta release
    SNAP_CORRUPTED,       ///< Snapshot data is corrupted

    // Drives
    DRV_UNCONNECTED,      ///< Floppy drive is not connected
    DRV_NO_DISK,          ///< Floppy drive contains no disk

    // Cartridges
    CRT_NO_CARTRIDGE,     ///< No cartridge attached
    CRT_UNKNOWN,          ///< Unknown cartridge type
    CRT_UNSUPPORTED,      ///< Unsupported cartridge type
    CRT_TOO_MANY_PACKETS, ///< CRT file contains too many Rom packets
    CRT_CORRUPTED_PACKET, ///< CRT file contains a corrupted Rom package

    // Remote servers
    SOCK_CANT_CREATE,
    SOCK_CANT_CONNECT,
    SOCK_CANT_BIND,
    SOCK_CANT_LISTEN,
    SOCK_CANT_ACCEPT,
    SOCK_CANT_RECEIVE,
    SOCK_CANT_SEND,
    SOCK_DISCONNECTED,
    SERVER_PORT_IN_USE,
    SERVER_ON,
    SERVER_OFF,
    SERVER_RUNNING,
    SERVER_NOT_RUNNING,
    SERVER_NO_CLIENT,

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

struct FaultEnum : Reflection<FaultEnum, Fault> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(Fault::FS_EXPECTED_MAX);

    static const char *_key(Fault value)
    {
        switch (value) {

            case Fault::OK:                      return "OK";
            case Fault::UNKNOWN:                 return "UNKNOWN";

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

            case Fault::DIR_NOT_FOUND:           return "DIR_NOT_FOUND";
            case Fault::DIR_ACCESS_DENIED:       return "DIR_ACCESS_DENIED";
            case Fault::DIR_CANT_CREATE:         return "DIR_CANT_CREATE";
            case Fault::DIR_NOT_EMPTY:           return "DIR_NOT_EMPTY";
            case Fault::FILE_NOT_FOUND:          return "FILE_NOT_FOUND";
            case Fault::FILE_EXISTS:             return "FILE_EXISTS";
            case Fault::FILE_IS_DIRECTORY:       return "FILE_IS_DIRECtORY";
            case Fault::FILE_ACCESS_DENIED:      return "FILE_ACCESS_DENIED";
            case Fault::FILE_TYPE_MISMATCH:      return "FILE_TYPE_MISMATCH";
            case Fault::FILE_TYPE_UNSUPPORTED:   return "FILE_TYPE_UNSUPPORTED";
            case Fault::FILE_CANT_READ:          return "FILE_CANT_READ";
            case Fault::FILE_CANT_WRITE:         return "FILE_CANT_WRITE";
            case Fault::FILE_CANT_CREATE:        return "FILE_CANT_CREATE";

            case Fault::ROM_BASIC_MISSING:       return "ROM_BASIC_MISSING";
            case Fault::ROM_CHAR_MISSING:        return "ROM_CHAR_MISSING";
            case Fault::ROM_KERNAL_MISSING:      return "ROM_KERNAL_MISSING";
            case Fault::ROM_DRIVE_MISSING:       return "ROM_DRIVE_MISSING";
            case Fault::ROM_MEGA65_MISMATCH:     return "ROM_MEGA65_MISMATCH";

            case Fault::SNAP_TOO_OLD:            return "SNAP_TOO_OLD";
            case Fault::SNAP_TOO_NEW:            return "SNAP_TOO_NEW";
            case Fault::SNAP_IS_BETA:		     return "SNAP_IS_BETA";
            case Fault::SNAP_CORRUPTED:		     return "SNAP_CORRUPTED";

            case Fault::DRV_UNCONNECTED:         return "DRV_UNCONNECTED";
            case Fault::DRV_NO_DISK:             return "DRV_NO_DISK";

            case Fault::CRT_NO_CARTRIDGE:        return "CRT_NO_CARTRIDGE";
            case Fault::CRT_UNKNOWN:             return "CRT_UNKNOWN";
            case Fault::CRT_UNSUPPORTED:         return "CRT_UNSUPPORTED";
            case Fault::CRT_TOO_MANY_PACKETS:    return "CRT_TOO_MANY_PACKETS";
            case Fault::CRT_CORRUPTED_PACKET:    return "CRT_CORRUPTED_PACKET";

            case Fault::SOCK_CANT_CREATE:        return "SOCK_CANT_CREATE";
            case Fault::SOCK_CANT_CONNECT:       return "SOCK_CANT_CONNECT";
            case Fault::SOCK_CANT_BIND:          return "SOCK_CANT_BIND";
            case Fault::SOCK_CANT_LISTEN:        return "SOCK_CANT_LISTEN";
            case Fault::SOCK_CANT_ACCEPT:        return "SOCK_CANT_ACCEPT";
            case Fault::SOCK_CANT_RECEIVE:       return "SOCK_CANT_RECEIVE";
            case Fault::SOCK_CANT_SEND:          return "SOCK_CANT_SEND";
            case Fault::SOCK_DISCONNECTED:       return "SOCK_DISCONNECTED";
            case Fault::SERVER_PORT_IN_USE:      return "SERVER_PORT_IN_USE";
            case Fault::SERVER_ON:               return "SERVER_ON";
            case Fault::SERVER_OFF:              return "SERVER_OFF";
            case Fault::SERVER_RUNNING:          return "SERVER_RUNNING";
            case Fault::SERVER_NOT_RUNNING:      return "SERVER_NOT_RUNNING";
            case Fault::SERVER_NO_CLIENT:        return "SERVER_NO_CLIENT";

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
