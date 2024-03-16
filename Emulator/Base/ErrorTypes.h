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

#include "Types.h"

/// Error condition
enum_long(ERROR_CODE)
{
    ERROR_OK,                   ///< No error
    ERROR_UNKNOWN,              ///< Unclassified error condition

    // Emulator state
    ERROR_POWERED_OFF,          ///< The emulator is powered off
    ERROR_POWERED_ON,           ///< The emulator is powered on
    ERROR_RUNNING,              ///< The emulator is running

    // Configuration
    ERROR_OPT_UNSUPPORTED,      ///< Unsupported configuration option
    ERROR_OPT_INVARG,           ///< Invalid argument
    ERROR_OPT_LOCKED,           ///< The option is temporarily locked

    // Property storage
    ERROR_INVALID_KEY,          ///< Invalid property key
    ERROR_SYNTAX,               ///< Syntax error

    // CPU
    ERROR_BP_NOT_FOUND,         ///< Breakpoint is not set
    ERROR_BP_ALREADY_SET,       ///< Breakpoint is already set
    ERROR_WP_NOT_FOUND,         ///< Watchpoint is not set
    ERROR_WP_ALREADY_SET,       ///< Watchpoint is already set

    // Memory
    ERROR_OUT_OF_MEMORY,        ///< Out of memory

    // File IO
    ERROR_DIR_NOT_FOUND,        ///< Directory does not exist
    ERROR_DIR_ACCESS_DENIED,    ///< File access denied
    ERROR_DIR_CANT_CREATE,      ///< Unable to create a directory
    ERROR_DIR_NOT_EMPTY,        ///< Directory is not empty
    ERROR_FILE_NOT_FOUND,       ///< File not found error
    ERROR_FILE_EXISTS,          ///< File already exists
    ERROR_FILE_IS_DIRECTORY,    ///< The file is a directory
    ERROR_FILE_ACCESS_DENIED,   ///< File access denied
    ERROR_FILE_TYPE_MISMATCH,   ///< File type mismatch
    ERROR_FILE_TYPE_UNSUPPORTED,///< Unsupported file type
    ERROR_FILE_CANT_READ,       ///< Can't read from file
    ERROR_FILE_CANT_WRITE,      ///< Can't write to file
    ERROR_FILE_CANT_CREATE,     ///< Can't create file

    // Roms
    ERROR_ROM_BASIC_MISSING,    ///< No Basic Rom installed
    ERROR_ROM_CHAR_MISSING,     ///< No Character Rom installed
    ERROR_ROM_KERNAL_MISSING,   ///< No Kernel Rom installed
    ERROR_ROM_DRIVE_MISSING,    ///< No Floppy Drive Rom installed
    ERROR_ROM_MEGA65_MISMATCH,  ///< MEGA65 Roms have different version numbers

    // Recorder
    ERROR_REC_LAUNCH,           ///< Can't launch the screen recorder

	// Snapshots
	ERROR_SNAP_TOO_OLD,         ///< Snapshot was created with an older version
	ERROR_SNAP_TOO_NEW,         ///< Snapshot was created with a later version
	ERROR_SNAP_IS_BETA,         ///< Snapshot was created with a beta release
	ERROR_SNAP_CORRUPTED,       ///< Snapshot data is corrupted

    // Drives
    ERROR_DRV_UNCONNECTED,      ///< Floppy drive is not connected

    // Cartridges
    ERROR_CRT_UNKNOWN,          ///< Unknown cartridge type
    ERROR_CRT_UNSUPPORTED,      ///< Unsupported cartridge type
    ERROR_CRT_TOO_MANY_PACKETS, ///< CRT file contains too many Rom packets
    ERROR_CRT_CORRUPTED_PACKET, ///< CRT file contains a corrupted Rom package

    // File systems
    ERROR_FS_UNSUPPORTED,       ///< Unsupported file system
    ERROR_FS_WRONG_CAPACITY,    ///< Wrong file system capacity
    ERROR_FS_CORRUPTED,         ///< File system is corrupted
    ERROR_FS_HAS_NO_FILES,      ///< File system is empty
    ERROR_FS_HAS_CYCLES,        ///< File system has cyclic links
    ERROR_FS_CANT_IMPORT,       ///< Failed to import a file system
    ERROR_FS_EXPECTED_VAL,      ///< Unexpected value found
    ERROR_FS_EXPECTED_MIN,      ///< Value is too small
    ERROR_FS_EXPECTED_MAX,      ///< Value is too big

    ERROR_COUNT
};
typedef ERROR_CODE ErrorCode;
