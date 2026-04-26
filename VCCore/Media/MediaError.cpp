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
#include "MediaError.h"

namespace vc64 {

MediaError::MediaError(long code, const string &s) : Error(code)
{
    switch (code) {

        case SNAP_TOO_OLD:
            set_msg("The snapshot was created with an older version of VirtualC64"
                    " and is incompatible with this release.");
            break;

        case SNAP_TOO_NEW:
            set_msg("The snapshot was created with a newer version of VirtualC64"
                    " and is incompatible with this release.");
            break;

        case SNAP_IS_BETA:
            set_msg("The snapshot was created with a beta version of VirtualC64"
                    " and is incompatible with this release.");
            break;

        case SNAP_CORRUPTED:
            set_msg("The snapshot data is corrupted and has put the"
                    " emulator into an inconsistent state.");
            break;

        case CRT_NO_CARTRIDGE:
            set_msg("No cartridge attached.");
            break;

        case CRT_UNKNOWN:
            set_msg("This cartridge is a type " + s +
                    " cartridge which is unknown to the emulator.");
            break;

        case CRT_UNSUPPORTED:
            set_msg("Cartridges of type " + s + " are not yet supported.");
            break;

        case CRT_TOO_MANY_PACKETS:
            set_msg("The packet counts exceeds the allowed limit.");
            break;

        case CRT_CORRUPTED_PACKET:
            set_msg("Corrputed packet detected.");

        default:
            set_msg(string("MediaError ") + std::to_string(code) + " (" + errstr() + ").");
            break;
    }
}

}
