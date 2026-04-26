// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DeviceTypes.h"
#include "utl/abilities/Loggable.h"
#include "utl/primitives/BitView.h"
#include "utl/primitives/Range.h"

namespace retro::vault {

using namespace utl;

/* Base class for decoding disk data from raw bit streams.
 *
 * A DiskDecoder converts a bit-level track representation into
 * byte-addressable data (tracks or sectors).
 *
 * Decoding can either write into caller-provided buffers or into
 * internal backing buffers managed by the decoder.
 */

class DiskDecoder : public Loggable {

    // Backing buffers
    std::vector<u8> trackBuffer;
    std::vector<u8> sectorBuffer;

public:

    virtual ~DiskDecoder() = default;

    // Reports the minimum number of bytes required to decode a track or sector
    virtual isize requiredTrackSize(TrackNr t) { return 16384; }
    virtual isize requiredSectorSize(TrackNr t, SectorNr s) { return 512; }

    // Decodes a track or sector into a caller-provided destination buffer
    virtual ByteView decodeTrack(BitView track, TrackNr t, std::span<u8> out) = 0;
    virtual ByteView decodeSector(BitView track, TrackNr t, SectorNr s, std::span<u8> out) = 0;

    // Decodes a track or sector into the internal backing buffers
    ByteView decodeTrack(BitView track, TrackNr t);
    ByteView decodeSector(BitView track, TrackNr t, SectorNr s);

    // Returns a range on the sector’s data area
    virtual optional<Range<isize>> seekSector(BitView track, SectorNr s, isize offset = 0) = 0;
    virtual std::unordered_map<isize, Range<isize>> seekSectors(BitView track) = 0;
};

}
