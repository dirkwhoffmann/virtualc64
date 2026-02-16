// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FloppyDiskImage.h"

namespace retro::vault::image {

class IMGFile : public FloppyDiskImage {

    using MFMTrack = std::vector<u8>;
    mutable std::vector<MFMTrack> mfmTracks {168};
    
public:

    static constexpr isize IMGSIZE_35_DD = 737280;  // 720 KB PC disk

    static optional<ImageInfo> about(const fs::path &path);


    //
    // Initializing
    //

public:

    explicit IMGFile(const fs::path &path) { init(path); }
    explicit IMGFile(isize len) { init(len); }
    explicit IMGFile(const u8 *buf, isize len) { init(buf, len); }
    explicit IMGFile(Diameter dia, Density den) { init(dia, den); }

    using AnyImage::init;
    void init(Diameter dia, Density den);


    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }

    ImageType type() const noexcept override { return ImageType::FLOPPY; }
    ImageFormat format() const noexcept override { return ImageFormat::IMG; }
    std::vector<string> describe() const noexcept override;


    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::DOS; }


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return 512; }


    //
    // Methods from DiskImage
    //

public:

    isize numCyls() const override;
    isize numHeads() const override;
    isize numSectors(isize) const override { return numSectors(); }
    isize numSectors() const;


    //
    // Methods from FloppyDiskImage
    //

    Diameter getDiameter() const noexcept override { return Diameter::INCH_35; }
    Density getDensity() const noexcept override { return Density::DD; }

    BitView encode(TrackNr t) const override;
    void decode(TrackNr t, BitView bits) override;
};

}
