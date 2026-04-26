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

class AmigaEncoder : public DiskEncoder {

    // Backing buffer
    std::vector<u8> trackBuffer;
    std::vector<u8> sectorBuffer;

public:

    // Methods from DiskDecoder
    BitView encodeTrack(ByteView bytes, TrackNr t) override;
    BitView encodeSector(ByteView bytes, TrackNr t, SectorNr s) override;

    // Recomputes the clock bit at the specified offset
    void rectifyClockBit(MutableBitView bytes, isize offset);
};

}
