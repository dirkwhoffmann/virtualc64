/*!
 * @header      File_types.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   Dirk W. Hoffmann. All rights reserved.
 */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


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
