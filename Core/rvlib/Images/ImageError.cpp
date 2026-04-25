// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "ImageTypes.h"
#include "ImageError.h"

namespace retro::vault {

ImageError::ImageError(long code, const string &s) : utl::Error(code)
{
    switch (code)
    {
        case DMS_CANT_CREATE:
            set_msg("Failed to extract the DMS archive.");
            break;

        case EXT_FACTOR5:
            set_msg("The file is encoded in an outdated format that was"
                    " introduced by Factor 5 to distribute Turrican images."
                    " The format has no relevance today and is not supported"
                    " by the emulator.");
            break;

        case EXT_INCOMPATIBLE:
            set_msg("This file utilizes encoding features of the extended "
                    " ADF format that are not supported by the emulator yet.");
            break;

        case EXT_CORRUPTED:
            set_msg("The disk encoder failed to extract the disk due to "
                    " corrupted or inconsistend file data.");
            break;

        default:
            set_msg(string("ImageError ") + std::to_string(code) + " (" + errstr() + ").");
            break;
    }
}

}
