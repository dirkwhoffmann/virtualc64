// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "HardDiskImage.h"
#include "HDFFile.h"
#include "HDZFile.h"
#include "utl/io.h"

using retro::vault::image::HDFFile;
using retro::vault::image::HDZFile;

namespace retro::vault {

optional<ImageInfo>
HardDiskImage::about(const fs::path& url)
{
    if (auto info = HDFFile::about(url)) return info;
    if (auto info = HDZFile::about(url)) return info;

    return {};
}

unique_ptr<HardDiskImage>
HardDiskImage::tryMake(const fs::path &path)
{
    if (HDFFile::about(path).has_value()) return make_unique<HDFFile>(path);
    if (HDZFile::about(path).has_value()) return make_unique<HDZFile>(path);

    return nullptr;
}

unique_ptr<HardDiskImage>
HardDiskImage::make(const fs::path &path)
{
    if (auto img = tryMake(path)) return img;
    throw IOError(IOError::FILE_TYPE_UNSUPPORTED);
}

isize
HardDiskImage::writePartitionToStream(std::ostream &stream, isize nr) const
{
    auto range = partition(nr);
    return writeToStream(stream, range.lower, range.size());
}

isize
HardDiskImage::writePartitionToFile(const fs::path &path, isize nr) const
{
    auto range = partition(nr);
    return writeToFile(path, range.lower, range.size());
}

}
