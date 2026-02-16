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

#include "config.h"
#include "Error.h"
#include "utl/hints.h"

namespace vc64 {

AppError::AppError(Fault code, const string &s)
{
    data = i64(code);
    
    switch (code) {
            
        case Fault::OK:
            fatalError;
            
        case Fault::FS_UNSUPPORTED:
            description = "Unsupported file system.";
            break;
            
        case Fault::FS_WRONG_CAPACITY:
            description = "Wrong file system capacity.";
            break;

        case Fault::FS_CORRUPTED:
            description = "Corrupted file system.";
            break;

        case Fault::FS_HAS_NO_FILES:
            description = "Directory is empty.";
            break;

        case Fault::FS_HAS_CYCLES:
            description = "Cyclic reference chain detected.";
            break;
            
        case Fault::FS_CANT_IMPORT:
            description = "Unable to import.";
            break;

        default:
            description = "Error code " + std::to_string(data) + " (" + FaultEnum::key((Fault)code) + ").";
            break;
    }
}

const char *
AppError::what() const throw()
{
    return description.c_str();
}

}
