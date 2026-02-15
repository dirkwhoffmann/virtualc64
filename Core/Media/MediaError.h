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

#include "utl/common.h"

namespace vc64 {

using namespace utl;

struct MediaError : public Error {

    static constexpr long OK                =   0; ///< No error
    static constexpr long UNKNOWN           =   1; ///< Unclassified

    // Snapshots
    static constexpr long SNAP_TOO_OLD      = 10; ///< Snapshot was created with an older version
    static constexpr long SNAP_TOO_NEW      = 11; ///< Snapshot was created with a later version
    static constexpr long SNAP_IS_BETA      = 12; ///< Snapshot was created with a beta release
    static constexpr long SNAP_CORRUPTED    = 13; ///< Snapshot data is corrupted

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                          return "OK";
            case UNKNOWN:                     return "UNKNOWN";

            case SNAP_TOO_OLD:                return "SNAP_TOO_OLD";
            case SNAP_TOO_NEW:                return "SNAP_TOO_NEW";
            case SNAP_IS_BETA:                return "SNAP_IS_BETA";
            case SNAP_CORRUPTED:              return "SNAP_CORRUPTED";
        }
        return "???";
    }

    explicit MediaError(long fault, const string &s);
    explicit MediaError(long fault, const char *s) : MediaError(fault, string(s)) { };
    explicit MediaError(long fault, const fs::path &p) : MediaError(fault, p.string()) { };
    explicit MediaError(long fault, std::integral auto v) : MediaError(fault, std::to_string(v)) { };
    explicit MediaError(long fault) : MediaError(fault, "") { }
};

}
