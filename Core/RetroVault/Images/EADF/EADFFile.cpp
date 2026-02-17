// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "EADFFile.h"
#include "DiskEncoder.h"
#include "AmigaEncoder.h"
#include "AmigaDecoder.h"
#include "FileSystems/Amiga/FileSystem.h"
#include "ImageError.h"
#include "utl/io.h"

namespace retro::vault::image {

const std::vector<string> EADFFile::extAdfHeaders =
{
    "UAE--ADF",
    "UAE-1ADF"
};

optional<ImageInfo>
EADFFile::about(const fs::path &path)
{
    for (auto &header : extAdfHeaders) {

        if (utl::matchingFileHeader(path, header))
            return {{ ImageType::FLOPPY, ImageFormat::EADF }};
    }
    return {};
}

isize
EADFFile::numCyls() const
{
    return (storedTracks() + 1) / 2;
}

isize
EADFFile::numHeads() const
{
    return 2;
}

isize
EADFFile::numSectors() const
{
    switch (getDensity()) {

        case Density::DD:   return 11;
        case Density::HD:   return 22;

        default:
            return 0;
    }
}

std::vector<string>
EADFFile::describeImage() const noexcept
{
    return {
        "Amiga Floppy Disk",
        "Extended ADF format",
        ""
    };
}

void
EADFFile::checkIntegrity()
{
    isize numTracks = storedTracks();
    
    if (std::strcmp((char *)data.ptr, "UAE-1ADF") != 0) {
        
        logwarn("Only UAE-1ADF files are supported\n");
        throw ImageError(ImageError::EXT_FACTOR5);
    }
    
    if (numTracks < 160 || numTracks > 168) {

        logwarn("Invalid number of tracks\n");
        throw ImageError(ImageError::EXT_CORRUPTED);
    }

    if (data.size < proposedHeaderSize() || data.size != proposedFileSize()) {
        
        logwarn("File size mismatch\n");
        throw ImageError(ImageError::EXT_CORRUPTED);
    }

    for (isize i = 0; i < numTracks; i++) {

        if (!isStandardTrack(i) && !isExtendedTrack(i)) {

            logwarn("Unsupported track format\n");
            throw ImageError(ImageError::EXT_INCOMPATIBLE);
        }

        if (isStandardTrack(i)) {

            if (usedBitsForTrack(i) != 11 * 512 * 8) {

                logwarn("Unsupported standard track size\n");
                throw ImageError(ImageError::EXT_CORRUPTED);
            }
        }

        if (usedBitsForTrack(i) > availableBytesForTrack(i) * 8) {
            
            logwarn("Corrupted length information\n");
            throw ImageError(ImageError::EXT_CORRUPTED);
        }
    }
}

void
EADFFile::didInitialize()
{
    isize numTracks = storedTracks();

    // Scan the EADF file for errors
    checkIntegrity();

    // Prepare the track storage
    tracks.resize(numTracks);

    // Read all tracks
    for (TrackNr t = 0; t < numTracks; ++t) {

        auto &track = tracks.at(t);

        if (isStandardTrack(t)) {

            loginfo(IMG_DEBUG, "Reading standard track %ld from EADF\n", t);

            // Copy bytes from the EADF
            track.data.assign(trackData(t), trackData(t) + usedBitsForTrack(t) / 8);

            // Run the MFM encoder on the copied bytes
            AmigaEncoder encoder;
            auto mfm = encoder.encodeTrack(track.dataByteView(), t);

            // Copy the encoded data
            track.mfm.assign(mfm.data(), mfm.data() + mfm.byteView().size());
            track.bitCnt = mfm.size();
         }

        if (isExtendedTrack(t)) {

            loginfo(IMG_DEBUG, "Reading extended track %ld from EADF\n", t);

            // Copy MFM bits from the EADF
            track.mfm.assign(trackData(t), trackData(t) + availableBytesForTrack(t));
            track.bitCnt = usedBitsForTrack(t);

            try {

                // Run the MFM decoder on the copied bit stream
                AmigaDecoder decoder;
                auto bytes = decoder.decodeTrack(track.mfmBitView(), t);

                // Copy the decoded data
                track.data.assign(bytes.data(), bytes.data() + bytes.size());

            } catch (...) {

                // Since the EADF format is designed to preserve custom MFM
                // tracks, decoding may fail for some tracks. In that case, no
                // MFM data is generated. Any attempt to read a block from such
                // a track via the LinearDevice, BlockDevice, or TrackDevice
                // interface will result in a read error—exactly as it would on
                // real hardware.
                
                track.data.resize(0);
            }
        }
    }
}

isize
EADFFile::size() const
{
    return numBlocks() * bsize();
}

void
EADFFile::read(u8 *dst, isize offset, isize count) const
{
    throw DeviceError(DeviceError::READ_ERR, "Linear access denied.");
}

void
EADFFile::write(const u8 *src, isize offset, isize count)
{
    throw DeviceError(DeviceError::READ_ERR, "Linear access denied.");
}

void
EADFFile::readBlocks(u8 *dst, Range<isize> r) const
{
    validateBlockNr(r.lower);
    validateBlockNr(r.upper);

    for (auto nr = r.lower; nr < r.upper; ++nr) {
        
        auto pos    = b2ts(nr);
        auto &track = tracks.at(pos.track);
        auto &bytes = track.data;
        auto offset = pos.sector * bsize();
        
        if (track.data.empty())
            throw(DeviceError::DeviceError::READ_ERR);
        
        assert(offset + bsize() <= isize(bytes.size()));
        memcpy(dst, bytes.data() + offset, bsize());
    }
}

void
EADFFile::writeBlocks(const u8 *src, Range<isize> r)
{
    validateBlockNr(r.lower);
    validateBlockNr(r.upper);

    for (auto nr = r.lower; nr < r.upper; ++nr) {
        
        auto pos    = b2ts(nr);
        auto &track = tracks.at(pos.track);
        
        if (track.data.empty())
            track.data.resize(numSectors() * numBytes());
        
        auto &bytes = track.data;
        auto offset = pos.sector * bsize();
        
        assert(offset + bsize() <= isize(bytes.size()));
        memcpy(bytes.data() + offset, src, bsize());
        
        // TODO: Update the MFM bit stream
    }
}

Diameter
EADFFile::getDiameter() const noexcept
{
    return Diameter::INCH_35;
}

Density
EADFFile::getDensity() const noexcept
{
    isize bitsInLargestTrack = 0;
    
    for (isize i = 0; i < storedTracks(); i++) {
        bitsInLargestTrack = std::max(bitsInLargestTrack, usedBitsForTrack(i));
    }
    
    return bitsInLargestTrack < 16000 * 8 ? Density::DD : Density::HD;
}

BitView
EADFFile::encode(TrackNr t) const
{
    validateTrackNr(t);

    assert(!tracks.at(t).mfm.empty());
    return tracks.at(t).mfmBitView();
}

void
EADFFile::decode(TrackNr t, BitView bits)
{
    throw std::runtime_error("NOT IMPLEMENTED YET");
}

isize
EADFFile::storedTracks() const noexcept
{
    assert(!data.empty());

    return HI_LO(data[10], data[11]);
}

isize
EADFFile::typeOfTrack(isize t) const
{
    validateTrackNr(t);

    assert(!data.empty());
    u8 *p = data.ptr + 12 + 12 * t + 2;

    return HI_LO(p[0], p[1]);
}

isize
EADFFile::availableBytesForTrack(isize t) const
{
    validateTrackNr(t);

    assert(!data.empty());
    u8 *p = data.ptr + 12 + 12 * t + 4;

    return HI_HI_LO_LO(p[0], p[1], p[2], p[3]);
}

isize
EADFFile::usedBitsForTrack(isize t) const
{
    validateTrackNr(t);

    assert(!data.empty());
    u8 *p = data.ptr + 12 + 12 * t + 8;

    return HI_HI_LO_LO(p[0], p[1], p[2], p[3]);
}

isize
EADFFile::proposedHeaderSize() const noexcept
{
    assert(!data.empty());
    
    return 12 + 12 * storedTracks();
}

isize
EADFFile::proposedFileSize() const noexcept
{
    assert(!data.empty());

    isize result = proposedHeaderSize();
    
    for (isize i = 0; i < storedTracks(); i++) {
        result += availableBytesForTrack(i);
    }
    
    return result;
}

u8 *
EADFFile::trackData(isize t) const
{
    validateTrackNr(t);

    assert(!data.empty());
    u8 *p = data.ptr + proposedHeaderSize();
    
    for (isize i = 0; i < t; i++) {
        p += availableBytesForTrack(i);
    }
    
    return p;
}

}
