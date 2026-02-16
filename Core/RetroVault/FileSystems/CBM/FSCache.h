// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "BlockDevice.h"
#include "FileSystems/CBM/FSTypes.h"
#include "FileSystems/CBM/FSBlock.h"
#include "FileSystems/CBM/FSService.h"
#include "Volume.h"
#include <iostream>
#include <ranges>
#include <unordered_set>

namespace retro::vault::cbm {

class FSCache final : public FSService {

    friend struct FSBlock;

private:

    // The underlying volume
    Volume &dev;

    // Cached blocks
    mutable std::unordered_map<BlockNr, std::unique_ptr<FSBlock>> blocks;

    // Dirty blocks
    mutable std::unordered_set<BlockNr> dirty;


    //
    // Initializing
    //

public:

    FSCache(FileSystem &fs, Volume &vol);
    virtual ~FSCache();

private:

    void dealloc();


    //
    // Printing debug information
    //

public:

    void dump(std::ostream &os) const;


    //
    // Querying devide properties
    //

public:

    // Returns capacity information
    isize bsize() const { return dev.bsize(); }
    isize capacity() const { return dev.capacity(); }
    isize numBytes() const { return capacity() * bsize(); }

    // Reports usage information
    isize freeBlocks() const { return capacity() - usedBlocks(); }
    isize usedBlocks() const { return (isize)blocks.size(); }
    isize freeBytes() const { return freeBlocks() * bsize(); }
    isize usedBytes() const { return usedBlocks() * bsize(); }
    double fillLevel() const { return capacity() ? double(100) * usedBlocks() / capacity() : 0; }
    bool isEmpty() const { return usedBlocks() == 0; }

    // Predicts the file system type based on stored data
    static FSFormat predictDOS(BlockDevice &dev) noexcept;
    FSFormat predictDOS() const noexcept { return predictDOS(dev); }


    //
    // Accessing blocks
    //

    // Returns a view for all keys
    auto keys() const { return std::views::keys(blocks); }

    // Returns a view for all keys in a particular range
    auto keys(BlockNr min, BlockNr max) const {

        auto in_range = [=](BlockNr key) { return key >= min && key <= max; };
        return std::views::keys(blocks) | std::views::filter(in_range);
    }

    // Returns a vector with all keys in sorted order
    std::vector<BlockNr> sortedKeys() const;

    // Checks if a block is empty
    bool isEmpty(BlockNr nr) const noexcept;

    // Gets or sets the block type
    FSBlockType getType(BlockNr nr) const noexcept;
    // void setType(BlockNr nr, FSBlockType type);

    // Caches a block (if not already cached)
    FSBlock *cache(BlockNr nr) const noexcept;

    // Returns a pointer to a block with read permissions (maybe null)
    const FSBlock *tryFetch(BlockNr nr) const noexcept;
    const FSBlock *tryFetch(BlockNr nr, FSBlockType type) const noexcept;
    const FSBlock *tryFetch(BlockNr nr, std::vector<FSBlockType> types) const noexcept;

    // Returns a reference to a block with read permissions (may throw)
    const FSBlock &fetch(BlockNr nr) const;
    const FSBlock &fetch(BlockNr nr, FSBlockType type) const;
    const FSBlock &fetch(BlockNr nr, std::vector<FSBlockType> types) const;

    // Returns a pointer to a block with write permissions (maybe null)
    FSBlock *tryModify(BlockNr nr) noexcept;
    FSBlock *tryModify(BlockNr nr, FSBlockType type) noexcept;
    FSBlock *tryModify(BlockNr nr, std::vector<FSBlockType> types) noexcept;

    // Returns a reference to a block with write permissions (may throw)
    FSBlock &modify(BlockNr nr);
    FSBlock &modify(BlockNr nr, FSBlockType type);
    FSBlock &modify(BlockNr nr, std::vector<FSBlockType> types);

    // Operator overload
    const FSBlock &operator[](size_t nr) const { return fetch(BlockNr(nr)); }

    // Wipes out a block (makes it an empty block)
    void erase(BlockNr nr);


    //
    // Caching
    //

    isize cachedBlocks() const { return (isize)blocks.size(); }
    isize dirtyBlocks() const { return (isize)dirty.size(); }
    void markAsDirty(BlockNr nr);

    void flush();
    void invalidate();
};

}
