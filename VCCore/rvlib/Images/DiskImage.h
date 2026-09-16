// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------
#pragma once

#include "Images/BinaryImage.h"
#include "Devices/TrackDevice.h"

namespace retro::vault {

class DiskImage : public BinaryImage, public TrackDevice {

public:

    static optional<ImageInfo> about(const fs::path& url);
    static unique_ptr<DiskImage> tryMake(const fs::path& path);
    static unique_ptr<DiskImage> make(const fs::path& path);


    //
    // Methods from LinearDevice
    //

public:

    isize size() const override { return getSize(); }
    void read(u8 *dst, isize offset, isize count) const override;
    void write(const u8 *src, isize offset, isize count) override;


    //
    // Querying meta information
    //

    // Returns the predicted file system family of this image
    virtual FSFamily fsFamily() const noexcept = 0;


    //
    // Obtaining views on tracks and sectors
    //

public:

    /* Views on a single track or sector.
     *
     * Named apart from byteView() on purpose: track and sector numbers are
     * plain integers, so a byteView(t, s) would quietly stand in for
     * byteView(offset, len).
     */
    utl::ByteView trackView(TrackNr t) const;
    utl::ByteView sectorView(TrackNr t, SectorNr s) const;
    utl::MutableByteView mutableTrackView(TrackNr t);
    utl::MutableByteView mutableSectorView(TrackNr t, SectorNr s);
};

}
