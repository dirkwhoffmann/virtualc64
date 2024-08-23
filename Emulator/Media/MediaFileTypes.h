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

//
// Enumerations
//

enum_long(FILETYPE)
{
    FILETYPE_UNKNOWN,
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

struct FileTypeEnum : util::Reflection<FileTypeEnum, FileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FILETYPE_VC1541_ROM;

    static const char *prefix() { return "FILETYPE"; }
    static const char *_key(long value)
    {
        switch (value) {

            case FILETYPE_UNKNOWN:    return "UNKNOWN";
            case FILETYPE_SNAPSHOT:   return "SNAPSHOT";
            case FILETYPE_SCRIPT:     return "SCRIPT";
            case FILETYPE_CRT:        return "CRT";
            case FILETYPE_T64:        return "T64";
            case FILETYPE_PRG:        return "PRG";
            case FILETYPE_FOLDER:     return "FOLDER";
            case FILETYPE_P00:        return "P00";
            case FILETYPE_D64:        return "D64";
            case FILETYPE_G64:        return "G64";
            case FILETYPE_TAP:        return "TAP";
            case FILETYPE_BASIC_ROM:  return "BASIC_ROM";
            case FILETYPE_CHAR_ROM:   return "CHAR_ROM";
            case FILETYPE_KERNAL_ROM: return "KERNAL_ROM";
            case FILETYPE_VC1541_ROM: return "VC1541_ROM";
        }
        return "???";
    }
};

enum_long(TAP_VERSION)
{
    TAP_VERSION_ORIGINAL,
    TAP_VERSION_ADVANCED
};
typedef TAP_VERSION TAPVersion;

struct TAPVersionEnum : util::Reflection<TAPVersionEnum, TAPVersion> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = TAP_VERSION_ADVANCED;

    static const char *prefix() { return "TAP_VERSION"; }
    static const char *_key(long value)
    {
        switch (value) {

            case TAP_VERSION_ORIGINAL:  return "ORIGINAL";
            case TAP_VERSION_ADVANCED:  return "ADVANCED";
            default:                    return "???";
        }
    }
};

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

}
