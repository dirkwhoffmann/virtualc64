// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "BlockDevice.h"
#include "DeviceDescriptors.h"
#include "utl/primitives.h"

namespace retro::vault {

class Volume : public BlockDevice {

    // The underlying block device
    class BlockDevice &device;

    // Blocks belonging to this volume
    Range<isize> range;

public:

    // Access statistics (transferred bytes)
    mutable i64 reads = 0;
    mutable i64 writes = 0;


    //
    // Methods
    //

public:

    Volume(BlockDevice &d);
    Volume(BlockDevice &d, Range<isize> partition);
    virtual ~Volume() = default;

    Range<isize> getRange() { return range; }

    
    //
    // Methods from LinearDevice
    //

public:

    isize size() const override { return capacity() * bsize(); }
    void read(u8 *dst, isize offset, isize count) const override;
    void write(const u8 *src, isize offset, isize count) override;

    
    //
    // Methods from BlockDevice
    //

public:

    isize capacity() const override { return range.size(); }
    isize bsize() const override { return device.bsize(); }
    void readBlocks(u8 *dst, Range<isize> range) const override;
    void writeBlocks(const u8 *src, Range<isize> range) override;
};

}
