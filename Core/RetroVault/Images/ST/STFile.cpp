// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "STFile.h"
#include "DeviceError.h"
#include "utl/io.h"
#include "utl/support/Strings.h"
#include <format>

namespace retro::vault::image {

optional<ImageInfo>
STFile::about(const fs::path &path)
{
    // Check suffix
    auto suffix = utl::uppercased(path.extension().string());
    if (suffix != ".ST") return {};

    // Check file size
    auto size = utl::getSizeOfFile(path);
    if (size != STSIZE_35_DD) return {};

    return {{ ImageType::FLOPPY, ImageFormat::ST }};
}

void
STFile::init(Diameter dia, Density den)
{
    if (dia != Diameter::INCH_35 || den != Density::DD) {

        // We only support 3.5"DD disks at the moment
        throw DeviceError(DeviceError::DSK_INVALID_LAYOUT);
    }

    init(9 * 160 * 512);
}

std::vector<string>
STFile::describeImage() const noexcept
{
    return {
        "AtariST Floppy Disk",
        std::format("{} {}",
                    getDiameterStr(), getDensityStr()),
        std::format("{} Cylinders, {} Sides, {} Sectors",
                    numCyls(), numHeads(), numSectors())
    };
}

isize
STFile::numCyls() const
{
    return 80;
}

isize
STFile::numHeads() const
{
    return 2;
}

isize
STFile::numSectors() const
{
    return 9;
}

BitView
STFile::encode(TrackNr t) const
{
    throw std::runtime_error("NOT IMPLEMENTED YET");
}

void
STFile::decode(TrackNr t, BitView bits)
{
    throw std::runtime_error("NOT IMPLEMENTED YET");
}

}
