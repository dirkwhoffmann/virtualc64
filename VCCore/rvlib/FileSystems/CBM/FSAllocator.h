// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/CBM/FSService.h"

namespace retro::vault::cbm {

class FSAllocator final : public FSService {

    // Allocation pointer (selects the block to allocate next)
    TSLink ap = {1,0};

public:

    using FSService::FSService;


    //
    // Computing block counts
    //

public:

    // Returns the number of required blocks to store a file of certain size
    isize requiredBlocks(isize size) const noexcept;


    //
    // Creating and deleting blocks
    //

public:

    // Seeks a free block and marks it as allocated
    BlockNr allocate();

    // Allocates multiple blocks
    std::vector<BlockNr> allocate(isize count);
    std::vector<BlockNr> allocate(isize count, std::vector<BlockNr> prealloc);

    // Deallocates a block
    void deallocateBlock(BlockNr nr);

    // Deallocates multiple blocks
    void deallocateBlocks(const std::vector<BlockNr> &nrs);

private:

    // Moves to the next block according to the CBM interleaving scheme
    TSLink advance(TSLink ts);


    //
    // Managing the block allocation bitmap
    //

public:

    // Checks if a block is allocated or unallocated
    bool isFree(BlockNr nr) const noexcept;
    bool isFree(TSLink ts) const noexcept;
    bool isAllocated(BlockNr nr) const noexcept { return !isFree(nr); }
    bool isAllocated(TSLink ts) const noexcept { return !isFree(ts); }

    // Returns the number of allocated or unallocated blocks
    isize numUnallocated() const noexcept;
    isize numAllocated() const noexcept;

    // Marks a block as allocated or free
    void setAllocBit(BlockNr nr, bool value);
    void setAllocBit(TSLink ts, bool value);

    // Convenience wrappers
    void markAsAllocated(BlockNr nr) { setAllocBit(nr, 0); }
    void markAsAllocated(TSLink ts) { setAllocBit(ts, 0); }
    void markAsFree(BlockNr nr) { setAllocBit(nr, 1); }
    void markAsFree(TSLink ts) { setAllocBit(ts, 1); }

    // Reads the allocation bits from the BAM, for a single track or all tracks.
    // The n-th bit set means the n-th block is free.
    std::vector<bool> readBitmap(TrackNr t) const;
    std::vector<bool> readBitmap() const;

    // Writes the allocation bits to the BAB, for a single track or all tracks.
    // The n-th bit set means the n-th block is free.
    void writeBitmap(TrackNr t, const std::vector<bool> &bitmap);
    void writeBitmap(const std::vector<bool> &bitmap);
};

}
