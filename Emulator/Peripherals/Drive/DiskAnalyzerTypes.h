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

/* Information about a single sector as gathered by analyzeSector()
 */
typedef struct
{
    isize headerBegin;
    isize headerEnd;
    isize dataBegin;
    isize dataEnd;
}
SectorInfo;

/* Information about a single track as gathered by analyzeTrack().
 * For faster access, the the track data is stored as a byte stream. Each byte
 * represents a single bit and is either 0 or 1. The stored sequence is
 * repeated twice to avoid the need of wrapping around.
 */
typedef struct
{
    isize length;                        // Length of the track in bits
    SectorInfo sectorInfo[22];           // Sector layout data
}
TrackInfo;
