// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/ImageTypes.h"
#include "Images/ImageError.h"
#include <format>

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

        case VM_NOT_FOUND:
            set_msg("Virtual machine not found.");
            break;

        case VM_CANT_OPEN:
            set_msg("Failed to open virtual machine.");
            break;

        case VM_CANT_RUN:
            set_msg("Virtual machine is not ready to run.");
            break;

        case VM_EXISTS:
            set_msg(std::format("'{}' already exists in the database.", s));
            break;

        case VM_RUNNING:
            set_msg(s.empty() ? "Virtual machine is running." : std::format("'{}' is running.", s));
            break;

        case VM_NO_MANIFEST:
            set_msg("Virtual machine contains no Manifest.");
            break;

        case VM_CORRUPTED:
            set_msg(s.empty() ?
                    "The virtual machine is corrupted." :
                    "The virtual machine is corrupted: " + s);
            break;

        case VM_SAME_UUID:
            set_msg("A virtual machine with the same UUID is already present.");
            break;

        case VM_READ_ONLY:
            set_msg("The virtual machine is write-protected.");
            break;

        case NO_CORE:
            set_msg("Missing emulator core.");
            break;

        case NO_SVM:
            if (msg.empty()) {
                set_msg("SVM file not found.");
            } else {
                set_msg("SVM file not found: " + msg);
            }
            break;

        case NO_WORKING_FOLDER:
            set_msg("Working folder not found" + (msg.empty() ? "." : ": " + msg));
            break;

        case NO_WORKSPACE_FOLDER:
            set_msg("Workspace folder not found" + (msg.empty() ? "." : ": " + msg));
            break;

        case NO_SNAPSHOT_FOLDER:
            set_msg("Snapshot folder not found" + (msg.empty() ? "." : ": " + msg));
            break;

        case EXECUTABLE_NOT_FOUND:
            set_msg(std::format("Executable '{}' not found.", s));
            break;

        default:
            set_msg(string("ImageError ") + std::to_string(code) + " (" + errstr() + ").");
            break;
    }
}

}
