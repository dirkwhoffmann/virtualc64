// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DeviceTypes.h"
#include "utl/abilities/Loggable.h"
#include "utl/primitives/BitView.h"

namespace retro::vault {

using namespace utl;

class DiskEncoder : public Loggable {

public:

    virtual ~DiskEncoder() = default;

    virtual BitView encodeTrack(ByteView src, TrackNr t) = 0;
    virtual BitView encodeSector(ByteView src, TrackNr t, TrackNr s) = 0;
};

}
