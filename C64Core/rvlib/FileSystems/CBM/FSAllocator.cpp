// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/CBM/FileSystem.h"
#include "utl/support.h"
#include <bit>

namespace retro::vault::cbm {

isize
FSAllocator::requiredBlocks(isize fileSize) const noexcept
{
    return (fileSize + 253) / 254;
}

BlockNr
FSAllocator::allocate()
{
    if (auto blocks = allocate(1); !blocks.empty())
        return blocks[0];

    throw FSError(FSError::FS_OUT_OF_SPACE);
}

std::vector<BlockNr>
FSAllocator::allocate(isize count)
{
    std::vector<BlockNr> result;
    TSLink ts = ap;

    // Gather 'count' free blocks
    while (count > 0) {

        if (auto nr = traits.blockNr(ts); nr.has_value()) {

            // Note this block if it is empty
            if (isFree(ts)) {

                result.push_back(*nr);
                count--;
            }

            // Move to the next block
            ts = advance(ts);
            if (ts != ap) continue;
        }
        throw FSError(FSError::FS_OUT_OF_SPACE);
    }

    // Allocate blocks
     for (const auto &b : result) {

        fs.fetch(b).mutate().type = FSBlockType::UNKNOWN;
        markAsAllocated(b);
    }

    return result;
}

std::vector<BlockNr>
FSAllocator::allocate(isize count, std::vector<BlockNr> prealloc)
{
    std::vector<BlockNr> result;
     result.reserve(count);

     // Step 1: Use pre-allocated blocks from the start
     auto it = prealloc.begin();
     while (it != prealloc.end() && count > 0) {

         result.push_back(*it);
         ++it;
         --count;
     }

     // Step 2: Allocate remaining blocks from free space
     if (count > 0) {

         auto more = allocate(count);
         result.insert(result.end(), more.begin(), more.end());
     }

     // Step 3: Free all unused preallocated blocks
     for (; it != prealloc.end(); ++it) deallocateBlock(*it);

     return result;
}

void
FSAllocator::deallocateBlock(BlockNr nr)
{
    fs.fetch(nr).mutate().init(FSBlockType::EMPTY);
    markAsFree(nr);
}

void
FSAllocator::deallocateBlocks(const std::vector<BlockNr> &nrs)
{
    for (BlockNr nr : nrs) { deallocateBlock(nr); }
}

TSLink
FSAllocator::advance(TSLink ts)
{
    // Interleave patterns used to determine the next sector
    static constexpr SectorNr next[5][21] = {

        // Speed zone 0 - 3
        { 10,11,12,13,14,15,16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 10,11,12,13,14,15,16,17, 1, 0, 2, 3, 4, 5, 6, 7, 8, 9 },
        { 10,11,12,13,14,15,16,17,18, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
        { 10,11,12,13,14,15,16,17,18,19,20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 },

        // Directory track
        {  3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18, 0, 1, 2 }
    };

    if (!traits.isValidLink(ts)) return {0,0};

    TrackNr t  = ts.t;
    SectorNr s = ts.s;

    if (t == 18) {

        // Take care of the directory track
        s = next[4][s];

        // Return immediately if we've wrapped over (directory track is full)
        if (s == 0) return {0,0};

    } else {

        // Take care of all other tracks
        s = next[traits.speedZone(ts)][s];

        // Move to the next track if we've wrapped over
        if (s == 0) t = t >= traits.numTracks() ? 1 : t == 17 ? 19 : t + 1;
    }

    assert(traits.isValidLink(TSLink{t,s}));
    return TSLink{t,s};
}

bool
FSAllocator::isFree(BlockNr nr) const noexcept
{
    assert(traits.isValidBlock(nr));

    if (auto ts = traits.tsLink(nr))
        return isFree(*ts);

    return false;
}

bool
FSAllocator::isFree(TSLink ts) const noexcept
{
    assert(traits.isValidLink(ts));

    auto &bam       = fs.fetchBAM();
    auto *data      = bam.data();
    auto *entry     = data + (4 * ts.t);
    auto *allocBits = entry + 1;

    return allocBits[ts.s / 8] & (1 << (ts.s % 8));
}

isize
FSAllocator::numUnallocated() const noexcept
{
    auto &bam       = fs.fetchBAM();
    auto *data      = bam.data();
    isize result    = 0;

    for (isize i = 0x04; i < 0x8F; i += 4)
        result += data[i];

    if constexpr (debug::FS_DEBUG) {

        isize count = 0;
        for (isize i = 0; i < fs.blocks(); ++i) { if (isFree(BlockNr(i))) count++; }
        loginfo(FS_DEBUG, "Unallocated blocks: Fast code: %ld Slow code: %ld\n", result, count);
        assert(count == result);
    }

    return result;
}

isize
FSAllocator::numAllocated() const noexcept
{
    return fs.blocks() - numUnallocated();
}

void
FSAllocator::setAllocBit(BlockNr nr, bool value)
{
    if (auto ts = traits.tsLink(nr))
        setAllocBit(*ts, value);
}

void
FSAllocator::setAllocBit(TSLink ts, bool value)
{
    assert(traits.isValidLink(ts));

    auto &bam       = fs.fetchBAM().mutate();
    auto *data      = bam.data();
    auto *entry     = data + (4 * ts.t);
    auto *allocBits = entry + 1;
    auto byteIndex  = ts.s / 8;
    auto mask       = 1 << (ts.s % 8);
    auto bit        = allocBits[byteIndex] & mask;

    if (value && !bit) {

        // Mark sector as free
        allocBits[byteIndex] |= mask;

        // Increase the number of free sectors
        entry[0]++;
    }

    if (!value && bit) {

        // Mark sector as allocated
        allocBits[byteIndex] &= ~mask;

        // Decrease the number of free sectors
        entry[0]--;
    }
}

std::vector<bool>
FSAllocator::readBitmap(TrackNr t) const
{
    auto numSectors = traits.numSectors(t);
    auto &bam       = fs.fetchBAM();
    auto *data      = bam.data();
    auto *entry     = data + 4 + (4 * t);
    auto *allocBits = entry + 1;

    std::vector<bool> result;
    result.reserve(numSectors);

    for (SectorNr s = 0; s < numSectors; ++s) {

        auto byteIndex = s / 8; // 0..2
        auto bitIndex  = s % 8; // LSB first

        bool free = (allocBits[byteIndex] >> bitIndex) & 1;
        result.push_back(free);
    }

    return result;
}

std::vector<bool>
FSAllocator::readBitmap() const
{
    std::vector<bool> result;
    result.reserve(traits.numBlocks());

    auto numTracks = traits.numTracks();

    for (TrackNr t = 0; t < numTracks; ++t) {

        auto trackBits = readBitmap(t);
        result.insert(result.end(), trackBits.begin(), trackBits.end());
    }

    assert(isize(result.size()) == traits.numBlocks());
    return result;
}

void
FSAllocator::writeBitmap(TrackNr t, const std::vector<bool> &bitmap)
{
    auto numSectors = traits.numSectors(t);
    auto &bam       = fs.fetchBAM().mutate();
    auto *data      = bam.data();
    auto *entry     = data + 4 + (4 * t);
    auto *allocBits = entry + 1;
    auto freeCount  = 0;

    assert(isize(bitmap.size()) == numSectors);

    // Clear bitmap bytes
    allocBits[0] = allocBits[1] = allocBits[2] = 0;

    for (SectorNr s = 0; s < numSectors; ++s) {

        if (bitmap[s]) {

            auto byteIndex = s / 8; // 0..2
            auto bitIndex  = s % 8; // LSB first

            allocBits[byteIndex] |= (1 << bitIndex);
            ++freeCount;
        }
    }

    entry[0] = u8(freeCount);
}

void
FSAllocator::writeBitmap(const std::vector<bool> &bitmap)
{
    assert(isize(bitmap.size()) == traits.numBlocks());

    auto numTracks = traits.numTracks();
    isize index = 0;

    for (TrackNr t = 0; t < numTracks; ++t) {

        auto numSectors = traits.numSectors(t);

        std::vector<bool> trackBits;
        trackBits.reserve(numSectors);

        for (SectorNr s = 0; s < numSectors; ++s)
            trackBits.push_back(bitmap[index++]);

        writeBitmap(t, trackBits);
    }

    assert(index == isize(bitmap.size()));
}

}
