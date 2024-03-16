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
