// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DiskDecoder.h"

namespace retro::vault {

class C64Decoder : public DiskDecoder {

public:

    //
    // Methods from DiskDecoder
    //

    using DiskDecoder::decodeTrack;
    using DiskDecoder::decodeSector;

    ByteView decodeTrack(BitView track, TrackNr t, std::span<u8> out) override;
    ByteView decodeSector(BitView track, TrackNr t, SectorNr s, std::span<u8> out) override;

    optional<Range<isize>> seekSector(BitView track, SectorNr s, isize offset = 0) override;
    std::unordered_map<isize, Range<isize>> seekSectors(BitView track) override;

private:

    // Moves the iterator to the bit following the next sync mark
    bool seekSync(BitView track, BitView::cyclic_iterator &it);

    // Moves the iterator to the bit following the next header block sync mark
    bool seekHeaderSync(BitView track, BitView::cyclic_iterator &it);

    // Locates the data areas of certain sectors on a track
    //
    // `wanted` specifies which sectors to locate. For example, { 7 } searches
    // only for sector 7. If empty, all sectors are searched for.
    //
    // `offset` specifies the bit position at which the search begins. If a
    // sector’s approximate position is already known, this can be used to
    // speed up the search.
    //
    // Returns a mapping from sector numbers to the respective data area range.

    std::unordered_map<SectorNr, Range<isize>> seekSectors(BitView track,
                                                           std::span<const SectorNr> wanted,
                                                           isize offset = 0);
};

}
