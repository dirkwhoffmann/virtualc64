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

namespace utl {

struct ScriptInterruption: Exception {

    using Exception::Exception;
};

struct RSError : public Error {

    static constexpr long OK                    = 0;
    static constexpr long GENERIC               = 1;
    static constexpr long SYNTAX_ERROR          = 2;
    static constexpr long TOO_FEW_ARGUMENTS     = 3;
    static constexpr long TOO_MANY_ARGUMENTS    = 4;
    static constexpr long MISSING_FLAG          = 5;
    static constexpr long UNKNOWN_FLAG          = 6;
    static constexpr long MISSING_KEY_VALUE     = 7;
    static constexpr long UNKNOWN_KEY_VALUE     = 8;

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                    return "OK";
            case GENERIC:               return "GENERIC";
            case SYNTAX_ERROR:          return "SYNTAX_ERROR";
            case TOO_FEW_ARGUMENTS:     return "TOO_FEW_ARGUMENTS";
            case TOO_MANY_ARGUMENTS:    return "TOO_MANY_ARGUMENTS";
            case MISSING_FLAG:          return "MISSING_FLAG";
            case UNKNOWN_FLAG:          return "UNKNOWN_FLAG";
            case MISSING_KEY_VALUE:     return "MISSING_KEY_VALUE";
            case UNKNOWN_KEY_VALUE:     return "UNKNOWN_KEY_VALUE";

            default:
                return "UNKNOWN";
        }
    }

    explicit RSError(long fault, const std::string &msg = "");
    explicit RSError(long fault, const char *str) : RSError(fault, string(str)) { }
    explicit RSError(long fault, const fs::path &path) : RSError(fault, path.string()) { }
};

}
