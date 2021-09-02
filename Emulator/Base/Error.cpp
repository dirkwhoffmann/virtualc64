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
            fatalError;

        case ERROR_OPT_UNSUPPORTED:
            description = "This option is not supported yet.";
            break;
            
        case ERROR_OPT_INVARG:
            description = "Invalid argument. Expected: " + s;
            break;
            
        case ERROR_OPT_LOCKED:
            description = "This option is locked because the C64 is powered on.";
            break;

        case ERROR_OUT_OF_MEMORY:
            description = "Out of memory.";
            break;

        case ERROR_FILE_NOT_FOUND:
            description = "File \"" + s + "\" not found.";
            break;
            
        case ERROR_FILE_TYPE_MISMATCH:
            description = "The file content and the file type do not match.";
            break;
            
        case ERROR_FILE_CANT_READ:
            description = "Failed to read from file \"" + s + "\".";
            break;
            
        case ERROR_FILE_CANT_WRITE:
            description = "Failed to write to file \"" + s + "\".";
            break;
            
        case ERROR_FILE_CANT_CREATE:
            description = "Failed to create file \"" + s + "\".";
            break;

        case ERROR_DIR_CANT_CREATE:
            description = "Failed to create directory \"" + s + "\".";
            break;

        case ERROR_DIR_NOT_EMPTY:
            description = "Directory \"" + s + "\" is not empty.";
            break;

        case ERROR_ROM_BASIC_MISSING:
            description = "No Basic Rom installed.";
            break;

        case ERROR_ROM_CHAR_MISSING:
            description = "No Character Rom installed.";
            break;

        case ERROR_ROM_KERNAL_MISSING:
            description = "No Kernal Rom installed.";
            break;

        case ERROR_ROM_DRIVE_MISSING:
            description = "No Floppy Drive Rom installed.";
            break;

        case ERROR_ROM_MEGA65_MISMATCH:
            description = "Mega65 Rom revisions do not match.";
            break;

        case ERROR_SNP_TOO_OLD:
            description = "The snapshot was created with an older version of VirtualC64";
            description += " and is incompatible with this release.";
            break;

        case ERROR_SNP_TOO_NEW:
            description = "The snapshot was created with a newer version of VirtualC64";
            description += " and is incompatible with this release.";
            break;

        case ERROR_DRV_UNCONNECTED:
            description = "Drive is unconnected.";
            break;

        case ERROR_CRT_UNKNOWN:
            description = "This cartridge is a type " + s + " cartridge which";
            description += " is unknown to the emulator.";
            break;

        case ERROR_CRT_UNSUPPORTED:
            description = "Cartridges of type " + s + " are not yet supported.";
            break;

        case ERROR_CRT_TOO_MANY_PACKETS:
            description = "The packet counts exceeds the allowed limit.";
            break;

        case ERROR_CRT_CORRUPTED_PACKET:
            description = "Corrputed packet detected.";
            
        case ERROR_FS_UNSUPPORTED:
            description = "Unsupported file system.";
            break;
            
        case ERROR_FS_WRONG_CAPACITY:
            description = "Wrong file system capacity.";
            break;

        case ERROR_FS_CORRUPTED:
            description = "Corrupted file system.";
            break;

        case ERROR_FS_HAS_NO_FILES:
            description = "Directory is empty.";
            break;

        case ERROR_FS_HAS_CYCLES:
            description = "Cyclic reference chain detected.";
            break;
            
        case ERROR_FS_CANT_IMPORT:
            description = "Unable to import.";
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
