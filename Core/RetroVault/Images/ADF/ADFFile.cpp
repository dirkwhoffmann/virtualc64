// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "FileSystems/Amiga/FSBootBlockImage.h"
#include "EADFFile.h"
#include "AmigaEncoder.h"
#include "AmigaDecoder.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "utl/io.h"
#include "utl/support/Strings.h"
#include <format>

namespace retro::vault::image {

using retro::vault::FSError;
using retro::vault::amiga::FSName;
using retro::vault::amiga::FSFormatEnum;
using retro::vault::amiga::FileSystem;
using retro::vault::amiga::FSBlock;
using retro::vault::amiga::FSDescriptor;

optional<ImageInfo>
ADFFile::about(const fs::path &path)
{
    // Check suffix
    auto suffix = utl::uppercased(path.extension().string());
    if (suffix != ".ADF") return {};

    // Get file size
    auto len = utl::getSizeOfFile(path);

    // Some ADFs contain an additional byte at the end. Ignore it.
    len &= ~1;

    // The size must be a multiple of the cylinder size
    if (len % 11264) return {};

    // Check some more limits
    if (len > ADFSIZE_35_DD_84 && len != ADFSIZE_35_HD) return {};

    // Make sure it's not an extended ADF
    if (EADFFile::about(path)) return {};

    return {{ ImageType::FLOPPY, ImageFormat::ADF }};
}

isize
ADFFile::fileSize(Diameter diameter, Density density)
{
    return fileSize(diameter, density, 160);
}

isize
ADFFile::fileSize(Diameter diameter, Density density, isize tracks)
{
    DiameterEnum::validate(diameter);
    DensityEnum::validate(density);

    if (diameter != Diameter::INCH_35) throw DeviceError(DeviceError::DSK_INVALID_DIAMETER);

    switch (density) {

        case Density::DD:

            switch (tracks) {

                case 2 * 80: return ADFSIZE_35_DD;
                case 2 * 81: return ADFSIZE_35_DD_81;
                case 2 * 82: return ADFSIZE_35_DD_82;
                case 2 * 83: return ADFSIZE_35_DD_83;
                case 2 * 84: return ADFSIZE_35_DD_84;

                default:
                    throw (DeviceError(DeviceError::DSK_INVALID_LAYOUT));
            }

        case Density::HD:

            return ADFSIZE_35_HD;

        default:
            throw DeviceError(DeviceError::DSK_INVALID_DENSITY);
    }
}

void ADFFile::init(isize len)
{
    switch (len) {

        case ADFFile::ADFSIZE_35_DD:
        case ADFFile::ADFSIZE_35_DD_81:
        case ADFFile::ADFSIZE_35_DD_82:
        case ADFFile::ADFSIZE_35_DD_83:
        case ADFFile::ADFSIZE_35_DD_84:
        case ADFFile::ADFSIZE_35_HD:

            FloppyDiskImage::init(len);
            break;

        default:
            throw DeviceError(DeviceError::DSK_INVALID_LAYOUT);
    }
}

void
ADFFile::init(Diameter dia, Density den)
{
    DiameterEnum::validate(dia);
    DensityEnum::validate(den);

    init(ADFFile::fileSize(dia, den));
}

void
ADFFile::init(const GeometryDescriptor &descr)
{
    init(descr.numBytes());
}

void
ADFFile::init(const FileSystem &volume)
{
    init(volume.bytes());
    volume.exporter.exportVolume(*this);
}

std::vector<string>
ADFFile::describeImage() const noexcept
{
    return {
        "Amiga Floppy Disk",
        std::format("{} {}",
                    getDiameterStr(), getDensityStr()),
        std::format("{} Cylinders, {} Sides, {} Sectors",
                    numCyls(), numHeads(), numSectors())
    };
}

void
ADFFile::didInitialize()
{
    // Add some empty cylinders if the file contains less than 80
    if (data.size < ADFSIZE_35_DD) data.resize(ADFSIZE_35_DD, 0);
}

isize
ADFFile::numCyls() const noexcept
{
    switch(data.size & ~1) {
            
        case ADFSIZE_35_DD:    return 80;
        case ADFSIZE_35_DD_81: return 81;
        case ADFSIZE_35_DD_82: return 82;
        case ADFSIZE_35_DD_83: return 83;
        case ADFSIZE_35_DD_84: return 84;
        case ADFSIZE_35_HD:    return 80;
            
        default:
            fatalError;
    }
}

isize
ADFFile::numHeads() const noexcept
{
    return 2;
}

isize
ADFFile::numSectors() const noexcept
{
    switch (getDensity()) {
            
        case Density::DD: return 11;
        case Density::HD: return 22;
            
        default:
            fatalError;
    }
}

Diameter
ADFFile::getDiameter() const noexcept
{
    return Diameter::INCH_35;
}

Density
ADFFile::getDensity() const noexcept
{
    return (data.size & ~1) == ADFSIZE_35_HD ? Density::HD : Density::DD;
}

BitView
ADFFile::encode(TrackNr t) const
{
    validateTrackNr(t);
    auto &track = mfmTracks.at(t);

    // Encode track
    AmigaEncoder encoder;
    auto mfm = encoder.encodeTrack(byteView(t), t);

    // Copy the encoded track data
    track.assign(mfm.data(), mfm.data() + mfm.byteView().size());

    // Return a bit view with the proper size
    return BitView(track.data(), mfm.size());
}

void
ADFFile::decode(TrackNr t, BitView bits)
{
    validateTrackNr(t);

    AmigaDecoder decoder;

    // Decode track
    auto bytes = decoder.decodeTrack(bits, t);
    assert(bytes.size() == 11 * 512);

    // Copy decoded bytes back to the ADF
    memcpy(byteView(t).data(), bytes.data(), bytes.size());
}

void
ADFFile::formatDisk(FSFormat dos, BootBlockId id, string name)
{
    retro::vault::amiga::FSFormatEnum::validate(dos);

    loginfo(IMG_DEBUG,
            "Formatting disk (%ld, %s)\n",
            numBlocks(), retro::vault::amiga::FSFormatEnum::key(dos));

    // Only proceed if a file system is given
    if (dos == FSFormat::NODOS) return;

    // Create a file system
    auto vol = Volume(*this);
    auto fs = FileSystem(vol);

    // Format the file system
    fs.format(dos);
    fs.setName(FSName(name));
    fs.makeBootable(id);

    // Update the underlying ADF
    fs.flush();
}

}
