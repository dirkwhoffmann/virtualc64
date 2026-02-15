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

struct NewParseError : public Error {

    static constexpr long PARSE_BOOL_ERROR   = 0;
    static constexpr long PARSE_ON_OFF_ERROR = 1;
    static constexpr long PARSE_NUM_ERROR    = 2;
    static constexpr long PARSE_ENUM_ERROR   = 3;

    const char *errstr() const noexcept override {

        switch (payload) {
                
            case PARSE_BOOL_ERROR:   return "PARSE_BOOL_ERROR";
            case PARSE_ON_OFF_ERROR: return "PARSE_ON_OFF_ERROR";
            case PARSE_NUM_ERROR:    return "PARSE_NUM_ERROR";
            case PARSE_ENUM_ERROR:   return "PARSE_ENUM_ERROR";
                
            default:
                return "UNKNOWN";
        }
    }

    NewParseError(long fault, const std::string &msg) : Error(fault) {

        switch (fault) {

            case PARSE_BOOL_ERROR:
                set_msg(msg + " must be true or false.");
                break;

            case PARSE_ON_OFF_ERROR:
                set_msg(msg + " must be on or off.");
                break;

            case PARSE_NUM_ERROR:
                set_msg(msg + " is not a number.");
                break;

            case PARSE_ENUM_ERROR:
                set_msg(msg + " is not a valid key.");
                break;

            default:
                set_msg("ParseError " + std::to_string(fault) + " (" + errstr() + ")");
        }
    }

    NewParseError(long d, const std::filesystem::path &p) : NewParseError(d, p.string()) { }
};

}
