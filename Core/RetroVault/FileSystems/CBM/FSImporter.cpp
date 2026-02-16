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
#include <fstream>

namespace retro::vault::cbm {

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

void FSImporter::import(const fs::path &path)
{
    if (!fs::exists(path))
        throw FSError(FSError::FS_NOT_FOUND, path);

    try {

        const fs::directory_entry entry{path};

        if (entry.is_directory()) {

            for (const auto &child : fs::directory_iterator(entry))
                import(child);

        } else {

            import(entry);
        }

    } catch (const FSError &) {
        throw;
    } catch (...) {
        throw IOError(IOError::FILE_CANT_READ, path);
    }

    // Verify the result
    if constexpr (debug::FS_DEBUG) fs.doctor.xray(true, std::cout, false);
}

void
FSImporter::import(const fs::directory_entry &entry)
{
    auto isHidden = [&](const fs::path &path) {

        string s = path.filename().string();
        return !s.empty() && s[0] == '.';
    };

    const auto path = entry.path().string();
    const auto name = entry.path().filename();
    auto fsname = PETName<16>(name);

    // Skip hidden files
    if (isHidden(name)) return;

    loginfo(FS_DEBUG, "  Importing file %s\n", path.c_str());

    Buffer<u8> buffer(entry.path());
    if (buffer) {
        fs.createFile(fsname, buffer.ptr, buffer.size);
    } else {
        fs.createFile(fsname);
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
