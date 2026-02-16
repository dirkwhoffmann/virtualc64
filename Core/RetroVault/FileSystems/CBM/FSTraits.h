// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/CBM/FSTypes.h"
#include "DeviceDescriptors.h"

namespace retro::vault::image { class D64File; }

namespace retro::vault::cbm {

using image::D64File;

/* To create a FileSystem, several layout parameters need to to be provided.
 * This is done by passing a FSDescriptor which contains the necessary
 * information.
 */
struct FSTraits {

    // File system type
    FSFormat dos = FSFormat::NODOS;

    // Number of blocks
    isize blocks = 0;

    // Number of bytes
    isize bytes = 0;

    // Number of cylinders
    isize numCyls = 0;

    // Number of heads
    isize numHeads = 0;

    // Block size in bytes
    isize bsize = 256;

    FSTraits() { }

    // Creates a device descriptor for a given block count
    FSTraits(FSFormat format, isize numBlocks) { init(format, numBlocks); }

    // Creates a device descriptor fitting a D64 file
    FSTraits(const D64File &d64) { init(d64); }

    void init(FSFormat format, isize numBlocks);
    void init(const D64File &d64);

    // Prints debug information
    void dump() const;
    void dump(std::ostream &os) const;

    // Throws an exception if the descriptor contains unsupported values
    void checkCompatibility() const;


    //
    // Performing integrity checks
    //

    bool isValidTrackNr(TrackNr t) const;
    bool isValidLink(TSLink ts) const;
    bool isValidBlock(BlockNr nr) const;


    //
    // Querying device properties
    //

    isize numTracks() const { return numCyls * numHeads; }
    isize speedZone(TrackNr track) const;
    isize speedZone(TSLink ts) const;
    isize numSectors(TrackNr track) const;
    isize numSectors(TSLink ts) const;
    isize numBlocks() const;


    //
    // Translating blocks, tracks, and sectors
    //

    optional<TSLink> tsLink(BlockNr b) const;
    optional<BlockNr> blockNr(TSLink ts) const;

    // [[deprecated]] optional<BlockNr> blockNr(TrackNr t, SectorNr s) const { return blockNr(TSLink{t,s}); }


    //
    // Ordering blocks
    //

    // [[deprecated]] bool nextBlock(BlockNr b, BlockNr *nb) const;
    optional<TSLink> nextBlockRef(TSLink ts) const;
    optional<TSLink> nextBlockRef(BlockNr b) const;
};

}
