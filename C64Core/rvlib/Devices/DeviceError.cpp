// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DeviceTypes.h"
#include "DeviceError.h"

namespace retro::vault {

DeviceError::DeviceError(long code, const string &s) : utl::Error(code)
{
    switch (code)
    {
        case INVALID_BLOCK_NR:
            set_msg("Invalid block index: " + s);
            break;

        case INVALID_TRACK_NR:
            set_msg("Invalid track index: " + s);
            break;

        case INVALID_SECTOR_NR:
            set_msg("Invalid sector index: " + s);
            break;

        case INVALID_CYLINDER_NR:
            set_msg("Invalid sector index: " + s);
            break;
            
        case INVALID_HEAD_NR:
            set_msg("Invalid sector index: " + s);
            break;

        case SEEK_ERR:
            set_msg("Seek error: " + s);
            break;

        case READ_ERR:
            set_msg("Read error: " + s);
            break;

        case WRITE_ERR:
            set_msg("Write error: " + s);
            break;

        case DSK_MISSING:
            set_msg("No disk in drive.");
            break;

        case DSK_INCOMPATIBLE:
            set_msg("This disk is not compatible with the selected drive.");
            break;

        case DSK_INVALID_DIAMETER:
            set_msg("Invalid disk diameter.");
            break;

        case DSK_INVALID_DENSITY:
            set_msg("Invalid disk density.");
            break;

        case DSK_INVALID_LAYOUT:
            set_msg("The disk density and disk diameter do not match.");
            break;

        case DSK_WRONG_SECTOR_CNT:
            set_msg("Unable to decode the MFM bit stream (wrong sector count).");
            break;

        case DSK_INVALID_TRACK_SIZE:
            set_msg("Invalid track size: " + s);
            break;

        case HDR_TOO_LARGE:
            set_msg(string("Hard drives with a maximum capacity of ") +
                    "504 MB are supported");
            break;

        case HDR_UNSUPPORTED_CYL_CNT:
            set_msg(string("The geometry of this drive is not supported. ") +
                    "Hard drives are supported with " +
                    "at least " + std::to_string(HDR_C_MIN) + " and " +
                    "at most " + std::to_string(HDR_C_MAX) + " cylinders. " +
                    "This drive has " + s + " cylinders.");
            break;

        case HDR_UNSUPPORTED_HEAD_CNT:
            set_msg(string("The geometry of this drive is not supported. ") +
                    "Hard drives are supported with " +
                    "at least " + std::to_string(HDR_H_MIN) + " and " +
                    "at most " + std::to_string(HDR_H_MAX) + " heads. " +
                    "The drive has " + s + " heads.");
            break;

        case HDR_UNSUPPORTED_SEC_CNT:
            set_msg(string("The geometry of this drive is not supported. ") +
                    "Hard drives are supported with " +
                    "at least " + std::to_string(HDR_S_MIN) + " and " +
                    "at most " + std::to_string(HDR_S_MAX) + " sectors. " +
                    "The drive stores " + s + " sectors per track.");
            break;

        case HDR_UNSUPPORTED_BSIZE:
            set_msg(string("The geometry of this drive is not supported. ") +
                    "Hard drives are supported with s " +
                    "block size of 512 bytes. " +
                    "The drive stores " + s + " bytes per block.");
            break;

        case HDR_UNKNOWN_GEOMETRY:
            set_msg("Failed to derive to geometry of this drive.");
            break;

        case HDR_UNMATCHED_GEOMETRY:
            set_msg("The drive geometry doesn't match the hard drive capacity.");
            break;

        case HDR_UNPARTITIONED:
            set_msg("The hard drive has no partitions.");
            break;

        case HDR_CORRUPTED_PTABLE:
            set_msg("Invalid partition table.");
            break;

        case HDR_CORRUPTED_FSH:
            set_msg("Invalid file system header block.");
            break;

        case HDR_UNSUPPORTED:
            set_msg("The hard drive is encoded in an unknown or unsupported format.");
            break;

        default:
            set_msg(string("Error code ") + std::to_string(payload) + " (" + errstr() + ").");
            break;
    }
}

}
