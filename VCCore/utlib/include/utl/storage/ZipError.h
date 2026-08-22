// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/types/Exception.h"

namespace utl {

struct ZipError : Error {

    static constexpr long INVALID_ZIP_MODE = 1;
    static constexpr long INVALID_ARCHIVE  = 2;
    static constexpr long NOT_A_FILE       = 3;
    static constexpr long NOT_A_DIRECTORY  = 4;
    static constexpr long KUBA_ZIP_ERROR   = 5;

    const char *errstr() const noexcept override
    {

        switch (payload) {

            case INVALID_ZIP_MODE: return "INVALID_ZIP_MODE";
            case INVALID_ARCHIVE:  return "INVALID_ARCHIVE";
            case NOT_A_FILE:       return "NOT_A_FILE";
            case NOT_A_DIRECTORY:  return "NOT_A_DIRECTORY";
            case KUBA_ZIP_ERROR:   return "KUBA_ZIP_ERROR";

            default:               return "UNKNOWN";
        }
    }

    explicit ZipError(long fault, const std::string &msg = "") : Error(fault)
    {

        switch (fault) {

            case INVALID_ZIP_MODE: set_msg("Invalid ZIP mode: " + msg); break;
            case INVALID_ARCHIVE:  set_msg("Invalid or unsupported ZIP file: " + msg); break;
            case NOT_A_FILE:       set_msg("Not a file: " + msg); break;
            case NOT_A_DIRECTORY:  set_msg("Not a directory: " + msg); break;
            case KUBA_ZIP_ERROR:   set_msg("KubaZip error code: " + msg); break;
            default:               set_msg("ZipError " + std::to_string(fault) + " (" + errstr() + ")");
        }
    }

    explicit ZipError(long fault, long ec) : Error(fault) { setError(fault, ec); }
    explicit ZipError(long fault, const char *str) : Error(fault) { setError(fault, string(str)); }
    explicit ZipError(long fault, const fs::path &path) : Error(fault) { setError(fault, path.string()); }

  private:

    void setError(long fault, long ec);
    void setError(long fault, const string &str);
};

}
