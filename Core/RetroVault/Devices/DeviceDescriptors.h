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
#include "utl/abilities/Streamable.h"
#include "utl/primitives.h"

namespace retro::vault {

struct GeometryDescriptor : Streamable {

    // Constants
    static constexpr isize cMin = HDR_C_MIN;
    static constexpr isize cMax = HDR_C_MAX;
    static constexpr isize hMin = HDR_H_MIN;
    static constexpr isize hMax = HDR_H_MAX;
    static constexpr isize sMin = HDR_S_MIN;
    static constexpr isize sMax = HDR_S_MAX;
    
    // Disk geometry (CHS)
    isize cylinders = 0;
    isize heads = 0;
    isize sectors = 0;

    // Size of a sector in bytes
    isize bsize = 512;

    template <class W>
    W& operator<<(W& worker)
    {
        worker

        << cylinders
        << heads
        << sectors
        << bsize;

        return worker;
    }

    // Returns a vector with compatible geometries for a given block count
    static std::vector<std::tuple<isize,isize,isize>> driveGeometries(isize numBlocks);
    static std::vector<GeometryDescriptor> driveGeometries(isize numBlocks, isize bsize);

    // Checks whether the geometry is unique
    bool unique() const;
    
    // Initializers
    GeometryDescriptor() { };
    GeometryDescriptor(isize size);
    GeometryDescriptor(isize c, isize h, isize s, isize b);
    GeometryDescriptor(Diameter type, Density density);

    // Operators
    bool operator == (const GeometryDescriptor &rhs) const;
    bool operator != (const GeometryDescriptor &rhs) const;
    bool operator < (const GeometryDescriptor &rhs) const;

    // Computed values
    isize numTracks() const { return cylinders * heads; }
    isize numBlocks() const { return cylinders * heads * sectors; }
    isize numBytes() const { return cylinders * heads * sectors * bsize; }
    isize upperCyl() const { return cylinders ? cylinders - 1 : 0; }
    isize upperHead() const { return heads ? heads - 1 : 0; }
    isize upperTrack() const { return numTracks() ? numTracks() - 1 : 0; }

    // Prints debug information
    void dump() const;
    void dump(std::ostream &os) const;

    // Throws an exception if inconsistent or unsupported values are present
    void checkCompatibility() const;
};

struct PartitionDescriptor : Streamable
{
    string name;
    u32 flags = 0;
    u32 sizeBlock = 128;
    u32 heads = 0;
    u32 sectors = 0;
    u32 reserved = 2;
    u32 interleave = 0;
    u32 lowCyl = 0;
    u32 highCyl = 0;
    u32 numBuffers = 1;
    u32 bufMemType = 0;
    u32 maxTransfer = 0x7FFFFFFF;
    u32 mask = 0xFFFFFFFE;
    u32 bootPri = 0;
    u32 dosType = 0x444f5300;

    template <class W>
    W& operator<<(W& worker)
    {
        worker

        << name
        << flags
        << sizeBlock
        << heads
        << sectors
        << reserved
        << interleave
        << lowCyl
        << highCyl
        << numBuffers
        << bufMemType
        << maxTransfer
        << mask
        << bootPri
        << dosType;

        return worker;
    }

    // Initializers
    PartitionDescriptor() { };
    PartitionDescriptor(const GeometryDescriptor &geo);

    // Computed values
    isize numCylinders() const noexcept { return highCyl - lowCyl + 1; }
    isize numBlocks() const noexcept { return numCylinders() * heads * sectors; }
    GeometryDescriptor geometry() const noexcept;

    // Returns the physical block number for a logical partition block
    isize translate(isize block) const noexcept;

    // Returns the partition's block range in [lower; upper) format
    Range<isize> range() const noexcept { return { translate(0), translate(0) + numBlocks() }; }

    // Prints debug information
    void dump() const;
    void dump(std::ostream &os) const;

    // Throws an exception if inconsistent or unsupported values are present
    void checkCompatibility(const GeometryDescriptor &geo) const;
};

struct DriverDescriptor : Streamable
{
    u32 dosType = 0;
    u32 dosVersion = 0;
    u32 patchFlags = 0;
    std::vector<u32> blocks;
    u32 segList = 0;

    template <class W>
    W& operator<<(W& worker)
    {
        worker

        << dosType
        << dosVersion
        << patchFlags
        << blocks
        << segList;

        return worker;
    }

    // Initializers
    DriverDescriptor() { };

    // Prints debug information
    void dump() const;
    void dump(std::ostream &os) const;
    
    // Throws an exception if inconsistent or unsupported values are present
    void checkCompatibility() const;
};

}
