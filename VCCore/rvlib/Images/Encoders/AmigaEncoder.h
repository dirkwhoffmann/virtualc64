// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/Encoders/DiskEncoder.h"

namespace retro::vault {

class AmigaEncoder : public DiskEncoder {

    // Backing buffer
    std::vector<u8> trackBuffer;
    std::vector<u8> sectorBuffer;

public:

    // Methods from DiskDecoder
    utl::BitView encodeTrack(utl::ByteView bytes, TrackNr t) override;
    utl::BitView encodeSector(utl::ByteView bytes, TrackNr t, SectorNr s) override;

    // Recomputes the clock bit at the specified offset
    void rectifyClockBit(utl::MutableBitView bytes, isize offset);
};

}
