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

namespace retro::vault::amiga {

class FSImporter final : public FSService {

public:

    using FSService::FSService;

    // Imports the volume from a buffer compatible with the ADF or HDF format
    void importVolume(const u8 *src, isize size);

    // Imports files and folders from the host file system
    void import(const fs::path &path, bool recursive = true, bool contents = false);
    void import(BlockNr top, const fs::path &path, bool recursive = true, bool contents = false);

    // Imports a single block
    void importBlock(BlockNr nr, const fs::path &path);

private:

    void import(BlockNr top, const fs::directory_entry &dir, bool recursive);
};

}
