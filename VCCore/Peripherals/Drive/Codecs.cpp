// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "vcconfig.h"
#include "Codecs.h"
#include "DiskEncoder.h"
#include "FileSystems/CBM/FileSystem.h"
#include "FloppyDisk.h"
#include "Drive.h"

using namespace retro::vault;

namespace vc64 {

std::unique_ptr<D64File>
Codec::makeD64(FloppyDisk &disk)
{
    // Find out how many bytes the decoder is going to produce (test run)
    auto size = disk.decodeDisk(nullptr);

    // The decoder handles 35, 40, or 42 tracks, so pick the smallest image
    // type that is able to hold the result
    auto capacity = D64File::D64_683_SECTORS;
    if (size > capacity) capacity = D64File::D64_768_SECTORS;
    if (size > capacity) capacity = D64File::D64_802_SECTORS;
    assert(size <= capacity);

    auto d64 = make_unique<D64File>(capacity);

    disk.decodeDisk(d64->mutableByteView(0, d64->getSize()).data());
    return d64;
}

std::unique_ptr<D64File>
Codec::makeD64(Drive &drive)
{
    if (drive.disk == nullptr) throw DeviceError(DeviceError::DSK_MISSING);
    return makeD64(*drive.disk);
}

}
