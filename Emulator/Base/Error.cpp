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
    return ErrorCodeEnum::key(data);
}

string
VC64Error::describe() const
{
    switch ((ErrorCode)data) {
            
        case ERROR_OPT_INV_ARG:
            return "Invalid argument. Expected " + description;
            
        case ERROR_FILE_NOT_FOUND:
            return "File " + description + " not found";
            
        case ERROR_FILE_CANT_READ:
            return "Failed read file " + description;

        case ERROR_FILE_CANT_WRITE:
            return "Failed write file " + description;

        default:
            return "Command failed with error code "
            + std::to_string(data) + " (" + ErrorCodeEnum::key(data) + ")";
    }
}
