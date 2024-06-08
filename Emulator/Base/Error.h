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
#include "Exception.h"

namespace vc64 {

/// Emulator exception
struct Error : public util::Exception
{
    Error(ErrorCode code, const string &s);
    Error(ErrorCode code, long v) : Error(code, std::to_string(v)) { };
    Error(ErrorCode code) : Error(code, "") { }
    Error() : Exception() { }

    /// Returns a textual description of this error
    const char *what() const throw() override;
    
    /// Return the numerical identifier of this error
    ErrorCode errorCode() const { return (ErrorCode)data; }
};

}
