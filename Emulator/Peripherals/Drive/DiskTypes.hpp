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

#pragma once

#include "Aliases.h"
#include "Reflection.hpp"
#include "Serializable.hpp"

#include "DiskTypes.h"

struct DiskTypeEnum : util::Reflection<DiskTypeEnum, DiskType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DISK_TYPE_DS_SD;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DISK_TYPE"; }
    static const char *key(DiskType value)
    {
        switch (value) {

            case DISK_TYPE_SS_SD:  return "SS_SD";
            case DISK_TYPE_DS_SD:  return "DS_SD";
        }
        return "???";
    }
};

struct CBMFileTypeEnum : util::Reflection<CBMFileTypeEnum, CBMFileType> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = CBM_FILE_REL;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "CBM"; }
    static const char *key(CBMFileType value)
    {
        switch (value) {

            case CBM_FILE_PRG:    return "PRG";
            case CBM_FILE_SEQ:    return "SEQ";
            case CBM_FILE_USR:    return "USR";
            case CBM_FILE_REL:    return "REL";
        }
        return "???";
    }
};

struct DiskErrorCodeEnum : util::Reflection<DiskErrorCodeEnum, DiskErrorCode> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DRIVE_NOT_READY_ERRROR;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return ""; }
    static const char *key(DiskErrorCode value)
    {
        switch (value) {

            case DISK_OK:                            return "DISK_OK";
            case HEADER_BLOCK_NOT_FOUND_ERROR:       return "HEADER_BLOCK_NOT_FOUND_ERROR";
            case NO_SYNC_SEQUENCE_ERROR:             return "NO_SYNC_SEQUENCE_ERROR";
            case DATA_BLOCK_NOT_FOUND_ERROR:         return "DATA_BLOCK_NOT_FOUND_ERROR";
            case DATA_BLOCK_CHECKSUM_ERROR:          return "DATA_BLOCK_CHECKSUM_ERROR";
            case WRITE_VERIFY_ERROR_ON_FORMAT_ERROR: return "WRITE_VERIFY_ERROR_ON_FORMAT_ERROR";
            case WRITE_VERIFY_ERROR:                 return "WRITE_VERIFY_ERROR";
            case WRITE_PROTECT_ON_ERROR:             return "WRITE_PROTECT_ON_ERROR";
            case HEADER_BLOCK_CHECKSUM_ERROR:        return "HEADER_BLOCK_CHECKSUM_ERROR";
            case WRITE_ERROR:                        return "WRITE_ERROR";
            case DISK_ID_MISMATCH_ERROR:             return "DISK_ID_MISMATCH_ERROR";
            case DRIVE_NOT_READY_ERRROR:             return "DRIVE_NOT_READY_ERRROR";
        }
        return "???";
    }
};

namespace vc64 {

struct DiskData : public Serializable
{
    union {

        struct
        {
            u8 _pad[maxBytesOnTrack];
            u8 halftrack[85][maxBytesOnTrack];
        };

        u8 track[43][2 * maxBytesOnTrack];
    };

    template <class W>
    void serialize(W& worker)
    {
        worker

        << track;

    } SERIALIZERS(serialize);

};

/* Length of each halftrack in bits
 *
 *     - length.halftack[i] is the length of halftrack i
 *     - length.track[i][0] is the length of track i
 *     - length.track[i][1] is the length of halftrack above track i
 */

struct DiskLength : public Serializable
{
    union {

        struct
        {
            isize _pad;
            isize halftrack[85];
        };

        isize track[43][2];
    };

    template <class W>
    void serialize(W& worker)
    {
        worker

        << track;

    } SERIALIZERS(serialize);
};

}
