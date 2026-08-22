// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "LinearDevice.h"

namespace retro::vault {

class BlockDevice : public LinearDevice {

public:

    BlockDevice() { }
    virtual ~BlockDevice() = default;

    // Block size in bytes
    virtual isize bsize() const = 0;

    // Number of blocks
    virtual isize capacity() const;

    // Reads a single block or a range of blocks
    virtual void readBlock(u8 *dst, isize nr) const { readBlocks(dst, utl::Range{nr,nr+1}); }
    virtual void readBlocks(u8 *dst, utl::Range<isize> range) const;

    // Writes a block
    virtual void writeBlock(const u8 *src, isize nr) { writeBlocks(src, utl::Range{nr,nr+1}); };
    virtual void writeBlocks(const  u8 *src, utl::Range<isize> range);

    // Safety wrappers
    void readBlock(span<u8> dst, isize nr) const;
    void writeBlock(span<const u8> src, isize nr);

    // Exports a single block or a block range to a file
    void exportBlock(const fs::path& path, isize nr) const;
    void exportBlocks(const fs::path& path, utl::Range<isize> range) const;
};

}
