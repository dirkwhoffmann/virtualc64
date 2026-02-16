// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "AmigaEncoder.h"
#include "DeviceError.h"
#include "MFM.h"
#include "utl/support/Bits.h"

namespace retro::vault {

static constexpr isize bsize  = 512;  // Block size in bytes
static constexpr isize ssize  = 1088; // MFM sector size in bytes

BitView
AmigaEncoder::encodeTrack(ByteView src, TrackNr t)
{
    assert(src.size() % bsize == 0);

    // Determine the number of sectors to encode
    const isize count = (isize)src.size() / bsize;

    if (count != 11 && count != 22)
        throw DeviceError(DeviceError::DSK_WRONG_SECTOR_CNT);

    loginfo(IMG_DEBUG, "Encoding Amiga track %ld with %ld sectors\n", t, count);

    // Start with a clean track
    if (trackBuffer.empty()) trackBuffer.resize(16384, 0xAA);

    // Create views
    auto bitView = MutableBitView(trackBuffer.data(), count * ssize * 8);
    auto view = bitView.byteView();

    for (SectorNr s = 0; s < count; s++) {

        // Encode sector
        auto mfm = encodeSector(ByteView(src.subspan(s * bsize, bsize)), t, s);
        assert(mfm.size() == 8 * ssize);

        // Copy data onto the track
        memcpy(view.data() + s * ssize, mfm.data(), ssize);

        // Rectify the clock bits at the border
        rectifyClockBit(bitView, 8 * ssize * s);
        rectifyClockBit(bitView, 8 * ssize * (s + 1));
    }

    // Compute a debug checksum
    loginfo(IMG_DEBUG, "Track %ld checksum = %x\n", t, view.fnv32());

    return BitView(view.data(), view.size() * 8);
}

BitView
AmigaEncoder::encodeSector(ByteView bytes, TrackNr t, SectorNr s)
{
    assert(bytes.size() == bsize);

    loginfo(IMG_DEBUG, "Encoding Amiga sector %ld:%ld\n", t, s);

    // Start with a clean track
    if (sectorBuffer.empty()) sectorBuffer.resize(ssize, 0xAA);

    // Block header layout:
    //
    //                         Start  Size   Value
    //     Bytes before SYNC   00      4     0xAA 0xAA 0xAA 0xAA
    //     SYNC mark           04      4     0x44 0x89 0x44 0x89
    //     Track & sector info 08      8     Odd/Even encoded
    //     Unused area         16     32     0xAA
    //     Block checksum      48      8     Odd/Even encoded
    //     Data checksum       56      8     Odd/Even encoded

    auto view = MutableByteView(sectorBuffer);
    auto it = view.begin();

    // Bytes before SYNC
    it[0] = 0xAA; // (it[-1] & 1) ? 0x2A : 0xAA;
    it[1] = 0xAA;
    it[2] = 0xAA;
    it[3] = 0xAA;

    // SYNC mark
    u16 sync = 0x4489;
    it[4] = HI_BYTE(sync);
    it[5] = LO_BYTE(sync);
    it[6] = HI_BYTE(sync);
    it[7] = LO_BYTE(sync);

    // Track and sector information
    u8 info[4] = { 0xFF, (u8)t, (u8)s, (u8)(11 - s) };
    MFM::encodeOddEven(&it[8], info, sizeof(info));

    // Unused area
    for (isize i = 16; i < 48; i++)
        it[i] = 0xAA;

    // Data
    MFM::encodeOddEven(&it[64], bytes.data(), bsize);

    // Block checksum
    u8 bcheck[4] = { 0, 0, 0, 0 };
    for(isize i = 8; i < 48; i += 4) {
        bcheck[0] ^= it[i];
        bcheck[1] ^= it[i+1];
        bcheck[2] ^= it[i+2];
        bcheck[3] ^= it[i+3];
    }
    MFM::encodeOddEven(&it[48], bcheck, sizeof(bcheck));

    // Data checksum
    u8 dcheck[4] = { 0, 0, 0, 0 };
    for(isize i = 64; i < ssize; i += 4) {
        dcheck[0] ^= it[i];
        dcheck[1] ^= it[i+1];
        dcheck[2] ^= it[i+2];
        dcheck[3] ^= it[i+3];
    }
    MFM::encodeOddEven(&it[56], dcheck, sizeof(dcheck));

    // Add clock bits
    // for(isize i = 8; i < ssize + 1; i++) {
    for(isize i = 8; i < ssize; i++) {
        it[i] = MFM::addClockBits(it[i], it[i-1]);
    }

    return BitView(view.data(), 8 * view.size());
}

void
AmigaEncoder::rectifyClockBit(MutableBitView view, isize offset)
{
    auto it = view.cyclic_begin(offset);
    view.set(offset, it[-1] || it[1] ? 0 : 1);
}

}
