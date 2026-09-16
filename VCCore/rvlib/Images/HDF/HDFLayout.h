// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Devices/DeviceDescriptors.h"
#include "utl/common.h"
#include <array>
#include <functional>
#include <map>

namespace retro::vault {

/* Reads what an Amiga hard disk image says about itself.
 *
 * Geometry, partition table and the file system drivers an image carries are
 * all described by a handful of blocks near the front, linked from the Rigid
 * Disk Block. This walks that structure through a block reader instead of a
 * pointer into a buffer, so the same code serves an image held in memory and
 * one that is only read where it is looked at.
 *
 * Blocks are kept for the lifetime of the object rather than re-read. A parse
 * touches a few hundred of them at most, and holding them is what lets seek()
 * hand out pointers that stay valid while a caller picks fields out of one
 * block and then asks for the next.
 */
class HDFLayout {

public:

    // Reads one block into 'dst'. Returns false if the block does not exist.
    using BlockReader = std::function<bool(isize nr, u8 *dst)>;

    // Block size of an Amiga hard disk image
    static constexpr isize bsize = 512;

private:

    // Where the blocks come from
    BlockReader reader;

    // Size of the whole image in bytes
    isize bytes = 0;

    // Blocks read so far
    mutable std::map<isize, std::array<u8, bsize>> blocks;


    //
    // Initializing
    //

public:

    HDFLayout(BlockReader reader, isize bytes) : reader(reader), bytes(bytes) { }

    // Reads the layout of a device (an image in memory, a file, ...)
    explicit HDFLayout(const class LinearDevice &device);


    //
    // Querying the layout
    //

public:

    // Returns true if the image carries a rigid disk block
    bool hasRDB() const { return seekRDB() != nullptr; }

    // Returns the drive geometry, from the RDB if there is one
    GeometryDescriptor getGeometryDescriptor() const;

    // Returns the partition table
    PartitionDescriptor getPartitionDescriptor(isize nr = 0) const;
    std::vector<PartitionDescriptor> getPartitionDescriptors() const;

    // Returns the loadable file system drivers
    DriverDescriptor getDriverDescriptor(isize nr = 0) const;
    std::vector<DriverDescriptor> getDriverDescriptors() const;

    // Returns a string from the rigid disk block if there is one
    optional<string> rdbString(isize offset, isize len) const;

    // Product information, as recorded in the rigid disk block
    optional<string> getDiskVendor() const { return rdbString(160, 8); }
    optional<string> getDiskProduct() const { return rdbString(168, 16); }
    optional<string> getDiskRevision() const { return rdbString(184, 4); }
    optional<string> getControllerVendor() const { return rdbString(188, 8); }
    optional<string> getControllerProduct() const { return rdbString(196, 16); }
    optional<string> getControllerRevision() const { return rdbString(212, 4); }

    /* Predicts the number of blocks of this image.
     *
     * Only meaningful for an image without an RDB, and expensive: with no
     * RDB to consult, finding the root block means scanning the image.
     */
    isize predictNumBlocks() const;


    //
    // Scanning raw disk data
    //

private:

    // Returns a pointer to a block if it exists
    u8 *seek(isize nr) const;

    // Checks whether the provided pointer points to a Root Block
    bool isRB(const u8 *ptr) const;

    // Returns the number of the Root Block if it exists
    optional<isize> seekRB() const;

    // Returns a pointer to the Rigid Disk Block if it exists
    u8 *seekRDB() const;

    // Returns a pointer to a certain partition block if it exists
    u8 *seekPB(isize nr) const;

    // Returns a pointer to a certain filesystem header block if it exists
    u8 *seekFSH(isize nr) const;
};

}
