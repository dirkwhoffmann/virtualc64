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

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

/// Datasette model
enum_long(DATASETTE_MODEL)
{
    DATASETTE_C1530     ///< Standard datasette (Commodore)
};
typedef DATASETTE_MODEL DatasetteModel;

#ifdef __cplusplus
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
#endif


//
// Structures
//

typedef struct
{
    DatasetteModel model;
    bool connected;
}
DatasetteConfig;

typedef struct
{
    bool hasTape;
    u8 type;
    bool motor;
    bool playKey;
    isize counter;
}
DatasetteInfo;
