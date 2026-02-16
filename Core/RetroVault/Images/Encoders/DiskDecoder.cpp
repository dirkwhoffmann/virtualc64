// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DiskDecoder.h"

namespace retro::vault {

ByteView
DiskDecoder::decodeTrack(BitView track, TrackNr t)
{
    trackBuffer.resize(requiredTrackSize(t));
    return decodeTrack(track, t, trackBuffer);
}

ByteView
DiskDecoder::decodeSector(BitView track, TrackNr t, SectorNr s)
{
    sectorBuffer.resize(requiredSectorSize(t, s));
    return decodeSector(track, t, s, sectorBuffer);
}

}
