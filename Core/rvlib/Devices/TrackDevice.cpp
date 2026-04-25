// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "TrackDevice.h"
#include "DeviceError.h"
#include <algorithm>

namespace retro::vault {

using CHS = TrackDevice::CHS;
using TS  = TrackDevice::TS;

void
TrackDevice::buildTrackMap() const
{
    track2block.clear();
    track2block.reserve(numTracks());

    for (isize t = 0, offset = 0; t < numTracks(); ++t) {

        track2block.push_back(offset);
        offset += numSectors(t);
    }
}

isize
TrackDevice::block2track(isize b) const
{
    assert(0 <= b && b < capacity());

    if (track2block.empty()) buildTrackMap();

    // Find the track via binary search
    auto it = std::upper_bound(track2block.begin(), track2block.end(), b);
    return isize(std::distance(track2block.begin(), it) - 1);
}

bool
TrackDevice::isValidBlockNr(isize b) const noexcept
{
    return b >= 0 && b < numBlocks();
}

bool
TrackDevice::isValidTrackNr(isize t) const noexcept
{
    return t >= 0 && t < numTracks();
}

bool
TrackDevice::isValidSectorNr(isize t, isize s) const noexcept
{
    return isValidTrackNr(t) && s >= 0 && s < numSectors(t);
}

bool
TrackDevice::isValidCylinderNr(isize c) const noexcept
{
    return c >= 0 && c < numCyls();
}

bool
TrackDevice::isValidHeadNr(isize h) const noexcept
{
    return h >= 0 && h < numHeads();
}

bool
TrackDevice::isValidTS(TS ts) const noexcept
{
    return isValidSectorNr(ts.track, ts.sector);
}

bool
TrackDevice::isValidCHS(CHS chs) const noexcept
{
    return isValidCylinderNr(chs.cylinder) &&
           isValidHeadNr(chs.head) &&
           isValidSectorNr(chs.cylinder * numHeads() + chs.head, chs.sector);
}

//
// Throwing validators
//

void
TrackDevice::validateBlockNr(isize b) const
{
    if (!isValidBlockNr(b))
        throw DeviceError(DeviceError::INVALID_BLOCK_NR, b);
}

void
TrackDevice::validateTrackNr(isize t) const
{
    if (!isValidTrackNr(t))
        throw DeviceError(DeviceError::INVALID_TRACK_NR, t);
}

void
TrackDevice::validateSectorNr(isize t, isize s) const
{
    if (!isValidSectorNr(t, s))
        throw DeviceError(DeviceError::INVALID_SECTOR_NR, s);
}

void
TrackDevice::validateCylinderNr(isize c) const
{
    if (!isValidCylinderNr(c))
        throw DeviceError(DeviceError::INVALID_CYLINDER_NR, c);
}

void
TrackDevice::validateHeadNr(isize h) const
{
    if (!isValidHeadNr(h))
        throw DeviceError(DeviceError::INVALID_HEAD_NR, h);
}

void
TrackDevice::validateTS(TS ts) const
{
    validateSectorNr(ts.track, ts.sector);
}

void
TrackDevice::validateCHS(CHS chs) const
{
    validateCylinderNr(chs.cylinder);
    validateHeadNr(chs.head);
    validateSectorNr(chs.cylinder * numHeads() + chs.head, chs.sector);
}

CHS
TrackDevice::b2chs(isize b) const
{
    auto t = block2track(b);
    auto c = t / numHeads();
    auto h = t % numHeads();
    auto s = b - track2block[t];

    return { c, h, s };
}

CHS
TrackDevice::ts2chs(isize t, isize s) const
{
    assert(0 <= t && t < numTracks());
    assert(0 <= s && s < numSectors(t));

    auto c = t / numHeads();
    auto h = t % numHeads();

    return { c, h, s };
}

TS
TrackDevice::b2ts(isize b) const
{
    assert(0 <= b && b < capacity());

    auto t = block2track(b);
    auto s = b - track2block[t];

    return { t, s };
}

TS
TrackDevice::chs2ts(isize c, isize h, isize s) const
{
    auto t = c * numHeads() + h;

    assert(0 <= t && t < numTracks());
    assert(0 <= s && s < numSectors(t));

    return { t, s };
}

isize
TrackDevice::bindex(const CHS &chs) const
{
    return bindex(TS {chs.cylinder * numHeads() + chs.head, chs.sector});
}

isize
TrackDevice::bindex(const TS &ts) const
{
    assert(0 <= ts.track && ts.track < numTracks());
    assert(0 <= ts.sector && ts.sector < numSectors(ts.track));

    if (track2block.empty()) buildTrackMap();

    return track2block[ts.track] + ts.sector;
}

void
TrackDevice::readTrack(u8 *dst, isize nr) const
{
    auto blocks = numSectors(nr);
    auto *ptr   = dst;

    for (isize i = 0; i < blocks; ++i, ptr += bsize())
        readBlock(ptr, i);
}

void
TrackDevice::readTrack(span<u8> dst, isize nr) const
{
    assert((isize)dst.size() >= numSectors(nr) * bsize());
    readTrack(dst.data(), nr);
}

void
TrackDevice::writeTrack(const u8 *src, isize nr)
{
    auto blocks = numSectors(nr);
    auto *ptr   = src;

    for (isize i = 0; i < blocks; ++i, ptr += bsize())
        writeBlock(ptr, i);
}

void
TrackDevice::writeTrack(span<const u8> src, isize nr)
{
    assert((isize)src.size() >= numSectors(nr) * bsize());
    writeTrack(src.data(), nr);
}

}
