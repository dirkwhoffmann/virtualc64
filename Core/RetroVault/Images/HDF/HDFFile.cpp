// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "HDFFile.h"
#include "FileSystems/Amiga/FSBlock.h"
#include "DeviceError.h"
#include "utl/common.h"
#include "utl/chrono.h"
#include "utl/io.h"
#include "utl/support.h"
#include <format>

namespace retro::vault::image {

optional<ImageInfo>
HDFFile::about(const fs::path &path)
{
    auto suffix = utl::uppercased(path.extension().string());
    if (suffix != ".HDF") return {};

    return {{ ImageType::HARDDISK, ImageFormat::HDF }};
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

void
HDFFile::didInitialize()
{
    // Retrieve geometry and partition information
    geometry = getGeometryDescriptor();
    ptable = getPartitionDescriptors();
    drivers = getDriverDescriptors();

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

GeometryDescriptor
HDFFile::getGeometryDescriptor() const
{
    GeometryDescriptor result;

    if (auto rdb = seekRDB(); rdb) {

        // Read the information from the rigid disk block
        result.cylinders    = R32BE(rdb + 64);
        result.sectors      = R32BE(rdb + 68);
        result.heads        = R32BE(rdb + 72);
        result.bsize        = R32BE(rdb + 16);

    } else {

        // Predict the number of blocks
        auto numBlocks = predictNumBlocks();

        // Predict the drive geometry
        auto geometries = GeometryDescriptor::driveGeometries(numBlocks, result.bsize);

        // Use the first match by default
        if (geometries.size()) result = geometries.front();
    }

    return result;
}

PartitionDescriptor
HDFFile::getPartitionDescriptor(isize part) const
{
    PartitionDescriptor result;

    if (auto pb = seekPB(part); pb) {

        // Extract information from the partition block
        result.name           = utl::createStr(pb + 37, 31);
        result.flags          = R32BE(pb + 20);
        result.sizeBlock      = R32BE(pb + 132);
        result.heads          = R32BE(pb + 140);
        result.sectors        = R32BE(pb + 148);
        result.reserved       = R32BE(pb + 152);
        result.interleave     = R32BE(pb + 160);
        result.lowCyl         = R32BE(pb + 164);
        result.highCyl        = R32BE(pb + 168);
        result.numBuffers     = R32BE(pb + 172);
        result.bufMemType     = R32BE(pb + 176);
        result.maxTransfer    = R32BE(pb + 180);
        result.mask           = R32BE(pb + 184);
        result.bootPri        = R32BE(pb + 188);
        result.dosType        = R32BE(pb + 192);

    } else {

        assert(part == 0);

        // Add a default partition spanning the entire disk
        auto geo = getGeometryDescriptor();
        result = PartitionDescriptor(geo);

        // Make the first partition bootable
        result.flags |= 1;
    }

    return result;
}

std::vector<PartitionDescriptor>
HDFFile::getPartitionDescriptors() const
{
    std::vector<PartitionDescriptor> result;

    // Add the first partition (which always exists)
    result.push_back(getPartitionDescriptor(0));

    // Add other partitions (if any)
    for (isize i = 1; i < 16 && seekPB(i); i++) {
        result.push_back(getPartitionDescriptor(i));
    }

    return result;
}

DriverDescriptor
HDFFile::getDriverDescriptor(isize driver) const
{
    DriverDescriptor result;

    if (auto fsh = seekFSH(driver); fsh) {

        // Extract information from the file system header block
        result.dosType      = R32BE(fsh + 32);
        result.dosVersion   = R32BE(fsh + 36);
        result.patchFlags   = R32BE(fsh + 40);

        // Traverse the seglist
        auto lsegRef = R32BE(fsh + 72);

        for (isize i = 0; lsegRef != u32(-1); i++) {

            auto lsegBlock = seekBlock(lsegRef);

            if (!lsegBlock || strcmp((const char *)lsegBlock, "LSEG")) {
                throw DeviceError(DeviceError::HDR_CORRUPTED_LSEG);
            }
            if (i >= 1024) {
                throw DeviceError(DeviceError::HDR_CORRUPTED_LSEG);
            }

            result.blocks.push_back(lsegRef);
            lsegRef = R32BE(lsegBlock + 16);
        }
    }

    return result;
}

std::vector<DriverDescriptor>
HDFFile::getDriverDescriptors() const
{
    std::vector<DriverDescriptor> result;

    for (isize i = 0; i < 16 && seekFSH(i); i++) {
        result.push_back(getDriverDescriptor(i));
    }

    return result;
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

bool
HDFFile::hasRDB() const
{
    // The rigid disk block must be among the first 16 blocks
    if (data.size >= 16 * 512) {
        for (isize i = 0; i < 16; i++) {
            if (strcmp((const char *)data.ptr + i * 512, "RDSK") == 0) return true;
        }
    }
    return false;
}

isize
HDFFile::partitionSize(isize nr) const
{
    auto &part = ptable[nr];
    return (part.highCyl - part.lowCyl + 1) * part.heads * part.sectors * 512;
}

isize
HDFFile::partitionOffset(isize nr) const
{
    auto &part = ptable[nr];
    return part.lowCyl * part.heads * part.sectors * 512;
}

u8 *
HDFFile::partitionData(isize nr) const
{
    return data.ptr + partitionOffset(nr);
}

isize
HDFFile::predictNumBlocks() const
{
    isize numReserved = 2;
    isize highKey = 0;

    auto match = [&]() {
        return isRB(seekBlock((numReserved + highKey) / 2));
    };

    if (auto root = seekRB(); root) {

        // Predict block count by analyzing the file size
        highKey = data.size / bsize() - 1;
        if (match()) return highKey + 1;

        // Predict block count by assuming a 32 sector standard geometry
        highKey = 32 * (data.size / (32 * bsize())) - 1;
        if (match()) return highKey + 1;

        // Predict by faking the numbers to fit
        highKey = 2 * isize(root - data.ptr) / bsize() - numReserved;
        if (match()) return highKey + 1;

        fatalError;
    }

    // No root
    return data.size / bsize();
}

u8 *
HDFFile::seekBlock(isize nr) const
{
    return nr >= 0 && 512 * (nr + 1) <= data.size ? data.ptr + (512 * nr) : nullptr;
}

bool
HDFFile::isRB(u8 *ptr) const
{
    return ptr && R32BE(ptr) == 2 && R32BE(ptr + bsize() - 4) == 1;
}

u8 *
HDFFile::seekRB() const
{
    auto max = data.size - 512;

    for (isize i = 0; i <= max; i += 512) {
        if (isRB(data.ptr + i)) return data.ptr + i;
    }

    return nullptr;
}

u8 *
HDFFile::seekRDB() const
{
    // The rigid disk block must be among the first 16 blocks
    for (isize i = 0; i < 16; i++) {
        if (auto p = seekBlock(i); p) {
            if (strcmp((const char *)p, "RDSK") == 0) return p;
        }
    }
    return nullptr;
}

u8 *
HDFFile::seekPB(isize nr) const
{
    u8 *result = nullptr;

    // Go to the rigid disk block
    if (auto rdb = seekRDB(); rdb) {

        // Go to the first partition block
        result = seekBlock(R32BE(rdb + 28));

        // Traverse the linked list
        for (isize i = 0; i < nr && result; i++) {
            result = seekBlock(R32BE(result + 16));
        }

        // Make sure the reached block is a partition block
        if (result && strcmp((const char *)result, "PART")) result = nullptr;
    }

    return result;
}

u8 *
HDFFile::seekFSH(isize nr) const
{
    u8 *result = nullptr;

    // Go to the rigid disk block
    if (auto rdb = seekRDB(); rdb) {

        // Go to the first file system header block
        result = seekBlock(R32BE(rdb + 32));

        // Traverse the linked list
        for (isize i = 0; i < nr && result; i++) {
            result = seekBlock(R32BE(result + 16));
        }

        // Make sure the reached block is a partition block
        if (result && strcmp((const char *)result, "FSHD")) result = nullptr;
    }

    return result;
}

std::optional<string>
HDFFile::rdbString(isize offset, isize len) const
{
    if (auto rdb = seekRDB(); rdb) {
        return utl::createStr(rdb + offset, len);
    }

    return { };
}

/*
FSFormat
HDFFile::dos(isize blockNr) const
{
    if (auto block = seekBlock(blockNr); block) {

        if (strncmp((const char *)block, "DOS", 3) || block[3] > 7) {
            return FSFormat::NODOS;
        }
        return (FSFormat)block[3];
    }

    return FSFormat::NODOS;
}
*/

isize
HDFFile::writePartitionToFile(const fs::path &path, isize nr) const
{
    auto offset = partitionOffset(nr);
    auto size = partitionSize(nr);

    return writeToFile(path, offset, size);
}

}
