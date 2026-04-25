// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "HardDiskImage.h"
#include "DeviceDescriptors.h"
#include "utl/common.h"

namespace retro::vault::image {

using namespace utl;


class HDFFile : public HardDiskImage {

public:

    // Derived drive geometry
    GeometryDescriptor geometry;

    // Derived partition table
    std::vector <PartitionDescriptor> ptable;

    // Included device drivers
    std::vector <DriverDescriptor> drivers;

    // Analyzes the type of the provided file
    static optional<ImageInfo> about(const fs::path &path);

    // Checks if the buffer is in ADF format (throws if not)
    static void ensureHDF(u8 *buf, isize len);

    
    //
    // Initializing
    //

public:

    explicit HDFFile() { }
    explicit HDFFile(isize len) { init(len); }
    explicit HDFFile(const u8 *buf, isize len) { init(buf, len); }
    explicit HDFFile(const Buffer<u8>& buffer) { init(buffer); }
    explicit HDFFile(const fs::path& path) { init(path); }

    using HardDiskImage::init;


    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }

    ImageType type() const noexcept override { return ImageType::HARDDISK; }
    ImageFormat format() const noexcept override { return ImageFormat::HDF; }
    std::vector<string> describeImage() const noexcept override;
    isize writeToFile(const fs::path &path) const override;
    isize writeToFile(const fs::path &path, isize offset, isize len) const override;
    void didInitialize() override;


    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::AMIGA; }


    //
    // Methods from TrackDevice
    //

public:

    isize numCyls() const override;
    isize numHeads() const override;
    isize numSectors(isize) const override { return numSectors(); }
    isize numSectors() const;


    //
    // Methods from HardDiskImage
    //

public:

    isize numPartitions() const override { return isize(ptable.size()); }
    Range<isize> partition(isize nr) const override { return ptable[nr].range(); }


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return 512; }


    //
    // Providing descriptors
    //

public:

    GeometryDescriptor getGeometryDescriptor() const;
    PartitionDescriptor getPartitionDescriptor(isize part = 0) const;
    std::vector<PartitionDescriptor> getPartitionDescriptors() const;
    DriverDescriptor getDriverDescriptor(isize driver = 0) const;
    std::vector<DriverDescriptor> getDriverDescriptors() const;


    //
    // Querying product information
    //

public:

    optional<string> getDiskVendor() const { return rdbString(160, 8); }
    optional<string> getDiskProduct() const { return rdbString(168, 16); }
    optional<string> getDiskRevision() const { return rdbString(184, 4); }
    optional<string> getControllerVendor() const { return rdbString(188, 8); }
    optional<string> getControllerProduct() const { return rdbString(196, 16); }
    optional<string> getControllerRevision() const { return rdbString(212, 4); }


    //
    // Querying volume information
    //

public:

    // Returns the (predicted) geometry for this disk
    const GeometryDescriptor getGeometry() const { return geometry; }

    // Returns true if this image contains a rigid disk block
    bool hasRDB() const;

    // Returns the number of loadable file system drivers
    isize numDrivers() const { return isize(drivers.size()); }


    // Returns the byte count and the location of a certain partition
    isize partitionSize(isize nr) const;
    isize partitionOffset(isize nr) const;
    u8 *partitionData(isize nr) const;

    // Predicts the number of blocks of this hard drive
    isize predictNumBlocks() const;


    //
    // Scanning raw disk data
    //

private:

    // Returns a pointer to a certain block if it exists
    u8 *seekBlock(isize nr) const;

    // Checks whether the provided pointer points to a Root Block
    bool isRB(u8 *ptr) const;

    // Return a pointer to the Root Block if it exists
    u8 *seekRB() const;

    // Return a pointer to the Rigid Disk Block if it exists
    u8 *seekRDB() const;

    // Returns a pointer to a certain partition block if it exists
    u8 *seekPB(isize nr) const;

    // Returns a pointer to a certain filesystem header block if it exists
    u8 *seekFSH(isize nr) const;

    // Returns a string from the Rigid Disk Block if it exists
    optional<string> rdbString(isize offset, isize len) const;


    //
    // Serializing
    //

public:

    isize writePartitionToFile(const fs::path &path, isize nr) const;
};

}
