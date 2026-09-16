// -----------------------------------------------------------------------------
// This file is part of RetroVault
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#include "rvconfig.h"
#include "Images/HDF/HDFLayout.h"
#include "Devices/DeviceError.h"
#include "Devices/LinearDevice.h"
#include "utl/support.h"
#include <cstring>

namespace retro::vault {

HDFLayout::HDFLayout(const LinearDevice &device) : bytes(device.size())
{
    reader = [&device](isize nr, u8 *dst) {

        auto offset = nr * bsize;
        if (nr < 0 || offset + bsize > device.size()) return false;

        device.read(dst, offset, bsize);
        return true;
    };
}

u8 *
HDFLayout::seek(isize nr) const
{
    if (nr < 0 || bsize * (nr + 1) > bytes) return nullptr;

    if (auto it = blocks.find(nr); it != blocks.end()) return it->second.data();

    std::array<u8, bsize> block;
    if (!reader(nr, block.data())) return nullptr;

    return blocks.emplace(nr, block).first->second.data();
}

bool
HDFLayout::isRB(const u8 *ptr) const
{
    return ptr && R32BE(ptr) == 2 && R32BE(ptr + bsize - 4) == 1;
}

optional<isize>
HDFLayout::seekRB() const
{
    for (isize nr = 0; bsize * (nr + 1) <= bytes; nr++) {
        if (isRB(seek(nr))) return nr;
    }

    return { };
}

u8 *
HDFLayout::seekRDB() const
{
    // The rigid disk block must be among the first 16 blocks
    for (isize i = 0; i < 16; i++) {
        if (auto p = seek(i); p) {
            if (strcmp((const char *)p, "RDSK") == 0) return p;
        }
    }
    return nullptr;
}

u8 *
HDFLayout::seekPB(isize nr) const
{
    u8 *result = nullptr;

    // Go to the rigid disk block
    if (auto rdb = seekRDB(); rdb) {

        // Go to the first partition block
        result = seek(R32BE(rdb + 28));

        // Traverse the linked list
        for (isize i = 0; i < nr && result; i++) {
            result = seek(R32BE(result + 16));
        }

        // Make sure the reached block is a partition block
        if (result && strcmp((const char *)result, "PART")) result = nullptr;
    }

    return result;
}

u8 *
HDFLayout::seekFSH(isize nr) const
{
    u8 *result = nullptr;

    // Go to the rigid disk block
    if (auto rdb = seekRDB(); rdb) {

        // Go to the first file system header block
        result = seek(R32BE(rdb + 32));

        // Traverse the linked list
        for (isize i = 0; i < nr && result; i++) {
            result = seek(R32BE(result + 16));
        }

        // Make sure the reached block is a file system header block
        if (result && strcmp((const char *)result, "FSHD")) result = nullptr;
    }

    return result;
}

isize
HDFLayout::predictNumBlocks() const
{
    isize numReserved = 2;
    isize highKey = 0;

    auto match = [&]() {
        return isRB(seek((numReserved + highKey) / 2));
    };

    if (auto root = seekRB(); root) {

        // Predict block count by analyzing the file size
        highKey = bytes / bsize - 1;
        if (match()) return highKey + 1;

        // Predict block count by assuming a 32 sector standard geometry
        highKey = 32 * (bytes / (32 * bsize)) - 1;
        if (match()) return highKey + 1;

        // Predict by faking the numbers to fit
        highKey = 2 * *root - numReserved;
        if (match()) return highKey + 1;

        fatalError;
    }

    // No root
    return bytes / bsize;
}

GeometryDescriptor
HDFLayout::getGeometryDescriptor() const
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
HDFLayout::getPartitionDescriptor(isize nr) const
{
    PartitionDescriptor result;

    if (auto pb = seekPB(nr); pb) {

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

        assert(nr == 0);

        // Add a default partition spanning the entire disk
        auto geo = getGeometryDescriptor();
        result = PartitionDescriptor(geo);

        // Make the first partition bootable
        result.flags |= 1;
    }

    return result;
}

std::vector<PartitionDescriptor>
HDFLayout::getPartitionDescriptors() const
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
HDFLayout::getDriverDescriptor(isize nr) const
{
    DriverDescriptor result;

    if (auto fsh = seekFSH(nr); fsh) {

        // Extract information from the file system header block
        result.dosType      = R32BE(fsh + 32);
        result.dosVersion   = R32BE(fsh + 36);
        result.patchFlags   = R32BE(fsh + 40);

        // Traverse the seglist
        auto lsegRef = R32BE(fsh + 72);

        for (isize i = 0; lsegRef != u32(-1); i++) {

            auto lsegBlock = seek(lsegRef);

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
HDFLayout::getDriverDescriptors() const
{
    std::vector<DriverDescriptor> result;

    for (isize i = 0; i < 16 && seekFSH(i); i++) {
        result.push_back(getDriverDescriptor(i));
    }

    return result;
}

optional<string>
HDFLayout::rdbString(isize offset, isize len) const
{
    if (auto rdb = seekRDB(); rdb) {
        return utl::createStr(rdb + offset, len);
    }

    return { };
}

}
