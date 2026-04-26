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

class DOSEncoder : public DiskEncoder {

    static constexpr isize bsize  = 512;  // Block size in bytes
    static constexpr isize ssize  = 1300; // MFM sector size in bytes
    static constexpr isize maxsec = 22;   // Maximum number of sectors

    // Backing buffer used by the MFM encoder
    u8 mfmBuffer[ssize * maxsec];

    // Backing buffer
    unique_ptr<u8> decoded;

public:

    BitView encodeTrack(ByteView bytes, TrackNr t) override;
    BitView encodeSector(ByteView bytes, TrackNr t, TrackNr s) override;

    void encodeSector(MutableByteView track, isize offset, TrackNr t, SectorNr s, ByteView src);
};

namespace Encoder { extern DOSEncoder ibm; }

}
