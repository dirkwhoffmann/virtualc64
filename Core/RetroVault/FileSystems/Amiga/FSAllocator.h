// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/Amiga/FSService.h"

namespace retro::vault::amiga {

class FSAllocator final : public FSService {

public:

    // Allocation pointer (selects the block to allocate next)
    BlockNr ap = 0;

    using FSService::FSService;


    //
    // Computing block counts
    //

public:
    
    // Returns the number of required blocks to store a file of certain size
    [[nodiscard]] isize requiredBlocks(isize fileSize) const noexcept;

private:

    // Returns the number of required file list or data blocks
    [[nodiscard]] isize requiredFileListBlocks(isize fileSize) const noexcept;
    [[nodiscard]] isize requiredDataBlocks(isize fileSize) const noexcept;


    //
    // Creating and deleting blocks
    //

public:

    // Returns true if at least 'count' free blocks are available
    [[nodiscard]] bool allocatable(isize count) const noexcept;

    // Seeks a free block and marks it as allocated
    BlockNr allocate();

    // Allocates multiple blocks
    void allocate(isize count, std::vector<BlockNr> &result, std::vector<BlockNr> prealloc = {});

    // Deallocates a block
    void deallocateBlock(BlockNr nr);

    // Allocates multiple blocks
    void deallocateBlocks(const std::vector<BlockNr> &nrs);

    // Allocates all blocks needed for a file
    void allocateFileBlocks(isize bytes,
                            std::vector<BlockNr> &listBlocks, std::vector<BlockNr> &dataBlocks);

    //
    // Managing the block allocation bitmap
    //

public:

    // Checks if a block is allocated or unallocated
    [[nodiscard]] bool isUnallocated(BlockNr nr) const noexcept;
    [[nodiscard]] bool isAllocated(BlockNr nr) const noexcept { return !isUnallocated(nr); }

    // Returns the number of allocated or unallocated blocks
    [[nodiscard]] isize numUnallocated() const noexcept;
    [[nodiscard]] isize numAllocated() const noexcept;

    // Marks a block as allocated or free
    void markAsAllocated(BlockNr nr) { setAllocBit(nr, 0); }
    void markAsFree(BlockNr nr) { setAllocBit(nr, 1); }
    void setAllocBit(BlockNr nr, bool value);

private:

    // Locates the allocation bit for a certain block
    // FSBlock *locateAllocationBit(BlockNr nr, isize *byte, isize *bit) noexcept;
    const FSBlock *locateAllocationBit(BlockNr nr, isize *byte, isize *bit) const noexcept;

    // Translate the bitmap into to a vector with the n-th bit set iff the n-th block is free
    std::vector<u32> readBitmap() const;
};

}
