// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FileSystems/CBM/FSService.h"
#include "Devices/TrackDevice.h"

namespace retro::vault::cbm {

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

    // Exports one or more files to the host system
    void exportFiles(const FSPattern &pattern, const fs::path &path) const;

private:

    void exportFile(const FSDirEntry &entry, const fs::path &path) const;

};

}
