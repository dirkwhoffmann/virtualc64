// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "BlockDevice.h"

namespace retro::vault {

isize
BlockDevice::capacity() const
{
    return size() / bsize();
}

void
BlockDevice::readBlocks(u8 *dst, Range<isize> range) const
{
    assert(range.subset(Range<isize>(0, capacity())));
    
    if (range.size() > 0)
        read(dst, range.lower * bsize(), range.size() * bsize());
}

void
BlockDevice::writeBlocks(const u8 *src, Range<isize> range)
{
    assert(range.subset(Range<isize>(0, capacity())));

    if (range.size() > 0)
        write(src, range.lower * bsize(), range.size() * bsize());
}

void
BlockDevice::readBlock(span<u8> dst, isize nr) const
{
    assert(bsize() <= (isize)dst.size());
    readBlock(dst.data(), nr);
}

void
BlockDevice::writeBlock(span<const u8> src, isize nr)
{
    assert(bsize() <= (isize)src.size());
    writeBlock(src.data(), nr);
}

void
BlockDevice::exportBlock(const fs::path& path, isize nr) const
{
    exportBlocks(path, Range(nr, nr + 1));
}

void
BlockDevice::exportBlocks(const fs::path& path, Range<isize> range) const
{
    exportBytes(path, Range(range.lower * bsize(), range.upper * bsize()));
}

}
