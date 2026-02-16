// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/io.h"
#include <fstream>

namespace retro::vault::amiga {

using namespace utl;

void
FSImporter::importVolume(const u8 *src, isize size)
{
    assert(src != nullptr);

    loginfo(FS_DEBUG, "Importing file system...\n");

    // Only proceed if the (predicted) block size matches
    if (size % traits.bsize != 0) throw FSError(FSError::FS_WRONG_BSIZE);

    // Only proceed if the source buffer contains the right amount of data
    if (traits.bytes != size) throw FSError(FSError::FS_WRONG_CAPACITY);

    // Only proceed if all partitions contain a valid file system
    if (traits.dos == FSFormat::NODOS) throw FSError(FSError::FS_UNSUPPORTED);

    // Import all blocks
    for (isize i = 0; i < fs.blocks(); i++) {

        const u8 *data = src + i * traits.bsize;

        // Determine the type of the new block
        if (FSBlockType type = fs.predictType((BlockNr)i, data); type != FSBlockType::EMPTY) {

            // Create new block
            auto &block = fs.fetch(BlockNr(i)).mutate();
            block.init(type);
            block.importBlock(data, traits.bsize);
        }
    }

    // Print some debug information
    loginfo(FS_DEBUG, "Success\n");
}

void
FSImporter::import(const fs::path &path, bool recursive, bool contents)
{
    import(fs.pwd(), path, recursive, contents);
}

void
FSImporter::import(BlockNr top, const fs::path &path, bool recursive, bool contents)
{
    fs::directory_entry dir;

    // Get the directory item
    try { dir = fs::directory_entry(path); } catch (...) {
        // throw IOError(IOError::FILE_CANT_READ);
        throw IOError(IOError::FILE_CANT_READ, path);
    }

    if (dir.is_directory() && contents) {

        // Add the directory contents
        for (const auto& it : fs::directory_iterator(dir)) import(top, it, recursive);

    } else {

        // Add the file or directory as a whole
        import(top, dir, recursive);
    }

    // Rectify the checksums of all blocks
    // fs.importer.updateChecksums();

    // Verify the result
    if constexpr (debug::FS_DEBUG)
        fs.doctor.xray(true, std::cout, false);
}

void
FSImporter::import(BlockNr top, const fs::directory_entry &entry, bool recursive)
{
    auto isHidden = [&](const fs::path &path) {

        string s = path.filename().string();
        return !s.empty() && s[0] == '.';
    };

    const auto path = entry.path().string();
    const auto name = entry.path().filename();
    FSName fsname = FSName(name);

    // Skip hidden files
    if (isHidden(name)) return;

    if (entry.is_regular_file()) {

        loginfo(FS_DEBUG, "  Importing file %s\n", path.c_str());

        Buffer<u8> buffer(entry.path());
        if (buffer) {
            fs.createFile(top, fsname, buffer.ptr, buffer.size);
        } else {
            fs.createFile(top, fsname);
        }

    } else {

        loginfo(FS_DEBUG, "Importing directory %s\n", fsname.c_str());

        // Create new directory
        auto subdir = fs.mkdir(top, fsname);
        // auto &subdir = fs.mkdir(top, fsname);

        // Import all items
        for (const auto& it : fs::directory_iterator(entry)) {

            if (it.is_regular_file() || recursive) import(subdir, it, recursive);
        }
    }
}

void
FSImporter::importBlock(BlockNr nr, const fs::path &path)
{
    std::ifstream stream(path, std::ios::binary);

    if (!stream.is_open()) {
        throw IOError(IOError::FILE_CANT_READ, path);
    }

    auto &block = fs.fetch(nr).mutate();
    stream.read((char *)block.data(), traits.bsize);

    if (!stream) {
        throw IOError(IOError::FILE_CANT_READ, path);
    }
}

}
