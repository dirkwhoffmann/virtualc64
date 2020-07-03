// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef FILE_TYPES_H
#define FILE_TYPES_H

/*! @enum     C64FileType
 *  @brief    The type of a file
 *  @constant CRT_FILE A cartridge that can be plugged into the expansion port.
 *  @constant V64_FILE A snapshot file (contains a frozen C64 state).
 *  @constant D64_FILE A floppy disk image with multiply files.
 *  @constant T64_FILE A tape archive with multiple files.
 *  @constant PRG_FILE A program archive containing a single file.
 *  @constant P00_FILE A program archive containing a single file.
 *  @constant G64_FILE A collection of bit-streams resembling a floppy disk.
 *  @constant TAP_FILE A bit-stream resembling a datasette tape.
 */
typedef enum {
    UNKNOWN_FILE_FORMAT = 0,
    CRT_FILE,
    V64_FILE,
    D64_FILE,
    T64_FILE,
    PRG_FILE,
    P00_FILE,
    G64_FILE,
    TAP_FILE,
    BASIC_ROM_FILE,
    CHAR_ROM_FILE,
    KERNAL_ROM_FILE,
    VC1541_ROM_FILE,
} C64FileType;

#endif
