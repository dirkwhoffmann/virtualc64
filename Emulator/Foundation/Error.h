// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "C64PublicTypes.h"
#include <exception>

struct Error : public std::exception
{
    ErrorCode errorCode;
    
    Error(ErrorCode code) : errorCode(code) { }
    
    const char *what() const throw() override {

        return ErrorCodeName(errorCode);
    }
};
