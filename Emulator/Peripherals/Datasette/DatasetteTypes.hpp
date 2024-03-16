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

#include "DatasetteTypes.h"

struct DatasetteModelEnum : util::Reflection<DatasetteModelEnum, DatasetteModel> {

    static constexpr long minVal = 0;
    static constexpr long maxVal = DATASETTE_C1530;
    static bool isValid(auto value) { return value >= minVal && value <= maxVal; }

    static const char *prefix() { return "DATASETTE"; }
    static const char *key(DatasetteModel value)
    {
        switch (value) {

            case DATASETTE_C1530:  return "C1530";
        }
        return "???";
    }
};
