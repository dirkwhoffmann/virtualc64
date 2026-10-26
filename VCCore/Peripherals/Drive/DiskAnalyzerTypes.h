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

#include "FloppyDiskTypes.h"

namespace vc64 {

/// Position of a sector on a track (in bits)
typedef struct
{
    isize headerBegin;                   ///< Start of the sector header
    isize headerEnd;                     ///< End of the sector header
    isize dataBegin;                     ///< Start of the data block
    isize dataEnd;                       ///< End of the data block
}
SectorLayout;

/// Layout of a track
typedef struct
{
    isize length;                        ///< Length of the track (in bits)
    SectorLayout sectorLayout[22];       ///< Layout of the sectors
}
TrackLayout;

/// Layout of a disk
typedef struct
{
    TrackLayout trackLayout[85];         ///< Layout of the tracks
}
DiskLayout;

}
