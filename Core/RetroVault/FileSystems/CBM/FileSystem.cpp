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
#include "utl/io.h"
#include "utl/support/Strings.h"
#include <climits>
#include <unordered_set>
#include <stack>
#include <algorithm>

namespace retro::vault::cbm {

FileSystem::FileSystem(Volume &vol) : cache(*this, vol)
{
    loginfo(FS_DEBUG, "Creating file system...\n");

    // Check consistency
    if (vol.capacity() != 683 && vol.capacity() != 768 && vol.capacity() != 802)
        throw FSError(FSError::FS_WRONG_CAPACITY);

    if (vol.bsize() != 256)
        throw FSError(FSError::FS_WRONG_BSIZE);

    // Derive persistant file system properties
    traits.init(cache.predictDOS(vol), vol.capacity());

    if constexpr (debug::FS_DEBUG) dumpStatfs();
    loginfo(FS_DEBUG, "Success\n");
}

void
FileSystem::dumpStatfs(std::ostream &os) const noexcept
{
    using namespace utl;

    auto st = stat();
    auto size = std::to_string(traits.blocks) + " (x " + std::to_string(traits.bsize) + ")";

    os << "Type   Size             Used    Free    Full  Name" << std::endl;

    if (isFormatted()) {

        auto fill = 100.0 * st.usedBlocks / st.blocks;

        os << std::setw(5) << std::left << FSFormatEnum::key(traits.dos);
        os << "  ";
        os << std::setw(15) << std::left << std::setfill(' ') << size;
        os << "  ";
        os << std::setw(6) << std::left << std::setfill(' ') << st.usedBlocks;
        os << "  ";
        os << std::setw(6) << std::left << std::setfill(' ') << st.freeBlocks;
        os << "  ";
        os << std::setw(3) << std::right << std::setfill(' ') << isize(fill);
        os << "%  ";
        os << st.name.c_str() << std::endl;

    } else {

        os << std::setw(5) << std::left << "NODOS";
        os << "  ";
        os << std::setw(15) << std::left << std::setfill(' ') << size;
        os << "  ";
        os << std::setw(6) << std::left << std::setfill(' ') << "--";
        os << "  ";
        os << std::setw(6) << std::left << std::setfill(' ') << "--";
        os << "  ";
        os << std::setw(3) << std::left << std::setfill(' ') << "--";
        os << "   ";
        os << "--" << std::endl;
    }
}

void
FileSystem::dumpProps(std::ostream &os) const noexcept
{
    using namespace utl;

    auto st   = stat();
    auto fill = 100.0 * st.usedBlocks / st.blocks;

    os << tab("Name");
    os << st.name << std::endl;
    os << tab("Capacity");
    os << utl::byteCountAsString(traits.blocks * traits.bsize) << std::endl;
    os << tab("Block size");
    os << dec(traits.bsize) << " Bytes" << std::endl;
    os << tab("Blocks");
    os << dec(traits.blocks) << std::endl;
    os << tab("Used");
    os << dec(st.usedBlocks);
    os << tab("Free");
    os << dec(st.freeBlocks);
    os << " (" <<  std::fixed << std::setprecision(2) << fill << "%)" << std::endl;
    os << tab("BAM");
    os << dec(bam()) << std::endl;
}

void
FileSystem::dumpBlocks(std::ostream &os) const noexcept
{
    cache.dump(os);
}

bool
FileSystem::isFormatted() const noexcept
{
    // Check the DOS type
    if (traits.dos == FSFormat::NODOS) return false;

    // Check if the BAM is present
    return fetch({18,0}).is(FSBlockType::BAM);
}

FSStat
FileSystem::stat() const noexcept
{
    auto &bam = fetch({18,0});

    FSStat result = {

        .name           = bam.getName().str(),
        .bsize          = traits.bsize,
        .blocks         = traits.blocks,
        .freeBlocks     = allocator.numUnallocated(),
        .usedBlocks     = allocator.numAllocated(),
        .cachedBlocks   = cache.cachedBlocks(),
        .dirtyBlocks    = cache.dirtyBlocks(),
        .generation     = generation
    };

    return result;
}


FSAttr
FileSystem::attr(const FSDirEntry &entry) const
{
    auto blocks     = collectDataBlocks(entry);
    isize numBlocks = isize(blocks.size());
    isize numBytes  = 0;

    if (numBlocks) {

        // All blocks except the last one contain 254 bytes
        numBytes = (numBlocks - 1) * 254;

        // Add the byte count of the last block (encoded in the sector field)
        numBytes += fetch(blocks.back()).data()[1];
    }

    return FSAttr {

        .size   = numBytes,
        .blocks = numBlocks,
        .isDir  = false
    };
}

optional<FSAttr>
FileSystem::attr(const PETName<16> &name) const
{
    if (auto item = searchDir(name)) {
        return attr(*item);
    }

    return {};
}

optional<FSAttr>
FileSystem::attr(const fs::path &path) const
{
    if (path == "/") {
        
        return FSAttr {
            
            .size   = 0,
            .blocks = 0,
            .isDir  = true
        };
    }
    
    return attr(PETName<16>(path));
}

}
