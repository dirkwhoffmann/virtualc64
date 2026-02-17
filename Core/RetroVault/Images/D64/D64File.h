// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "FloppyDiskImage.h"

namespace retro::vault::image {

// Disk parameters of a standard floppy disk
typedef struct
{
    u8     sectors;         // Typical number of sectors in this track
    u8     speedZone;       // Default speed zone for this track
    u16    lengthInBytes;   // Typical track size in bits
    u16    lengthInBits;    // Typical track size in bits
    isize  firstSectorNr;   // Logical number of first sector in track
    isize  tailGap;         // Gap between two sectors (number of 0x55 bytes)
    double stagger;         // Relative position of first bit (from Hoxs64)
}
TrackDefaults;

class D64File : public FloppyDiskImage {

    using GCRTrack = std::vector<u8>;
    mutable std::vector<GCRTrack> gcrTracks {42};

public:

    // D64 files come in six different sizes
    static constexpr isize D64_683_SECTORS     = 174848;
    static constexpr isize D64_683_SECTORS_ECC = 175531;
    static constexpr isize D64_768_SECTORS     = 196608;
    static constexpr isize D64_768_SECTORS_ECC = 197376;
    static constexpr isize D64_802_SECTORS     = 205312;
    static constexpr isize D64_802_SECTORS_ECC = 206114;

    static optional<ImageInfo> about(const fs::path &path);
    static const TrackDefaults &trackDefaults(isize t);

private:

    // Backing buffer used by the GCR encoder
    mutable u8 gcrbuffer[8192];


    //
    // Initializing
    //

public:

    explicit D64File();
    explicit D64File(const fs::path &path) { init(path); }
    explicit D64File(isize len) { init(len); }
    explicit D64File(const u8 *buf, isize len) { init(buf, len); }

    using FloppyDiskImage::init;


    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }
    
    ImageType type() const noexcept override { return ImageType::FLOPPY; }
    ImageFormat format() const noexcept override { return ImageFormat::D64; }
    std::vector<string> describeImage() const noexcept override;
    

    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::CBM; }


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return 256; }


    //
    // Methods from DiskImage
    //

public:

    isize numCyls() const noexcept override;
    isize numHeads() const noexcept override;
    isize numSectors(isize track) const noexcept override;


    //
    // Methods from FloppyDiskImage
    //

public:

    Diameter getDiameter() const noexcept override;
    Density getDensity() const noexcept override;

    BitView encode(TrackNr t) const override;
    void decode(TrackNr t, BitView bits) override;

private:

    [[deprecated]] isize encodeSector(MutableBitView &track, TrackNr t, SectorNr s, isize offset) const;


    //
    // Querying disk properties
    //

public:

    // Returns true if the file contains error correction codes
    bool hasEcc() const noexcept;

    // Returns the error correction codes (if any)
    // optional<std::span<const u8>> ecc() const noexcept;
    std::vector<u8> ecc() const noexcept;

    // Returns the error code for the specified sector (01 = no error)
    u8 getErrorCode(isize b) const ;

private:

    // Translates a track and sector number into an offset (-1 if invalid)
    isize tsOffset(isize t, isize s) const;


    //
    // Formatting
    //

public:

    // void formatDisk(FSFormat fs, BootBlockId id, string name);
};

}
