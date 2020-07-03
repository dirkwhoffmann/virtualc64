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

typedef enum {
    
    UNKNOWN_FILE_FORMAT = 0,
    CRT_FILE,                     // Expansion port cartridge
    V64_FILE,                     // Snapshot
    D64_FILE,                     // Floppy disk
    T64_FILE,                     // Archive with multiple files
    PRG_FILE,                     // Archive with a single file
    P00_FILE,                     // Archive with a single file
    G64_FILE,                     // Bit-stream of a floppy disk
    TAP_FILE,                     // Datasette tape
    BASIC_ROM_FILE,               // Basic Rom
    CHAR_ROM_FILE,                // Character Rom
    KERNAL_ROM_FILE,              // Kernal Rom
    VC1541_ROM_FILE,              // Floppy drive Rom
}
C64FileType;

#endif
