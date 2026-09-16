// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/HardDiskImage.h"
#include "Images/HDF/HDFLayout.h"
#include "Devices/DeviceDescriptors.h"
#include "utl/common.h"
#include "Images/ImageTypes.h"

namespace retro::vault {


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
    static void ensureHDF(isize len);

    
    //
    // Initializing
    //

public:

    explicit HDFFile() { }
    explicit HDFFile(isize len) { init(len); }
    explicit HDFFile(const fs::path& path) { init(path); }
    explicit HDFFile(const LinearDevice& device) { init(device); }

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
    std::unique_ptr<utl::Backing> makeBacking(const fs::path &path) const override;
    isize imageSize(utl::Backing &backing) const override;


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
    utl::Range<isize> partition(isize nr) const override { return ptable[nr].range(); }


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return 512; }


    //
    // Providing descriptors
    //

public:

    GeometryDescriptor getGeometryDescriptor() const {
        return layout().getGeometryDescriptor();
    }
    PartitionDescriptor getPartitionDescriptor(isize part = 0) const {
        return layout().getPartitionDescriptor(part);
    }
    std::vector<PartitionDescriptor> getPartitionDescriptors() const {
        return layout().getPartitionDescriptors();
    }
    DriverDescriptor getDriverDescriptor(isize driver = 0) const {
        return layout().getDriverDescriptor(driver);
    }
    std::vector<DriverDescriptor> getDriverDescriptors() const {
        return layout().getDriverDescriptors();
    }


    //
    // Querying product information
    //

public:

    optional<string> getDiskVendor() const { return layout().getDiskVendor(); }
    optional<string> getDiskProduct() const { return layout().getDiskProduct(); }
    optional<string> getDiskRevision() const { return layout().getDiskRevision(); }
    optional<string> getControllerVendor() const { return layout().getControllerVendor(); }
    optional<string> getControllerProduct() const { return layout().getControllerProduct(); }
    optional<string> getControllerRevision() const { return layout().getControllerRevision(); }


    //
    // Querying volume information
    //

public:

    // Returns the (predicted) geometry for this disk
    const GeometryDescriptor getGeometry() const { return geometry; }

    // Returns true if this image contains a rigid disk block
    bool hasRDB() const { return layout().hasRDB(); }

    // Returns the number of loadable file system drivers
    isize numDrivers() const { return isize(drivers.size()); }

    // Predicts the number of blocks of this hard drive
    isize predictNumBlocks() const { return layout().predictNumBlocks(); }


    //
    // Scanning raw disk data
    //

private:

    // Reads this image's layout (geometry, partitions, drivers)
    HDFLayout layout() const { return HDFLayout(*this); }

    // Returns a string from the Rigid Disk Block if it exists
    optional<string> rdbString(isize offset, isize len) const {
        return layout().rdbString(offset, len);
    }
};

}
