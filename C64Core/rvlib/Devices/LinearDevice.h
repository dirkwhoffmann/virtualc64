// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DeviceTypes.h"
#include "utl/common.h"
#include "utl/primitives/Range.h"

namespace retro::vault {

using namespace utl;

class LinearDevice {

public:

    LinearDevice() { }
    virtual ~LinearDevice() = default;

    // Returns a textual description of the file system
    virtual vector<string> describeDevice() const noexcept;

    // Total size in bytes
    virtual isize size() const = 0;

    // Reads a sequence of bytes starting at byte offset
    virtual void read(u8 *dst, isize offset, isize count) const = 0;

    // Writes a sequence of bytes starting at byte offset
    virtual void write(const u8 *src, isize offset, isize count) = 0;

    // Reads a single byte
    virtual u8 readByte(isize offset) const;

    // Writes a single byte
    virtual void writeByte(isize offset, u8 value);

    // Exports all bytes or a selected range to a file
    void exportBytes(const fs::path& path) const;
    void exportBytes(const fs::path& path, Range<isize> range) const;
};

}
