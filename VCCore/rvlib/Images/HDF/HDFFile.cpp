// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/HDF/HDFFile.h"
#include "Images/HDF/HDFLayout.h"
#include "Images/ImageError.h"
#include "FileSystems/Amiga/FSBlock.h"
#include "Devices/DeviceError.h"
#include "utl/common.h"
#include "utl/chrono.h"
#include "utl/io.h"
#include "utl/support.h"
#include <format>

namespace retro::vault {

optional<ImageInfo>
HDFFile::about(const fs::path &path)
{
    auto suffix = utl::uppercased(path.extension().string());

    if (suffix == ".HDZ") {
        
        return {{ ImageType::HARDDISK, ImageFormat::HDF }};
    }
    
    if (suffix == ".HDF") {
        
        // ensureHDF(utl::getSizeOfFile(path));
        return {{ ImageType::HARDDISK, ImageFormat::HDF }};
    }
    
    return {};
}

void
HDFFile::ensureHDF(isize len)
{
    // The size must be a multiple of 512 (block size)
    // if (len % 512) throw ImageError(ImageError::SIZE_MISMATCH);
    
}

std::vector<string>
HDFFile::describeImage() const noexcept
{
    return {
        "Amiga Hard Drive",
        std::format("{} Partition{}", numPartitions(),
                    numPartitions() != 1 ? "s" : ""),
        std::format("{} Cylinders, {} Heads, {} Sectors",
                    numCyls(), numHeads(), numSectors())
    };
}

isize
HDFFile::writeToFile(const fs::path &path) const
{
    return writeToFile(path, 0, size());
}

isize
HDFFile::writeToFile(const fs::path &path, isize offset, isize len) const
{
    if (utl::lowercased(path.extension().string()) == ".hdz") {

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
HDFFile::makeBacking(const fs::path &path) const
{
    // An .hdz file is compressed and has to be unpacked as a whole
    if (utl::lowercased(path.extension().string()) == ".hdz") {
        return std::make_unique<utl::GzipBacking>(path);
    }
    return HardDiskImage::makeBacking(path);
}

isize
HDFFile::imageSize(utl::Backing &backing) const
{
    auto available = backing.size();

    // Look for a rigid disk block, reading straight from the backing
    HDFLayout lay([&backing](isize nr, u8 *dst) {

        auto offset = nr * HDFLayout::bsize;
        if (nr < 0 || offset + HDFLayout::bsize > backing.size()) return false;

        backing.read(dst, offset, HDFLayout::bsize);
        return true;

    }, available);

    // Without one, the geometry is derived from the file, which fits by definition
    if (!lay.hasRDB()) return available;

    /* Pad a file that is shorter than the drive its RDB describes. A geometry
     * that is not plausible is left alone here, for didInitialize() to reject,
     * rather than turned into an absurdly large image first.
     */
    auto geometry = lay.getGeometryDescriptor();
    try { geometry.checkCompatibility(); } catch (...) { return available; }

    return std::max(available, geometry.numBytes());
}

void
HDFFile::didInitialize()
{
    // Run a consistency check on the buffer contents
    ensureHDF(getSize());
    
    // Retrieve geometry and partition information
    auto lay = layout();
    geometry = lay.getGeometryDescriptor();
    ptable = lay.getPartitionDescriptors();
    drivers = lay.getDriverDescriptors();

    // Check the hard drive descriptor for consistency
    geometry.checkCompatibility();

    // Check the partition table for consistency
    for (auto &it : ptable) { it.checkCompatibility(geometry); }

    // Check the device driver descriptors for consistency
    for (auto &it : drivers) { it.checkCompatibility(); }
}

isize
HDFFile::numCyls() const
{
    return geometry.cylinders;
}

isize
HDFFile::numHeads() const
{
    return geometry.heads;
}

isize
HDFFile::numSectors() const
{
    return geometry.sectors;
}






/*
FSDescriptor
HDFFile::getFileSystemDescriptor(isize nr) const
{
    FSDescriptor result;

    auto &part = ptable[nr];

    auto c = part.highCyl - part.lowCyl + 1;
    auto h = part.heads;
    auto s = part.sectors;

    result.numBlocks = c * h * s;

    // Determine block bounds
    auto first = part.lowCyl * h * s;
    auto dptr = data.ptr + first * 512;

    // Set the number of reserved blocks
    result.numReserved = 2;

    // Only proceed if the hard drive is formatted
    if (dos(first) == FSFormat::NODOS) return result;

    // Determine the location of the root block
    i64 highKey = result.numBlocks - 1;
    i64 rootKey = (result.numReserved + highKey) / 2;

    // Add partition
    result.rootBlock = (BlockNr)rootKey;

    // Seek bitmap blocks
    BlockNr ref = BlockNr(rootKey);
    isize cnt = 25;
    isize offset = 512 - 49 * 4;

    while (ref && ref < (BlockNr)result.numBlocks) {

        const u8* p = dptr + (ref * 512) + offset;

        // Collect all references to bitmap blocks stored in this block
        for (isize i = 0; i < cnt; i++, p += 4) {
            if (BlockNr bmb = R32BE(p)) { // } FSBlock::read32(p)) {
                if (isize(bmb) < result.numBlocks) {
                    result.bmBlocks.push_back(bmb);
                }
            }
        }

        // Continue collecting in the next extension bitmap block
        if ((ref = R32BE(p)) != 0) { // FSBlock::read32(p)) != 0) {
            if (isize(ref) < result.numBlocks) result.bmExtBlocks.push_back(ref);
            cnt = (512 / 4) - 1;
            offset = 0;
        }
    }

    return result;
}
*/


}
