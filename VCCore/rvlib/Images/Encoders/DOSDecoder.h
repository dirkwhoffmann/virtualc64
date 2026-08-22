// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/Encoders/DiskDecoder.h"

namespace retro::vault {

class DOSDecoder : public DiskDecoder {

    static constexpr u64 IDAM_SYNC = u64(0x4489448944895554);
    static constexpr u64 DAM_SYNC  = u64(0x4489448944895545);

    static constexpr isize bsize   = 512;  // Block size in bytes
    static constexpr isize ssize   = 1300; // MFM sector size in bytes
    static constexpr isize maxsec  = 22;   // Maximum number of sectors


    //
    // Methods from DiskDecoder
    //

public:

    using DiskDecoder::decodeTrack;
    using DiskDecoder::decodeSector;

    utl::ByteView decodeTrack(utl::BitView track, TrackNr t, std::span<u8> out) override;
    utl::ByteView decodeSector(utl::BitView track, TrackNr t, SectorNr s, std::span<u8> out) override;

private:

    optional<utl::Range<isize>> seekSector(utl::BitView track, SectorNr s, isize offset = 0) override;
    std::unordered_map<isize, utl::Range<isize>> seekSectors(utl::BitView track) override;

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

    std::unordered_map<SectorNr, utl::Range<isize>> seekSectors(utl::BitView track,
                                                           std::span<const SectorNr> wanted,
                                                           isize offset = 0);
};

}
