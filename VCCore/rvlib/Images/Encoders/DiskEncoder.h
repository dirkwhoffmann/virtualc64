// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Devices/DeviceTypes.h"
#include "utl/abilities/Loggable.h"
#include "utl/primitives/BitView.h"
#include "Images/ImageTypes.h"

namespace retro::vault {

class DiskEncoder : public utl::Loggable {

public:

    virtual ~DiskEncoder() = default;

    virtual utl::BitView encodeTrack(utl::ByteView src, TrackNr t) = 0;
    virtual utl::BitView encodeSector(utl::ByteView src, TrackNr t, TrackNr s) = 0;
};

}
