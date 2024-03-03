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

//
// VCError
//

/// Emulator exception
struct VC64Error : public util::Exception
{
    VC64Error(ErrorCode code, const string &s);
    VC64Error(ErrorCode code, long v) : VC64Error(code, std::to_string(v)) { };
    VC64Error(ErrorCode code) : VC64Error(code, "") { }

    const char *what() const throw() override;
};

}
