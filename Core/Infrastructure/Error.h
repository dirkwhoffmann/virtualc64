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

#include "ErrorTypes.h"

namespace vc64 {

struct AppException : public std::exception {
    
    // Payload
    i64 data;

    // Auxiliary information about the thrown errow
    string description;
    
    AppException(i64 d, const string &s) : data(d), description(s) { }
    AppException(i64 d) : data(d), description("") { }
    AppException(const string &s) : data(0), description(s) { }
    AppException() : data(0) { }
    
    const char *what() const noexcept override { return description.c_str(); }
};

/// Emulator exception
struct AppError : public AppException
{
    AppError(Fault code, const string &s);
    AppError(Fault code, const char *s) : AppError(code, string(s)) { };
    AppError(Fault code, const std::filesystem::path &path) : AppError(code, path.string()) { };
    AppError(Fault code, long v) : AppError(code, std::to_string(v)) { };
    AppError(Fault code) : AppError(code, "") { }

    /// Returns a textual description of this error
    const char *what() const throw() override;
};

}
