// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DOSDecoder.h"
#include "DeviceError.h"
#include "MFM.h"
#include <unordered_set>

namespace retro::vault {

ByteView
DOSDecoder::decodeTrack(BitView track, TrackNr t, std::span<u8> out)
{
    loginfo(IMG_DEBUG, "Decoding DOS track %ld\n", t);

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
        MFM::decodeMFM(out.data() + s * bsize, mfm, bsize);
    }

    return ByteView(out.data(), numSectors * bsize);
}

ByteView
DOSDecoder::decodeSector(BitView track, TrackNr t, SectorNr s, std::span<u8> out)
{
    loginfo(IMG_DEBUG, "Decoding DOS track %ld:%ld\n", t, s);

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
    MFM::decodeMFM(out.data(), mfm, bsize);

    return ByteView(out);
}

optional<Range<isize>>
DOSDecoder::seekSector(BitView track, SectorNr s, isize offset)
{
    auto map = seekSectors(track, std::vector<SectorNr>{s}, offset);

    if (!map.contains(s))
        throw DeviceError(DeviceError::INVALID_SECTOR_NR);

    return map[s];
}

std::unordered_map<isize, Range<isize>>
DOSDecoder::seekSectors(BitView track)
{
    return seekSectors(track, std::vector<SectorNr>{});
}

std::unordered_map<SectorNr, Range<isize>>
DOSDecoder::seekSectors(BitView track, std::span<const SectorNr> wanted, isize offset)
{
    std::unordered_map<SectorNr, Range<isize>> result;
    std::unordered_set<SectorNr> visited;

    // Loop until a sector header repeats or no sync marks are found
    for (auto it = track.cyclic_begin(offset);;) {

        // Move behind the next IDAM sync mark
        if (!track.forward(it, IDAM_SYNC, 64))
            throw DeviceError(DeviceError::SEEK_ERR);

        // Read the next 8 MFM bytes
        u8 mfm[8]; for (isize i = 0; i < 8; ++i) mfm[i] = it.readByte();

        // Decode the CHRN block
        u8 info[4]; MFM::decodeMFM(info, mfm, 4);

        // The sector number is encoded in the third byte, counting 1,2,...
        u8 s = info[2] - 1;

        // Break the loop if we've seen this sector before
        if (!visited.insert(s).second) break;

        // If the sector is requested...
        if (wanted.empty() || std::find(wanted.begin(), wanted.end(), s) != wanted.end()) {

            // Move behind the next IDAM sync mark
            if (!track.forward(it, DAM_SYNC, 64))
                throw DeviceError(DeviceError::SEEK_ERR);

            // Record the sector number
            result[s] = Range<isize>(it.offset(), it.offset() + 1024 * 8);

            // Check for early exit
            if (!wanted.empty() && result.size() == wanted.size()) break;
        }
    }
    return result;
}

}
