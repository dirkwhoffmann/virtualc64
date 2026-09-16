// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "FileSystems/Amiga/FSBootBlockImage.h"
#include "Images/EADF/EADFFile.h"
#include "Images/Encoders/AmigaEncoder.h"
#include "Images/Encoders/AmigaDecoder.h"
#include "Images/ImageError.h"
#include "utl/io.h"
#include "utl/support/Strings.h"
#include <format>

namespace retro::vault {

using retro::vault::FSError;
using retro::vault::amiga::FSName;
using retro::vault::amiga::FSFormatEnum;
using retro::vault::amiga::FileSystem;
using retro::vault::amiga::FSBlock;
using retro::vault::amiga::FSDescriptor;
using retro::vault::amiga::FSFormat;
using retro::vault::amiga::BootBlockId;

optional<ImageInfo>
ADFFile::about(const fs::path &path)
{
    auto suffix = utl::uppercased(path.extension().string());

    if (suffix == ".ADZ") {
        
        return {{ ImageType::FLOPPY, ImageFormat::ADF }};
    }
    
    if (suffix == ".ADF") {
        
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
    
    return {};
}

void
ADFFile::ensureADF()
{
    auto len = getSize();
    
    // Some ADFs contain an additional byte at the end. Ignore it.
    len &= ~1;
    
    // The size must be a multiple of the cylinder size
    if (len % 11264)
        throw ImageError(ImageError::SIZE_MISMATCH);
    
    // Check some more limits
    if (len > ADFSIZE_35_DD_84 && len != ADFSIZE_35_HD)
        throw ImageError(ImageError::SIZE_MISMATCH);
    
    // Make sure it's not an extended ADF
    auto head = byteView(0, std::min(getSize(), isize(16)));
    if (utl::matchingBufferHeader(head.data(), head.size(), "UAE--ADF") ||
        utl::matchingBufferHeader(head.data(), head.size(), "UAE--1ADF"))
        throw ImageError(ImageError::FORMAT_MISMATCH);
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

isize
ADFFile::writeToFile(const fs::path &path) const
{
    return writeToFile(path, 0, size());
}

isize
ADFFile::writeToFile(const fs::path &path, isize offset, isize len) const
{
    if (utl::lowercased(path.extension().string()) == ".adz") {

        // Compress the requested range and write the result as a whole
        utl::Buffer<u8> copy;
        copy.init(byteView(offset, len).data(), len);
        copy.gzip();
        copy.write(path);
        return copy.size;

    } else {

        return BinaryImage::writeToFile(path, offset, len);
    }
}

std::unique_ptr<utl::Backing>
ADFFile::makeBacking(const fs::path &path) const
{
    // An .adz file is compressed and has to be unpacked as a whole
    if (utl::lowercased(path.extension().string()) == ".adz") {
        return std::make_unique<utl::GzipBacking>(path);
    }
    return FloppyDiskImage::makeBacking(path);
}

isize
ADFFile::imageSize(utl::Backing &backing) const
{
    // Add some empty cylinders if the file contains less than 80
    return std::max(backing.size(), isize(ADFSIZE_35_DD));
}

void
ADFFile::didInitialize()
{
    // Run a consistency check on the buffer contents
    ensureADF();
}

isize
ADFFile::numCyls() const noexcept
{
    switch(getSize() & ~1) {
            
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
    return (getSize() & ~1) == ADFSIZE_35_HD ? Density::HD : Density::DD;
}

utl::BitView
ADFFile::encode(TrackNr t) const
{
    validateTrackNr(t);
    auto &track = mfmTracks.at(t);

    // Encode track
    AmigaEncoder encoder;
    auto mfm = encoder.encodeTrack(trackView(t), t);

    // Copy the encoded track data
    track.assign(mfm.data(), mfm.data() + mfm.byteView().size());

    // Return a bit view with the proper size
    return utl::BitView(track.data(), mfm.size());
}

void
ADFFile::decode(TrackNr t, utl::BitView bits)
{
    validateTrackNr(t);

    AmigaDecoder decoder;

    // Decode track
    auto bytes = decoder.decodeTrack(bits, t);
    assert(bytes.size() == 11 * 512);

    // Copy decoded bytes back to the ADF
    memcpy(mutableTrackView(t).data(), bytes.data(), bytes.size());
}

void
ADFFile::formatDisk(FSFormat dos, BootBlockId id, string name)
{
    retro::vault::amiga::FSFormatEnum::validate(dos);

    logmsg(LOG_IMG,
            "Formatting disk (%td, %s)\n",
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
