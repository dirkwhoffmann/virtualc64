// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "ADFFile.h"

namespace retro::vault::image {

/* This class represents a file in the extended ADF format. The file layout is
 * organized as follows:
 *
 *   1. Header section:
 *
 *      8 bytes : ASCII signature "UAE-1ADF"
 *      2 bytes : Reserved
 *      2 bytes : Number of tracks (typically 2 × 80 = 160)
 *
 *   2. Track header section (one entry per track):
 *
 *      2 bytes : Reserved
 *      2 bytes : Track type
 *                0 = Standard AmigaDOS track
 *                1 = Raw MFM data (upper byte = number of disk revolutions − 1)
 *      4 bytes : Available space for the track, in bytes (must be even)
 *      4 bytes : Track length, in bits
 *
 *   3. Track data section:
 *
 *      Raw track data for each track, stored consecutively.
 *
 * Note:
 * There exists a related format identified by the header "UAE--ADF". This
 * variant was introduced by Factor 5 to distribute Turrican disk images and
 * appears to be the only known use of this format. It is not supported by the
 * emulator and will not be supported in the future.
 */

class EADFFile : public FloppyDiskImage {

    struct EADFTrack {

        isize bitCnt;           // Track length in bits
        std::vector<u8> mfm;    // Encoded MFM bit stream
        std::vector<u8> data;   // Decoded data bytes

        MutableBitView mfmBitView() { return MutableBitView(mfm.data(), bitCnt); }
        MutableByteView mfmByteView() { return MutableByteView(mfm.data(), isize(mfm.size())); }
        MutableByteView dataByteView() { return MutableByteView(data.data(), isize(data.size())); }
    };

    // Track cache
    mutable std::vector<EADFTrack> tracks;

    // Accepted header signatures
    static const std::vector<string> extAdfHeaders;

public:

    static optional<ImageInfo> about(const fs::path &path);


    //
    // Initializing
    //

public:

    explicit EADFFile(isize len) { init(len); }
    explicit EADFFile(const fs::path &path) { init(path); }
    explicit EADFFile(const u8 *buf, isize len) { init(buf, len); }

    // Checks if the buffer is in EADF format (throws if not)
    void ensureEADF();

    
    //
    // Methods from AnyImage
    //

public:

    bool validateURL(const fs::path& path) const noexcept override {
        return about(path).has_value();
    }
    
    ImageType type() const noexcept override { return ImageType::FLOPPY; }
    ImageFormat format() const noexcept override { return ImageFormat::EADF; }
    std::vector<string> describeImage() const noexcept override;
    
    void checkIntegrity() override;
    void didInitialize() override;


    //
    // Methods from DiskImage
    //

public:

    FSFamily fsFamily() const noexcept override { return FSFamily::AMIGA; }


    //
    // Methods from LinearDevice
    //

public:

    isize size() const override;
    void read(u8 *dst, isize offset, isize count) const override;
    void write(const u8 *src, isize offset, isize count) override;


    //
    // Methods from BlockDevice
    //

public:

    isize bsize() const override { return 512; }
    void readBlocks(u8 *dst, Range<isize> r) const override;
    void writeBlocks(const u8 *src, Range<isize> r) override;


    //
    // Methods from DiskImage
    //

public:

    isize numCyls() const override;
    isize numHeads() const override;
    isize numSectors(isize) const override { return numSectors(); }
    isize numSectors() const;


    //
    // Methods from FloppyDiskImage
    //

public:

    Diameter getDiameter() const noexcept override;
    Density getDensity() const noexcept override;

    BitView encode(TrackNr t) const override;
    void decode(TrackNr t, BitView bits) override;

private:

    // BitView encodeStandardTrack(TrackNr t) const;
    // BitView encodeExtendedTrack(TrackNr t) const;


    //
    // Scanning the raw data
    //
    
public: // TODO: MAKE PRIVATE

    isize storedTracks() const noexcept;
    isize typeOfTrack(isize t) const;
    isize availableBytesForTrack(isize t) const;
    isize usedBitsForTrack(isize t) const;
    isize proposedHeaderSize() const noexcept;
    isize proposedFileSize() const noexcept;

private:

    // Returns a pointer to the first data byte of a certain track
    u8 *trackData(isize t) const;

    // Convenience wrappers
    bool isStandardTrack(isize t) const { return typeOfTrack(t) == 0; }
    bool isExtendedTrack(isize t) const { return typeOfTrack(t) == 1; }
};

}
