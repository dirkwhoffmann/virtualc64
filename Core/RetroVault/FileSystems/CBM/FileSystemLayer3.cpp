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

namespace retro::vault::cbm {

optional<BlockNr>
FileSystem::trySeek(const PETName<16> &path) const
{
    if (auto entry = searchDir(path))
        return traits.blockNr(entry->firstBlock());

    return {};
}

BlockNr
FileSystem::seek(const PETName<16> &path) const
{
    if (auto it = trySeek(path)) return *it;
    throw FSError(FSError::FS_NOT_FOUND, path.str());
}

}
