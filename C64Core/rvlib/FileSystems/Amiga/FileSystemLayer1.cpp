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

namespace retro::vault::amiga {

FSBlockType
FileSystem::predictType(BlockNr nr, const u8 *buf) const noexcept
{
    if (buf) {

        // Is it a boot block?
        if (nr == 0 || nr == 1) return FSBlockType::BOOT;

        // Is it a bitmap block?
        if (std::find(bmBlocks.begin(), bmBlocks.end(), nr) != bmBlocks.end())
            return FSBlockType::BITMAP;

        // Is it a bitmap extension block?
        if (std::find(bmExtBlocks.begin(), bmExtBlocks.end(), nr) != bmExtBlocks.end())
            return FSBlockType::BITMAP_EXT;

        // For all other blocks, check the type and subtype fields
        u32 type = FSBlock::read32(buf);
        u32 subtype = FSBlock::read32(buf + traits.bsize - 4);

        if (type == 2  && subtype == 1)       return FSBlockType::ROOT;
        if (type == 2  && subtype == 2)       return FSBlockType::USERDIR;
        if (type == 2  && subtype == (u32)-3) return FSBlockType::FILEHEADER;
        if (type == 16 && subtype == (u32)-3) return FSBlockType::FILELIST;

        // Check if this block is a data block
        if (traits.ofs()) {
            if (type == 8) return FSBlockType::DATA_OFS;
        } else {
            for (isize i = 0; i < traits.bsize; i++) if (buf[i]) return FSBlockType::DATA_FFS;
        }
    }
    
    return FSBlockType::EMPTY;
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
