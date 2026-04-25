// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64Encoder.h"
#include "DeviceError.h"
#include "GCR.h"
#include "utl/support/Bits.h"

/* Notes on tracks vs. halftracks
 *
 * In Commodore 64 disk terminology, track numbers start at 1, not 0.
 * Track 1 is the first physical track on the disk. VC1541 drives can
 * position the head not only on full tracks but also in between. These
 * intermediate positions are called halftracks and are typically
 * addressed as x.5 (e.g. 1.5, 2.5). Halftracks are mainly relevant for
 * copy-protected disks, which may store data at non-standard head positions.
 *
 * All functions that take a track number of argument index the first track
 * with 0. Thus, the following mapping applies:
 *
 * TrackNr      0     1     2     3           68     69           82     83
 *            ---------------------------------------------------------------
 * C64 Track |  1  | 1.5 |  2  | 2.5 | ... |  35  | 35.5 | ... |  42  | 42.5 |
 *            ---------------------------------------------------------------
 */

namespace retro::vault {

namespace Encoder { C64Encoder c64; }

const C64Encoder::TrackDefaults &
C64Encoder::trackDefaults(isize t)
{
    static constexpr TrackDefaults trackDefaults[42] = {

        // Speedzone 3 (outer tracks)                   // Index    Track   Halftracks
        { 21, 3, 7693, 7693 * 8,   0,  8, 0.268956 },   // 0,       1,      1,  2
        { 21, 3, 7693, 7693 * 8,  21,  8, 0.724382 },   // 1,       2,      3,  4
        { 21, 3, 7693, 7693 * 8,  42,  8, 0.177191 },   // 2,       3,      5,  6
        { 21, 3, 7693, 7693 * 8,  63,  8, 0.632698 },   // 3,       4,      7,  8
        { 21, 3, 7693, 7693 * 8,  84,  8, 0.088173 },   // 4,       5,      9,  10
        { 21, 3, 7693, 7693 * 8, 105,  8, 0.543583 },   // 5,       6,      11, 12
        { 21, 3, 7693, 7693 * 8, 126,  8, 0.996409 },   // 6,       7,      13, 14
        { 21, 3, 7693, 7693 * 8, 147,  8, 0.451883 },   // 7,       8,      15, 16
        { 21, 3, 7693, 7693 * 8, 168,  8, 0.907342 },   // 8,       9,      17, 18
        { 21, 3, 7693, 7693 * 8, 289,  8, 0.362768 },   // 9,       10,     19, 20
        { 21, 3, 7693, 7693 * 8, 210,  8, 0.815512 },   // 10,      11,     21, 22
        { 21, 3, 7693, 7693 * 8, 231,  8, 0.268338 },   // 11,      12,     23, 24
        { 21, 3, 7693, 7693 * 8, 252,  8, 0.723813 },   // 12,      13,     25, 26
        { 21, 3, 7693, 7693 * 8, 273,  8, 0.179288 },   // 13,      14,     27, 28
        { 21, 3, 7693, 7693 * 8, 294,  8, 0.634779 },   // 14,      15,     29, 30
        { 21, 3, 7693, 7693 * 8, 315,  8, 0.090253 },   // 15,      16,     31, 32
        { 21, 3, 7693, 7693 * 8, 336,  8, 0.545712 },   // 16,      17,     33, 34

        // Speedzone 2
        { 19, 2, 7143, 7143 * 8, 357, 17, 0.945418 },   // 17,      18,     35, 36
        { 19, 2, 7143, 7143 * 8, 376, 17, 0.506081 },   // 18,      19,     37, 38
        { 19, 2, 7143, 7143 * 8, 395, 17, 0.066622 },   // 19,      20,     39, 40
        { 19, 2, 7143, 7143 * 8, 414, 17, 0.627303 },   // 20,      21,     41, 42
        { 19, 2, 7143, 7143 * 8, 433, 17, 0.187862 },   // 21,      22,     43, 44
        { 19, 2, 7143, 7143 * 8, 452, 17, 0.748403 },   // 22,      23,     45, 46
        { 19, 2, 7143, 7143 * 8, 471, 17, 0.308962 },   // 23,      24,     47, 48

        // Speedzone 1
        { 18, 1, 6667, 6667 * 8, 490, 12, 0.116926 },   // 24,      25,     49, 50
        { 18, 1, 6667, 6667 * 8, 508, 12, 0.788086 },   // 25,      26,     51, 52
        { 18, 1, 6667, 6667 * 8, 526, 12, 0.459190 },   // 26,      27,     53, 54
        { 18, 1, 6667, 6667 * 8, 544, 12, 0.130238 },   // 27,      28,     55, 56
        { 18, 1, 6667, 6667 * 8, 562, 12, 0.801286 },   // 28,      29,     57, 58
        { 18, 1, 6667, 6667 * 8, 580, 12, 0.472353 },   // 29,      30,     59, 60

        // Speedzone 0 (inner tracks)
        { 17, 0, 6250, 6250 * 8, 598,  9, 0.834120 },   // 30,      31,     61, 62
        { 17, 0, 6250, 6250 * 8, 615,  9, 0.614880 },   // 31,      32,     63, 64
        { 17, 0, 6250, 6250 * 8, 632,  9, 0.395480 },   // 32,      33,     65, 66
        { 17, 0, 6250, 6250 * 8, 649,  9, 0.176140 },   // 33,      34,     67, 68
        { 17, 0, 6250, 6250 * 8, 666,  9, 0.956800 },   // 34,      35,     69, 70

        // Speedzone 0 (usually unused tracks)
        { 17, 0, 6250, 6250 * 8, 683,  9, 0.300 },      // 35,      36,     71, 72
        { 17, 0, 6250, 6250 * 8, 700,  9, 0.820 },      // 36,      37,     73, 74
        { 17, 0, 6250, 6250 * 8, 717,  9, 0.420 },      // 37,      38,     75, 76
        { 17, 0, 6250, 6250 * 8, 734,  9, 0.940 },      // 38,      39,     77, 78
        { 17, 0, 6250, 6250 * 8, 751,  9, 0.540 },      // 39,      40,     79, 80
        { 17, 0, 6250, 6250 * 8, 768,  9, 0.130 },      // 40,      41,     81, 82
        { 17, 0, 6250, 6250 * 8, 785,  9, 0.830 }       // 41,      42,     83, 84
    };

    assert(t >= 0 && t < 42);
    return trackDefaults[t];
}

BitView
C64Encoder::encodeTrack(ByteView src, TrackNr t)
{
    assert(src.size() % 256 == 0);

    // Determine the number of sectors to encode
    const isize count = (isize)src.size() / 256;

    if (count != trackDefaults(t).sectors)
        throw DeviceError(DeviceError::DSK_WRONG_SECTOR_CNT);

    loginfo(IMG_DEBUG, "Encoding C64 track %ld with %ld sectors\n", t, count);

    // Setup the backing buffer
    if (gcrbuffer.empty()) gcrbuffer.resize(16384);

    // Create a bit view with proper length
    auto &defaults = trackDefaults(t);
    auto view = MutableBitView(gcrbuffer, defaults.lengthInBits);

    // Format the track
    view.byteView().clear(0x55);

    // Compute start position inside the bit view
    // auto offset = isize(view.size() * defaults.stagger);
    auto offset = 0;

    // For each sector in this track ...
    isize totalBits = 0;
    for (SectorNr s = 0; s < defaults.sectors; ++s) {

        // Create a view on the sector data
        ByteView sview(src.subspan(s * 256, 256));

        // Encode the sector
        isize encodedBits = encodeSector(view, offset, t, s, sview);
        offset += encodedBits;
        totalBits += encodedBits;
    }

    if constexpr (debug::IMG_DEBUG) {

        loginfo(IMG_DEBUG,
                "\nTrack size: %ld Encoded: %ld Checksum: %x\n",
                view.size(), totalBits, view.byteView().fnv32());
    }

    // for (isize i = 0; i < 64; ++i) printf("%02X ", gcrbuffer[i]);
    // printf("\n");
    
    return view;
}

BitView
C64Encoder::encodeSector(ByteView bytes, TrackNr t, SectorNr s)
{
    return BitView(nullptr, 0);
}

isize
C64Encoder::encodeSector(MutableBitView view,
                         isize offset, TrackNr t, SectorNr s, ByteView src)
{
    loginfo(IMG_DEBUG, "%ld (%ld) ", s, offset);

    BlockNr b = trackDefaults(t).firstSectorNr + s;

    auto head      = offset;
    auto &defaults = trackDefaults(t);
    auto errorCode = size_t(b) < ecc.size() ? ecc[b] : 0;

    // Compute disk id checksum
    u8 checksum = (u8)(id1 ^ id2 ^ (t + 1) ^ s); // Header checksum byte

    // SYNC (0xFF 0xFF 0xFF 0xFF 0xFF)
    if (errorCode == 0x3) {
        view.setBytes(head, std::vector<u8>(5, 0x00)); // HEADER_CHECKSUM_ERROR
    } else {
        view.setBytes(head, std::vector<u8>(5, 0xFF));
    }
    head += 40;

    // Header ID
    if (errorCode == 0x2) {
        GCR::encodeGcr(view, head, 0x00); // HEADER_BLOCK_NOT_FOUND_ERROR
    } else {
        GCR::encodeGcr(view, head, 0x08);
    }
    head += 10;

    // Checksum
    if (errorCode == 0x9) {
        GCR::encodeGcr(view, head, checksum ^ 0xFF); // HEADER_BLOCK_CHECKSUM_ERROR
    } else {
        GCR::encodeGcr(view, head, checksum);
    }
    head += 10;

    // Sector and track number
    GCR::encodeGcr(view, head, u8(s));
    head += 10;
    GCR::encodeGcr(view, head, u8(t + 1));
    head += 10;

    // Disk ID (two bytes)
    if (errorCode == 0xB) {
        GCR::encodeGcr(view, head, id2 ^ 0xFF); // DISK_ID_MISMATCH_ERROR
        head += 10;
        GCR::encodeGcr(view, head, id1 ^ 0xFF); // DISK_ID_MISMATCH_ERROR
    } else {
        GCR::encodeGcr(view, head, id2);
        head += 10;
        GCR::encodeGcr(view, head, id1);
    }
    head += 10;

    // 0x0F, 0x0F
    GCR::encodeGcr(view, head, 0x0F);
    head += 10;
    GCR::encodeGcr(view, head, 0x0F);
    head += 10;

    // 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55 0x55
    view.setBytes(head, std::vector<u8>(9, 0x55));
    // writeGapToTrack(t, offset, 9);
    head += 9 * 8;

    // SYNC (0xFF 0xFF 0xFF 0xFF 0xFF)
    if (errorCode == 3) {
        view.setBytes(head, std::vector<u8>(5, 0x00)); // NO_SYNC_SEQUENCE_ERROR
    } else {
        view.setBytes(head, std::vector<u8>(5, 0xFF));
    }
    head += 40;

    // Data block ID
    if (errorCode == 0x4) {
        // The error value is important here:
        // (1) If the first GCR bit equals 0, the sector can still be read.
        // (2) If the first GCR bit equals 1, the SYNC sequence continues.
        //     In this case, the bit sequence gets out of sync and the data
        //     can't be read.
        // Hoxs64 and VICE 3.2 write 0x00 which results in option (1)
        GCR::encodeGcr(view, head, 0x00); // DATA_BLOCK_NOT_FOUND_ERROR
    } else {
        GCR::encodeGcr(view, head, 0x07);
    }
    head += 10;

    // Data bytes
    checksum = 0;
    for (isize i = 0; i < 256; i++, head += 10) {

        u8 byte = src[i];
        checksum ^= byte;
        GCR::encodeGcr(view, head, byte);
    }

    // Checksum
    if (errorCode == 0x5) {
        GCR::encodeGcr(view, head, checksum ^ 0xFF); // DATA_BLOCK_CHECKSUM_ERROR
    } else {
        GCR::encodeGcr(view, head, checksum);
    }
    head += 10;

    // 0x00, 0x00
    GCR::encodeGcr(view, head, 0x00);
    head += 10;
    GCR::encodeGcr(view, head, 0x00);
    head += 10;

    // Tail gap (0x55 0x55 ... 0x55)
    view.setBytes(head, std::vector<u8>(defaults.tailGap, 0x55));
    head += defaults.tailGap * 8;

    // Return the number of encoded bits
    return head - offset;
}

}
