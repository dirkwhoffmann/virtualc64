// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "FileSystems/FSError.h"
// #include "FileSystems/Amiga/FileSystem.h"

namespace retro::vault {

FSError::FSError(FSFault fault, const string &s) : utl::Error(fault)
{
    switch (fault)
    {
        case FS_INVALID_PATH:
            set_msg("Invalid path: " + s);
            break;

        case FS_INVALID_ARGUMENT:
            set_msg("Invalid argument: " + s);
            break;

        case FS_NOT_A_DIRECTORY:
            set_msg(s.empty() ? "Not a directory." : s + " is not a directory.");
            break;

        case FS_NOT_A_FILE:
            set_msg(s.empty() ? "Not a file." : s + " is not a file.");
            break;

        case FS_NOT_FOUND:
            set_msg(s.empty() ? "Not found." : s + " not found.");
            break;

        case FS_EXISTS:
            set_msg(s.empty() ? "Item already exists." : s + " already exists.");
            break;

        case FS_UNFORMATTED:
            set_msg("Unformatted device.");
            break;

        case FS_UNSUPPORTED:
            set_msg("Unsupported file system.");
            break;

        case FS_READ_ERROR:
            set_msg("Read-only file system.");
            break;

        case FS_WRONG_BSIZE:
            set_msg("Invalid block size.");
            break;

        case FS_WRONG_DOS_TYPE:
            set_msg("Wrong DOS type.");
            break;

        case FS_WRONG_CAPACITY:
            set_msg("Wrong file system capacity.");
            break;

        case FS_CORRUPTED:
            set_msg("Corrupted file system.");
            break;

        case FS_INVALID_HANDLE:
            set_msg("Invalid file handle: " + s);
            break;

        case FS_OUT_OF_SPACE:
            set_msg("File system capacity exceeded.");
            break;

        case FS_NOT_EMPTY:
            set_msg("Directory is not empty.");
            break;

        default:
            set_msg("FSError " + std::to_string(fault) + " (" + errstr() + ")");
            break;
    }
}

}
