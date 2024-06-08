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
/// @file

#pragma once

#include "Reflection.h"

namespace vc64 {

//
// Enumerations
//

/// Datasette model
enum_long(DATASETTE_MODEL)
{
    DATASETTE_C1530     ///< Standard datasette (Commodore)
};
typedef DATASETTE_MODEL DatasetteModel;

struct DatasetteModelEnum : util::Reflection<DatasetteModelEnum, DatasetteModel> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DATASETTE_C1530;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DATASETTE"; }
    static const char *key(long value)
    {
        switch (value) {

            case DATASETTE_C1530:  return "C1530";
        }
        return "???";
    }
};

//
// Structures
//

/// Datasette configuration
typedef struct
{
    DatasetteModel model;       ///< Datasette model
    bool connected;             ///< Connection status
}
DatasetteConfig;

/// Datasette current state
typedef struct
{
    bool hasTape;               ///< Is a tape inserted?
    u8 type;                    ///< TAP file format
    bool motor;                 ///< Is the motor on?
    bool playKey;               ///< Is the play key pressed?
    isize counter;              ///< Progress counter
}
DatasetteInfo;

}
