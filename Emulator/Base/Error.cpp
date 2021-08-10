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

VC64Error::VC64Error(ErrorCode code, const string &s)
{
    data = code;
    
    switch (code) {
            
        case ERROR_OK:
            assert(false);
            break;

        case ERROR_OPT_INV_ARG:
            description = "Invalid argument. Expected:  " + s;
            break;
            
        case ERROR_FILE_NOT_FOUND:
            description = "File \"" + s + "\" not found.";
            break;

        case ERROR_FILE_CANT_READ:
            description = "Failed to read from file \"" + s + "\".";
            break;
            
        case ERROR_FILE_CANT_WRITE:
            description = "Failed to write to file \"" + s + "\".";
            break;

        default:
            description = "Error code " + std::to_string(data) + " (" + ErrorCodeEnum::key(data) + ").";
            break;
    }
}

const char *
VC64Error::what() const throw()
{
    return description.c_str();
}
