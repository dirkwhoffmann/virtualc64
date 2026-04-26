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

struct ImageError : public Error {

    static constexpr long OK                =  0;
    static constexpr long CUSTOM            =  1;

    // General
    static constexpr long SIZE_MISMATCH     = 10;
    static constexpr long FORMAT_MISMATCH   = 11;

    // DMS images
    static constexpr long DMS_CANT_CREATE   = 20;

    // Extended ADF images
    static constexpr long EXT_FACTOR5       = 30;
    static constexpr long EXT_INCOMPATIBLE  = 31;
    static constexpr long EXT_CORRUPTED     = 32;

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                          return "OK";
            case CUSTOM:                      return "CUSTOM";

            case SIZE_MISMATCH:               return "SIZE_MISMATCH";
            case FORMAT_MISMATCH:             return "FORMAT_MISMATCH";

            case DMS_CANT_CREATE:             return "DMS_CANT_CREATE";
            case EXT_FACTOR5:                 return "EXT_UNSUPPORTED";
            case EXT_INCOMPATIBLE:            return "EXT_INCOMPATIBLE";
            case EXT_CORRUPTED:               return "EXT_CORRUPTED";
        }
        return "???";
    }

    explicit ImageError(long fault, const string &s);
    explicit ImageError(long fault, const char *s) : ImageError(fault, string(s)) { };
    explicit ImageError(long fault, const fs::path &p) : ImageError(fault, p.string()) { };
    explicit ImageError(long fault, std::integral auto v) : ImageError(fault, std::to_string(v)) { };
    explicit ImageError(long fault) : ImageError(fault, "") { }
};

}
