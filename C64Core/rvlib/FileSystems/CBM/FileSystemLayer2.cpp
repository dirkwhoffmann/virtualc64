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
#include <cstring>
#include <span>

namespace retro::vault::cbm {

void
FileSystem::format(FSFormat dos) {

    // Assign the new DOS type
    traits.dos = dos;
    if (dos == FSFormat::NODOS) return;

    // Perform some consistency checks
    assert(blocks() > 0);

    // Start with an empty block device
    for (isize i = 0; i < traits.blocks; i++) {
        (*this)[i].mutate().init(FSBlockType::EMPTY);
    }

    // Create the BAM
    formatBAM(dos, PETName<16>(""));

    // Create the first directory block
    (*this)[bam() + 1].mutate().init(FSBlockType::DIR);
}

void
FileSystem::formatBAM(FSFormat dos, const PETName<16> &name) {

    auto &block = (*this)[bam()].mutate();
    block.init(FSBlockType::BAM);

    // Obtain write access for the data area
    auto *data = (*this)[bam()].mutate().data();

    // Location of the first directory sector
    data[0x00] = 18;
    data[0x01] = 1;

    // Disk DOS version type ('A')
    data[0x02] = 0x41;

    // Unused
    data[0x03] = 0x00;

    // BAM entries for each track (in groups of four bytes)
    for (TrackNr k = 1; k <= 35; k++) {

        u8 *p = data + 4 * k;

        if (k == 18) {

            p[0] = 17;      // 17 out of 19 blocks are free
            p[1] = 0xFC;    // Mark first two blocks as allocated
            p[2] = 0xFF;
            p[3] = 0x07;

        } else {

            p[0] = (u8)traits.numSectors(k);
            p[1] = 0xFF;
            p[2] = 0xFF;
            p[3] = p[0] == 21 ? 0x1F : p[0] == 19 ? 0x07 : p[0] == 18 ? 0x03 : 0x01;
        }
    }

    // Disk Name (padded with $A0)
    name.write(data + 0x90);

    // Filled with $A0
    data[0xA0] = 0xA0;
    data[0xA1] = 0xA0;

    // Disk ID
    data[0xA2] = 0x56;
    data[0xA3] = 0x54;

    // Usually $A0
    data[0xA4] = 0xA0;

    // DOS type
    data[0xA5] = 0x32;  // "2"
    data[0xA6] = 0x41;  // "A"

    // Filled with $A0
    data[0xA7] = 0xA0;
    data[0xA8] = 0xA0;
    data[0xA9] = 0xA0;
    data[0xAA] = 0xA0;
}

void
FileSystem::setName(const PETName<16> &name)
{
    if (auto bam = tryFetchBAM())
        bam->mutate().setName(name);
}

optional<FSDirEntry>
FileSystem::searchDir(const PETName<16> &name) const
{
    for (auto &entry: readDir()) {

        if (entry.getName() == name)
            return entry;
    }
    return {};
}

vector<FSDirEntry>
FileSystem::searchDir(const FSPattern &pattern) const
{
    vector<FSDirEntry> result;

    for (auto &entry: readDir()) {

        if (!entry.deleted() && pattern.match(entry.getName().str()))
           result.push_back(entry);
    }
    return result;
}

void
FileSystem::link(const FSDirEntry &entry)
{
    auto dir = readDir();

    // Find a free slot
    for (auto &slot : dir) {

        if (slot.deleted()) {

            slot = entry;
            writeDir(dir);
            return;
        }
    }

    // Append at the end
    dir.push_back(entry);
    writeDir(dir);
}

void
FileSystem::unlink(BlockNr node)
{
    if (auto ts = traits.tsLink(node)) {

        auto dirBlocks = collectDirBlocks();

        for (auto &b : dirBlocks) {

            forEachDirEntry(b, [&](FSDirEntry *e) {
                if (e->firstDataTrack == ts->t && e->firstDataSector == ts->s) *e = {};
            });
        }
    }
}

vector<FSDirEntry>
FileSystem::readDirBlock(BlockNr block) const
{
    vector<FSDirEntry> entries;
    entries.reserve(8);

    auto *data = fetch(block).data();

    // Each directory block contains 8 directory entries
    for (int i = 0; i < 8; i++) {
        entries.emplace_back(std::span(data + i * 0x20, 0x20));
    }

    return entries;
}

vector<FSDirEntry>
FileSystem::readDir() const
{
    auto dirBlocks = collectDirBlocks();

    vector<FSDirEntry> result;
    result.reserve(dirBlocks.size() * 8);

    for (auto block : dirBlocks) {

        auto entries = readDirBlock(block);
        result.insert(result.end(), entries.begin(), entries.end());
    }

    return result;
}

void
FileSystem::writeDir(const vector<FSDirEntry> &dir)
{
    static constexpr u8 interleave[18] = {
        1, 4, 7, 10, 13, 16, 2, 5, 8, 11, 14, 17, 3, 6, 9, 12, 15, 18
    };

    // A directory contains up to 144 files
    if (dir.size() > 144) throw FSError(FSError::FS_OUT_OF_SPACE);

    // Compute the number of required directory blocks
    auto numDirBlocks = (dir.size() + 7) / 8;

    for (usize b = 0, i = 0; b < numDirBlocks; ++b) {

        auto &block = fetch(TSLink{18,interleave[b]}).mutate();
        auto *data  = block.data();

        for (isize j = 0; j < 8; ++j, ++i) {

            // Write directory entry
            FSDirEntry *entry = (FSDirEntry *)data + j;
            if (i < dir.size()) {
                memcpy(entry, &dir[i], sizeof(FSDirEntry));
            } else {
                entry = {};
            }
        }

        // Write TS link
        data[0] = (b + 1 < numDirBlocks) ? 18 : 0;
        data[1] = (b + 1 < numDirBlocks) ? interleave[b + 1] : 0;
    }
}

isize
FileSystem::numItems() const
{
    isize result = 0;

    for (auto &slot : readDir())
        result += slot.deleted() ? 0 : 1;

    return result;
}

BlockNr
FileSystem::createFile(const PETName<16> &name)
{
    return createFile(name, nullptr, 0);
}

BlockNr
FileSystem::createFile(const PETName<16> &name, const u8 *buf, isize size)
{
    // Allocate required blocks
    auto blocks = allocator.allocate(std::max(isize(1), (size + 253) / 254));
    auto first  = traits.tsLink(blocks[0]);

    // Add data
    if (buf) replace(blocks, buf, size);

    // Create a directory entry
    FSDirEntry entry;
    entry.setName(name);
    entry.firstDataTrack  = u8(first->t);
    entry.firstDataSector = u8(first->s);
    entry.fileSizeHi      = HI_BYTE(blocks.size());
    entry.fileSizeLo      = LO_BYTE(blocks.size());
    entry.fileType        = 0xA2; // PRG

    // Add the file to the directory
    link(entry);

    return blocks[0];
}

BlockNr
FileSystem::createFile(const PETName<16> &name, const Buffer<u8> &buf)
{
    return createFile(name, buf.ptr, buf.size);
}

BlockNr
FileSystem::createFile(const PETName<16> &name, const string &str)
{
    return createFile(name, (const u8 *)str.c_str(), (isize)str.size());
}

void
FileSystem::rm(BlockNr node)
{
    // Remove the file from the directory
    unlink(node);

    // Reclaim all associated storage blocks
    reclaim(node);
}

void
FileSystem::rm(const PETName<16> file)
{
    rm(seek(file));
}

void
FileSystem::rename(const PETName<16> &src, const PETName<16> &dst)
{
    auto dirBlocks = collectDirBlocks();

    for (auto &b : dirBlocks) {

        forEachDirEntry(b, [&](FSDirEntry *entry) {

            if (entry->getName() == src)
                entry->setName(dst);
        });
    }
}

isize
FileSystem::extractData(BlockNr b, Buffer<u8> &buf) const
{
    auto blocks = collectDataBlocks(b);
    if (blocks.empty()) return 0;

    // Compute total size
    isize byteCount = 0;
    for (auto blockNr : blocks) {
        byteCount += isize(fetch(blockNr).dataSection().size());
    }

    // Resize target buffer
    buf.resize(byteCount);

    // Copy data
    u8 *p = buf.ptr;
    for (auto blockNr : blocks) {

        auto chunk = fetch(blockNr).dataSection();
        memcpy(p, chunk.data(), chunk.size());
        p += chunk.size();
    }

    return byteCount;
}

isize
FileSystem::extractData(TSLink ts, Buffer<u8> &buf) const
{
    auto b = traits.blockNr(ts);
    return b ? extractData(*b, buf) : 0;
}

isize
FileSystem::extractData(const FSDirEntry &entry, Buffer<u8> &buf) const
{
    return extractData(entry.firstBlock(), buf);
}

void
FileSystem::resize(BlockNr at, isize size)
{
    // Extract file data
    Buffer<u8> buffer; extractData(at, buffer);

    // Adjust size (pads with zero when growing)
    buffer.resize(size, 0);

    // Write resized data back
    replace(at, buffer);
}

void
FileSystem::replace(BlockNr at, const u8 *buf, isize size)
{
    // Collect all blocks occupied by this file
    auto blocks = collectDataBlocks(at);

    // Compute how many blocks we need
    auto needed = allocator.requiredBlocks(size);

    // Allocate additional blocks if necessary
    if (needed > isize(blocks.size())) {

        auto more = allocator.allocate(needed - isize(blocks.size()));
        blocks.insert(blocks.end(), more.begin(), more.end());
    }

    // Update the file contents
    replace(blocks, buf, size);
}

void
FileSystem::replace(BlockNr at, const Buffer<u8> &data)
{
    replace(at, data.ptr, data.size);
}

void
FileSystem::replace(BlockNr at, const string &str)
{
    replace(at, (const u8 *)str.c_str(), (isize)str.size());
}

void
FileSystem::replace(std::vector<BlockNr> blocks, const u8 *buf, isize size)
{
    for (usize i = 0; i < blocks.size() && size > 0; ++i) {

        auto &block  = fetch(blocks[i]).mutate();
        auto *data   = block.data();
        auto written = std::min(size, isize(254));

        // Mark the block as a data block
        block.type = FSBlockType::DATA;

        // Write payload
        std::memcpy(data + 2, buf, written);

        buf  += written;
        size -= written;

        if (i < blocks.size() - 1) {

            // Intermediate block: TS link
            auto ts = traits.tsLink(blocks[i + 1]);
            data[0] = u8(ts->t);
            data[1] = u8(ts->s);

        } else {

            // Last block: Byte count
            data[0] = 0;
            data[1] = u8(written);
        }
    }

    assert(size == 0);
}

void
FileSystem::reclaim(BlockNr b)
{
    for (auto &block : collectDataBlocks(b)) {

        allocator.markAsFree(block);
        cache.erase(block);
    }
}

optional<BlockNr>
FileSystem::nextBlock(BlockNr b) const noexcept
{
    if (auto *block = tryFetch(b))
        return traits.blockNr(block->tsLink());

    return {};
}

optional<TSLink>
FileSystem::nextBlock(TSLink ts) const noexcept
{
    if (auto *block = tryFetch(ts))
        return block->tsLink();

    return {};
}

vector<BlockNr>
FileSystem::collectDirBlocks() const
{
    return collect(bam() + 1, [&](const FSBlock *node) {
        return tryFetch(node->tsLink());
    });
}

vector<BlockNr>
FileSystem::collectDataBlocks(BlockNr ref) const
{
    return collect(ref, [&](const FSBlock *node) {
        return tryFetch(node->tsLink());
    });
}

vector<BlockNr>
FileSystem::collectDataBlocks(const FSDirEntry &entry) const
{
    if (auto b = traits.blockNr(entry.firstBlock()))
        return collectDataBlocks(*b);

    return {};
}

std::vector<const FSBlock *>
FileSystem::collect(const FSBlock &node, BlockIterator succ) const noexcept
{
    std::vector<const FSBlock *> result;
    std::unordered_set<BlockNr> visited;

    for (auto *block = tryFetch(node.nr); block != nullptr; block = succ(block)) {

        // Break the loop if this block has been visited before
        if (visited.contains(block->nr)) break;

        // Add the block
        result.push_back(block);

        // Remember the block as visited
        visited.insert(block->nr);
    }

    return result;
}

std::vector<BlockNr>
FileSystem::collect(const BlockNr nr, BlockIterator succ) const noexcept
{
    std::vector<BlockNr> result;
    std::unordered_set<BlockNr> visited;

    for (auto *block = tryFetch(nr); block; block = succ(block)) {

        // Break the loop if this block has been visited before
        if (visited.contains(block->nr)) break;

        // Add the block
        result.push_back(block->nr);

        // Remember the block as visited
        visited.insert(block->nr);
    }

    return result;
}

}
