// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DiskImage.h"

namespace retro::vault {

class HardDiskImage : public DiskImage {

public:

    // Static functions
    static optional<ImageInfo> about(const fs::path& url);
    static unique_ptr<HardDiskImage> tryMake(const fs::path &path);
    static unique_ptr<HardDiskImage> make(const fs::path &path);

    // Informs about the contained partitions
    virtual isize numPartitions() const = 0;
    virtual Range<isize> partition(isize nr) const = 0;

    // Exports a single partition
    isize writePartitionToStream(std::ostream &stream, isize nr) const;
    isize writePartitionToFile(const fs::path &path, isize nr) const;
};

}
