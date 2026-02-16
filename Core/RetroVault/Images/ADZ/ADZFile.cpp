// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "ADZFile.h"
#include "utl/chrono.h"
#include "utl/io.h"
#include "utl/support.h"

namespace retro::vault::image {

optional<ImageInfo>
ADZFile::about(const fs::path &path)
{
    auto suffix = utl::uppercased(path.extension().string());
    if (suffix != ".ADZ") return {};

    return {{ ImageType::FLOPPY, ImageFormat::ADZ }};
}

std::vector<string>
ADZFile::describe() const noexcept
{
    return adf.describe();
}

void
ADZFile::didInitialize()
{
    loginfo(ADF_DEBUG, "Decompressing %ld bytes...\n", data.size);
    
    try {
        data.gunzip();
    } catch (std::runtime_error &err) {
        throw IOError(IOError::ZLIB_ERROR, err.what());
    }
    
    loginfo(ADF_DEBUG, "Restored %ld bytes.\n", data.size);
    
    // Initialize the ADF with the decompressed data (may throw)
    adf.init(data.ptr, data.size);

    // Delete the original data
    data.dealloc();
}

}
