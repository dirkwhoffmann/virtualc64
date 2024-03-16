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

#include "FSTypes.h"

struct DOSTypeEnum : util::Reflection<DOSTypeEnum, DOSType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DOS_TYPE_CBM;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DOS_TYPE"; }
    static const char *key(DOSType value)
    {
        switch (value) {

            case DOS_TYPE_NODOS:  return "NODOS";
            case DOS_TYPE_CBM:    return "CBM";
        }
        return "???";
    }
};

struct FSFileTypeEnum : util::Reflection<FSFileTypeEnum, FSFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FS_FILETYPE_REL;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "FS_FILETYPE"; }
    static const char *key(FSFileType value)
    {
        switch (value) {

            case FS_FILETYPE_DEL:    return "DEL";
            case FS_FILETYPE_SEQ:    return "SEQ";
            case FS_FILETYPE_PRG:    return "PRG";
            case FS_FILETYPE_USR:    return "USR";
            case FS_FILETYPE_REL:    return "REL";
        }
        return "???";
    }
};

struct FSBlockTypeEnum : util::Reflection<FSBlockTypeEnum, FSBlockType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FS_BLOCKTYPE_DATA;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "FS_BLOCKTYPE"; }
    static const char *key(FSBlockType value)
    {
        switch (value) {

            case FS_BLOCKTYPE_UNKNOWN:  return "UNKNOWN";
            case FS_BLOCKTYPE_EMPTY:    return "EMPTY";
            case FS_BLOCKTYPE_BAM:      return "BAM";
            case FS_BLOCKTYPE_DIR:      return "DIR";
            case FS_BLOCKTYPE_DATA:     return "DATA";
        }
        return "???";
    }
};

struct FSUsageEnum : util::Reflection<FSUsageEnum, FSUsage> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = FS_USAGE_DATA;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "FS_USAGE"; }
    static const char *key(FSUsage value) { return "???"; }
};
