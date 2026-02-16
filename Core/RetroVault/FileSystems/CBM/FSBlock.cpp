// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/CBM/FSBlock.h"
#include "FileSystems/CBM/FileSystem.h"
#include "utl/io.h"
#include "utl/support.h"
#include <algorithm>
#include <fstream>

namespace retro::vault::cbm {

FSBlock::FSBlock(FileSystem *ref, BlockNr nr) : fs(ref), cache(ref->cache)
{
    this->nr = nr;
}

FSBlock::FSBlock(FileSystem *ref, BlockNr nr, FSBlockType t) : FSBlock(ref, nr)
{
    init(t);
}

FSBlock::~FSBlock()
{

}

void
FSBlock::init(FSBlockType t)
{
    type = t;
    dataCache.clear();

    switch (type) {

        case FSBlockType::EMPTY:

            break;

        case FSBlockType::UNKNOWN:

            break;

        case FSBlockType::BAM:

            break;

        case FSBlockType::DIR:

            break;

        case FSBlockType::DATA:

            break;
    }
}

std::vector<BlockNr>
FSBlock::refs(const std::vector<const FSBlock *> blocks)
{
    std::vector<BlockNr> result;
    for (auto &it : blocks) { if (it) result.push_back(it->nr); }
    return result;
}

bool
FSBlock::is(FSBlockType type) const
{
    return this->type == type;
}

bool
FSBlock::isEmpty() const
{
    return type == FSBlockType::EMPTY;
}

bool
FSBlock::isBAM() const
{
    return type == FSBlockType::BAM;
}

bool
FSBlock::isData() const
{
    return type == FSBlockType::DATA;
}

fs::path
FSBlock::sanitizedPath() const
{
    return "";
    /*
    fs::path result;

    auto nodes = fs->collect(*this, [](auto *node) { return node->getParentDirBlock(); });

    // Delete the root node
    if (!nodes.empty()) nodes.pop_back();

    for (auto &it : nodes) {

        auto name = it->getName().path();
        result = result.empty() ? name : name / result;
    }

    return result;
    */
}

bool
FSBlock::matches(const FSPattern &pattern) const
{
    return false;
    /*
    if (pattern.isAbsolute()) {
        printf("Abs matching %s and %s (%d)\n", absName().c_str(), pattern.glob.c_str(), pattern.match(absName()));
        return pattern.match(absName());
    } else {
        printf("Rel matching %s and %s (%d)\n", relName().c_str(), pattern.glob.c_str(), pattern.match(relName()));
        return pattern.match(cppName());
    }
    */
}

isize
FSBlock::bsize() const
{
    return fs->traits.bsize;
}

isize
FSBlock::dsize() const
{
    switch (type) {

        case FSBlockType::DATA:     return bsize() - 24;
        case FSBlockType::EMPTY:    return bsize();

        default:
            fatalError;
    }
}

FSItemType
FSBlock::itemType(isize byte) const
{
    switch (type) {

        case FSBlockType::BAM:

            switch (byte) {

                case 0x00: return FSItemType::FIRST_DIR_TRACK;
                case 0x01: return FSItemType::FIRST_DIR_SECTOR;
                case 0x02: return FSItemType::DOS_VERSION;
                case 0xA2: return FSItemType::DISK_ID;
                case 0xA3: return FSItemType::DISK_ID;
                case 0xA5: return FSItemType::DOS_TYPE;
                case 0xA6: return FSItemType::DOS_TYPE;
            }
            if (byte >= 0x04 && byte <= 0x8F) return FSItemType::ALLOCATION_BITS;
            if (byte >= 0x90 && byte <= 0x9F) return FSItemType::DISK_NAME;

            return FSItemType::UNUSED;

        case FSBlockType::DIR:

            if (byte == 0) return FSItemType::TRACK_LINK;
            if (byte == 1) return FSItemType::SECTOR_LINK;

            byte &= 0x1F;

            switch (byte) {

                case 0x02: return FSItemType::FILE_TYPE;
                case 0x03: return FSItemType::FIRST_FILE_TRACK;
                case 0x04: return FSItemType::FIRST_FILE_SECTOR;
                case 0x15: return FSItemType::FIRST_REL_TRACK;
                case 0x16: return FSItemType::FIRST_REL_SECTOR;
                case 0x17: return FSItemType::REL_RECORD_LENGTH;
                case 0x1E: return FSItemType::FILE_LENGTH_LO;
                case 0x1F: return FSItemType::FILE_LENGTH_HI;
            }

            if (byte >= 0x05 && byte <= 0x14) return FSItemType::FILE_NAME;
            if (byte >= 0x18 && byte <= 0x1D) return FSItemType::GEOS;

            return FSItemType::UNUSED;

        case FSBlockType::DATA:

            if (byte == 0) return FSItemType::TRACK_LINK;
            if (byte == 1) return FSItemType::SECTOR_LINK;

            return FSItemType::DATA;

        default:
            fatalError;
    }
}

u8 *
FSBlock::data()
{
    if (dataCache.empty()) {

        dataCache.alloc(bsize());
        cache.dev.readBlock(dataCache.ptr, nr);
    }

    assert(dataCache.size == bsize());
    assert(dataCache.ptr);

    return dataCache.ptr;
}

const u8 *
FSBlock::data() const
{
    return const_cast<const u8 *>(const_cast<FSBlock *>(this)->data());
}

std::span<u8>
FSBlock::dataSection()
{
    isize count = 0;

    if (isData()) {

        auto ts = tsLink();
        count = (ts.t == 0 && ts.s <= 254) ? ts.s : 254;
    }

    return std::span<u8>(data() + 2, count);
}

std::span<const u8>
FSBlock::dataSection() const
{
    return const_cast<FSBlock *>(this)->dataSection();
}

FSBlock &
FSBlock::mutate() const
{
    cache.markAsDirty(nr);
    return const_cast<FSBlock &>(*this);
}

void
FSBlock::invalidate()
{
    cache.markAsDirty(nr);
}

void
FSBlock::flush()
{
    if (!dataCache.empty()) {
        
        cache.dev.writeBlock(dataCache.ptr, nr);
    }
}

string
FSBlock::rangeString(const std::vector<BlockNr> &vec)
{
    if (vec.empty()) return "";

    // Create a sorted copy
    std::vector<BlockNr> v = vec;
    std::sort(v.begin(), v.end());

    // Replicate the last element to get the last interval right
    v.push_back(v.back());

    isize start = v[0], end = v[0];
    std::vector<string> chunks;

    for (usize i = 1; i < v.size(); i++) {

        if (v[i - 1] + 1 != v[i]) {

            if (start == end) chunks.push_back(std::to_string(start));
            else chunks.push_back(std::to_string(start) + " - " + std::to_string(end));
            start = v[i];
        }
        end = v[i];
    }

    return utl::concat(chunks, ", ");
}

void
FSBlock::importBlock(const u8 *src, isize size)
{    
    assert(src);
    assert(size == bsize());

    auto *bdata = data();

    if (bdata) {
        std::memcpy(data(), src, size);
    }
}

void
FSBlock::exportBlock(u8 *dst, isize size) const
{
    assert(dst);
    assert(size == bsize());

    auto *bdata = data();

    if (bdata) {
        std::memcpy(dst, data(), size);
    } else {
        std::memset(dst, 0, size);
    }
}

FSFault
FSBlock::exportBlock(const fs::path &path) const
{
    /*
     switch (type) {

     case FSBlockType::DIR:    return exportUserDirBlock(path);
     case FSBlockType::FILEHEADER: return exportFileHeaderBlock(path);

     default:
     return FSError::FS_OK;
     }
     */
    return FSError::FS_OK;
}

FSFault
FSBlock::exportUserDirBlock(const fs::path &path) const
{
    // Assemble the host file name
    auto filename = path / sanitizedPath();
    loginfo(FS_DEBUG >= 2, "Creating directory %s\n", filename.string().c_str());

    // Create directory
    if (!utl::createDirectory(filename)) return FSError::FS_EXPORT_ERROR;

    return FSError::FS_OK;
}

FSFault
FSBlock::exportFileHeaderBlock(const fs::path &path) const
{
    // Assemble the host file name
    auto filename = path; //  / sanitizedPath();
    loginfo(FS_DEBUG >= 2, "  Exporting file %s\n", filename.string().c_str());

    // Open file
    std::ofstream file(filename, std::ofstream::binary);
    if (!file.is_open()) return FSError::FS_EXPORT_ERROR;

    // Write data
    writeData(file);
    return FSError::FS_OK;
}

bool
FSBlock::hasName() const
{
    switch (type) {

        case FSBlockType::BAM:

            return true;

        default:

            return false;
    }
}

PETName<16>
FSBlock::getName() const
{
    switch (type) {

        case FSBlockType::BAM:

            return PETName<16>(data() + 0x90);

        default:
            return PETName<16>("");
    }
}

void
FSBlock::setName(PETName<16> name)
{
    switch (type) {

        case FSBlockType::BAM:

            name.write(data() + 0x90);

        default:
            break;
    }
}

isize
FSBlock::writeData(std::ostream &os) const
{
    auto data = dataSection();

    // Write block into buffer
    os.write((const char *)data.data(), std::streamsize(data.size()));

    return isize(data.size());
}

isize
FSBlock::writeData(std::ostream &os, isize size) const
{
    isize count = std::min(dsize(), size);
    auto *bdata = data();

    switch (type) {
            
        case FSBlockType::DATA:
            
            os.write((char *)(bdata + 2), count);
            return count;

        default:
            fatalError;
    }
}

isize
FSBlock::writeData(Buffer<u8> &buf, isize offset, isize count) const
{
    count = std::min(dsize(), count);
    auto *bdata = data();

    switch (type) {
            
        case FSBlockType::DATA:
            
            std::memcpy((void *)(buf.ptr + offset), (void *)(bdata + 22), count);
            return count;

        case FSBlockType::EMPTY:

            std::memset((void *)(buf.ptr + offset), 0, count);
            return count;

        default:
            fatalError;
    }
}

}
