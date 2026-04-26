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

#pragma once

#include "BasicTypes.h"

namespace vc64 {

struct CoreError : public Error {

    static constexpr long OK                =   0; ///< No error
    static constexpr long UNKNOWN           =   1; ///< Unclassified error condition

    // Execution state
    static constexpr long LAUNCH            = 100; ///< Launch error
    static constexpr long POWERED_OFF       = 101; ///< The emulator is powered off
    static constexpr long POWERED_ON        = 102; ///< The emulator is powered on
    static constexpr long RUNNING           = 103; ///< The emulator is running

    // Configuration
    static constexpr long OPT_UNSUPPORTED   = 200; ///< Unsupported configuration option
    static constexpr long OPT_INV_ARG       = 201; ///< Invalid argument
    static constexpr long OPT_INV_ID        = 202; ///< Invalid component id
    static constexpr long OPT_LOCKED        = 203; ///< The option is temporarily locked

    // Property storage
    static constexpr long INVALID_KEY       = 300; ///< Invalid property key
    static constexpr long SYNTAX            = 301; ///< Syntax error

    // CPU
    static constexpr long GUARD_NOT_FOUND   = 401; ///< Guard is not set
    static constexpr long GUARD_ALREADY_SET = 402; ///< Guard is already set
    static constexpr long BP_NOT_FOUND      = 403; ///< Breakpoint is not set (DEPRECATED)
    static constexpr long BP_ALREADY_SET    = 404; ///< Breakpoint is already set (DEPRECATED)
    static constexpr long WP_NOT_FOUND      = 405; ///< Watchpoint is not set (DEPRECATED)
    static constexpr long WP_ALREADY_SET    = 406; ///< Watchpoint is already set (DEPRECATED)

    // Memory
    static constexpr long OUT_OF_MEMORY     = 500; ///< Out of memory

    // Roms
    static constexpr long ROM_BASIC_MISSING     = 600; ///< No Basic Rom installed
    static constexpr long ROM_CHAR_MISSING      = 601; ///< No Character Rom installed
    static constexpr long ROM_KERNAL_MISSING    = 602; ///< No Kernal Rom installed
    static constexpr long ROM_DRIVE_MISSING     = 603; ///< No Floppy Drive Rom installed
    static constexpr long ROM_MEGA65_MISMATCH   = 604; ///< MEGA65 Roms have different version numbers
    
    // Drives
    static constexpr long DRV_UNCONNECTED       = 700; ///< Floppy drive is not connected
    static constexpr long DRV_NO_DISK           = 701; ///< Floppy drive contains no disk

    const char *errstr() const noexcept override {

        switch (payload) {

            case OK:                    return "OK";
            case UNKNOWN:               return "UNKNOWN";

            case LAUNCH:                return "LAUNCH";
            case POWERED_OFF:           return "POWERED_OFF";
            case POWERED_ON:            return "POWERED_ON";
            case RUNNING:               return "RUNNING";

            case OPT_UNSUPPORTED:       return "OPT_UNSUPPORTED";
            case OPT_INV_ARG:           return "OPT_INV_ARG";
            case OPT_INV_ID:            return "OPT_INV_ID";
            case OPT_LOCKED:            return "OPT_LOCKED";

            case INVALID_KEY:           return "INVALID_KEY";
            case SYNTAX:                return "SYNTAX";

            case GUARD_NOT_FOUND:       return "GUARD_NOT_FOUND";
            case GUARD_ALREADY_SET:     return "GUARD_ALREADY_SET";
            case BP_NOT_FOUND:          return "BP_NOT_FOUND";
            case BP_ALREADY_SET:        return "BP_ALREADY_SET";
            case WP_NOT_FOUND:          return "WP_NOT_FOUND";
            case WP_ALREADY_SET:        return "WP_ALREADY_SET";

            case OUT_OF_MEMORY:         return "OUT_OF_MEMORY";

            case ROM_BASIC_MISSING:     return "ROM_BASIC_MISSING";
            case ROM_CHAR_MISSING:      return "ROM_CHAR_MISSING";
            case ROM_KERNAL_MISSING:    return "ROM_KERNAL_MISSING";
            case ROM_DRIVE_MISSING:     return "ROM_DRIVE_MISSING";
            case ROM_MEGA65_MISMATCH:   return "ROM_MEGA65_MISMATCH";

            case DRV_UNCONNECTED:       return "DRV_UNCONNECTED";
            case DRV_NO_DISK:           return "DRV_NO_DISK";
        }
        return "???";
    }

    explicit CoreError(long fault, const string &s);
    explicit CoreError(long fault, const char *s) : CoreError(fault, string(s)) { };
    explicit CoreError(long fault, const fs::path &p) : CoreError(fault, p.string()) { };
    explicit CoreError(long fault, std::integral auto v) : CoreError(fault, std::to_string(v)) { };
    explicit CoreError(long fault) : CoreError(fault, "") { }
};

}
