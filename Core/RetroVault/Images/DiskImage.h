// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "AnyImage.h"
#include "TrackDevice.h"

namespace retro::vault {

class DiskImage : public AnyImage, public TrackDevice {

public:

    static optional<ImageInfo> about(const fs::path& url);
    static unique_ptr<DiskImage> tryMake(const fs::path& path);
    static unique_ptr<DiskImage> make(const fs::path& path);


    //
    // Methods from LinearDevice
    //

public:

    isize size() const override { return data.size; }
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

    using AnyImage::byteView;
    ByteView byteView(TrackNr t) const;
    ByteView byteView(TrackNr t, SectorNr s) const;
    MutableByteView byteView(TrackNr t);
    MutableByteView byteView(TrackNr t, SectorNr s);
    
    
    //
    // Exporting
    //

public:

    // Update portions of the image file on disk with the current contents
    void saveBlocks(const Range<BlockNr>);
    void saveBlocks(const std::vector<Range<BlockNr>>);
};

}
