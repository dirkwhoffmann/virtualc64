// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "HDFFile.h"

namespace retro::vault::image {

class HDZFile : public HardDiskImage {

public:

    HDFFile hdf;
    
    static optional<ImageInfo> about(const fs::path &path);

    
    //
    // Initializing
    //
    
public:

    explicit HDZFile(const fs::path& path) { init(path); }
    // explicit HDZFile(const class HDFFile &hdf) { init(hdf); }

    using HardDiskImage::init;
    void init(const class HDFFile &hdf);


    //
    // Methods from Hashable
    //

public:

    u64 hash(HashAlgorithm algorithm) const noexcept override {
        return hdf.hash(algorithm);
    }


    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }

    ImageType type() const noexcept override { return ImageType::HARDDISK; }
    ImageFormat format() const noexcept override { return ImageFormat::HDZ; }
    std::vector<string> describeImage() const noexcept override;
    

    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::AMIGA; }


    //
    // Methods from AnyFile
    //

public:

    void didInitialize() override;

    
    //
    // Methods from DiskImage
    //

public:

    isize numCyls() const override { return hdf.numCyls(); }
    isize numHeads() const override { return hdf.numHeads(); }
    isize numSectors(isize t) const override { return hdf.numSectors(t); }


    //
    // Methods from HardDiskImage
    //

public:

    isize numPartitions() const override { return isize(hdf.ptable.size()); }
    Range<isize> partition(isize nr) const override { return hdf.ptable[nr].range(); }


    //
    // Methods from BlockDevice
    //

public:

    isize capacity() const override { return hdf.numBlocks(); }
    isize bsize() const override { return hdf.bsize(); }
    void readBlocks(u8 *dst, Range<isize> r) const override { hdf.readBlocks(dst, r); }
    void writeBlocks(const u8 *src, Range<isize> r) override { hdf.writeBlocks(src, r); }


    //
    // Serializing
    //
    
public:
    
    isize writePartitionToFile(const fs::path &path, isize nr) const;
};

}
