// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DiskEncoder.h"

namespace retro::vault {

class C64Encoder : public DiskEncoder {

    typedef struct
    {
        u8     sectors;         // Typical number of sectors in this track
        u8     speedZone;       // Default speed zone for this track
        u16    lengthInBytes;   // Typical track size in bits
        u16    lengthInBits;    // Typical track size in bits
        isize  firstSectorNr;   // Logical number of first sector in track
        isize  tailGap;         // Gap between two sectors (number of 0x55 bytes)
        double stagger;         // Relative position of first bit (from Hoxs64)
    }
    TrackDefaults;

    static const TrackDefaults &trackDefaults(isize t);

    // Backing buffer
    std::vector<u8> gcrbuffer;

    // Disk ID bytes
    u8 id1 = '2';
    u8 id2 = 'A';

    // Optional error codes for each block
    std::vector<u8> ecc;

public:

    C64Encoder() = default;
    C64Encoder(u8 id1, u8 id2, std::vector<u8> ecc) :
    id1(id1), id2(id2), ecc(std::move(ecc)) { }
    
    // Methods from DiskDecoder
    BitView encodeTrack(ByteView bytes, TrackNr t) override;
    BitView encodeSector(ByteView bytes, TrackNr t, TrackNr s) override;

    // Encodes a single sector (returns the number of encoded bits)
    isize encodeSector(MutableBitView track,
                       isize offset, TrackNr t, SectorNr s, ByteView src);
};

// DEPRECATED
namespace Encoder { extern C64Encoder c64; }

}
