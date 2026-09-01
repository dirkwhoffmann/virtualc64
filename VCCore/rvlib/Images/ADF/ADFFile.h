// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "Images/FloppyDiskImage.h"
#include "Devices/DeviceDescriptors.h"
#include "FileSystems/Amiga/FileSystem.h"

/* Amiga file-system types are named with their namespace throughout this
 * header rather than pulled in with using-declarations.
 *
 * A using-declaration at namespace scope in a header is inherited by every
 * translation unit that includes it, so the names would land in retro::vault
 * for everyone -- and amiga::FileSystem would then collide with the CBM
 * FileSystem in files that use neither, as it once did in VCCore. The
 * corresponding block in ADFFile.cpp is fine: it reaches no further than that
 * one file.
 */
namespace retro::vault {

class ADFFile : public FloppyDiskImage {

    using MFMTrack = std::vector<u8>;
    mutable std::vector<MFMTrack> mfmTracks {168};

public:

    static constexpr isize ADFSIZE_35_DD    = 901120;   //  880 KB
    static constexpr isize ADFSIZE_35_DD_81 = 912384;   //  891 KB (+ 1 cyl)
    static constexpr isize ADFSIZE_35_DD_82 = 923648;   //  902 KB (+ 2 cyls)
    static constexpr isize ADFSIZE_35_DD_83 = 934912;   //  913 KB (+ 3 cyls)
    static constexpr isize ADFSIZE_35_DD_84 = 946176;   //  924 KB (+ 4 cyls)
    static constexpr isize ADFSIZE_35_HD    = 1802240;  // 1760 KB
    
    static optional<ImageInfo> about(const fs::path &path);

    // Returns the size of an ADF file of a given disk type in bytes
    static isize fileSize(Diameter diameter, Density density);
    static isize fileSize(Diameter diameter, Density density, isize tracks);

    
    //
    // Initializing
    //
    
public:
    
    explicit ADFFile() { }
    explicit ADFFile(isize len) { init(len); }
    explicit ADFFile(const u8 *buf, isize len) { init(buf, len); }
    explicit ADFFile(const utl::Buffer<u8>& buffer) { init(buffer); }
    explicit ADFFile(const fs::path& path) { init(path); }
    explicit ADFFile(Diameter dia, Density den) { init(dia, den); }
    explicit ADFFile(const GeometryDescriptor &descr) { init(descr); }
    explicit ADFFile(amiga::FileSystem &volume) { init(volume); }

    using FloppyDiskImage::init;
    void init(isize len);
    void init(Diameter dia, Density den);
    void init(const GeometryDescriptor &descr);
    void init(const amiga::FileSystem &volume);

    // Checks if the buffer is in ADF format (throws if not)
    void ensureADF();

    
    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }

    ImageType type() const noexcept override { return ImageType::FLOPPY; }
    ImageFormat format() const noexcept override { return ImageFormat::ADF; }
    std::vector<string> describeImage() const noexcept override;
    isize writeToFile(const fs::path &path) const override;
    isize writeToFile(const fs::path &path, isize offset, isize len) const override;
    void didInitialize() override;


    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::AMIGA; }


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return 512; }


    //
    // Methods from DiskImage
    //

public:
    
    isize numCyls() const noexcept override;
    isize numHeads() const noexcept override;
    isize numSectors(isize) const noexcept override { return numSectors(); }
    isize numSectors() const noexcept;


    //
    // Methods from FloppyDiskImage
    //
    
public:
    
    Diameter getDiameter() const noexcept override;
    Density getDensity() const noexcept override;

    utl::BitView encode(TrackNr t) const override;
    void decode(TrackNr t, utl::BitView bits) override;


    //
    // Formatting
    //

public:
    
    void formatDisk(amiga::FSFormat fs, amiga::BootBlockId id, string name);
};

}
