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

#include "DiskTypes.h"

namespace vc64 {

typedef struct
{
    isize headerBegin;
    isize headerEnd;
    isize dataBegin;
    isize dataEnd;
}
SectorInfo;

typedef struct
{
    isize length;
    SectorInfo sectorInfo[22];
}
TrackInfo;

typedef struct
{
    TrackInfo trackInfo[85];
}
DiskInfo;

}
