// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DOSEncoder.h"
#include "DeviceError.h"
#include "MFM.h"
#include "utl/support/Bits.h"
#include <array>

namespace retro::vault {

namespace Encoder { DOSEncoder ibm; }

BitView
DOSEncoder::encodeTrack(ByteView src, TrackNr t)
{
    // Determine the number of sectors to encode
    const isize count = (isize)src.size() / bsize;
    if (count >= maxsec) throw DeviceError(DeviceError::DSK_WRONG_SECTOR_CNT);

    loginfo(IMG_DEBUG, "Encoding DOS track %ld with %ld sectors\n", t, count);
    assert(src.size() % bsize == 0);

    // Start with a clean track
    auto view = MutableByteView(mfmBuffer, count * ssize);
    view.clear(0xAA);

    // Encode all sectors
    for (SectorNr s = 0; s < count; s++)
        encodeSector(view, s * ssize, t, s, ByteView(src.subspan(s * bsize, bsize)));

    // Compute a debug checksum
    loginfo(IMG_DEBUG, "Track %ld checksum = %x\n", t, view.fnv32());
    
    return BitView(view.data(), view.size() * 8);
}

BitView
DOSEncoder::encodeSector(ByteView bytes, TrackNr t, SectorNr s)
{
    return BitView(nullptr, 0);
}

/*
void
DOSEncoder::encodeTrack(MutableByteView track, TrackNr t, ByteView src)
{
    const isize bsize = 512;                       // Block size in bytes
    const isize ssize = 1300;                      // MFM sector size in bytes
    const isize count = (isize)src.size() / bsize; // Number of sectors to encode

    loginfo(IMG_DEBUG, "Encoding DOS track %ld with %ld sectors\n", t, count);
    assert(src.size() % bsize == 0);

    // Format track
    track.clear( std::array<u8,2> { 0x92, 0x54 } );

    auto p = track.cyclic_begin();

    // Encode track header
    p = p + 82;                                         // GAP
    for (isize i = 0; i < 24; i++) { p[i] = 0xAA; }     // SYNC
    p = p + 24;
    p[0] = 0x52; p[1] = 0x24;                           // IAM
    p[2] = 0x52; p[3] = 0x24;
    p[4] = 0x52; p[5] = 0x24;
    p[6] = 0x55; p[7] = 0x52;
    p = p + 8;
    p = p + 80;                                         // GAP

    // Encode all sectors
    for (SectorNr s = 0; s < count; s++)
        encodeSector(track, 194 + s * ssize, t, s, ByteView(src.subspan(s * bsize, bsize)));

    // Compute a debug checksum
    loginfo(IMG_DEBUG, "Track %ld checksum = %x\n", t, track.fnv32());
}
*/

void
DOSEncoder::encodeSector(MutableByteView track, isize offset, TrackNr t, SectorNr s, ByteView data)
{
    const isize bsize = 512;   // Block size in bytes

    loginfo(IMG_DEBUG, "Encoding sector %ld\n", s);
    assert(data.size() == bsize);

    u8 buf[60 + 512 + 2 + 109]; // Header + Data + CRC + Gap

    // Write SYNC
    for (isize i = 0; i < 12; ++i) { buf[i] = 0x00; }

    // Write IDAM
    buf[12] = 0xA1;
    buf[13] = 0xA1;
    buf[14] = 0xA1;
    buf[15] = 0xFE;

    // Write CHRN
    buf[16] = (u8)(t / 2);
    buf[17] = (u8)(t % 2);
    buf[18] = (u8)(s + 1);
    buf[19] = 2;

    // Compute and write CRC
    u16 crc = Hashable::crc16(&buf[12], 8);
    buf[20] = HI_BYTE(crc);
    buf[21] = LO_BYTE(crc);

    // Write GAP
    for (isize i = 22; i < 44; ++i) { buf[i] = 0x4E; }

    // Write SYNC
    for (isize i = 44; i < 56; ++i) { buf[i] = 0x00; }

    // Write DATA AM
    buf[56] = 0xA1;
    buf[57] = 0xA1;
    buf[58] = 0xA1;
    buf[59] = 0xFB;

    // Write DATA
    for (isize i = 0; i < bsize; ++i) { buf[60 + i] = data[i]; }

    // Compute and write CRC
    crc = Hashable::crc16(&buf[56], 516);
    buf[572] = HI_BYTE(crc);
    buf[573] = LO_BYTE(crc);

    // Write GAP
    for (usize i = 574; i < sizeof(buf); ++i) { buf[i] = 0x4E; }

    // Compute the start of this sector
    u8 *p = track.data() + offset;

    // Create the MFM data stream
    MFM::encodeMFM(p, buf, sizeof(buf));
    MFM::addClockBits(p, 2 * sizeof(buf));

    // Remove certain clock bits in IDAM block
    p[2*12+1] &= 0xDF;
    p[2*13+1] &= 0xDF;
    p[2*14+1] &= 0xDF;

    // Remove certain clock bits in DATA AM block
    p[2*56+1] &= 0xDF;
    p[2*57+1] &= 0xDF;
    p[2*58+1] &= 0xDF;
}

}
