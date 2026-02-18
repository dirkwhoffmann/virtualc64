// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "C64.h"
#include "Codecs.h"
#include "ADFFile.h"
#include "Images/ImageError.h"
#include "Images/D64/D64File.h"
#include "Images/Encoders/C64Encoder.h"
#include "Images/Encoders/C64Decoder.h"
#include "utl/abilities/Hashable.h"
#include <stdarg.h>

// using retro::vault::ImageError;

namespace vc64 {

using namespace retro::vault;

const TrackDefaults FloppyDisk::trackDefaults[43] = {
    
    { 0, 0, 0, 0, 0, 0 }, // Padding
    
    // Speedzone 3 (outer tracks)
    { 21, 3, 7693, 7693 * 8,   0, 0.268956 }, // Track 1
    { 21, 3, 7693, 7693 * 8,  21, 0.724382 }, // Track 2
    { 21, 3, 7693, 7693 * 8,  42, 0.177191 }, // Track 3
    { 21, 3, 7693, 7693 * 8,  63, 0.632698 }, // Track 4
    { 21, 3, 7693, 7693 * 8,  84, 0.088173 }, // Track 5
    { 21, 3, 7693, 7693 * 8, 105, 0.543583 }, // Track 6
    { 21, 3, 7693, 7693 * 8, 126, 0.996409 }, // Track 7
    { 21, 3, 7693, 7693 * 8, 147, 0.451883 }, // Track 8
    { 21, 3, 7693, 7693 * 8, 168, 0.907342 }, // Track 9
    { 21, 3, 7693, 7693 * 8, 289, 0.362768 }, // Track 10
    { 21, 3, 7693, 7693 * 8, 210, 0.815512 }, // Track 11
    { 21, 3, 7693, 7693 * 8, 231, 0.268338 }, // Track 12
    { 21, 3, 7693, 7693 * 8, 252, 0.723813 }, // Track 13
    { 21, 3, 7693, 7693 * 8, 273, 0.179288 }, // Track 14
    { 21, 3, 7693, 7693 * 8, 294, 0.634779 }, // Track 15
    { 21, 3, 7693, 7693 * 8, 315, 0.090253 }, // Track 16
    { 21, 3, 7693, 7693 * 8, 336, 0.545712 }, // Track 17
    
    // Speedzone 2
    { 19, 2, 7143, 7143 * 8, 357, 0.945418 }, // Track 18
    { 19, 2, 7143, 7143 * 8, 376, 0.506081 }, // Track 19
    { 19, 2, 7143, 7143 * 8, 395, 0.066622 }, // Track 20
    { 19, 2, 7143, 7143 * 8, 414, 0.627303 }, // Track 21
    { 19, 2, 7143, 7143 * 8, 433, 0.187862 }, // Track 22
    { 19, 2, 7143, 7143 * 8, 452, 0.748403 }, // Track 23
    { 19, 2, 7143, 7143 * 8, 471, 0.308962 }, // Track 24
    
    // Speedzone 1
    { 18, 1, 6667, 6667 * 8, 490, 0.116926 }, // Track 25
    { 18, 1, 6667, 6667 * 8, 508, 0.788086 }, // Track 26
    { 18, 1, 6667, 6667 * 8, 526, 0.459190 }, // Track 27
    { 18, 1, 6667, 6667 * 8, 544, 0.130238 }, // Track 28
    { 18, 1, 6667, 6667 * 8, 562, 0.801286 }, // Track 29
    { 18, 1, 6667, 6667 * 8, 580, 0.472353 }, // Track 30
    
    // Speedzone 0 (inner tracks)
    { 17, 0, 6250, 6250 * 8, 598, 0.834120 }, // Track 31
    { 17, 0, 6250, 6250 * 8, 615, 0.614880 }, // Track 32
    { 17, 0, 6250, 6250 * 8, 632, 0.395480 }, // Track 33
    { 17, 0, 6250, 6250 * 8, 649, 0.176140 }, // Track 34
    { 17, 0, 6250, 6250 * 8, 666, 0.956800 }, // Track 35
    
    // Speedzone 0 (usually unused tracks)
    { 17, 0, 6250, 6250 * 8, 683, 0.300 }, // Track 36
    { 17, 0, 6250, 6250 * 8, 700, 0.820 }, // Track 37
    { 17, 0, 6250, 6250 * 8, 717, 0.420 }, // Track 38
    { 17, 0, 6250, 6250 * 8, 734, 0.940 }, // Track 39
    { 17, 0, 6250, 6250 * 8, 751, 0.540 }, // Track 40
    { 17, 0, 6250, 6250 * 8, 768, 0.130 }, // Track 41
    { 17, 0, 6250, 6250 * 8, 785, 0.830 }  // Track 42
};

isize
FloppyDisk::numberOfSectorsInTrack(Track t)
{
    return (t < 1) ? 0 : (t < 18) ? 21 : (t < 25) ? 19 : (t < 31) ? 18 : (t < 43) ? 17 : 0;
}
isize
FloppyDisk::numberOfSectorsInHalftrack(Halftrack ht)
{
    return numberOfSectorsInTrack((ht + 1) / 2);
}

isize
FloppyDisk::speedZoneOfTrack(Track t)
{
    return (t < 18) ? 3 : (t < 25) ? 2 : (t < 31) ? 1 : 0;
}

isize
FloppyDisk::speedZoneOfHalftrack(Halftrack ht)
{
    return (ht < 35) ? 3 : (ht < 49) ? 2 : (ht < 61) ? 1 : 0;
}

bool
FloppyDisk::isValidTrackSectorPair(Track t, Sector s)
{
    return s < numberOfSectorsInTrack(t);
}

bool
FloppyDisk::isValidHalftrackSectorPair(Halftrack ht, Sector s)
{
    return s < numberOfSectorsInHalftrack(ht);
}

FloppyDisk::FloppyDisk()
{    
    clearDisk();
}

void
FloppyDisk::init(const fs::path &path, bool wp)
{
    if (auto info = FloppyDiskImage::about(path)) {
        
        // path points to a disk image
        switch (info->format) {

            case ImageFormat::D64:
                
                printf("D64 file: %s\n", path.string().c_str());
                if (auto img = FloppyDiskImage::make(path)) {
                    init(*img, wp);
                }
                return;
                
            default:
                throw IOError(IOError::FILE_TYPE_UNSUPPORTED);
        }
    }
    
    if (G64File::isCompatible(path)) {

        auto file = G64File(path);
        init(file, wp);
        return;
    }
    
    // TODO: 
    // auto fs = OldFileSystem(path);
    // init(fs, wp);
}

void
FloppyDisk::init(const class FloppyDiskImage &file, bool wp)
{
    encodeDisk(file);
    setWriteProtection(wp);
}

void
FloppyDisk::init(FSFormat type, const PETName<16> &name, bool wp)
{
    assert(FSFormatEnum::isValid(type));
    
    if (type == FSFormat::CBM) {
        
        // Create a D64 container with a file system on top
        auto d64 = D64File();
        auto vol = Volume(d64);
        auto fs  = FileSystem(vol);
        
        // Format disk
        fs.format(type);
        fs.setName(name);
        fs.flush();
        
        // Initialize the disk with the formatted D64 container
        init(d64, wp);

    } else {

        clearDisk();
    }
}

/*
void
Disk::init(const OldFileSystem &fs, bool wp)
{
    clearDisk();
    encode(fs);
    setWriteProtection(wp);
}
*/

void
FloppyDisk::init(const G64File &g64, bool wp)
{
    clearDisk();
    encodeG64(g64);
    setWriteProtection(wp);
}

void
FloppyDisk::init(AnyCollection &collection, bool wp)
{
    // TODO:
    // auto fs = OldFileSystem(collection);
    // init(fs, wp);
}

void
FloppyDisk::init(SerReader &reader)
{
    serialize(reader);
}

void
FloppyDisk::readBlock(u8 *dst, isize nr) const
{
    C64Decoder decoder;

    auto [t,s] = b2ts(nr);
    loginfo(GCR_DEBUG, "readBlock: %ld (%ld,%ld)\n", nr, t, s);
    
    auto bv = BitView(data.track[t], lengthOfTrack(nr));
    auto bytes = decoder.decodeSector(bv, t, s);
    assert(bytes.size() == bsize());
    memcpy(dst, bytes.data(), bytes.size());
}

void
FloppyDisk::readBlocks(u8 *dst, Range<isize> range) const
{
    auto *ptr = dst;
    
    for (isize b = range.lower; b < range.upper; ++b) {
        
        readBlock(ptr, b);
        ptr += bsize();
    }
}

void
FloppyDisk::writeBlock(const u8 *src, isize nr)
{
    C64Encoder encoder;
    C64Decoder decoder;

    auto [t,s]  = b2ts(nr);
    loginfo(GCR_DEBUG, "writeBlock: %ld (%ld,%ld)\n", nr, t, s);

    // Compute the GCR bit stream
    auto gcr = encoder.encodeSector(ByteView(src, bsize()), t, s);

    // Locate the sector inside the track
    auto bv = MutableBitView(data.track[t], lengthOfTrack(nr));
    auto sector = decoder.seekSector(bv, s);

    if (!sector.has_value())
        throw IOError(DeviceError::SEEK_ERR, "Block " + std::to_string(nr));

    // auto tr = track[t];
    auto it = bv.cyclic_begin() + sector->lower;

    // Replace the sector data
    assert(gcr.size() == (*sector).size());
    for (isize i = 0; i < gcr.size(); ++i, ++it)
        bv.set(it.offset(), gcr[i]);
}

void
FloppyDisk::writeBlocks(const  u8 *src, Range<isize> range)
{
    auto *ptr = src;
    
    for (isize b = range.lower; b < range.upper; ++b) {
        
        writeBlock(ptr, b);
        ptr += bsize();
    }
}

isize
FloppyDisk::numSectors(isize t) const
{
    if (t < 0 || t >= numTracks()) return 0;

    return D64File::trackDefaults(t).sectors;
}

void
FloppyDisk::readTrack(u8 *dst, isize nr) const
{
    fatalError;
}

void
FloppyDisk::writeTrack(const u8 *src, isize nr)
{
    fatalError;
}

void
FloppyDisk::_dump(Category category, std::ostream &os) const
{
    using namespace utl;
    
    if (category == Category::Disk) {

        auto checksum = utl::Hashable::fnv32((const u8 *)data.track, sizeof(data.track));

        os << tab("Write protected") << bol(writeProtected) << std::endl;
        os << tab("Modified") << bol(modified) << std::endl;
        os << tab("Checksum") << hex(checksum) << std::endl;
    }

    if (category == Category::Layout) {

        for (Halftrack ht = 1; ht <= highestHalftrack; ht++) {
            
            auto length = lengthOfHalftrack(ht);
            
            os << tab("Halftrack " + std::to_string(ht));
            os << dec(length) << " Bits (" << dec(length / 8) << " Bytes)";
            os << std::endl;
        }
    }
}

void
FloppyDisk::setModified(bool b)
{
    if (b != modified) {
        modified = b;
    }
}

void
FloppyDisk::encodeGcr(u8 value, Track t, HeadPos offset)
{
    assert(isTrackNumber(t));
    
    u8 nibble1 = bin2gcr(value >> 4);
    u8 nibble2 = bin2gcr(value & 0xF);
    
    writeBitToTrack(t, offset++, nibble1 & 0x10);
    writeBitToTrack(t, offset++, nibble1 & 0x08);
    writeBitToTrack(t, offset++, nibble1 & 0x04);
    writeBitToTrack(t, offset++, nibble1 & 0x02);
    writeBitToTrack(t, offset++, nibble1 & 0x01);

    writeBitToTrack(t, offset++, nibble2 & 0x10);
    writeBitToTrack(t, offset++, nibble2 & 0x08);
    writeBitToTrack(t, offset++, nibble2 & 0x04);
    writeBitToTrack(t, offset++, nibble2 & 0x02);
    writeBitToTrack(t, offset++, nibble2 & 0x01);
}

void
FloppyDisk::encodeGcr(u8 *values, isize length, Track t, HeadPos offset)
{
    for (isize i = 0; i < length; i++, values++, offset += 10) {
        encodeGcr(*values, t, offset);
    }
}

bool
FloppyDisk::isValidHeadPos(Halftrack ht, HeadPos pos) const
{
    return isHalftrackNumber(ht) && pos >= 0 && pos < length.halftrack[ht];
}

HeadPos
FloppyDisk::wrap(Halftrack ht, HeadPos pos) const
{
    auto len = length.halftrack[ht];
    return pos < 0 ? pos + len : pos >= len ? pos - len : pos;
}

u64
FloppyDisk::_bitDelay(Halftrack ht, HeadPos pos) const {
    
    assert(isValidHeadPos(ht, pos));

    // In the current implementation, we assume that the density bits were
    // set to their correct values when a bit was written to disk. According
    // to this assumption, the returned value is determined solely by the
    // track position of the drive head.
    
    if (ht <= 33)
        return 4 * 10000; // Density bits = 00: 4 * 16/16 * 10^4 1/10 nsec
    if (ht <= 47)
        return 4 * 9375;  // Density bits = 01: 4 * 15/16 * 10^4 1/10 nsec
    if (ht <= 59)
        return 4 * 8750;  // Density bits = 10: 4 * 14/16 * 10^4 1/10 nsec
    
    return 4 * 8125;     // Density bits = 11: 4 * 13/16 * 10^4 1/10 nsec
}

void
FloppyDisk::clearHalftrack(Halftrack ht)
{
    memset(&data.halftrack[ht], 0x55, sizeof(data.halftrack[ht]));
    length.halftrack[ht] = sizeof(data.halftrack[ht]) * 8;
}

void
FloppyDisk::clearDisk()
{
    // memset(&data, 0x55, sizeof(data));
    for (Halftrack ht = 1; ht <= highestHalftrack; ht++) {
        clearHalftrack(ht);
    }
    writeProtected = false;
    modified = false;
}

bool
FloppyDisk::halftrackIsEmpty(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    for (usize i = 0; i < sizeof(data.halftrack[ht]); i++)
        if (data.halftrack[ht][i] != 0x55) return false;
    return true;
}

bool
FloppyDisk::trackIsEmpty(Track t) const
{
    assert(isTrackNumber(t));
    return halftrackIsEmpty(2 * t - 1);
}

isize
FloppyDisk::nonemptyHalftracks() const
{
    isize result = 0;
    
    for (Halftrack ht = 1; ht < 85; ht++) {
        if (!halftrackIsEmpty(ht))
            result++;
    }
    
    return result;
}

isize
FloppyDisk::lengthOfTrack(Track t) const
{
    assert(isTrackNumber(t));
    return length.track[t][0];
}

isize
FloppyDisk::lengthOfHalftrack(Halftrack ht) const
{
    assert(isHalftrackNumber(ht));
    return length.halftrack[ht];
}


//
// Decoding disk data
//

isize
FloppyDisk::decodeDisk(u8 *dest)
{
    // Analyze the GCR bit stream
    DiskAnalyzer analyzer(*this);
    
    // Determine highest non-empty track
    Track t = 42;
    while (t > 0 && trackIsEmpty(t)) t--;
    
    // Decode disk
    if (t <= 35)
        return decodeDisk(dest, 35, analyzer);

    if (t <= 40)
        return decodeDisk(dest, 40, analyzer);
    
    return decodeDisk(dest, 42, analyzer);
}

isize
FloppyDisk::decodeDisk(u8 *dest, isize numTracks, DiskAnalyzer &analyzer)
{
    isize numBytes = 0;

    assert(numTracks == 35 || numTracks == 40 || numTracks == 42);

    // For each full track ...
    for (Track t = 1; t <= numTracks; t++) {
        
        if (trackIsEmpty(t))
            break;
        
        logdebug(GCR_DEBUG, "Decoding track %ld %s\n", t, dest ? "" : "(test run)");
        numBytes += decodeTrack(t, dest + (dest ? numBytes : 0), analyzer);
    }
    
    return numBytes;
}

isize
FloppyDisk::decodeTrack(Track t, u8 *dest, DiskAnalyzer &analyzer)
{
    assert(isTrackNumber(t));
    
    return decodeHalfrack(2 * t - 1, dest, analyzer);
}

isize
FloppyDisk::decodeHalfrack(Halftrack ht, u8 *dest, DiskAnalyzer &analyzer)
{
    assert(isHalftrackNumber(ht));
    
    isize numBytes = 0;
    isize numSectors = numberOfSectorsInHalftrack(ht);

    // For each sector ...
    for (Sector s = 0; s < numSectors; s++) {
        
        logdebug(GCR_DEBUG, "   Decoding sector %ld\n", s);
        SectorLayout info = analyzer.sectorLayout(ht, s);
        if (info.dataBegin != info.dataEnd) {
            numBytes += decodeSector(ht, info.dataBegin, dest + (dest ? numBytes : 0), analyzer);
        } else {

            // The decoder failed to decode this sector.
            break;
        }
    }
    
    return numBytes;
}

isize
FloppyDisk::decodeSector(Halftrack ht, isize offset, u8 *dest, DiskAnalyzer &analyzer)
{
    // The first byte must be 0x07 (indicating a data block)
    assert(analyzer.decodeGcr(ht, offset) == 0x07);
    offset += 10;
    
    if (dest) {
        for (isize i = 0; i < 256; i++) {
            dest[i] = analyzer.decodeGcr(ht, offset);
            offset += 10;
        }
    }
    
    return 256;
}

//
// Encoding disk data
//

void
FloppyDisk::encodeDisk(const FloppyDiskImage &image)
{
    using namespace retro::vault;
    
    loginfo(DSK_DEBUG,
            "Encoding floppy disk image %s...\n", image.path.string().c_str());

    // Start with an unformatted disk
    clearDisk();

    // Encode all tracks
    for (TrackNr t = 0; t < image.numTracks(); ++t) {
        
        auto gcr = image.encode(t).byteView();

        memcpy(data.track[t], gcr.span().data(), gcr.span().size());
        length.track[t][0] = length.track[t][1] = gcr.size() * 8;
    }

    /*
    if constexpr (debug::IMG_DEBUG) {

        string tmp = "/tmp/debug.img";
        fprintf(stderr, "Saving image to %s for debugging\n", tmp.c_str());
        Codec::makeIMG(*this)->writeToFile(tmp);
    }
     */
    if constexpr (debug::IMG_DEBUG) {

        /*
        string tmp = "/tmp/debug.img";
        fprintf(stderr, "Saving image to %s for debugging\n", tmp.c_str());
        Codec::makeD64(*this)->writeToFile(tmp);
        */
    }

    // In debug mode, also run the decoder
    /*
    if constexpr (debug::ADF_DEBUG) {

        string tmp = "/tmp/debug.adf";
        fprintf(stderr, "Saving image to %s for debugging\n", tmp.c_str());
        Codec::makeADF(*this)->writeToFile(tmp);
    }
    */
}

void
FloppyDisk::encodeG64(const G64File &a)
{
    logdebug(GCR_DEBUG, "Encoding G64 archive\n");

    clearDisk();
    for (Halftrack ht = 1; ht <= 84; ht++) {
        
        isize size = a.getSizeOfHalftrack(ht);
        
        if (size == 0) {
            if (ht > 1) {
                // Make this halftrack as long as the previous halftrack
                length.halftrack[ht] = length.halftrack[ht - 1];
            }
            continue;
        }
        
        if (size > 7928) {
            logwarn("Halftrack %ld has %ld bytes. Must be less than 7928\n", ht, size);
            continue;
        }
        logdebug(GCR_DEBUG, "  Encoding halftrack %ld (%ld bytes)\n", ht, size);
        length.halftrack[ht] = (u16)(8 * size);
        
        a.copyHalftrack(ht, data.halftrack[ht]);
    }
}

void
FloppyDisk::writeToFile(const fs::path& path)
{
    auto ext = utl::uppercased(path.extension().string());

    if (ext == ".D64") writeToFile(path, ImageFormat::D64);
    
    throw IOError(IOError::FILE_TYPE_UNSUPPORTED);
}

void
FloppyDisk::writeToFile(const fs::path& path, ImageFormat fmt)
{
    switch (fmt) {

        case ImageFormat::D64: Codec::makeD64(*this)->writeToFile(path); break;

        default:
            throw IOError(IOError::FILE_TYPE_UNSUPPORTED);
    }
}

}
