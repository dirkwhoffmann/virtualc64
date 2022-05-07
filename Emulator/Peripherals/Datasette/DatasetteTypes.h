// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Aliases.h"
#include "Reflection.h"

//
// Enumerations
//

enum_long(DATASETTE_MODEL)
{
    DATASETTE_C1530
};
typedef DATASETTE_MODEL DatasetteModel;

#ifdef __cplusplus
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
