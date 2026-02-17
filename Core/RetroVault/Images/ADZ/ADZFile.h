// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ADFFile.h"

namespace retro::vault::image {

class ADZFile : public FloppyDiskImage {

public:

    ADFFile adf;

    static optional<ImageInfo> about(const fs::path &path);

    
    //
    // Initializing
    //
    
public:

    explicit ADZFile(const fs::path& path) { init(path); }
    // explicit ADZFile(const class ADFFile &adf) { init(adf); }

    using FloppyDiskImage::init;
    // void init(const class ADFFile &adf);
    

    //
    // Methods from Hashable
    //

public:

    u64 hash(HashAlgorithm algorithm) const noexcept override {
        return adf.hash(algorithm);
    }


    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }
    
    ImageType type() const noexcept override { return ImageType::FLOPPY; }
    ImageFormat format() const noexcept override { return ImageFormat::ADZ; }
    std::vector<string> describeImage() const noexcept override;

    void didInitialize() override;


    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::AMIGA; }


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return adf.bsize(); }
    isize capacity() const override { return adf.capacity(); }
    void readBlocks(u8 *dst, Range<isize> r) const override { adf.readBlocks(dst, r); }
    void writeBlocks(const u8 *src, Range<isize> r) override { adf.writeBlocks(src, r); };


    //
    // Methods from TrackDevice
    //

public:
    
    isize numCyls() const noexcept override { return adf.numCyls(); }
    isize numHeads() const noexcept override { return adf.numHeads(); }
    isize numSectors(isize t) const noexcept override { return adf.numSectors(t); }


    //
    // Methods from FloppyDiskImage
    //

public:

    Diameter getDiameter() const noexcept override { return adf.getDiameter(); }
    Density getDensity() const noexcept override { return adf.getDensity(); }

    BitView encode(TrackNr t) const override { return adf.encode(t); }
    void decode(TrackNr t, BitView bits) override { return adf.decode(t, bits); }

};

}
