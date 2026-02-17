// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "LinearDevice.h"
#include "utl/storage/Buffer.h"
#include <format>

namespace retro::vault {

std::vector<std::string>
LinearDevice::describeDevice() const noexcept
{
    double b  = size();
    double kb = b  / 1024.0;
    double mb = kb / 1024.0;
    double gb = mb / 1024.0;

    if (gb >= 1.0) return { std::format("{:.1f} GB", gb) };
    if (mb >= 1.0) return { std::format("{} MB", isize(mb)) };
    if (kb >= 1.0) return { std::format("{} KB", isize(kb)) };

    return { std::format("{} Bytes", b) };
}

u8
LinearDevice::readByte(isize offset) const
{
    u8 value;
    read(&value, offset, 1);

    return value;
}

void
LinearDevice::writeByte(isize offset, u8 value)
{
    write(&value, offset, 1);
}

void
LinearDevice::exportBytes(const fs::path& path) const
{
    // Read data
    auto buffer = Buffer<u8>(size());
    read(buffer.ptr, 0, size());

    // Write to file
    ByteView(span<u8>(buffer.ptr, size())).writeToFile(path);
}

void
LinearDevice::exportBytes(const fs::path& path, Range<isize> range) const
{
    assert(range.subset(Range<isize>(0,size())));

    // Read data
    auto buffer = Buffer<u8>(range.size());
    read(buffer.ptr, range.lower, range.size());

    // Write to file
    buffer.byteView().writeToFile(path);
}

}
