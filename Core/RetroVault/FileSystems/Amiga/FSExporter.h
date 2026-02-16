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
#include "FileSystems/Amiga/FSTree.h"
#include "Devices/TrackDevice.h"

namespace retro::vault::amiga {

class FSExporter final : public FSService {

public:

    using FSService::FSService;

    // Exports the file system to a buffer
    void exportVolume(u8 *dst, isize size) const;

    // Exports the file system to a TrackDevice
    void exportVolume(TrackDevice &dev) const;

    // Exports the file system to a file
    void exportVolume(const fs::path &path) const;

    // Exports a single block or a range of blocks to a buffer
    void exportBlock(BlockNr nr, u8 *dst, isize size) const;
    void exportBlocks(BlockNr first, BlockNr last, u8 *dst, isize size) const;

    // Exports a single block or a range of blocks to a file
    void exportBlock(BlockNr nr, const fs::path &path) const;
    void exportBlocks(BlockNr first, BlockNr last, const fs::path &path) const;

    // Exports the volume to a buffer
    void exportFiles(BlockNr nr, const fs::path &path,
                     bool recursive = true, bool contents = false) const;
    void exportFiles(const fs::path &path,
                     bool recursive = true, bool contents = false) const;

private:

    // Exports a tree to the host file system
    void save(const FSTree &tree, const fs::path &path, bool recursive = true) const;
    void saveFile(const FSTree &tree, const fs::path &path, bool recursive = true) const;
    void saveDir(const FSTree &tree, const fs::path &path, bool recursive = true) const;
};

}
