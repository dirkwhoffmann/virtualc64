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

enum class FileType : long
{
    UNKNOWN,
    SNAPSHOT,
    SCRIPT,
    CRT,
    T64,
    P00,
    PRG,
    FOLDER,
    D64,
    G64,
    TAP,
    BASIC_ROM,
    CHAR_ROM,
    KERNAL_ROM,
    VC1541_ROM
};

struct FileTypeEnum : util::Reflection<FileTypeEnum, FileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(FileType::VC1541_ROM);

    static const char *prefix() { return "FILETYPE"; }
    static const char *_key(FileType value)
    {
        switch (value) {

            case FileType::UNKNOWN:    return "UNKNOWN";
            case FileType::SNAPSHOT:   return "SNAPSHOT";
            case FileType::SCRIPT:     return "SCRIPT";
            case FileType::CRT:        return "CRT";
            case FileType::T64:        return "T64";
            case FileType::PRG:        return "PRG";
            case FileType::FOLDER:     return "FOLDER";
            case FileType::P00:        return "P00";
            case FileType::D64:        return "D64";
            case FileType::G64:        return "G64";
            case FileType::TAP:        return "TAP";
            case FileType::BASIC_ROM:  return "BASIC_ROM";
            case FileType::CHAR_ROM:   return "CHAR_ROM";
            case FileType::KERNAL_ROM: return "KERNAL_ROM";
            case FileType::VC1541_ROM: return "VC1541_ROM";
        }
        return "???";
    }
    
    static const char *help(FileType value)
    {
        return "";
    }
};

enum class TAPVersion : long
{
    ORIGINAL,
    ADVANCED
};

struct TAPVersionEnum : util::Reflection<TAPVersionEnum, TAPVersion> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = long(TAPVersion::ADVANCED);

    static const char *_key(TAPVersion value)
    {
        switch (value) {

            case TAPVersion::ORIGINAL:  return "ORIGINAL";
            case TAPVersion::ADVANCED:  return "ADVANCED";
            default:                    return "???";
        }
    }
    
    static const char *help(TAPVersion value)
    {
        return "";
    }
};

enum class FileError : long
{
    FILE_OK,

    // General
    FILE_NOT_FOUND,
    INVALID_TYPE,
    CANT_READ,
    CANT_WRITE,
    OUT_OF_MEMORY,
    UNKNOWN,

    // Snapshots
    UNSUPPORTED_SNAPSHOT
};

}
