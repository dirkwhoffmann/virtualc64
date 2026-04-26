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
#include "CoreError.h"
#include "Constants.h"

namespace vc64 {

CoreError::CoreError(long code, const string &s) : Error(code)
{
    switch (code)
    {

        case OK:
            fatalError;
            break;

        case LAUNCH:
            set_msg("Launch error.");
            break;

        case POWERED_OFF:
            set_msg("The emulator is powered off.");
            break;

        case POWERED_ON:
            set_msg("The emulator is powered on.");
            break;

        case RUNNING:
            set_msg("The emulator is running.");
            break;

        case OPT_UNSUPPORTED:
            set_msg(s == "" ? "This option is not supported yet." : s);
            break;
            
        case OPT_INV_ARG:
            set_msg("Invalid argument. Expected: " + s);
            break;

        case OPT_INV_ID:
            set_msg("Invalid component ID. Expected: " + s);
            break;

        case OPT_LOCKED:
            set_msg("This option is locked because the Amiga is powered on.");
            break;

        case INVALID_KEY:
            set_msg("Invalid key: " + s + ".");
            break;
            
        case SYNTAX:
            set_msg("Syntax error" + (s.empty() ? "" : " in line " + s + "."));
            break;

        case GUARD_NOT_FOUND:
            set_msg("Entry " + s + " not found.");
            break;

        case GUARD_ALREADY_SET:
            set_msg("Target " + s + " is already observed.");
            break;

        case BP_NOT_FOUND:
            set_msg("Breakpoint " + s + " not found.");
            break;
            
        case BP_ALREADY_SET:
            set_msg("A breakpoint at " + s + " is already set.");
            break;
            
        case WP_NOT_FOUND:
            set_msg("Watchpoint " + s + " not found.");
            break;

        case WP_ALREADY_SET:
            set_msg("A watchpoint at " + s + " is already set.");
            break;

        case OUT_OF_MEMORY:
            set_msg("Out of memory.");
            break;
            

        case ROM_BASIC_MISSING:
            set_msg("No Basic Rom installed.");
            break;

        case ROM_CHAR_MISSING:
            set_msg("No Character Rom installed.");
            break;

        case ROM_KERNAL_MISSING:
            set_msg("No Kernal Rom installed.");
            break;

        case ROM_DRIVE_MISSING:
            set_msg("No Floppy Drive Rom installed.");
            break;

        case ROM_MEGA65_MISMATCH:
            set_msg("MEGA65 Roms have different version numbers.");
            break;

        case DRV_UNCONNECTED:
            set_msg("Floppy drive is not connected.");
            break;

        case DRV_NO_DISK:
            set_msg("Floppy drive contains no disk.");
            break;

        default:
            set_msg(string("CoreError ") + std::to_string(code) + " (" + errstr() + ").");
            break;
    }
}

}
