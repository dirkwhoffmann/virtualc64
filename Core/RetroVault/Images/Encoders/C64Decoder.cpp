// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64Decoder.h"
#include "DeviceError.h"
#include "D64File.h"
#include "GCR.h"
#include "utl/support/Bits.h"
#include <unordered_set>

namespace retro::vault {

using image::D64File;

static constexpr isize bsize  = 256;

struct SYNC
{
    isize ones = 0;

    bool sync(bool bit) {

        bool result = ones == 50;
        ones = bit ? ones + 1 : 0;
        return result;
    }
};

ByteView
C64Decoder::decodeTrack(BitView track, TrackNr t, std::span<u8> out)
{
    loginfo(IMG_DEBUG, "Decoding C64 track %ld\n", t);

    // Find all sectors
    auto sectors = seekSectors(track);
    auto numSectors = isize(sectors.size());
    assert(numSectors == D64File::trackDefaults(t).sectors);

    // Ensure the output buffer is large enough
    assert(isize(out.size()) >= numSectors * bsize);

    // Iterate through all sectors
    for (isize s = 0; s < numSectors; ++s) {

        if (!sectors.contains(s))
            throw DeviceError(DeviceError::SEEK_ERR);

        // Decode data
        for (isize i = 0, offset = sectors[s].lower; i < bsize; ++i, offset += 10)
            out[s * bsize + i] = GCR::decodeGcr(track, offset);
    }

    return ByteView(out.data(), numSectors * bsize);
}

ByteView
C64Decoder::decodeSector(BitView track, TrackNr t, SectorNr s, std::span<u8> out)
{
    loginfo(IMG_DEBUG, "Decoding C64 sector %ld:%ld\n", t, s);

    // Ensure the output buffer is large enough
    assert(isize(out.size()) >= bsize);

    // Find sector
    auto sector = seekSector(track, s);

    if (!sector.has_value())
        throw DeviceError(DeviceError::SEEK_ERR);

    // Decode data
    for (isize i = 0, offset = sector->lower; i < bsize; ++i, offset += 10)
        out[i] = GCR::decodeGcr(track, offset);

    return ByteView(out.data(), bsize);
}

bool
C64Decoder::seekSync(BitView track, BitView::cyclic_iterator &it)
{
    for (isize i = 0, ones = 0; i < track.size() + 40; ++i, ++it) {

        if (it[0] == 0 && ones >= 40)
            return true;

        ones = it[0] == 1 ? ones + 1 : 0;
    }

    return false;
}

bool
C64Decoder::seekHeaderSync(BitView track, BitView::cyclic_iterator &it)
{
    for (isize i = 0, ones = 0; i < track.size() + 40; ++i, ++it) {

        if (it[0] == 0 && ones >= 40) {

            // $08 indicates a header block
            if (auto id = GCR::decodeGcr(track, it.offset()); id == 0x08) {
                return true;
            }
        }
        ones = it[0] == 1 ? ones + 1 : 0;
    }

    return false;
}

optional<Range<isize>>
C64Decoder::seekSector(BitView track, SectorNr s, isize offset)
{
    auto map = seekSectors(track, std::span<const SectorNr>(&s, 1), offset);

    if (auto it = map.find(s); it != map.end())
        return it->second;

    return std::nullopt;
}

std::unordered_map<SectorNr, Range<isize>>
C64Decoder::seekSectors(BitView track)
{
    return seekSectors(track, std::vector<SectorNr>());
}

std::unordered_map<SectorNr, Range<isize>>
C64Decoder::seekSectors(BitView track, std::span<const SectorNr> wanted, isize offset)
{
    std::unordered_set<SectorNr> visited;
    std::unordered_map<SectorNr, Range<isize>> result;

    // Loop until a sector header repeats or no sync marks are found
    for (auto it = track.cyclic_begin(offset);;) {

        // Move to the next header block
        if (!seekHeaderSync(track, it)) break;

        // Skip id and checksum
        it += 2 * GCR::bitsPerByte;

        // The next byte stores the sector number
        SectorNr nr = GCR::decodeGcr(track, it.offset());

        // Break the loop if we've seen this sector before
        if (!visited.insert(nr).second) break;

        // Record the sector if requested
        if (wanted.empty() || std::find(wanted.begin(), wanted.end(), nr) != wanted.end()) {

            // Move beyond the next sync mark
            if (!seekSync(track, it)) break;

            // The next byte contains the block id
            if (auto id = GCR::decodeGcr(track, it.offset()); id == 0x07) {

                // Skip id
                it += GCR::bitsPerByte;

                // At this point, the offset references the first data bit
                result[nr] = Range<isize>(it.offset(),
                                          it.offset() + GCR::bitsPerByte * bsize);

                // Check for early exit
                if (!wanted.empty() && result.size() == wanted.size()) break;
            }
        }
    }
    return result;
}

}
