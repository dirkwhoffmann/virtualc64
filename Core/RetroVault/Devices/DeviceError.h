// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"

namespace retro::vault {

struct DeviceError : public Error {

    static constexpr long OK                            = 0;
    static constexpr long CUSTOM                        = 1;

    // All devices
    static constexpr long INVALID_BLOCK_NR              = 10;
    static constexpr long INVALID_TRACK_NR              = 11;
    static constexpr long INVALID_SECTOR_NR             = 12;
    static constexpr long INVALID_CYLINDER_NR           = 13;
    static constexpr long INVALID_HEAD_NR               = 14;
    static constexpr long SEEK_ERR                      = 15;
    static constexpr long READ_ERR                      = 16;
    static constexpr long WRITE_ERR                     = 17;

    // Floppy disks
    static constexpr long DSK_MISSING                   = 20;
    static constexpr long DSK_INCOMPATIBLE              = 21;
    static constexpr long DSK_INVALID_DIAMETER          = 22;
    static constexpr long DSK_INVALID_DENSITY           = 23;
    static constexpr long DSK_INVALID_LAYOUT            = 24;
    static constexpr long DSK_WRONG_SECTOR_CNT          = 25;
    static constexpr long DSK_INVALID_TRACK_SIZE        = 26;

    // Hard disks
    static constexpr long HDR_TOO_LARGE                 = 30;
    static constexpr long HDR_UNSUPPORTED_CYL_CNT       = 31;
    static constexpr long HDR_UNSUPPORTED_HEAD_CNT      = 32;
    static constexpr long HDR_UNSUPPORTED_SEC_CNT       = 33;
    static constexpr long HDR_UNSUPPORTED_BSIZE         = 34;
    static constexpr long HDR_UNKNOWN_GEOMETRY          = 35;
    static constexpr long HDR_UNMATCHED_GEOMETRY        = 36;
    static constexpr long HDR_UNPARTITIONED             = 37;
    static constexpr long HDR_CORRUPTED_PTABLE          = 38;
    static constexpr long HDR_CORRUPTED_FSH             = 39;
    static constexpr long HDR_CORRUPTED_LSEG            = 40;
    static constexpr long HDR_UNSUPPORTED               = 41;

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                          return "OK";
            case CUSTOM:                      return "CUSTOM";

            case INVALID_BLOCK_NR:            return "INVALID_BLOCK_NR";
            case INVALID_TRACK_NR:            return "INVALID_TRACK_NR";
            case INVALID_SECTOR_NR:           return "INVALID_SECTOR_NR";
            case INVALID_CYLINDER_NR:         return "INVALID_CYLINDER_NR";
            case INVALID_HEAD_NR:             return "INVALID_HEAD_NR";

            case SEEK_ERR:                    return "SEEK_ERR";
            case READ_ERR:                    return "READ_ERR";
            case WRITE_ERR:                   return "WRITE_ERR";

            case DSK_MISSING:                 return "DSK_MISSING";
            case DSK_INCOMPATIBLE:            return "DSK_INCOMPATIBLE";
            case DSK_INVALID_DIAMETER:        return "DSK_INVALID_DIAMETER";
            case DSK_INVALID_DENSITY:         return "DSK_INVALID_DENSITY";
            case DSK_INVALID_LAYOUT:          return "DSK_INVALID_LAYOUT";
            case DSK_WRONG_SECTOR_CNT:        return "DSK_WRONG_SECTOR_CNT";
            case DSK_INVALID_TRACK_SIZE:      return "DSK_INVALID_TRACK_SIZE";

            case HDR_TOO_LARGE:               return "HDR_TOO_LARGE";
            case HDR_UNSUPPORTED_CYL_CNT:     return "HDR_UNSUPPORTED_CYL_CNT";
            case HDR_UNSUPPORTED_HEAD_CNT:    return "HDR_UNSUPPORTED_HEAD_CNT";
            case HDR_UNSUPPORTED_SEC_CNT:     return "HDR_UNSUPPORTED_SEC_CNT";
            case HDR_UNSUPPORTED_BSIZE:       return "HDR_UNSUPPORTED_BSIZE";
            case HDR_UNKNOWN_GEOMETRY:        return "HDR_UNKNOWN_GEOMETRY";
            case HDR_UNMATCHED_GEOMETRY:      return "HDR_UNMATCHED_GEOMETRY";
            case HDR_UNPARTITIONED:           return "HDR_UNPARTITIONED";
            case HDR_CORRUPTED_PTABLE:        return "HDR_CORRUPTED_PTABLE";
            case HDR_CORRUPTED_FSH:           return "HDR_CORRUPTED_FSH";
            case HDR_CORRUPTED_LSEG:          return "HDR_CORRUPTED_LSEG";
            case HDR_UNSUPPORTED:             return "HDR_UNSUPPORTED";

            default:
                return "???";
        }
    }

public:

    explicit DeviceError(long fault, const std::string &s = "");
    explicit DeviceError(long fault, const char *s) : DeviceError(fault, std::string(s)) { };
    explicit DeviceError(long fault, const std::filesystem::path &p) : DeviceError(fault, p.string()) { };
    explicit DeviceError(long fault, std::integral auto v) : DeviceError(fault, std::to_string(v)) { };
};

}
