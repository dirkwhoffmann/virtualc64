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

namespace retro::vault::cbm {

FSBlockType
FileSystem::predictType(BlockNr nr, const u8 *buf) const noexcept
{
    assert(traits.isValidBlock(nr));

    if (buf) {

        auto ts = traits.tsLink(nr);

        // Track 18 contains the BAM and the directory blocks
        if (ts->t == 18) return ts->s == 0 ? FSBlockType::BAM : FSBlockType::DIR;

        // Check if this block is a data block
        for (isize i = 0; i < traits.bsize; i++) if (buf[i]) return FSBlockType::DATA;
    }

    return FSBlockType::EMPTY;
}

const FSBlock *
FileSystem::tryFetch(TSLink ts) const noexcept
{
    if (auto b = traits.blockNr(ts))
        return tryFetch(*b);

    return nullptr;
}

const FSBlock *
FileSystem::tryFetch(TSLink ts, FSBlockType t) const noexcept
{
    if (auto b = traits.blockNr(ts))
        return tryFetch(*b, t);

    return nullptr;
}

const FSBlock &
FileSystem::fetch(TSLink ts) const
{
    if (auto b = traits.blockNr(ts))
        return fetch(*b);

    throw FSError(FSError::FS_OUT_OF_RANGE,
                  "{" + std::to_string(ts.t) + ":" + std::to_string(ts.s) + "}");
}

const FSBlock &
FileSystem::fetch(TSLink ts, FSBlockType t) const
{
    if (auto b = traits.blockNr(ts))
        return fetch(*b, t);

    throw FSError(FSError::FS_OUT_OF_RANGE,
                  "{" + std::to_string(ts.t) + ":" + std::to_string(ts.s) + "}");
}

void
FileSystem::flush()
{
    cache.flush();
}

void
FileSystem::invalidate()
{
    cache.invalidate();
}

}
