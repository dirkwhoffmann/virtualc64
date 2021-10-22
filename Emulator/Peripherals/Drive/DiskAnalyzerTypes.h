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
#include "DiskTypes.h"

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
