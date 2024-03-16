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
#include "Reflection.h"

//
// Enumerations
//

enum_long(FILETYPE)
{
    FILETYPE_UNKNOWN = 0,
    FILETYPE_SNAPSHOT,
    FILETYPE_SCRIPT,
    FILETYPE_CRT,
    FILETYPE_T64,
    FILETYPE_P00,
    FILETYPE_PRG,
    FILETYPE_FOLDER,
    FILETYPE_D64,
    FILETYPE_G64,
    FILETYPE_TAP,
    FILETYPE_BASIC_ROM,
    FILETYPE_CHAR_ROM,
    FILETYPE_KERNAL_ROM,
    FILETYPE_VC1541_ROM
};
typedef FILETYPE FileType;

enum_long(TAP_VERSION)
{
    TAP_VERSION_ORIGINAL,
    TAP_VERSION_ADVANCED
};
typedef TAP_VERSION TAPVersion;

enum_long(FILE_ERROR)
{
    ERR_FILE_OK,
    
    // General
    ERR_FILE_NOT_FOUND,
    ERR_INVALID_TYPE,
    ERR_CANT_READ,
    ERR_CANT_WRITE,
    ERR_OUT_OF_MEMORY,
    ERR_UNKNOWN,
    
    // Snapshots
    ERR_UNSUPPORTED_SNAPSHOT
};
typedef FILE_ERROR FileError;
