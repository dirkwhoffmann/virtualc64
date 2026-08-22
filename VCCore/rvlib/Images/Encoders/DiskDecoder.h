// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Devices/DeviceTypes.h"
#include "utl/abilities/Loggable.h"
#include "utl/primitives/BitView.h"
#include "utl/primitives/Range.h"
#include "Images/ImageTypes.h"

namespace retro::vault {

/* Base class for decoding disk data from raw bit streams.
 *
 * A DiskDecoder converts a bit-level track representation into
 * byte-addressable data (tracks or sectors).
 *
 * Decoding can either write into caller-provided buffers or into
 * internal backing buffers managed by the decoder.
 */

class DiskDecoder : public utl::Loggable {

    // Backing buffers
    std::vector<u8> trackBuffer;
    std::vector<u8> sectorBuffer;

public:

    virtual ~DiskDecoder() = default;

    // Reports the minimum number of bytes required to decode a track or sector
    virtual isize requiredTrackSize(TrackNr t) { return 16384; }
    virtual isize requiredSectorSize(TrackNr t, SectorNr s) { return 512; }

    // Decodes a track or sector into a caller-provided destination buffer
    virtual utl::ByteView decodeTrack(utl::BitView track, TrackNr t, std::span<u8> out) = 0;
    virtual utl::ByteView decodeSector(utl::BitView track, TrackNr t, SectorNr s, std::span<u8> out) = 0;

    // Decodes a track or sector into the internal backing buffers
    utl::ByteView decodeTrack(utl::BitView track, TrackNr t);
    utl::ByteView decodeSector(utl::BitView track, TrackNr t, SectorNr s);

    // Returns a range on the sector’s data area
    virtual optional<utl::Range<isize>> seekSector(utl::BitView track, SectorNr s, isize offset = 0) = 0;
    virtual std::unordered_map<isize, utl::Range<isize>> seekSectors(utl::BitView track) = 0;
};

}
