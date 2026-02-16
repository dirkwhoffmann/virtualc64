// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FSCache.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/io.h"
#include <algorithm>

namespace retro::vault::amiga {

FSCache::FSCache(FileSystem &fs, Volume &v) : FSService(fs), dev(v) {

    blocks.reserve(v.capacity());
};

FSCache::~FSCache()
{
    dealloc();
}

void
FSCache::dealloc()
{
    blocks.clear();
}

void
FSCache::dump(std::ostream &os) const
{
    using namespace utl;

    os << tab("Capacity") << capacity() << " blocks (x " << bsize() << " bytes)" << std::endl;
    os << tab("Hashed blocks") << blocks.size() << std::endl;
}

FSFormat
FSCache::predictDOS(BlockDevice &dev) noexcept
{
    Buffer<u8> data(dev.bsize());

    // Analyze the signature of the first block
    dev.readBlock(data.ptr, 0);
    if (strncmp((const char *)data.ptr, "DOS", 3) == 0 && data.ptr[3] <= 7) {
        return FSFormat(data.ptr[3]);
    }

    return FSFormat::NODOS;
}

std::vector<BlockNr>
FSCache::sortedKeys() const
{
    std::vector<BlockNr> result;
    result.reserve(blocks.size());

    for (const auto& [key, _] : blocks) result.push_back(key);
    std::ranges::sort(result);

    return result;
}

bool
FSCache::isEmpty(BlockNr nr) const noexcept
{
    return getType(nr) == FSBlockType::EMPTY;
}

FSBlockType
FSCache::getType(BlockNr nr) const noexcept
{
    if (isize(nr) >= capacity()) return FSBlockType::UNKNOWN;
    return blocks.contains(nr) ? blocks.at(nr)->type : FSBlockType::EMPTY;
}

/*
void
FSCache::setType(BlockNr nr, FSBlockType type)
{
    if (isize(nr) >= capacity()) throw FSError(FSError::FS_OUT_OF_RANGE);
    blocks.at(nr)->init(type);
}
*/

FSBlock *
FSCache::cache(BlockNr nr) const noexcept
{
    if (isize(nr) >= capacity()) return nullptr;

    // Look up the block in the cache and return it if already present
    // On a miss, reserve an entry with a placeholder value
    auto [it, inserted] = blocks.try_emplace(nr, nullptr);
    if (!inserted) return it->second.get();

    // Create the block cache entry
    auto block = std::make_unique<FSBlock>(&fs, nr);
    block->dataCache.alloc(bsize());

    // Read block data from the underlying block device
    dev.readBlock(block->dataCache.ptr, nr);

    // Predict the block type based on its number and cached data
    block->type = fs.predictType(nr, block->dataCache.ptr);

    // Populate the reserved cache entry
    it->second = std::move(block);
    return it->second.get();
}

const FSBlock *
FSCache::tryFetch(BlockNr nr) const noexcept
{
    return cache(nr);
}

const FSBlock *
FSCache::tryFetch(BlockNr nr, FSBlockType type) const noexcept
{
    if (auto *ptr = tryFetch(nr); ptr) {

        if (ptr->type == type) { return ptr; }
    }
    return nullptr;
}

const FSBlock *
FSCache::tryFetch(BlockNr nr, std::vector<FSBlockType> types) const noexcept
{
    if (auto *ptr = tryFetch(nr); ptr) {

        for (auto &type: types) if (ptr->type == type) { return ptr; }
    }
    return nullptr;
}

const FSBlock &
FSCache::fetch(BlockNr nr) const
{
    if (auto *result = tryFetch(nr)) return *result;

    throw FSError(FSError::FS_OUT_OF_RANGE, std::to_string(nr));
}

const FSBlock &
FSCache::fetch(BlockNr nr, FSBlockType type) const
{
    if (auto *result = tryFetch(nr, type)) return *result;

    if (tryFetch(nr)) {
        throw FSError(FSError::FS_WRONG_BLOCK_TYPE, std::to_string(nr));
    } else {
        throw FSError(FSError::FS_OUT_OF_RANGE, std::to_string(nr));
    }
}

const FSBlock &
FSCache::fetch(BlockNr nr, std::vector<FSBlockType> types) const
{
    if (auto *result = tryFetch(nr, types); result) return *result;

    if (tryFetch(nr)) {
        throw FSError(FSError::FS_WRONG_BLOCK_TYPE, std::to_string(nr));
    } else {
        throw FSError(FSError::FS_OUT_OF_RANGE, std::to_string(nr));
    }
}

FSBlock *
FSCache::tryModify(BlockNr nr) noexcept
{
    markAsDirty(nr);
    return cache(nr);
}

FSBlock *
FSCache::tryModify(BlockNr nr, FSBlockType type) noexcept
{
    if (auto *ptr = tryModify(nr); ptr) {

        if (ptr->type == type) {

            markAsDirty(nr);
            return ptr;
        }
    }
    return nullptr;
}

FSBlock *
FSCache::tryModify(BlockNr nr, std::vector<FSBlockType> types) noexcept
{
    if (auto *ptr = tryModify(nr); ptr) {

        for (auto &type: types) {

            if (ptr->type == type) {

                markAsDirty(nr);
                return ptr;
            }
        }
    }
    return nullptr;
}

FSBlock &
FSCache::modify(BlockNr nr)
{
    if (isize(nr) >= capacity()) throw FSError(FSError::FS_OUT_OF_RANGE);
    if (auto *result = tryModify(nr)) return *result;
    throw FSError(FSError::FS_READ_ERROR);
}

FSBlock &
FSCache::modify(BlockNr nr, FSBlockType type)
{
    if (isize(nr) >= capacity()) throw FSError(FSError::FS_OUT_OF_RANGE);
    if (auto *result = tryModify(nr, type)) return *result;
    throw FSError(FSError::FS_WRONG_BLOCK_TYPE, std::to_string(nr));
}

FSBlock &
FSCache::modify(BlockNr nr, std::vector<FSBlockType> types)
{
    if (isize(nr) >= capacity()) throw FSError(FSError::FS_OUT_OF_RANGE);
    if (auto *result = tryModify(nr, types); result) return *result;
    throw FSError(FSError::FS_WRONG_BLOCK_TYPE, std::to_string(nr));
}

void
FSCache::erase(BlockNr nr)
{
    if (blocks.contains(nr)) { blocks.erase(nr); }
}

void
FSCache::markAsDirty(BlockNr nr)
{
    dirty.insert(nr);
    fs.stepGeneration();
}

void
FSCache::flush()
{
    loginfo(FS_DEBUG, "Flushing %zd dirty blocks\n", dirty.size());
    
    std::vector<u8> buffer;
    auto bs = bsize();

    // Arrage dirty blocks in segments
    auto segments = Range<BlockNr>::coalesce(dirty);
    
    for (auto seg: segments) {

        // Resize the flushing buffer
        buffer.resize(seg.size() * bs);
        
        // Gather block data
        for (isize i = seg.lower; i < seg.upper; ++i) {
            
            auto it = blocks.find(i);
            
            if (it == blocks.end())
                throw FSError(FSError::FS_CORRUPTED, "Cache mismatch: " + std::to_string(i));
            
            memcpy(buffer.data() + (i - seg.lower) * bs, it->second->data(), bs);
        }
        
        // Write the buffer back to the device
        dev.writeBlocks(buffer.data(), seg);
        
    }
    
    // Mark all blocks as up-to-date
    dirty.clear();
}

void
FSCache::invalidate()
{
    blocks.clear();
    dirty.clear();
}

}
