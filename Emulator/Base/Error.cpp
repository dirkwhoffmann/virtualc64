// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Error.h"

const char *
VC64Error::what() const throw()
{
    return  ErrorCodeEnum::key(data);
}

const char *
ConfigError::what() const throw()
{
    return  description.c_str();
}
