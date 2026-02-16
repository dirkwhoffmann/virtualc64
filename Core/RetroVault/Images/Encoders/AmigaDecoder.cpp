// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "AmigaDecoder.h"
#include "DeviceError.h"
#include "MFM.h"
#include "utl/support/Bits.h"
#include <unordered_set>

namespace retro::vault {

static constexpr isize bsize  = 512;

ByteView
AmigaDecoder::decodeTrack(BitView track, TrackNr t, std::span<u8> out)
{
    loginfo(IMG_DEBUG, "Decoding Amiga track %ld\n", t);

    // Find all sectors
    auto sectors    = seekSectors(track);
    auto numSectors = isize(sectors.size());

    // Ensure the output buffer is large enough
    assert(isize(out.size()) >= numSectors * bsize);

    // Iterator through all sectors
    for (isize s = 0; s < numSectors; ++s) {

        if (!sectors.contains(s))
            throw DeviceError(DeviceError::SEEK_ERR);

        auto it = track.cyclic_begin() + sectors[s].lower;

        // Read sector data
        assert(sectors[s].size() == 1024*8);
        u8 mfm[1024]; for (isize i = 0; i < 1024; ++i) mfm[i] = it.readByte();

        // Decode data
        MFM::decodeOddEven(out.data() + s * bsize, mfm, bsize);
    }

    return ByteView(out.data(), numSectors * bsize);
}

ByteView
AmigaDecoder::decodeSector(BitView track, TrackNr t, SectorNr s, std::span<u8> out)
{
    loginfo(IMG_DEBUG, "Decoding Amiga sector %ld:%ld\n", t, s);

    // Ensure the output buffer is large enough
    assert(isize(out.size()) >= bsize);

    // Find sector
    auto sector = seekSector(track, s);

    if (!sector.has_value())
        throw DeviceError(DeviceError::SEEK_ERR);

    auto it = track.cyclic_begin() + sector->lower;

    // Read sector data
    assert(sector->size() == 1024*8);
    u8 mfm[1024]; for (isize i = 0; i < 1024; ++i) mfm[i] = it.readByte();

    // Decode data
    MFM::decodeOddEven(out.data(), mfm, bsize);

    return ByteView(out);
}

optional<Range<isize>>
AmigaDecoder::seekSector(BitView track, SectorNr s, isize offset)
{
    auto map = seekSectors(track, std::vector<SectorNr>{s}, offset);

    if (!map.contains(s))
        throw DeviceError(DeviceError::INVALID_SECTOR_NR);

    return map[s];
}

std::unordered_map<isize, Range<isize>>
AmigaDecoder::seekSectors(BitView track)
{
    return seekSectors(track, std::vector<SectorNr>{});
}

std::unordered_map<SectorNr, Range<isize>>
AmigaDecoder::seekSectors(BitView track, std::span<const SectorNr> wanted, isize offset)
{
    static constexpr u64 SYNC = u64(0x44894489);

    std::unordered_map<SectorNr, Range<isize>> result;
    std::unordered_set<SectorNr> visited;

    // Loop until a sector header repeats or no sync marks are found
    for (auto it = track.cyclic_begin(offset);;) {

        // Move behind the next sync mark
        if (!track.forward(it, SYNC, 32)) throw DeviceError(DeviceError::SEEK_ERR);

        // Read the next 8 MFM bytes
        u8 mfm[8]; for (isize i = 0; i < 8; ++i) mfm[i] = it.readByte();

        // Make sure it's not a DOS track
        if (mfm[1] == 0x89) continue;

        // Decode track & sector info
        u8 info[4]; MFM::decodeOddEven(info, mfm, 4);

        // The sector number is encoded in the third byte
        u8 s = info[2];

        // Break the loop if we've seen this sector before
        if (!visited.insert(s).second) break;

        // If the sector is requested...
        if (wanted.empty() || std::find(wanted.begin(), wanted.end(), s) != wanted.end()) {

            // Record the sector number
            result[s] = Range<isize>(it.offset() + 48 * 8,
                                     it.offset() + 48 * 8 + 1024 * 8);

            // Check for early exit
            if (!wanted.empty() && result.size() == wanted.size()) break;
        }
    }
    return result;
}

}
