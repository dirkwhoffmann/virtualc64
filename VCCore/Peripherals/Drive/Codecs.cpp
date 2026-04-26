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

#include "config.h"
#include "Codecs.h"
#include "DiskEncoder.h"
#include "FileSystems/CBM/FileSystem.h"
#include "FloppyDisk.h"
#include "Drive.h"

using namespace retro::vault;

namespace vc64 {

std::unique_ptr<image::D64File>
Codec::makeD64(FloppyDisk &disk)
{
    // auto d64 = make_unique<D64File>(disk.getDiameter(), disk.getDensity());
    auto d64 = make_unique<image::D64File>(image::D64File::D64_683_SECTORS);
    
    auto size = disk.decodeDisk(nullptr);
    printf("size = %ld d64.size = %ld\n", size, d64->data.size);
    
    disk.decodeDisk(d64->data.ptr);
    return d64;
}

std::unique_ptr<image::D64File>
Codec::makeD64(Drive &drive)
{
    if (drive.disk == nullptr) throw DeviceError(DeviceError::DSK_MISSING);
    return makeD64(*drive.disk);
}

}
