// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/Amiga/FSTypes.h"
#include "DeviceDescriptors.h"

namespace retro::vault::amiga {

/* To create a FileSystem, several layout parameters need to to be provided.
 * This is done by passing a FSDescriptor which contains the necessary
 * information.
 *
 * A FSDescriptor can be obtained in several ways. If a descriptor for
 * a floppy disk is needed, it can be created by specifiying the form factor
 * and density of the disk. In addition, a suitabe descriptors can be extracted
 * directly from an ADF or HDF.
 */
struct FSDescriptor {

    // Capacity of the file system in blocks
    isize numBlocks = 0;
    
    // Size of a block in bytes
    isize bsize = 512;
    
    // Number of reserved blocks
    isize numReserved = 0;

    // Location of the root block
    BlockNr rootBlock = 0;

    // References to all bitmap blocks and bitmap extension blocks
    std::vector<BlockNr> bmBlocks;
    std::vector<BlockNr> bmExtBlocks;
    
    // Initializing
    FSDescriptor() { };
    FSDescriptor(isize numBlocks);
    FSDescriptor(const GeometryDescriptor &geometry);
    FSDescriptor(const PartitionDescriptor &des);
    FSDescriptor(Diameter dia, Density den);
    
    void init(isize numBlocks);
    void init(const GeometryDescriptor &geometry);
    void init(const PartitionDescriptor &des);
    void init(Diameter type, Density density);

    // Computed values
    isize numBytes() const { return numBlocks * bsize; }
    
    // Prints debug information
    void dump() const;
    void dump(std::ostream &os) const;
    
    // Throws an exception if the descriptor contains unsupported values
    void checkCompatibility() const;
};

}
