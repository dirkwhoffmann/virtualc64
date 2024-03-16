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
#include "Reflection.hpp"

#include "FileTypes.h"

struct FileTypeEnum : util::Reflection<FileTypeEnum, FileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FILETYPE_VC1541_ROM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "FILETYPE"; }
    static const char *key(FileType value)
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

struct TAPVersionEnum : util::Reflection<TAPVersionEnum, TAPVersion> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = TAP_VERSION_ADVANCED;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "TAP_VERSION"; }
    static const char *key(TAPVersion value)
    {
        switch (value) {

            case TAP_VERSION_ORIGINAL:  return "ORIGINAL";
            case TAP_VERSION_ADVANCED:  return "ADVANCED";
            default:                    return "???";
        }
    }
};
