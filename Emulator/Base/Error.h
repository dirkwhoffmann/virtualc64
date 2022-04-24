// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ErrorTypes.h"
#include "Exception.h"

//
// VCError
//

struct VC64Error : public util::Exception
{
    VC64Error(ErrorCode code, const string &s);
    VC64Error(ErrorCode code, long v) : VC64Error(code, std::to_string(v)) { };
    VC64Error(ErrorCode code) : VC64Error(code, "") { }

    const char *what() const throw() override;
};
